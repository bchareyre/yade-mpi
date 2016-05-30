// © 2007 Václav Šmilauer <eudoxos@arcig.cz>
// © 2013 Jan Stránský <jan.stransky@fsv.cvut.cz>

#include"Tetra.hpp"

#include<core/Interaction.hpp>
#include<core/Omega.hpp>
#include<core/Scene.hpp>
#include<core/State.hpp>
#include<pkg/common/ElastMat.hpp>

#include<pkg/common/Aabb.hpp>

#ifdef YADE_CGAL
	#include <CGAL/intersections.h>
#endif

YADE_PLUGIN(/* self-contained in hpp: */ (Tetra) (TTetraGeom) (TTetraSimpleGeom) (Bo1_Tetra_Aabb) 
	/* some code in cpp (this file): */ (TetraVolumetricLaw) 
	(Ig2_Tetra_Tetra_TTetraGeom)
	#ifdef YADE_CGAL
		(Ig2_Tetra_Tetra_TTetraSimpleGeom)
		(Law2_TTetraSimpleGeom_NormPhys_Simple)
	#endif
	#ifdef YADE_OPENGL
		(Gl1_Tetra)
	#endif	
	);

Tetra::~Tetra(){}
TTetraGeom::~TTetraGeom(){}
TTetraSimpleGeom::~TTetraSimpleGeom(){}



