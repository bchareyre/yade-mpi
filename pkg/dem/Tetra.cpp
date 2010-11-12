// © 2007 Václav Šmilauer <eudoxos@arcig.cz>

#include"Tetra.hpp"

YADE_PLUGIN(/* self-contained in hpp: */ (Tetra) (TTetraGeom) (Bo1_Tetra_Aabb) 
	/* some code in cpp (this file): */ (TetraVolumetricLaw) (Ig2_Tetra_Tetra_TTetraGeom)
	#ifdef YADE_OPENGL
		(Gl1_Tetra)
	#endif	
	);

Tetra::~Tetra(){}
TTetraGeom::~TTetraGeom(){}

#include<yade/core/Interaction.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/State.hpp>
#include<yade/pkg/common/ElastMat.hpp>

#include<yade/pkg/common/Aabb.hpp>

void Bo1_Tetra_Aabb::go(const shared_ptr<Shape>& ig, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){
	Tetra* t=static_cast<Tetra*>(ig.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	Quaternionr invRot=se3.orientation.conjugate();
	Vector3r v_g[4]; for(int i=0; i<4; i++) v_g[i]=se3.orientation*t->v[i]; // vertices in global coordinates
	#define __VOP(op,ix) op(v_g[0][ix],op(v_g[1][ix],op(v_g[2][ix],v_g[3][ix])))
		aabb->min=se3.position+Vector3r(__VOP(std::min,0),__VOP(std::min,1),__VOP(std::min,2));
		aabb->max=se3.position+Vector3r(__VOP(std::max,0),__VOP(std::max,1),__VOP(std::max,2));
	#undef __VOP
}

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
			Real dV=fabs(Vector3r(v1-v0).Dot((v2-v0).Cross(v3-v0)))/6.;
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
	if(tAB.size()==0) { /* LOG_DEBUG("No intersection."); */ return false;} //no intersecting volume

	Real V(0); // volume of intersection (cummulative)
	Vector3r Sg(0,0,0); // static moment of intersection

	Vector3r tt[4]; for(int i=0; i<4; i++) tt[i]=tA.v[i];
	//DEBUG TRWM3VEC(tt[0]); TRWM3VEC(tt[1]); TRWM3VEC(tt[2]); TRWM3VEC(tt[3]); TRVAR1(TetrahedronVolume(tA.v)); TRVAR1(TetrahedronVolume(tt)); TRWM3MAT(TetrahedronInertiaTensor(tA.v));

	for(list<Tetra>::iterator II=tAB.begin(); II!=tAB.end(); II++){
		Real dV=TetrahedronVolume(II->v);
		V+=dV;
		//DEBUG TRVAR1(dV); TRWM3VEC(II->v[0]); TRWM3VEC(II->v[1]); TRWM3VEC(II->v[2]); TRWM3VEC(II->v[3]); LOG_TRACE("====")
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
	//DEBUG
	TRWM3MAT(Ip); TRWM3MAT(I);
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
		/* TRWM3VEC(P); TRWM3VEC(normal); LOG_TRACE("DUMP initial tetrahedron list:"); for(list<Tetra>::iterator I=ret.begin(); I!=ret.end(); I++) (*I).dump(); */
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

	/* TRWM3VEC(P); TRWM3VEC(normal); T.dump(); */

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
		/* LOG_TRACE("NEG(in)="<<NEG<<", POS(out)="<<POS<<", ZER(boundary)="<<ZER); TRWM3VEC(v[0]); TRWM3VEC(v[1]); TRWM3VEC(v[2]); TRWM3VEC(v[3]); */
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
		const shared_ptr<TTetraGeom>& contactGeom(dynamic_pointer_cast<TTetraGeom>(I->geom));
		if(!contactGeom) continue;

		const Body::id_t idA=I->getId1(), idB=I->getId2();
		const shared_ptr<Body>& A=Body::byId(idA), B=Body::byId(idB);
			
		const shared_ptr<ElastMat>& physA(dynamic_pointer_cast<ElastMat>(A->material));
		const shared_ptr<ElastMat>& physB(dynamic_pointer_cast<ElastMat>(B->material));
		

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
		TRWM3VEC(contactGeom->normal);
		TRWM3VEC(F);
		TRWM3VEC((A->state->pos-contactGeom->contactPoint).cross(F));

		scene->forces.addForce (idA,-F);
		scene->forces.addForce (idB, F);
		scene->forces.addTorque(idA,-(A->state->pos-contactGeom->contactPoint).cross(F));
		scene->forces.addTorque(idB, (B->state->pos-contactGeom->contactPoint).cross(F));
	}
}

#ifdef YADE_OPENGL
	#include<yade/lib/opengl/OpenGLWrapper.hpp>
	void Gl1_Tetra::go(const shared_ptr<Shape>& cm, const shared_ptr<State>&,bool,const GLViewInfo&)
	{
		glMaterialv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,Vector3r(cm->color[0],cm->color[1],cm->color[2]));
		glColor3v(cm->color);
		Tetra* t=static_cast<Tetra*>(cm.get());
		if (0) { // wireframe, as for Tetrahedron
			glDisable(GL_LIGHTING);
			glBegin(GL_LINES);
				#define __ONEWIRE(a,b) glVertex3v(t->v[a]);glVertex3v(t->v[b])
					__ONEWIRE(0,1);__ONEWIRE(0,2);__ONEWIRE(0,3);__ONEWIRE(1,2);__ONEWIRE(1,3);__ONEWIRE(2,3);
				#undef __ONEWIRE
			glEnd();
		}
		else
		{
			Vector3r center = (t->v[0]+t->v[1]+t->v[2]+t->v[3])*.25, faceCenter, n;
			glDisable(GL_CULL_FACE); glEnable(GL_LIGHTING);
			glBegin(GL_TRIANGLES);
				#define __ONEFACE(a,b,c) n=(t->v[b]-t->v[a]).cross(t->v[c]-t->v[a]); n.normalize(); faceCenter=(t->v[a]+t->v[b]+t->v[c])/3.; if((faceCenter-center).dot(n)<0)n=-n; glNormal3v(n); glVertex3v(t->v[a]); glVertex3v(t->v[b]); glVertex3v(t->v[c]);
					__ONEFACE(3,0,1);
					__ONEFACE(0,1,2);
					__ONEFACE(1,2,3);
					__ONEFACE(2,3,0);
				#undef __ONEFACE
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
	detJ=fabs(detJ);
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
	const shared_ptr<Tetra>& tMold(dynamic_pointer_cast<Tetra>(tetraBody->shape));

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


Real TetrahedronVolume(const Vector3r v[4]) { return fabs((Vector3r(v[3])-Vector3r(v[0])).dot((Vector3r(v[3])-Vector3r(v[1])).cross(Vector3r(v[3])-Vector3r(v[2]))))/6.; }
Real TetrahedronVolume(const vector<Vector3r>& v) { return fabs(Vector3r(v[1]-v[0]).dot(Vector3r(v[2]-v[0]).cross(v[3]-v[0])))/6.; }