void Bo1_Tetra_Aabb::go(const shared_ptr<Shape>& ig, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){
	Tetra* t=static_cast<Tetra*>(ig.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	//Quaternionr invRot=se3.orientation.conjugate();   //The variable set but not used
	Vector3r v_g[4]; for(int i=0; i<4; i++) v_g[i]=se3.orientation*t->v[i]; // vertices in global coordinates
	#define __VOP(op,ix) op(v_g[0][ix],op(v_g[1][ix],op(v_g[2][ix],v_g[3][ix])))
		aabb->min=se3.position+Vector3r(__VOP(std::min,0),__VOP(std::min,1),__VOP(std::min,2));
		aabb->max=se3.position+Vector3r(__VOP(std::max,0),__VOP(std::max,1),__VOP(std::max,2));
	#undef __VOP
}




#ifdef YADE_CGAL
const int Ig2_Tetra_Tetra_TTetraSimpleGeom::psMap[4][3] = { // segments of point
	{0,2,3},
	{0,1,4},
	{1,2,5},
	{3,4,5}
};

const int Ig2_Tetra_Tetra_TTetraSimpleGeom::ptMap[4][3] = { // triangles of point
	{0,1,3},
	{0,1,2},
	{1,2,3},
	{0,2,3}
};

const int Ig2_Tetra_Tetra_TTetraSimpleGeom::stMap[6][2] = { // triangles of segments
	{0,1},
	{1,2},
	{1,3},
	{0,3},
	{0,2},
	{2,3}
};

const int Ig2_Tetra_Tetra_TTetraSimpleGeom::ppsMap[4][4] = { // point-point pair to segment
	{-1, 0, 2, 3},
	{ 0,-1, 1, 4},
	{ 2, 1,-1, 5},
	{ 3, 4, 5,-1}
};

const int Ig2_Tetra_Tetra_TTetraSimpleGeom::tsMap[4][3] = { // segmnts of triangle
	{0,3,4},
	{0,1,2},
	{1,4,5},
	{2,3,5}
};

const int Ig2_Tetra_Tetra_TTetraSimpleGeom::sstMap[6][6] = { // segment-segment pair to triangle
	{-1, 1, 1, 0, 0,-1},
	{ 1,-1, 1,-1, 2, 2},
	{ 1, 1,-1, 3,-1, 3},
	{ 0,-1, 3,-1, 0, 3},
	{ 0, 2,-1, 0,-1, 2},
	{-1, 2, 3, 3, 2,-1}
};

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::checkVertexToTriangleCase(
														const Triangle tA[4],
														const Point pB[4],
														const Segment sB[6],
														Vector3r& normal,
														Vector3r& contactPoint,
														Real& penetrationVolume)
{
	for (int i=0; i<4; i++) { // loop over triangles 1
		const Triangle& t = tA[i]; // choose triangle 1
		for (int j=0; j<4; j++) { // loop over vertices 2
			const Point& p = pB[j]; // choose vertex 2
			// choose edges posessing p
			const Segment& sa = sB[psMap[j][0]];
			const Segment& sb = sB[psMap[j][1]];
			const Segment& sc = sB[psMap[j][2]];
			if ( !(do_intersect(t,sa) && do_intersect(t,sb) && do_intersect(t,sc)) ) { continue; }// if all edges intersect with t
				// evaluate the points
			CGAL::Object oa = intersection(t,sa);
			const Point* pa = CGAL::object_cast<Point>(&oa);
			CGAL::Object ob = intersection(t,sb);
			const Point* pb = CGAL::object_cast<Point>(&ob);
			CGAL::Object oc = intersection(t,sc);
			const Point* pc = CGAL::object_cast<Point>(&oc);
			if ( !(pa && pb && pc) ) { continue; } // check that the intrsection really exists
			Vector_3 n = CGAL::normal(t[0],t[1],t[2]); // normal of triangle
			for (int k=0; k<3; k++) {
				normal[k] = n[k]; // sets normal of contact = nornal of triangle
				// contact point is center of mass of overlaping tetrahedron
				contactPoint[k] = .25*(p[k]+pa->operator[](k)+pb->operator[](k)+pc->operator[](k));
			}
			normal.normalize();
			const Point* v[4] = {&p,pa,pb,pc};
			penetrationVolume = TetrahedronVolume(v);
			Real vol = TetrahedronVolume(pB);
			if (penetrationVolume > .5*vol) { penetrationVolume = vol-penetrationVolume; }
			return true;
		}
	}
	return false;
}

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::checkEdgeToEdgeCase(
														const Segment sA[6],
														const Segment sB[6],
														const Triangle tA[4],
														const Triangle tB[4],
														Vector3r& normal,
														Vector3r& contactPoint,
														Real& penetrationVolume)
{
	for (int i=0; i<6; i++) {
		const Segment& sa = sA[i];
		const Triangle& ta0 = tA[stMap[i][0]];
		const Triangle& ta1 = tA[stMap[i][1]];
		for (int j=0; j<6; j++) {
			const Segment sb = sB[j];
			if ( !(do_intersect(sb,ta0) && do_intersect(sb,ta1) ) ) { continue; }
			const Triangle tb0 = tB[stMap[j][0]];
			const Triangle tb1 = tB[stMap[j][1]];
			if ( !(do_intersect(sa,tb0) && do_intersect(sa,tb1) ) ) { continue; }
			CGAL::Object osb1 = intersection(sb,ta0);
			CGAL::Object osb2 = intersection(sb,ta1);
			CGAL::Object osa1 = intersection(sa,tb0);
			CGAL::Object osa2 = intersection(sa,tb1);
			const Point* psb1 = CGAL::object_cast<Point>(&osb1);
			const Point* psb2 = CGAL::object_cast<Point>(&osb2);
			const Point* psa1 = CGAL::object_cast<Point>(&osa1);
			const Point* psa2 = CGAL::object_cast<Point>(&osa2);
			if ( !(psb1 && psb2 && psa1 && psa2) ) { continue; }
			Vector_3 n = CGAL::cross_product(sa.to_vector(),sb.to_vector());
			Vector3r nApprox;
			for (int k=0; k<3; k++) {
				normal[k] = n[k];
				#define OP(p) p->operator[](k)
				nApprox[k] = .5*(OP(psa1)+OP(psa2)) - .5*(OP(psb1)+OP(psb2));
				contactPoint[k] = .25*(OP(psa1)+OP(psa2)+OP(psb1)+OP(psb2));
				#undef OP
			}
			if ( nApprox.dot(normal) < 0 ) { normal *= (Real)-1.; }
			normal.normalize();
			const Point* p[4] = {psb1,psb2,psa1,psa2};
			penetrationVolume = TetrahedronVolume(p);
			return true;
		}
	}
	return false;
}

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::checkEdgeToTriangleCase1( // edge smaller than triangle
														const Triangle tA[4],
														const Segment sB[6],
														const Point pB[6],
														Vector3r& normal,
														Vector3r& contactPoint,
														Real& penetrationVolume)
{
	for (int i=0; i<4; i++) {
		const Triangle& ta = tA[i];
		int ni = 0;
		for (int j=0; j<6; j++) {
			const Segment& s = sB[j];
			if ( do_intersect(ta,s) ) { ni++; }
		}
		if ( ni != 4 ) { continue; }
		Vector_3 n = CGAL::normal(ta[0],ta[1],ta[2]);
		for (int j=0; j<3; j++) {
			const Point& p1 = pB[j];
			if ( Vector_3(ta[0],p1)*n > 0.) { continue; }
			const Segment& s10 = sB[psMap[j][0]];
			const Segment& s11 = sB[psMap[j][1]];
			const Segment& s12 = sB[psMap[j][2]];
			bool b10 = do_intersect(ta,s10);
			bool b11 = do_intersect(ta,s11);
			bool b12 = do_intersect(ta,s12);
			if ( !((b10 && b11) || (b11 && b12) || (b12 && b10) ) ) { continue; }
			for (int k=j+1; k<4; k++) {
				const Point& p2 = pB[k];
				if ( Vector_3(ta[0],p2)*n > 0. ) { continue; }
				const Segment& s20 = sB[psMap[k][0]];
				const Segment& s21 = sB[psMap[k][1]];
				const Segment& s22 = sB[psMap[k][2]];
				bool b20 = do_intersect(ta,s20);
				bool b21 = do_intersect(ta,s21);
				bool b22 = do_intersect(ta,s22);
				if ( !((b20 && b21) || (b21 && b22) || (b22 && b20) ) ) { continue; }
				int l,m;
				for (l=0; l<3; l++) {
					if (l!=j && l!=k) { break; }
				}
				for (m=l+1; m<4; m++) {
					if (m!=j && m!=k) { break; }
				}
				const Segment& s13 = sB[ppsMap[j][l]];
				const Segment& s14 = sB[ppsMap[j][m]];
				const Segment& s23 = sB[ppsMap[k][l]];
				const Segment& s24 = sB[ppsMap[k][m]];
				CGAL::Object o13 = intersection(ta,s13);
				CGAL::Object o14 = intersection(ta,s14);
				CGAL::Object o23 = intersection(ta,s23);
				CGAL::Object o24 = intersection(ta,s24);
				const Point* ps13 = CGAL::object_cast<Point>(&o13);
				const Point* ps14 = CGAL::object_cast<Point>(&o14);
				const Point* ps23 = CGAL::object_cast<Point>(&o23);
				const Point* ps24 = CGAL::object_cast<Point>(&o24);
				if ( !(ps13 && ps14 && ps23 && ps24) ) { continue; }
				const Point* pp1[4] = {&p1,&p2,ps13,ps14};
				const Point* pp2[4] = {&p2,ps23,ps24,ps14};
				const Point* pp3[4] = {&p2,ps23,ps13,ps14};
				Real v1 = TetrahedronVolume(pp1);
				Real v2 = TetrahedronVolume(pp2);
				Real v3 = TetrahedronVolume(pp3);
				Vector3r cg1,cg2,cg3;
				for (l=0; l<3; l++) {
					normal[l] = n[l];
					#define OP(p) p->operator[](l)
					cg1[l] = .25*(p1[l]+p2[l]+OP(ps13)+OP(ps14));
					cg2[l] = .25*(p2[l]+OP(ps23)+OP(ps24)+OP(ps14));
					cg3[l] = .25*(p2[l]+OP(ps23)+OP(ps13)+OP(ps14));
					#undef OP
				}
				penetrationVolume = v1 + v2 + v3;
				contactPoint = (v1*cg1 + v2*cg2 + v3*cg3) / penetrationVolume;
				normal.normalize();
				return true;
			}
		}
	}
	return false;
}

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::checkEdgeToTriangleCase2( // edge larger than triangle
														const Triangle tA[4],
														const Triangle tB[4],
														const Segment sA[6],
														const Segment sB[6],
														Vector3r& normal,
														Vector3r& contactPoint,
														Real& penetrationVolume)
{
	for (int i=0; i<6; i++) {
		const Segment& sb = sB[i];
		int ni = 0;
		for (int j=0; j<4; j++) {
			const Triangle& t = tA[j];
			if ( do_intersect(t,sb) ) { ni++; }
		}
		if ( ni != 2 ) { continue; }
		for (int j=0; j<3; j++) {
			const Triangle& ta1 = tA[j];
			if ( !(do_intersect(ta1,sb) ) ) { continue; }
			for (int k=j+1; k<4; k++) {
				const Triangle& ta2 = tA[k];
				if ( !(do_intersect(ta2,sb) ) ) { continue; }
				const Triangle& tb1 = tB[stMap[i][0]];
				const Triangle& tb2 = tB[stMap[i][1]];
				const Segment& sa1a = sA[tsMap[j][0]];
				const Segment& sa1b = sA[tsMap[j][1]];
				const Segment& sa1c = sA[tsMap[j][2]];
				bool b1a = do_intersect(sa1a,tb1) && do_intersect(sa1a,tb2);
				bool b1b = do_intersect(sa1b,tb1) && do_intersect(sa1b,tb2);
				bool b1c = do_intersect(sa1c,tb1) && do_intersect(sa1c,tb2);
				if ( !(b1a || b1b || b1c) ) { continue; }
				const Segment& sa2a = sA[tsMap[k][0]];
				const Segment& sa2b = sA[tsMap[k][1]];
				const Segment& sa2c = sA[tsMap[k][2]];
				bool b2a = do_intersect(sa2a,tb1) && do_intersect(sa2a,tb2);
				bool b2b = do_intersect(sa2b,tb1) && do_intersect(sa2b,tb2);
				bool b2c = do_intersect(sa2c,tb1) && do_intersect(sa2c,tb2);
				if ( !(b2a || b2b || b2c) ) { continue; } 
				int l = b1a? tsMap[j][0] : b1b? tsMap[j][1] : tsMap[j][2];
				int m = b2a? tsMap[k][0] : b2b? tsMap[k][1] : tsMap[k][2];
            if (sstMap[l][m] == -1) { continue; }
				const Segment& sa1 = sA[l];
				const Segment& sa2 = sA[m];
				const Triangle& taN = tA[sstMap[l][m]];
				CGAL::Object o1  = intersection(sb,ta1);
				CGAL::Object o2  = intersection(sb,ta2);
				CGAL::Object o11 = intersection(sa1,tb1);
				CGAL::Object o12 = intersection(sa1,tb2);
				CGAL::Object o21 = intersection(sa2,tb1);
				CGAL::Object o22 = intersection(sa2,tb2);
				const Point* p1  = CGAL::object_cast<Point>(&o1);
				const Point* p2  = CGAL::object_cast<Point>(&o2);
				const Point* p11 = CGAL::object_cast<Point>(&o11);
				const Point* p12 = CGAL::object_cast<Point>(&o12);
				const Point* p21 = CGAL::object_cast<Point>(&o21);
				const Point* p22 = CGAL::object_cast<Point>(&o22);
				if ( !(p1 && p2 && p11 && p12 && p21 && p22) ) { continue; }
				const Point* pp1[4] = {p1,p2,p11,p12};
				const Point* pp2[4] = {p2,p21,p22,p12};
				const Point* pp3[4] = {p2,p21,p11,p12};
				Real v1 = TetrahedronVolume(pp1);
				Real v2 = TetrahedronVolume(pp2);
				Real v3 = TetrahedronVolume(pp3);
				Vector3r cg1,cg2,cg3;
				Vector_3 n = CGAL::normal(taN[0],taN[1],taN[2]);
				for (int l=0; l<3; l++) {
					normal[l] = n[l];
					#define OP(p) p->operator[](l)
					cg1[l] = .25*(OP(p1)+OP(p2)+OP(p11)+OP(p12));
					cg2[l] = .25*(OP(p2)+OP(p21)+OP(p22)+OP(p12));
					cg3[l] = .25*(OP(p2)+OP(p21)+OP(p11)+OP(p12));
					#undef OP
				}
				penetrationVolume = v1 + v2 + v3;
				contactPoint = (v1*cg1 + v2*cg2 + v3*cg3) / penetrationVolume;
				normal.normalize();
				return true;
			}
		}
	}
	return false;
}

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::checkVertexToEdgeCase(
														const Point pA[4],
														const Segment sA[6],
														const Triangle tA[4],
														const Segment sB[6],
														const Triangle tB[4],
														Vector3r& normal,
														Vector3r& contactPoint,
														Real& penetrationVolume)
{
	for (int i=0; i<4; i++) {
		const Point& pa = pA[i];
		if ( Vector_3(tB[0][0],pa)*CGAL::normal(tB[0][0],tB[0][1],tB[0][2]) > 0. ) { continue; }
		if ( Vector_3(tB[1][0],pa)*CGAL::normal(tB[1][0],tB[1][1],tB[1][2]) > 0. ) { continue; }
		if ( Vector_3(tB[2][0],pa)*CGAL::normal(tB[2][0],tB[2][1],tB[2][2]) > 0. ) { continue; }
		if ( Vector_3(tB[3][0],pa)*CGAL::normal(tB[3][0],tB[3][1],tB[3][2]) > 0. ) { continue; }
		const Segment& sa1 = sA[psMap[i][0]];
		const Segment& sa2 = sA[psMap[i][1]];
		const Segment& sa3 = sA[psMap[i][2]];
		for (int j=0; j<6; j++) {
			const Segment& sb = sB[j];
			const Triangle& tb1 = tB[stMap[j][0]];
			const Triangle& tb2 = tB[stMap[j][1]];
			const Triangle& ta1 = tA[ptMap[i][0]];
			const Triangle& ta2 = tA[ptMap[i][1]];
			const Triangle& ta3 = tA[ptMap[i][2]];
			bool bsa1tb1 = do_intersect(sa1,tb1);
			bool bsa1tb2 = do_intersect(sa1,tb2);
			bool bsa2tb1 = do_intersect(sa2,tb1);
			bool bsa2tb2 = do_intersect(sa2,tb2);
			bool bsa3tb1 = do_intersect(sa3,tb1);
			bool bsa3tb2 = do_intersect(sa3,tb2);
			bool bsbta1 = do_intersect(sb,ta1);
			bool bsbta2 = do_intersect(sb,ta2);
			bool bsbta3 = do_intersect(sb,ta3);
			if ( !( (bsa1tb1 || bsa1tb2) && (bsa2tb1 || bsa2tb2) && (bsa3tb1 || bsa3tb2) && ((bsbta1 && bsbta2) || (bsbta2 && bsbta3) || (bsbta3 && bsbta1)) ) ) { continue; }
			CGAL::Object oa1 = intersection(sa1,bsa1tb1? tb1 : tb2);
			CGAL::Object oa2 = intersection(sa2,bsa2tb1? tb1 : tb2);
			CGAL::Object oa3 = intersection(sa3,bsa3tb1? tb1 : tb2);
			CGAL::Object ob1 = intersection(sb, (bsbta1 && bsbta2)? ta1 : (bsbta2 && bsbta3)? ta2 : ta3);
			CGAL::Object ob2 = intersection(sb, (bsbta1 && bsbta2)? ta2 : (bsbta2 && bsbta3)? ta3 : ta1);
			const Point* pa1 = CGAL::object_cast<Point>(&oa1);
			const Point* pa2 = CGAL::object_cast<Point>(&oa2);
			const Point* pa3 = CGAL::object_cast<Point>(&oa3);
			const Point* pb1 = CGAL::object_cast<Point>(&ob1);
			const Point* pb2 = CGAL::object_cast<Point>(&ob2);
			if ( !(pa1 && pa2 && pa3 && pb1 && pb2) ) { continue; }
			Segment sa(*pa1,*pa2);
			Real d1 = sqrt(CGAL::squared_distance(sa,*pb1));
			Real d2 = sqrt(CGAL::squared_distance(sa,*pb2));
			const Point* ppb1 = d1<d2? pb1 : pb2;
			const Point* ppb2 = d1<d2? pb2 : pb1;
			const Point* pp1[4] = {&pa,pa1,pa2,pa3};
			const Point* pp2[4] = {pa1,pa2,pa3,ppb2};
			const Point* pp3[4] = {pa1,pa2,ppb1,ppb2};
			Real v1 = TetrahedronVolume(pp1);
			Real v2 = TetrahedronVolume(pp2);
			Real v3 = TetrahedronVolume(pp3);
			Vector3r cg1,cg2,cg3;
			Vector_3 n(pa,sb.supporting_line().projection(pa));
			for (int l=0; l<3; l++) {
				normal[l] = n[l];
				#define OP(p) p->operator[](l)
				cg1[l] = .25*(pa[l]+OP(pa1)+OP(pa2)+OP(pa3));
				cg2[l] = .25*(OP(pa1)+OP(pa2)+OP(pa3)+OP(ppb2));
				cg3[l] = .25*(OP(pa1)+OP(pa2)+OP(ppb1)+OP(ppb2));
				#undef OP
			}
			penetrationVolume = v1 + v2 + v3;
			contactPoint = (v1*cg1 + v2*cg2 + v3*cg3) / penetrationVolume;
			normal.normalize();
			return true;
		}
	}
	return false;
}

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::checkVertexToVertexCase(
														const Point pA[4],
														const Point pB[4],
														const Segment sA[6],
														const Triangle tA[4],
														const Triangle tB[4],
														Vector3r& normal,
														Vector3r& contactPoint,
														Real& penetrationVolume)
{
	for (int i=0; i<4; i++) {
		const Point& pa = pA[i];
		if ( Vector_3(tB[0][0],pa)*CGAL::normal(tB[0][0],tB[0][1],tB[0][2]) > 0. ) { continue; }
		if ( Vector_3(tB[1][0],pa)*CGAL::normal(tB[1][0],tB[1][1],tB[1][2]) > 0. ) { continue; }
		if ( Vector_3(tB[2][0],pa)*CGAL::normal(tB[2][0],tB[2][1],tB[2][2]) > 0. ) { continue; }
		if ( Vector_3(tB[3][0],pa)*CGAL::normal(tB[3][0],tB[3][1],tB[3][2]) > 0. ) { continue; }
		const Segment& sa1 = sA[psMap[i][0]];
		const Segment& sa2 = sA[psMap[i][1]];
		const Segment& sa3 = sA[psMap[i][2]];
		for (int j=0; j<4; j++) {
			const Point& pb = pB[j];
			if ( Vector_3(tA[0][0],pb)*CGAL::normal(tA[0][0],tA[0][1],tA[0][2]) > 0. ) { continue; }
			if ( Vector_3(tA[1][0],pb)*CGAL::normal(tA[1][0],tA[1][1],tA[1][2]) > 0. ) { continue; }
			if ( Vector_3(tA[2][0],pb)*CGAL::normal(tA[2][0],tA[2][1],tA[2][2]) > 0. ) { continue; }
			if ( Vector_3(tA[3][0],pb)*CGAL::normal(tA[3][0],tB[3][1],tB[3][2]) > 0. ) { continue; }
			const Triangle& tb1 = tB[ptMap[j][0]];
			const Triangle& tb2 = tB[ptMap[j][1]];
			const Triangle& tb3 = tB[ptMap[j][2]];
			bool b11 = do_intersect(sa1,tb1);
			bool b12 = do_intersect(sa1,tb2);
			bool b13 = do_intersect(sa1,tb3);
			bool b21 = do_intersect(sa2,tb1);
			bool b22 = do_intersect(sa2,tb2);
			bool b23 = do_intersect(sa2,tb3);
			bool b31 = do_intersect(sa3,tb1);
			bool b32 = do_intersect(sa3,tb2);
			bool b33 = do_intersect(sa3,tb3);
			if ( !(b11 || b12 || b13) && (b21 || b22 || b23) && (b31 || b32 || b33) ) { continue; }
			CGAL::Object o1 = intersection(sa1, b11? tb1: b12? tb2 : tb3);
			CGAL::Object o2 = intersection(sa2, b21? tb1: b22? tb2 : tb3);
			CGAL::Object o3 = intersection(sa3, b31? tb1: b32? tb2 : tb3);
			const Point* p1 = CGAL::object_cast<Point>(&o1);
			const Point* p2 = CGAL::object_cast<Point>(&o2);
			const Point* p3 = CGAL::object_cast<Point>(&o3);
			if ( !(p1 && p2 && p3) ) { continue; }
			const Point* pp1[4] = {&pa,p1,p2,p3};
			const Point* pp2[4] = {&pb,p2,p3,p3};
			Real v1 = TetrahedronVolume(pp1);
			Real v2 = TetrahedronVolume(pp2);
			Vector3r cg1,cg2;
			Vector_3 n(pa,pb);
			for (int l=0; l<3; l++) {
				normal[l] = n[l];
				#define OP(p) p->operator[](l)
				cg1[l] = .25*(pa[l]+OP(p1)+OP(p2)+OP(p3));
				cg2[l] = .25*(pb[l]+OP(p1)+OP(p2)+OP(p3));
				#undef OP
			}
			penetrationVolume = v1 + v2;
			contactPoint = (v1*cg1 + v2*cg2) / penetrationVolume;
			normal.normalize();
			return true;
		}
	}
	return false;
}

bool Ig2_Tetra_Tetra_TTetraSimpleGeom::go(
														const shared_ptr<Shape>& cm1,
														const shared_ptr<Shape>& cm2,
														const State& state1,
														const State& state2,
														const Vector3r& shift2,
														const bool& force,
														const shared_ptr<Interaction>& interaction)
{
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;
	Tetra* shape1 = static_cast<Tetra*>(cm1.get());
	Tetra* shape2 = static_cast<Tetra*>(cm2.get());

	Point p1[4], p2[4];
	Vector3r vTemp;
	// vertices in global coordinates
	for (int i=0; i<4; i++) {
		vTemp = se31.position + se31.orientation*shape1->v[i];
		p1[i] = Point(vTemp[0],vTemp[1],vTemp[2]);
		vTemp = se32.position + se32.orientation*shape2->v[i] + shift2;
		p2[i] = Point(vTemp[0],vTemp[1],vTemp[2]);
	}

	// Faces (CGAL triangles) of each tetra
	#define T(p,i,j,k) Triangle(p[i],p[j],p[k])
	const Triangle t1[4] = {T(p1,0,1,3),T(p1,0,2,1),T(p1,1,2,3),T(p1,0,3,2)};
	const Triangle t2[4] = {T(p2,0,1,3),T(p2,0,2,1),T(p2,1,2,3),T(p2,0,3,2)};
	#undef T
	// Edges (CGAL segments) of each tetra
	#define S(p,i,j) Segment(p[i],p[j])
	const Segment s1[6] = {S(p1,0,1),S(p1,1,2),S(p1,0,2),S(p1,0,3),S(p1,1,3),S(p1,2,3)};
	const Segment s2[6] = {S(p2,0,1),S(p2,1,2),S(p2,0,2),S(p2,0,3),S(p2,1,3),S(p2,2,3)};
	#undef S

	Vector3r n;
	Vector3r cp;
	Real V;
	int flag;

	# define SET_GEOM_AND_RETURN_TRUE \
		shared_ptr<TTetraSimpleGeom> geom; \
		if (!interaction->geom) geom=shared_ptr<TTetraSimpleGeom>(new TTetraSimpleGeom()); \
		else geom=YADE_PTR_CAST<TTetraSimpleGeom>(interaction->geom); \
		interaction->geom=geom; \
		geom->normal = n; \
		geom->contactPoint = cp; \
		geom->penetrationVolume = V; \
		geom->flag = flag; \
		return true;


	if (checkVertexToTriangleCase(t1,p2,s2,n,cp,V)) {
		flag = 1;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkVertexToTriangleCase(t2,p1,s1,n,cp,V)) {
		n *= -1.;
		flag = 2;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkEdgeToEdgeCase(s1,s2,t1,t2,n,cp,V)) {
		flag = 3;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkEdgeToTriangleCase1(t1,s2,p2,n,cp,V)) {
		flag = 4;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkEdgeToTriangleCase1(t2,s1,p1,n,cp,V)) {
		n *= -1.;
		flag = 5;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkEdgeToTriangleCase2(t1,t2,s1,s2,n,cp,V)) {
		flag = 6;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkEdgeToTriangleCase2(t2,t1,s2,s1,n,cp,V)) {
		n *= -1.;
		flag = 7;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkVertexToEdgeCase(p1,s1,t1,s2,t2,n,cp,V)) {
		n *= -1.;
		flag = 8;
		SET_GEOM_AND_RETURN_TRUE
	}
	if (checkVertexToEdgeCase(p2,s2,t2,s1,t1,n,cp,V)) {
		flag = 9;
		SET_GEOM_AND_RETURN_TRUE
	}

	#undef SET_GEOM_AND_RETURN_TRUE

	
	if (interaction->geom) {
		TTetraSimpleGeom* geom = static_cast<TTetraSimpleGeom*>(interaction->geom.get());
		geom->penetrationVolume = (Real)-1.;
		geom->flag = 0;
		return true;
	}
	return false;
}
#endif







CREATE_LOGGER(Ig2_Tetra_Tetra_TTetraGeom);

/*! Calculate configuration of Tetra - Tetra intersection.
 *
 * Wildmagick's functions are used here: intersection is returned as a set of tetrahedra (may be empty, inwhich case there is no real intersection).
 * Then we calcualte volumetric proeprties of this intersection volume: inertia, centroid, volume.
 *
 * Contact normal (the direction in which repulsive force will act) coincides with the direction of least inertia,
 * since that is the gradient that maximizes the drop of elastic deformation energy and will reach minimum fastest.
 *
 * Equivalent cross section of the penetrating volume (as if it were a cuboid with the same inertia) and equivalent penetration depth are calculated;
 * Equivalent solid size in the dimension of normal serves as reference for strain calculation and is different for solids A and B.
 *
 * Strain will be then approximated by equivalentPenetrationDepth/.5*(maxPenetrationDepthA+maxPenetrationDepthB) (the average of A and B)
 *
 * All the relevant results are fed into TTetraGeom which is passed to TetraVolumetricLaw later that makes actual use of all this.
 *
 * @todo thoroughly test this for numerical correctness.
 *
 */
bool Ig2_Tetra_Tetra_TTetraGeom::go(const shared_ptr<Shape>& cm1,const shared_ptr<Shape>& cm2,const State& state1,const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& interaction){
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;
	Tetra* A = static_cast<Tetra*>(cm1.get());
	Tetra* B = static_cast<Tetra*>(cm2.get());
	//return false;
	
	shared_ptr<TTetraGeom> bang;
	// depending whether it's a new interaction: create new one, or use the existing one.
	if (!interaction->geom) bang=shared_ptr<TTetraGeom>(new TTetraGeom());
	else bang=YADE_PTR_CAST<TTetraGeom>(interaction->geom);	
	interaction->geom=bang;
	
	// use wildmagick's intersection routine?
	#if 0
		// transform to global coordinates, build Tetrahedron3r objects to make wm3 happy
		Tetrahedron3r tA(se31.orientation*A->v[0]+se31.position,se31.orientation*A->v[1]+se31.position,se31.orientation*A->v[2]+se31.position,se31.orientation*A->v[3]+se31.position);
		Tetrahedron3r tB(se32.orientation*B->v[0]+se32.position,se32.orientation*B->v[1]+se32.position,se32.orientation*B->v[2]+se32.position,se32.orientation*B->v[3]+se32.position);

		IntrTetrahedron3Tetrahedron3r iAB(tA,tB);
		bool found=iAB.Find();  //calculates the intersection volume as a composition of 0 or more tetrahedra

		if(!found) return false; // no intersecting volume

		Real V(0); // volume of intersection (cummulative)
		Vector3r Sg(0,0,0); // static moment of intersection
		vector<vector<Vector3r> > tAB;

		Wm3::TArray<Wm3::Tetrahedron3d> iABinfo(iAB.GetIntersection()); // retrieve the array of 4hedra
		for(int i=0; i<iABinfo.GetQuantity(); i++){
			iABinfo[i];  // has i-th tehtrahedron as Tetrahedron3r&
			#define v0 iABinfo[i].V[0]
			#define v1 iABinfo[i].V[1]
			#define v2 iABinfo[i].V[2]
			#define v3 iABinfo[i].V[3]
			Real dV=std::abs(Vector3r(v1-v0).Dot((v2-v0).Cross(v3-v0)))/6.;
			V+=dV;
			Sg+=dV*(v0+v1+v2+v3)*.25;
			vector<Vector3r> t; t.push_back(v0); t.push_back(v1); t.push_back(v2); t.push_back(v3);
			tAB.push_back(t);
			#undef v0
			#undef v1
			#undef v2
			#undef v3
		}
	#endif

	// transform to global coordinates, build Tetra objects
	Tetra tA(se31.orientation*A->v[0]+se31.position,se31.orientation*A->v[1]+se31.position,se31.orientation*A->v[2]+se31.position,se31.orientation*A->v[3]+se31.position);
	Tetra tB(se32.orientation*B->v[0]+se32.position+shift2,se32.orientation*B->v[1]+se32.position+shift2,se32.orientation*B->v[2]+se32.position+shift2,se32.orientation*B->v[3]+se32.position+shift2);
	// calculate intersection
	#if 0
		tB=Tetra(Vector3r(0,0,0),Vector3r(1.5,1,1),Vector3r(0.5,1,1),Vector3r(1,1,.5));
		tA=Tetra(Vector3r(0,0,0),Vector3r(1,0,0),Vector3r(0,1,0),Vector3r(0,0,1));
	#endif
	list<Tetra> tAB=Tetra2TetraIntersection(tA,tB);
	if (!interaction->isReal() && !force) {
		if(tAB.size()==0) { /* LOG_DEBUG("No intersection."); */ return false;} //no intersecting volume
	}

	Real V(0); // volume of intersection (cummulative)
	Vector3r Sg(0,0,0); // static moment of intersection

	Vector3r tt[4]; for(int i=0; i<4; i++) tt[i]=tA.v[i];

	for(list<Tetra>::iterator II=tAB.begin(); II!=tAB.end(); II++){
		Real dV=TetrahedronVolume(II->v);
		V+=dV;
		Sg+=dV*(II->v[0]+II->v[1]+II->v[2]+II->v[3])*.25;
	}
	Vector3r centroid=Sg/V;
	Matrix3r I(Matrix3r::Zero()); // inertia tensor for the composition; zero matrix initially
		// I is purely geometrical (as if with unit density)
	
	// get total 
	Vector3r dist;	for(list<Tetra>::iterator II=tAB.begin(); II!=tAB.end(); II++){
		II->v[0]-=centroid; II->v[1]-=centroid; II->v[2]-=centroid; II->v[3]-=centroid;
		dist=(II->v[0]+II->v[1]+II->v[2]+II->v[3])*.25-centroid;
		/* use parallel axis theorem */ 
		Matrix3r distSq(Matrix3r::Zero()); distSq(0,0)=dist[0]*dist[0]; distSq(1,1)=dist[1]*dist[1]; distSq(2,2)=dist[2]*dist[2]; // could be done more intelligently with eigen
		I+=TetrahedronInertiaTensor(II->v)+TetrahedronVolume(II->v)*distSq;
	}
	
	/* Now, we have the collision volumetrically described by intersection volume (V), its inertia tensor (I) and centroid (centroid; contact point).
	 * The inertia tensor is in global coordinates; by eigendecomposition, we find principal axes, which will give us
	 *  1. normal, the direction of the lest inertia; this is the gradient of penetration energy
	 *  	it may have either direction mathematically, but since 4hedra are convex, 
	 *  	normal will be always the direction pointing more towards the centroid of the other 4hedron
	 *  2. tangent?! hopefully not needed at all. */

	Matrix3r Ip, R; // principal moments of inertia, rotation matrix
	/* should check convergence*/	matrixEigenDecomposition(I,R,Ip);
	// according to the documentation in Wm3 header, diagonal entries are in ascending order: d0<=d1<=d2;
	// but keep it algorithmic for now and just assert that.
	int ix=(Ip(0,0)<Ip(1,1) && Ip(0,0)<Ip(2,2))?0:( (Ip(1,1)<Ip(0,0) && Ip(1,1)<Ip(2,2))?1:2); // index of the minimum moment of inertia
	// the other two indices, modulated by 3, since they are ∈ {0,1,2}
	int ixx=(ix+1)%3, ixxx=(ix+2)%3;
	// assert what the documentation says (d0<=d1<=d2)
	assert(ix==0);
	Vector3r minAxis(0,0,0); minAxis[ix]=1; // the axis of minimum inertia
	Vector3r normal=R*minAxis; normal.normalize(); // normal is minAxis in global coordinates (normalization shouldn't be needed since R is rotation matrix, but to make sure...)

	// centroid of B
	Vector3r Bcent=se31.orientation*((B->v[0]+B->v[1]+B->v[2]+B->v[3])*.25)+se31.position;
	// reverse direction if projection of the (contact_point-centroid_of_B) vector onto the normal is negative (i.e. the normal points more towards A)
	if((Bcent-centroid).dot(normal)<0) normal*=-1;

	/* now estimate the area of the solid that is perpendicular to the normal. This will be needed to estimate elastic force based on Young's modulus.
	 * Suppose we have cuboid, with edges of lengths x,y,z in the direction of respective axes.
	 * It's inertia are Ix=(V/12)*(y^2+z^2), Iy=(V/12)*(x^2+z^2), Iz=(V/12)*(x^2+y^2) and suppose Iz is maximal; Ix, Iy and Iz are known (from decomposition above).
	 * Then the area perpendicular to z (normal direction) is given by x*y=V/z, where V is known.
	 * Ix+Iy-Iz=(V/12)*(y^2+z^2+x^2+z^2-x^2-y^2)=(V*z^2)/6, z=√(6*(Ix+Iy-Iz)/V)
	 * Az=V/z=√(V^3/(6*(Ix+Iy-Iz))).
	 *
	 * In our case, the greatest inertia is along ixxx, the other coordinates are ixx and ix. equivalentPenetrationDepth means what was z.
	 */

	Real equivalentPenetrationDepth=sqrt(6.*(Ip(ix,ix)+Ip(ixx,ixx)-Ip(ixxx,ixxx))/V);
	Real equivalentCrossSection=V/equivalentPenetrationDepth;
	TRVAR3(V,equivalentPenetrationDepth,equivalentCrossSection);

	/* Now rotate the whole inertia tensors of A and B and estimate maxPenetrationDepth -- the length of the body in the direction of the contact normal.
	 * This will be used to calculate relative deformation, which is needed for elastic response. */
	const State* physA=Body::byId(interaction->getId1())->state.get(); const State* physB=Body::byId(interaction->getId2())->state.get();
	// WARNING: Matrix3r(Vector3r(...)) is compiled, but gives zero matrix??!! Use explicitly constructor from diagonal entries
	//Matrix3r IA(physA->inertia[0],physA->inertia[1],physA->inertia[2]); Matrix3r IB(physB->inertia[0],physB->inertia[1],physB->inertia[2]);
	Matrix3r IA=Matrix3r::Zero(), IB=Matrix3r::Zero(); for(int i=0; i<3; i++){ IA(i,i)=physA->inertia[i]; IB(i,i)=physB->inertia[i]; }
	// see Clump::inertiaTensorRotate for references
	IA=R.transpose()*IA*R; IB=R.transpose()*IB*R;

	Real maxPenetrationDepthA=sqrt(6*(IA(ix,ix)+IA(ixx,ixx)-IA(ixxx,ixxx))/V);
	Real maxPenetrationDepthB=sqrt(6*(IB(ix,ix)+IB(ixx,ixx)-IB(ixxx,ixxx))/V);
	TRVAR2(maxPenetrationDepthA,maxPenetrationDepthB);

	//normal = se32.position - se31.position; normal.normalize();

	/* store calculated stuff in bang; some is redundant */
	bang->normal=normal;
	bang->equivalentCrossSection=equivalentCrossSection;
	bang->contactPoint=centroid;
	bang->penetrationVolume=V;

	bang->equivalentPenetrationDepth=equivalentPenetrationDepth;
	bang->maxPenetrationDepthA=maxPenetrationDepthA;
	bang->maxPenetrationDepthB=maxPenetrationDepthB;

	return true;
}

/*! Calculate intersection o Tetrahedron A and B as union of set (std::list) of 4hedra.
 *
 * intersecting tetrahedra A and B
 * S=intersection set (4hedra)
 * S={A}
 * for face in B_faces:
 *		for t in S:  [ S is mutable, but if list, iterators remain valid? ]
 * 		tmp = clip t by face // may return multiple 4hedra or none
 * 		replace t by tmp (possibly none) in S
 * return S
 *
 */
list<Tetra> Ig2_Tetra_Tetra_TTetraGeom::Tetra2TetraIntersection(const Tetra& A, const Tetra& B){
	// list of 4hedra to split; initially A
	list<Tetra> ret; ret.push_back(A);
	/* I is vertex index at B;
	 * clipping face is [i i1 i2], normal points away from i3 */
	int i,i1,i2,i3;
	Vector3r normal;
	/* LOG_TRACE("===========================================================================================")
	LOG_TRACE("===========================================================================================")
	LOG_TRACE(ret.size());
	LOG_TRACE("DUMP A and B:"); A.dump(); B.dump(); */
	for(i=0; i<4; i++){
		i1=(i+1)%4; i2=(i+2)%4; i3=(i+3)%4;
		const Vector3r& P(B.v[i]); // reference point on the plane
		normal=(B.v[i1]-P).cross(B.v[i2]-P); normal.normalize(); // normal
		if((B.v[i3]-P).dot(normal)>0) normal*=-1; // outer normal
		for(list<Tetra>::iterator I=ret.begin(); I!=ret.end(); /* I++ */ ){
			list<Tetra> splitDecomposition=TetraClipByPlane(*I,P,normal);
			// replace current list element by the result of decomposition;
			// I points after the erased one, so decomposed 4hedra will not be touched in this iteration, just as we want.
			// Since it will be incremented by I++ at the end of the cycle, compensate for that by I--;
			I=ret.erase(I); ret.insert(I,splitDecomposition.begin(),splitDecomposition.end()); /* I--; */
			/* LOG_TRACE("DUMP current tetrahedron list:"); for(list<Tetra>::iterator I=ret.begin(); I!=ret.end(); I++) (*I).dump();*/ 
		}
	}
	//exit(0);
	return ret;
}

/*! Clip Tetra T by plane give by point P and outer normal n.
 *
 * Algorithm: 
 *
 * clip t by face
 * 	sort points of t into positive, negative, zero (face normal n points outside)
 * 		-: inside; +: outside; 0: on face
 * 		homogeneous cases (no split):
 * 			++++, +++0, ++00, +000 :
 * 				0Δ full clip (everything outside), nothing left; return ∅
 * 			----, ---0, --00, -000 :
 * 				1Δ all inside, return identity
 *			split (at least one - and one +)
 *				-+++
 * 				1Δ [A AB AC AD]
 *				-++0
 * 				1Δ [A AB AC D]
 *				-+00:
 * 				1Δ [A AB C D]
 * 			--++:
 * 				3Δ [A AC AD B BC BD] ⇒ (e.g.) [A AC AD B] [B BC BD AD] [B AD AC BC]
 * 			--+0:
 * 				2Δ [A B AC BC D] ⇒ (e.g.) [A AC BC D] [B BC A D] 
 * 			---+:
 * 				3Δ tetrahedrize [A B C AD BD CD]
 *
 * http://members.tripod.com/~Paul_Kirby/vector/Vplanelineint.html
 */
list<Tetra> Ig2_Tetra_Tetra_TTetraGeom::TetraClipByPlane(const Tetra& T, const Vector3r& P, const Vector3r& normal){
	
	list<Tetra> ret;
	// scaling factor for Mathr::EPSILON: average edge length
	Real scaledEPSILON=Mathr::EPSILON*(1/6.)*((T.v[1]-T.v[0])+(T.v[2]-T.v[0])+(T.v[3]-T.v[0])+(T.v[2]-T.v[1])+(T.v[3]-T.v[1])+(T.v[3]-T.v[2])).norm();

	vector<size_t> pos, neg, zer; Real dist[4];
	for(size_t i=0; i<4; i++){
		dist[i]=(T.v[i]-P).dot(normal);
		if(dist[i]>scaledEPSILON) pos.push_back(i);
		else if(dist[i]<-scaledEPSILON) neg.push_back(i);
		else zer.push_back(i);
	}
	/* LOG_TRACE("dist[i]=["<<dist[0]<<","<<dist[1]<<","<<dist[2]<<","<<dist[3]<<"]"); */
	#define NEG neg.size()
	#define POS pos.size()
	#define ZER zer.size()
	#define PTPT(i,j) PtPtPlaneIntr(v[i],v[j],P,normal)
	assert(NEG+POS+ZER==4);

	// HOMOGENEOUS CASES
		// ++++, +++0, ++00, +000, 0000 (degenerate (planar) tetrahedron)
		if(POS==4 || (POS==3 && ZER==1) || (POS==2 && ZER==2) || (POS==1 && ZER==3) || ZER==4) return ret; // ∅
	 	// ----, ---0, --00, -000 :
		if(NEG==4 || (NEG==3 && ZER==1) || (NEG==2 && ZER==2) || (NEG==1 && ZER==3)) {ret.push_back(T); return ret;}
	// HETEROGENEOUS CASES
		// points are ordered -+0
		Vector3r v[4];
		for(size_t i=0; i<NEG; i++) v[i+  0+  0]=T.v[neg[i]];
		for(size_t i=0; i<POS; i++) v[i+  0+NEG]=T.v[pos[i]];
		for(size_t i=0; i<ZER; i++) v[i+POS+NEG]=T.v[zer[i]];

		#define _A v[0]
		#define _B v[1]
		#define _C v[2]
		#define _D v[3]
		#define _AB PTPT(0,1)
		#define _AC PTPT(0,2)
		#define _AD PTPT(0,3)
		#define _BC PTPT(1,2)
		#define _BD PTPT(1,3)
		#define _CD PTPT(2,3)
		// -+++ → 1Δ [A AB AC AD]
		if(NEG==1 && POS==3){ret.push_back(Tetra(_A,_AB,_AC,_AD)); return ret;}
		// -++0 → 1Δ [A AB AC D]
		if(NEG==1 && POS==2 && ZER==1){ret.push_back(Tetra(_A,_AB,_AC,_D)); return ret;}
		//	-+00 → 1Δ [A AB C D]
		if(NEG==1 && POS==1 && ZER==2){ret.push_back(Tetra(_A,_AB,_C,_D)); return ret;}
		// --++ → 3Δ [A AC AD B BC BD] ⇒ (e.g.) [A AC AD B] [B BC BD AD] [B AD AC BC]
		if(NEG==2 && POS ==2){
			// [A AC AD B]
			ret.push_back(Tetra(_A,_AC,_AD,_B));
			// [B BC BD AD]
			ret.push_back(Tetra(_B,_BC,_BD,_AD));
			// [B AD AC BC]
			ret.push_back(Tetra(_B,_AD,_AC,_BC));
			return ret;
		}
		// --+0 → 2Δ [A B AC BC D] ⇒ (e.g.) [A AC BC D] [B BC A D] 
		if(NEG==2 && POS==1 && ZER==1){
			// [A AC BC D]
			ret.push_back(Tetra(_A,_AC,_BC,_D));
			// [B BC A D]
			ret.push_back(Tetra(_B,_BC,_A,_D));
			return ret;
		}
		// ---+ → 3Δ [A B C AD BD CD] ⇒ (e.g.) [A B C AD] [AD BD CD B] [AD C B BD]
		if(NEG==3 && POS==1){
			//[A B C AD]
			ret.push_back(Tetra(_A,_B,_C,_AD));
			//[AD BD CD B]
			ret.push_back(Tetra(_AD,_BD,_CD,_B));
			//[AD C B BD]
			ret.push_back(Tetra(_AD,_C,_B,_BD));
			return ret;
		}
		#undef _A
		#undef _B
		#undef _C
		#undef _D
		#undef _AB
		#undef _AC
		#undef _AD
		#undef _BC
		#undef _BD
		#undef _CD

	#undef PTPT
	#undef NEG
	#undef POS
	#undef ZER
	// unreachable
	assert(false);
	return(ret); // prevent warning
}


CREATE_LOGGER(TetraVolumetricLaw);

/*! Apply forces on tetrahedra in collision based on geometric configuration provided by Ig2_Tetra_Tetra_TTetraGeom.
 *
 * DO NOT USE, probably doesn't work.
 * Comments on functionality limitations are in the code. It has not been tested at all!!! */
void TetraVolumetricLaw::action()
{
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		// normally, we would test isReal(), but TetraVolumetricLaw doesn't use phys at all
		if (!I->geom) continue; // Ig2_Tetra_Tetra_TTetraGeom::go returned false for this interaction, skip it
		const shared_ptr<TTetraGeom>& contactGeom(YADE_PTR_DYN_CAST<TTetraGeom>(I->geom));
		if(!contactGeom) continue;

		const Body::id_t idA=I->getId1(), idB=I->getId2();
		const shared_ptr<Body>& A=Body::byId(idA), B=Body::byId(idB);
			
		const shared_ptr<ElastMat>& physA(YADE_PTR_DYN_CAST<ElastMat>(A->material));
		const shared_ptr<ElastMat>& physB(YADE_PTR_DYN_CAST<ElastMat>(B->material));
		

		/* Cross-section is volumetrically equivalent to the penetration configuration */
		Real averageStrain=contactGeom->equivalentPenetrationDepth/(.5*(contactGeom->maxPenetrationDepthA+contactGeom->maxPenetrationDepthB));

		/* Do not use NormPhys::kn (as calculated by ElasticBodySimpleRelationship).
		 * NormPhys::kn is not Young's modulus, it is calculated by MacroMicroElasticRelationships. So perhaps
		 * a new IPhysFunctor will be needed that will just pass the average Young's modulus here?
		 * For now, just go back to Young's moduli directly here. */
		Real young=.5*(physA->young+physB->young);
		TRVAR3(young,averageStrain,contactGeom->equivalentCrossSection);
		// F=σA=εEA
		// this is unused; should it?: contactPhys->kn
		Vector3r F=contactGeom->normal*averageStrain*young*contactGeom->equivalentCrossSection;

		scene->forces.addForce (idA,-F);
		scene->forces.addForce (idB, F);
		scene->forces.addTorque(idA,-(A->state->pos-contactGeom->contactPoint).cross(F));
		scene->forces.addTorque(idB, (B->state->pos-contactGeom->contactPoint).cross(F));
	}
}

#ifdef YADE_OPENGL
	#include <lib/opengl/OpenGLWrapper.hpp>
	
	bool Gl1_Tetra::wire;
	void Gl1_Tetra::go(const shared_ptr<Shape>& cm, const shared_ptr<State>&,bool wire2,const GLViewInfo&)
	{
		glMaterialv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,Vector3r(cm->color[0],cm->color[1],cm->color[2]));
		glColor3v(cm->color);
		Tetra* t=static_cast<Tetra*>(cm.get());
		if (wire && wire2) { // wireframe, as for Tetrahedron
			glDisable(GL_LIGHTING);
			glBegin(GL_LINES);
				glOneWire(t, 0, 1);
				glOneWire(t, 0, 2);
				glOneWire(t, 0, 3);
				glOneWire(t, 1, 2);
				glOneWire(t, 1, 3);
				glOneWire(t, 2, 3);
			glEnd();
		}
		else
		{
			glDisable(GL_CULL_FACE); glEnable(GL_LIGHTING);
			glBegin(GL_TRIANGLES);
				glOneFace (t, 0,2,1);
				glOneFace (t, 0,1,3);
				glOneFace (t, 1,2,3);
				glOneFace (t, 0,3,2);
			glEnd();
		}
	}
#endif

/*! Calculates tetrahedron inertia relative to the origin (0,0,0), with unit density (scales linearly).

See article F. Tonon, "Explicit Exact Formulas for the 3-D Tetrahedron Inertia Tensor in Terms of its Vertex Coordinates", http://www.scipub.org/fulltext/jms2/jms2118-11.pdf

Numerical example to check:

vertices:
	(8.33220, 11.86875, 0.93355)
	(0.75523 ,5.00000, 16.37072)
	(52.61236, 5.00000, 5.38580)
	(2.00000, 5.00000, 3.00000)
centroid:
	(15.92492, 0.78281, 3.72962)
intertia/density WRT centroid:
	a/μ = 43520.33257 m⁵
	b/μ = 194711.28938 m⁵
	c/μ = 191168.76173 m⁵
	a’/μ= 4417.66150 m⁵
	b’/μ=-46343.16662 m⁵
	c’/μ= 11996.20119 m⁵

The numerical testcase (in TetraTestGen::generate) is exact as in the article for inertia (as well as centroid):

43520.3
194711
191169
4417.66
-46343.2
11996.2

*/
//Matrix3r TetrahedronInertiaTensor(const Vector3r v[4]){
Matrix3r TetrahedronInertiaTensor(const vector<Vector3r>& v){
	#define x1 v[0][0]
	#define y1 v[0][1]
	#define z1 v[0][2]
	#define x2 v[1][0]
	#define y2 v[1][1]
	#define z2 v[1][2]
	#define x3 v[2][0]
	#define y3 v[2][1]
	#define z3 v[2][2]
	#define x4 v[3][0]
	#define y4 v[3][1]
	#define z4 v[3][2]

// FIXME - C array
	assert(v.size()==4);

	// Jacobian of transformation to the reference 4hedron
	double detJ=(x2-x1)*(y3-y1)*(z4-z1)+(x3-x1)*(y4-y1)*(z2-z1)+(x4-x1)*(y2-y1)*(z3-z1)
		-(x2-x1)*(y4-y1)*(z3-z1)-(x3-x1)*(y2-y1)*(z4-z1)-(x4-x1)*(y3-y1)*(z2-z1);
	detJ=std::abs(detJ);
	double a=detJ*(y1*y1+y1*y2+y2*y2+y1*y3+y2*y3+
		y3*y3+y1*y4+y2*y4+y3*y4+y4*y4+z1*z1+z1*z2+
		z2*z2+z1*z3+z2*z3+z3*z3+z1*z4+z2*z4+z3*z4+z4*z4)/60.;
	double b=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+
		x1*x4+x2*x4+x3*x4+x4*x4+z1*z1+z1*z2+z2*z2+z1*z3+
		z2*z3+z3*z3+z1*z4+z2*z4+z3*z4+z4*z4)/60.;
	double c=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+x1*x4+
		x2*x4+x3*x4+x4*x4+y1*y1+y1*y2+y2*y2+y1*y3+
		y2*y3+y3*y3+y1*y4+y2*y4+y3*y4+y4*y4)/60.;
	// a' in the article etc.
	double a__=detJ*(2*y1*z1+y2*z1+y3*z1+y4*z1+y1*z2+
		2*y2*z2+y3*z2+y4*z2+y1*z3+y2*z3+2*y3*z3+
		y4*z3+y1*z4+y2*z4+y3*z4+2*y4*z4)/120.;
	double b__=detJ*(2*x1*z1+x2*z1+x3*z1+x4*z1+x1*z2+
		2*x2*z2+x3*z2+x4*z2+x1*z3+x2*z3+2*x3*z3+
		x4*z3+x1*z4+x2*z4+x3*z4+2*x4*z4)/120.;
	double c__=detJ*(2*x1*y1+x2*y1+x3*y1+x4*y1+x1*y2+
		2*x2*y2+x3*y2+x4*y2+x1*y3+x2*y3+2*x3*y3+
		x4*y3+x1*y4+x2*y4+x3*y4+2*x4*y4)/120.;

	Matrix3r ret; ret<<
		a   , -b__, -c__,
		-b__, b   , -a__,
		-c__, -a__, c    ;
	return ret;

	#undef x1
	#undef y1
	#undef z1
	#undef x2
	#undef y2
	#undef z2
	#undef x3
	#undef y3
	#undef z3
	#undef x4
	#undef y4
	#undef z4
}

/*! Caluclate tetrahedron's central inertia tensor */
//Matrix3r TetrahedronCentralInertiaTensor(const Vector3r v[4]){
Matrix3r TetrahedronCentralInertiaTensor(const vector<Vector3r>& v){
	assert(v.size()==4);
	vector<Vector3r> vv;

//	Vector3r vv[4];
	Vector3r cg=(v[0]+v[1]+v[2]+v[3])*.25;
//	vv[0]=v[0]-cg;
//	vv[1]=v[1]-cg;
//	vv[2]=v[2]-cg;
//	vv[3]=v[3]-cg;
	vv.push_back(v[0]-cg);
	vv.push_back(v[1]-cg);
	vv.push_back(v[2]-cg);
	vv.push_back(v[3]-cg);

	return TetrahedronInertiaTensor(vv);
}

/*! Rotate and translate terahedron body so that its local axes are principal, keeping global position by updating vertex positions as well.
 * Updates all body parameters as need.
 *
 * @returns rotation that was done as Wm3::Quaternionr.
 * @todo check for geometrical correctness...
 * */
Quaternionr TetrahedronWithLocalAxesPrincipal(shared_ptr<Body>& tetraBody){
	//const shared_ptr<RigidBodyParameters>& rbp(YADE_PTR_CAST<RigidBodyParameters>(tetraBody->physicalParameters));
	State* rbp=tetraBody->state.get();
	const shared_ptr<Tetra>& tMold(YADE_PTR_DYN_CAST<Tetra>(tetraBody->shape));

	#define v0 tMold->v[0]
	#define v1 tMold->v[1]
	#define v2 tMold->v[2]
	#define v3 tMold->v[3]

	// adjust position (origin to centroid)
	Vector3r cg=(v0+v1+v2+v3)*.25;
	v0-=cg; v1-=cg; v2-=cg; v3-=cg;
	//tMold->v[0]=v0; tMold->v[1]=v1; tMold->v[2]=v2; tMold->v[3]=v3;
	rbp->se3.position+=cg;

	// adjust orientation (local axes to principal axes)
	Matrix3r I_old=TetrahedronInertiaTensor(tMold->v); //≡TetrahedronCentralInertiaTensor
	Matrix3r I_rot(Matrix3r::Zero()), I_new(Matrix3r::Zero()); 
	matrixEigenDecomposition(I_old,I_rot,I_new);
	Quaternionr I_Qrot(I_rot);
	//! @fixme from right to left: rotate by I_rot, then add original rotation (?!!)
	rbp->se3.orientation=rbp->se3.orientation*I_Qrot;
	for(size_t i=0; i<4; i++){
		tMold->v[i]=I_Qrot.conjugate()*tMold->v[i];
	}

	// set inertia
	rbp->inertia=Vector3r(I_new(0,0),I_new(1,1),I_new(2,2));

	return I_Qrot;
	#undef v0
	#undef v1
	#undef v2
	#undef v3
}




Real TetrahedronSignedVolume(const Vector3r v[4]) { return (Vector3r(v[3])-Vector3r(v[0])).dot((Vector3r(v[3])-Vector3r(v[1])).cross(Vector3r(v[3])-Vector3r(v[2])))/6.; }
Real TetrahedronVolume(const Vector3r v[4]) { return std::abs(TetrahedronSignedVolume(v)); }
Real TetrahedronSignedVolume(const vector<Vector3r>& v) { return Vector3r(v[1]-v[0]).dot(Vector3r(v[2]-v[0]).cross(v[3]-v[0]))/6.; }
Real TetrahedronVolume(const vector<Vector3r>& v) { return std::abs(TetrahedronSignedVolume(v)); }
#ifdef YADE_CGAL
Real TetrahedronVolume(const CGAL::Point_3<CGAL::Cartesian<Real> >* v[4]) {
	Vector3r vv[4];
	for (int i=0; i<4; i++) {
		for (int j=0; j<3; j++) {
			vv[i][j] = v[i]->operator[](j);
		}
	}
	return TetrahedronVolume(vv);
}
Real TetrahedronVolume(const CGAL::Point_3<CGAL::Cartesian<Real> > v[4]) {
	Vector3r vv[4];
	for (int i=0; i<4; i++) {
		for (int j=0; j<3; j++) {
			vv[i][j] = v[i][j];
		}
	}
	return TetrahedronVolume(vv);
}
#endif



#ifdef YADE_CGAL
bool Law2_TTetraSimpleGeom_NormPhys_Simple::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	int id1 = contact->getId1(), id2 = contact->getId2();
	TTetraSimpleGeom* geom= static_cast<TTetraSimpleGeom*>(ig.get());
	NormPhys* phys = static_cast<NormPhys*>(ip.get());
	if ( geom->flag == 0 || geom->penetrationVolume <= 0. ) {
		return false;
	}
	Real& un=geom->penetrationVolume;
	phys->normalForce=phys->kn*std::max(un,(Real) 0)*geom->normal;

	State* de1 = Body::byId(id1,scene)->state.get();
	State* de2 = Body::byId(id2,scene)->state.get();
	applyForceAtContactPoint(-phys->normalForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
	// TODO periodic
	return true;
}
#endif

