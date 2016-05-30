// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL
// NDEBUG causes crashes in CGAL sometimes. Anton
#ifdef NDEBUG
	#undef NDEBUG
#endif

#include "Polyhedra.hpp"

//EMPRIRICAL CONSTANTS - ADJUST IF SEGMENTATION FAULT OCCUR, IT IS A PROBLEM OF CGAL. THESE ARE USED TO CHECK CGAL IMPUTS
//DISTANCE_LIMIT controls numerical issues in calculating intersection. It should be small enough to neglect only extremely
//small overlaps, but large enough to prevent errors during computation of convex hull
const Real DISTANCE_LIMIT = 2E-11;
//MERGE_PLANES_LIMIT - if two facets of two intersecting polyhedron differ less, then they are treated ose one only
const Real MERGE_PLANES_LIMIT = 1E-18; //18
//SIMPLIFY_LIMIT - if two facets of one polyhedron differ less, then they are joint into one facet
const Real SIMPLIFY_LIMIT = 1E-19; //19
//FIND_NORMAL_LIMIT - to determine which facet of intersection belongs to which polyhedron
const Real FIND_NORMAL_LIMIT = 1E-40;
//SPLITTER_GAP - creates gap between splitted polyhedrons
const Real SPLITTER_GAP = 1E-8;


//**********************************************************************************
//return volume and centroid of polyhedron

bool P_volume_centroid(Polyhedron P, Real * volume, Vector3r * centroid){
	Vector3r basepoint = FromCGALPoint(P.vertices_begin()->point()); 
	Vector3r A,B,C,D; 
	(*volume) = 0;
	Real vtet;
	(*centroid) = Vector3r(0.,0.,0.);
	
	//compute centroid and volume
	for (Polyhedron::Facet_iterator fIter = P.facets_begin(); fIter != P.facets_end(); fIter++){
		Polyhedron::Halfedge_around_facet_circulator hfc0;
		hfc0 = fIter->facet_begin();
		int n = fIter->facet_degree();	
		A = FromCGALPoint(hfc0->vertex()->point());
		C = FromCGALPoint(hfc0->next()->vertex()->point());
		for (int i=2; i<n; i++){
			++hfc0;
			B = C;
			C = FromCGALPoint(hfc0->next()->vertex()->point());
 			vtet = std::abs((basepoint-C).dot((A-C).cross(B-C)))/6.;
			*volume += vtet;
			*centroid = *centroid + (basepoint+A+B+C) / 4. * vtet;
		}
	}
	*centroid = *centroid/(*volume);
	return true;
}

//**********************************************************************************
//STOLEN FROM TETRA body of Vaclav Smilauer

/*! Calculates tetrahedron inertia relative to the origin (0,0,0), with unit density (scales linearly).
* See article F. Tonon, "Explicit Exact Formulas for the 3-D Tetrahedron Inertia Tensor in Terms of its Vertex Coordinates",
* http://docsdrive.com/pdfs/sciencepublications/jmssp/2005/8-11.pdf
* [Tonon2005]
*/

//Centroid MUST be [0,0,0]
Matrix3r TetraInertiaTensor(Vector3r av,Vector3r bv,Vector3r cv,Vector3r dv){
	const auto x1 = av[0];
	const auto y1 = av[1];
	const auto z1 = av[2];
	const auto x2 = bv[0];
	const auto y2 = bv[1];
	const auto z2 = bv[2];
	const auto x3 = cv[0];
	const auto y3 = cv[1];
	const auto z3 = cv[2];
	const auto x4 = dv[0];
	const auto y4 = dv[1];
	const auto z4 = dv[2];

	// Jacobian of transformation to the reference 4hedron
	Real detJ=(x2-x1)*(y3-y1)*(z4-z1)+
						(x3-x1)*(y4-y1)*(z2-z1)+
						(x4-x1)*(y2-y1)*(z3-z1)
						-(x2-x1)*(y4-y1)*(z3-z1)
						-(x3-x1)*(y2-y1)*(z4-z1)
						-(x4-x1)*(y3-y1)*(z2-z1);
	detJ=std::abs(detJ);
	const Real a=detJ*(y1*y1+y1*y2+y2*y2+y1*y3+y2*y3+
		y3*y3+y1*y4+y2*y4+y3*y4+y4*y4+z1*z1+z1*z2+
		z2*z2+z1*z3+z2*z3+z3*z3+z1*z4+z2*z4+z3*z4+z4*z4)/60.;
	const Real b=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+
		x1*x4+x2*x4+x3*x4+x4*x4+z1*z1+z1*z2+z2*z2+z1*z3+
		z2*z3+z3*z3+z1*z4+z2*z4+z3*z4+z4*z4)/60.;
	const Real c=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+x1*x4+
		x2*x4+x3*x4+x4*x4+y1*y1+y1*y2+y2*y2+y1*y3+
		y2*y3+y3*y3+y1*y4+y2*y4+y3*y4+y4*y4)/60.;
	// a' in the article etc.
	const Real a__=detJ*(2*y1*z1+y2*z1+y3*z1+y4*z1+y1*z2+
		2*y2*z2+y3*z2+y4*z2+y1*z3+y2*z3+2*y3*z3+
		y4*z3+y1*z4+y2*z4+y3*z4+2*y4*z4)/120.;
	const Real b__=detJ*(2*x1*z1+x2*z1+x3*z1+x4*z1+x1*z2+
		2*x2*z2+x3*z2+x4*z2+x1*z3+x2*z3+2*x3*z3+
		x4*z3+x1*z4+x2*z4+x3*z4+2*x4*z4)/120.;
	const Real c__=detJ*(2*x1*y1+x2*y1+x3*y1+x4*y1+x1*y2+
		2*x2*y2+x3*y2+x4*y2+x1*y3+x2*y3+2*x3*y3+
		x4*y3+x1*y4+x2*y4+x3*y4+2*x4*y4)/120.;

	Matrix3r ret; ret<<
		a   , -c__, -b__,
		-c__, b   , -a__,
		-b__, -a__, c   ;
	return ret;
}

//**********************************************************************************
//distace of point from a plane (squared) with sign
Real Oriented_squared_distance(Plane P, CGALpoint x){
	Real h = P.a()*x.x()+P.b()*x.y()+P.c()*x.z()+P.d();
	return ((h>0.)-(h<0.))*pow(h,2)/(CGALvector(P.a(),P.b(),P.c())).squared_length();
}

//**********************************************************************************
// test if point is inside polyhedra in strong sence, i.e. boundary location is not enough
bool Is_inside_Polyhedron(Polyhedron P, CGALpoint inside){
	Polyhedron::Plane_iterator pi;	
	for(pi=P.planes_begin(); pi!=P.planes_end(); ++pi){
		if( ! pi->has_on_negative_side(inside)) return false;
	}
	return true;
}

//**********************************************************************************
// test if point is inside polyhedra not closer than lim to its boundary
bool Is_inside_Polyhedron(Polyhedron P, CGALpoint inside, Real lim){
	Polyhedron::Plane_iterator pi;	
	lim = pow(lim,2);
	for(pi=P.planes_begin(); pi!=P.planes_end(); ++pi){
		if(Oriented_squared_distance(*pi, inside)>-lim) return false;
	}
	return true;
}

//**********************************************************************************
//test if two polyhedron intersect
bool do_intersect(Polyhedron A, Polyhedron B){
	std::vector<int> sep_plane;
	sep_plane.assign(3,0);
	return do_intersect(A, B, sep_plane);
}

//**********************************************************************************
//test if two polyhedron intersect based on previous data
bool do_intersect(Polyhedron A, Polyhedron B, std::vector<int> &sep_plane){
	bool found;
	//check previous separation plane
	switch (sep_plane[0]){
		case 1://separation plane was previously determined as sep_plane[2]-th plane of A polyhedron 
			{
				if(unlikely((unsigned) sep_plane[2]>=A.size_of_facets())) break;
				Polyhedron::Facet_iterator fIter = A.facets_begin(); 					
				for (int i=0; i<sep_plane[2]; i++) ++fIter;
				found = true;
				for (Polyhedron::Vertex_iterator vIter = B.vertices_begin(); vIter != B.vertices_end(); ++vIter){
					if (! fIter->plane().has_on_positive_side(vIter->point())) {found = false; break;};	
				}  
			if(found) return false;			
			}			
			break;
		case 2://separation plane was previously determined as sep_plane[2]-th plane of B polyhedron 
			{
				if(unlikely((unsigned) sep_plane[2]>=B.size_of_facets())) break; 
				Polyhedron::Facet_iterator fIter = B.facets_begin(); 		
				for (int i=0; i<sep_plane[2]; i++) ++fIter;
				found = true;
				for (Polyhedron::Vertex_iterator vIter = A.vertices_begin(); vIter != A.vertices_end(); ++vIter){
					if (! fIter->plane().has_on_positive_side(vIter->point())) {found = false; break;};	
				}  
			if(found) return false;
			}
			break;
		case 3://separation plane was previously given by sep_plane[1]-th and sep_plane[2]-th edge of A & B polyhedrons
			{
				if(unlikely((unsigned) sep_plane[1]>=A.size_of_halfedges()/2)) break; 
				if(unlikely((unsigned) sep_plane[2]>=B.size_of_halfedges()/2)) break;  	
				Polyhedron::Edge_iterator eIter1 = A.edges_begin(); 
				Polyhedron::Edge_iterator eIter2 = B.edges_begin();
				for (int i=0; i<sep_plane[1]; i++) ++eIter1;
				for (int i=0; i<sep_plane[2]; i++) ++eIter2;
				found = true;
				Plane X(eIter1->vertex()->point(),CGAL::cross_product(
								(eIter1->vertex()->point() - eIter1->opposite()->vertex()->point()),
								(eIter2->vertex()->point() - eIter2->opposite()->vertex()->point())));
				if (!X.has_on_positive_side(B.vertices_begin()->point())) X = X.opposite();

				Real lim = pow(DISTANCE_LIMIT,2);
				for (Polyhedron::Vertex_iterator vIter = A.vertices_begin(); vIter != A.vertices_end(); ++vIter){
					if (Oriented_squared_distance(X, vIter->point())>lim) {found = false; break;};	
				}				
				for (Polyhedron::Vertex_iterator vIter = B.vertices_begin(); vIter != B.vertices_end(); ++vIter){
					if (! X.has_on_positive_side(vIter->point())) {found = false; break;};	
				}
			if(found) return false;
			}
			break;
	}

	//regular test with no previous information about separating plane
	//test all planes from A
	int i = 0;
	for (Polyhedron::Facet_iterator fIter = A.facets_begin(); fIter != A.facets_end(); fIter++, i++){
		found = true;
		for (Polyhedron::Vertex_iterator vIter = B.vertices_begin(); vIter != B.vertices_end(); ++vIter){
			if (! fIter->plane().has_on_positive_side(vIter->point())) {found = false; break;};
		}  
		if(found) {sep_plane[0] = 1; sep_plane[1] = 1; sep_plane[2] = i; return false;}
	}
	//test all planes from B
	i = 0;
	for (Polyhedron::Facet_iterator fIter = B.facets_begin(); fIter != B.facets_end(); fIter++, i++){
		found = true;
		for (Polyhedron::Vertex_iterator vIter = A.vertices_begin(); vIter != A.vertices_end(); ++vIter){
			if (! fIter->plane().has_on_positive_side(vIter->point())) {found = false; break;};	
		}  
		if(found) {sep_plane[0] = 2; sep_plane[1] = 2; sep_plane[2] = i; return false;}
	}
	//test all pairs of edges from A & B
	Plane X;
	CGALvector vA;
	Real lim = pow(DISTANCE_LIMIT,2);
	i = 0;
	for (Polyhedron::Edge_iterator eIter1 = A.edges_begin(); eIter1 != A.edges_end(); ++eIter1, i++){
		vA = eIter1->vertex()->point() - eIter1->opposite()->vertex()->point();
		int j = 0;
		for (Polyhedron::Edge_iterator eIter2 = B.edges_begin(); eIter2 != B.edges_end(); ++eIter2, j++){
			found = true;
			X = Plane(eIter1->vertex()->point(),CGAL::cross_product(vA,
				(eIter2->vertex()->point() - eIter2->opposite()->vertex()->point())));
			if (!X.has_on_positive_side(B.vertices_begin()->point())) X = X.opposite();
			for (Polyhedron::Vertex_iterator vIter = A.vertices_begin(); vIter != A.vertices_end(); ++vIter){
				if (Oriented_squared_distance(X, vIter->point())>lim) {found = false; break;};
			}
			for (Polyhedron::Vertex_iterator vIter = B.vertices_begin(); vIter != B.vertices_end(); ++vIter){
				if (! X.has_on_positive_side(vIter->point())) {found = false; break;};
			}
			if(found) {
				sep_plane[0] = 3;
				sep_plane[1] = i;
				sep_plane[2] = j;
				return false;
			}
		}
	}
	
	sep_plane[0] = 0;
	return true;
}

//**********************************************************************************
//norm of difference between two planes
Real PlaneDifference(const Plane &a, const Plane &b){
	Real la = sqrt(pow(a.a(),2) + pow(a.b(),2) + pow(a.c(),2) + pow(a.d(),2)); 
	Real lb = sqrt(pow(b.a(),2) + pow(b.b(),2) + pow(b.c(),2) + pow(b.d(),2)); 
	return pow(a.a()/la-b.a()/lb,2) + pow(a.b()/la-b.b()/lb,2) + pow(a.c()/la-b.c()/lb,2) + pow(a.d()/la-b.d()/lb,2);

	//in case we do not care of the orientation	
	//return min(pow(a.a()/la-b.a()/lb,2) + pow(a.b()/la-b.b()/lb,2) + pow(a.c()/la-b.c()/lb,2) + pow(a.d()/la-b.d()/lb,2),pow(a.a()/la+b.a()/lb,2) + pow(a.b()/la+b.b()/lb,2) + pow(a.c()/la+b.c()/lb,2) + pow(a.d()/la+b.d()/lb,2));
}

//**********************************************************************************
//connect triagular facets if possible
Polyhedron Simplify(Polyhedron P, Real limit){
	bool elimination = true;
	while(elimination){
		elimination = false;
		for (Polyhedron::Edge_iterator hei = P.edges_begin(); hei!=P.edges_end(); ++hei){			
			if (PlaneDifference(hei->facet()->plane(),hei->opposite()->facet()->plane()) < limit){			
				if (hei->vertex()->vertex_degree() < 3) hei = P.erase_center_vertex(hei); 
				else if(hei->opposite()->vertex()->vertex_degree() < 3) hei = P.erase_center_vertex(hei->opposite());
				else hei = P.join_facet(hei);
				elimination = true;
				break;
			}
		}
	}
	if (P.size_of_facets() < 4) P.clear();
	return P;
}

//**********************************************************************************
//list of facets + edges
void PrintPolyhedron2File(Polyhedron P,FILE* X){
	Vector3r A,B,C;
	fprintf(X,"*** faces ***\n");
 	for (Polyhedron::Facet_iterator fIter = P.facets_begin(); fIter!=P.facets_end(); ++fIter){
		Polyhedron::Halfedge_around_facet_circulator hfc0;
		hfc0 = fIter->facet_begin();
		int n = fIter->facet_degree();	
		A = FromCGALPoint(hfc0->vertex()->point());
		C = FromCGALPoint(hfc0->next()->vertex()->point());
		for (int i=2; i<n; i++){
			++hfc0;
			B = C;
			C = FromCGALPoint(hfc0->next()->vertex()->point());
			fprintf(X,"%e\t%e\t%e\t%e\t%e\t%e\t%e\t%e\t%e\n",A[0],A[1],A[2],B[0],B[1],B[2],C[0],C[1],C[2]);
		}
	}
	fprintf(X,"*** edges ***\n");
 	for (Polyhedron::Edge_iterator hei = P.edges_begin(); hei!=P.edges_end(); ++hei){
		fprintf(X,"%e\t%e\t%e\t%e\t%e\t%e\n",
			hei->vertex()->point()[0],
			hei->vertex()->point()[1],
			hei->vertex()->point()[2],
			hei->opposite()->vertex()->point()[0],
			hei->opposite()->vertex()->point()[1],
			hei->opposite()->vertex()->point()[2]);
	}
}

//**********************************************************************************
//list of facets + edges
void PrintPolyhedron(Polyhedron P){
	Vector3r A,B,C;
	cout << "*** faces ***" << endl;
 	for (Polyhedron::Facet_iterator fIter = P.facets_begin(); fIter!=P.facets_end(); ++fIter){
		Polyhedron::Halfedge_around_facet_circulator hfc0;
		hfc0 = fIter->facet_begin();
		int n = fIter->facet_degree();	
		A = FromCGALPoint(hfc0->vertex()->point());
		C = FromCGALPoint(hfc0->next()->vertex()->point());
		for (int i=2; i<n; i++){
			++hfc0;
			B = C;
			C = FromCGALPoint(hfc0->next()->vertex()->point());
 			cout << A << " "<< B << " "<< C << endl; 
		}
	}
	cout << "*** edges ***" << endl;
 	for (Polyhedron::Edge_iterator hei = P.edges_begin(); hei!=P.edges_end(); ++hei){
		cout << hei->vertex()->point() << " " <<  hei->opposite()->vertex()->point() << endl;
	}
}

//**********************************************************************************
//list of facets
void PrintPolyhedronFacets(Polyhedron P){
	Vector3r A,B,C;
 	for (Polyhedron::Facet_iterator fIter = P.facets_begin(); fIter!=P.facets_end(); ++fIter){
		cout << "***" << endl;
		Polyhedron::Halfedge_around_facet_circulator hfc0;
		hfc0 = fIter->facet_begin();
		int n = fIter->facet_degree();	
		for (int i = 0; i<n; ++hfc0, i++){
 			cout << hfc0->vertex()->point() << endl; 
		}
	}
}

//**********************************************************************************
//solve system of 3x3 by Cramers rule
Vector3r SolveLinSys3x3(Matrix3r A, Vector3r y){
	//only system 3x3 by Cramers rule 
	Real det = 
		A(0,0)*A(1,1)*A(2,2)+
		A(0,1)*A(1,2)*A(2,0)+
		A(0,2)*A(1,0)*A(2,1)-
		A(0,2)*A(1,1)*A(2,0)-
		A(0,1)*A(1,0)*A(2,2)-
		A(0,0)*A(1,2)*A(2,1);
	if (det == 0) {
		LOG_WARN("error in linear solver");
		return Vector3r(0,0,0);
	}
	return Vector3r(
		(
		y(0)*A(1,1)*A(2,2)+
		A(0,1)*A(1,2)*y(2)+
		A(0,2)*y(1)*A(2,1)-
		A(0,2)*A(1,1)*y(2)-
		A(0,1)*y(1)*A(2,2)-
		y(0)*A(1,2)*A(2,1)
		)/det,
		(
		A(0,0)*y(1)*A(2,2)+
		y(0)*A(1,2)*A(2,0)+
		A(0,2)*A(1,0)*y(2)-
		A(0,2)*y(1)*A(2,0)-
		y(0)*A(1,0)*A(2,2)-
		A(0,0)*A(1,2)*y(2)
		)/det,
		(
		A(0,0)*A(1,1)*y(2)+
		A(0,1)*y(1)*A(2,0)+
		y(0)*A(1,0)*A(2,1)-
		y(0)*A(1,1)*A(2,0)-
		A(0,1)*A(1,0)*y(2)-
		A(0,0)*y(1)*A(2,1)
		)/det
		);
}

//**********************************************************************************
/*
 * Return convex hull of points 
 * critical point, because CGAL often returnes segmentation fault.
 * The planes must be sufficiently "different". This is, however,
 * checked elswhere by DISTANCE_LIMIT variable.
*/
Polyhedron ConvexHull(vector<CGALpoint> &planes){
	Polyhedron Int;
	for (const auto p : planes) {
		if (std::isnan(p.x()) || std::isnan(p.y()) || std::isnan(p.z())) return Int;
	}
	if (planes.size()>3) CGAL::convex_hull_3(planes.begin(), planes.end(), Int);
	return Int;
}

//**********************************************************************************
//determination of normal direction of intersection

Vector3r FindNormal(Polyhedron Int, Polyhedron PA, Polyhedron PB){
	//determine which plane is from which polyhedra
	Polyhedron::Plane_iterator pi, pj;
	std::transform( Int.facets_begin(), Int.facets_end(), Int.planes_begin(),Plane_equation());
	std::transform( PA.facets_begin(),  PA.facets_end(),  PA.planes_begin(), Plane_equation());
	std::transform( PB.facets_begin(),  PB.facets_end(),  PB.planes_begin(), Plane_equation());
	vector<bool> from_A(Int.size_of_facets());
	vector<Real> minsA(Int.size_of_facets());
	vector<Real> minsB(Int.size_of_facets());
	int i=0;
	Real minA, minB, k;
	for(pi = Int.planes_begin(); pi!=Int.planes_end(); ++pi,i++){
		minA = 1.;
		minB = 1.;
		for(pj=PA.planes_begin(); pj!=PA.planes_end(); ++pj){
			k = PlaneDifference(*pi, *pj);
			if (k<minA) {
				minA = k;
				minsA[i] = minA;
				if (minA<FIND_NORMAL_LIMIT) {
					from_A[i] = true;
					break;
				} //already satisfactory
			}
		}
		if (minA<FIND_NORMAL_LIMIT) continue;
		for(pj=PB.planes_begin(); pj!=PB.planes_end(); ++pj){
			k = PlaneDifference(*pi, *pj);
			
			if (k<minB){
				minB = k;
				minsB[i] = minB;
				if (minB<FIND_NORMAL_LIMIT || minB<minA)  break; //already satisfactory
			}
		}
		from_A[i] = ((minA < minB) ? true : false);
	}
	//check that not all belongs to A and not all belongs to B
	if (*std::min_element(from_A.begin(),from_A.end())==1){
		int loc = std::min_element(minsB.begin(),minsB.end()) - minsB.begin();
		from_A[loc] = false;
	} else if (*std::max_element(from_A.begin(),from_A.end())==0){
		int loc = std::min_element(minsA.begin(),minsA.end()) - minsA.begin();
		from_A[loc] = true;
	}

	//find intersecting segments
	vector<Segment> segments;
	int a,b;

	for (Polyhedron::Edge_iterator hei = Int.edges_begin(); hei!=Int.edges_end(); ++hei){
		a = std::distance(Int.facets_begin(), hei->facet());
		b = std::distance(Int.facets_begin(), hei->opposite()->facet());
		if ((from_A[a] && !from_A[b]) || (from_A[b] && !from_A[a])) {
			segments.push_back(Segment(hei->vertex()->point(),hei->opposite()->vertex()->point()));
		}
	}

	//find normal direction
	Plane fit;
	linear_least_squares_fitting_3(segments.begin(),segments.end(),fit,CGAL::Dimension_tag<1>());
	CGALvector CGALnormal=fit.orthogonal_vector();
	CGALnormal = CGALnormal/sqrt(CGALnormal.squared_length()); 
	// reverse direction if projection of the (contact_point-centroid_of_B) vector onto the normal is negative (i.e. the normal points more towards B)

	return FromCGALVector(CGALnormal);
}

//**********************************************************************************
//prepare data for CGAL convex hull
vector<Plane> MergePlanes(vector<Plane> planes1, vector<Plane> planes2, Real limit){
	vector<Plane> P = planes1;
	bool add;
	for(vector<Plane>::iterator i = planes2.begin(); i!=planes2.end(); ++i){
		add = true;
		for(vector<Plane>::iterator j = planes1.begin(); j!=planes1.end(); ++j){
			if (PlaneDifference(*i,*j) < limit){	
				add = false;
				break;
			}
		}
		if (add) P.push_back(*i);
	}
	return P;
}

//**********************************************************************************
//returnes intersecting polyhedron of polyhedron & plane (possibly empty)
Polyhedron Polyhedron_Plane_intersection(Polyhedron A, Plane B, CGALpoint centroid, CGALpoint X){
	
	Polyhedron Intersection;
	CGALpoint inside;
	vector<Plane> planes1, planes2;
	vector<CGALpoint> dual_planes;
	// test if do intersect, find some intersecting point
	bool intersection_found = false;
	Real lim = pow(DISTANCE_LIMIT,2);
	std::transform( A.facets_begin(), A.facets_end(), A.planes_begin(),Plane_equation());
	// test centroid of previous intersection
	if(Is_inside_Polyhedron(A,X,DISTANCE_LIMIT) && Oriented_squared_distance(B,X)<=-lim) {
		intersection_found = true;
		inside = X;
	// find new point by checking polyhedron vertices that lies on negative side of the plane
	} else {
		for(Polyhedron::Vertex_iterator vIter = A.vertices_begin(); vIter != A.vertices_end() && !intersection_found ; vIter++){
			if(Oriented_squared_distance(B,vIter->point())<=-lim) {
				if (Oriented_squared_distance(B,centroid)<lim/10.){
					inside = vIter->point() + 0.5*CGALvector(vIter->point(),centroid);
				}else{
					CGAL::Object result = CGAL::intersection(Line(vIter->point(),centroid), B);			
					if (const CGALpoint *ipoint = CGAL::object_cast<CGALpoint>(&result)) {
						inside = vIter->point() + 0.5*CGALvector(vIter->point(),*ipoint);
					}else{
						LOG_WARN("Error in line-plane intersection"); 
					}
				}
				if(Is_inside_Polyhedron(A,inside,DISTANCE_LIMIT) && Oriented_squared_distance(B,inside)<=-lim) intersection_found = true;
			}
		}
	}
	//no intersectiong point => no intersection polyhedron
	if(!intersection_found) return Intersection;

	//set the intersection point to origin
	Transformation transl_back(CGAL::TRANSLATION, inside - CGALpoint(0.,0.,0.));
	Transformation transl(CGAL::TRANSLATION, CGALpoint(0.,0.,0.)-inside);

	std::transform( A.points_begin(), A.points_end(), A.points_begin(), transl);
	B = transl.transform(B);

	//dualize plane
	planes1.push_back(B);

	//dualize polyhedron
	std::transform( A.facets_begin(), A.facets_end(), A.planes_begin(),Plane_equation());
	for(Polyhedron::Plane_iterator pi =A.planes_begin(); pi!=A.planes_end(); ++pi) planes2.push_back(*pi);;

	//merge planes
	planes1 = MergePlanes(planes1,planes2, MERGE_PLANES_LIMIT);//MERGE_PLANES_LIMIT);
	for(vector<Plane>::iterator pi =planes1.begin(); pi!= planes1.end(); ++pi) dual_planes.push_back(CGALpoint(-pi->a()/pi->d(),-pi->b()/pi->d(),-pi->c()/pi->d()));

 	//compute convex hull of it
	Intersection = ConvexHull(dual_planes);
	if (Intersection.empty()) return Intersection;

	//simplify
	std::transform( Intersection.facets_begin(), Intersection.facets_end(), Intersection.planes_begin(),Plane_equation());
	Intersection = Simplify(Intersection, SIMPLIFY_LIMIT);
	std::transform( Intersection.facets_begin(), Intersection.facets_end(), Intersection.planes_begin(),Plane_equation());

	//dualize again
	dual_planes.clear();
	for(Polyhedron::Plane_iterator pi =Intersection.planes_begin(); pi!=Intersection.planes_end(); ++pi) dual_planes.push_back(CGALpoint(-pi->a()/pi->d(),-pi->b()/pi->d(),-pi->c()/pi->d()));

 	//compute convex hull of it
	Intersection = ConvexHull(dual_planes);
	if (Intersection.empty()) return Intersection;

	//return to original position
	std::transform( Intersection.points_begin(), Intersection.points_end(), Intersection.points_begin(), transl_back);
	
	if (Intersection.size_of_facets() < 4) Intersection.clear();
	return Intersection;
}

//**********************************************************************************
//returnes intersecting polyhedron of polyhedron & plane defined by direction and point
Polyhedron Polyhedron_Plane_intersection(Polyhedron A, Vector3r direction,  Vector3r plane_point){
	Plane B(ToCGALPoint(plane_point), ToCGALVector(direction)); 
	CGALpoint X = ToCGALPoint(plane_point) - 1E-8*ToCGALVector(direction);
	return Polyhedron_Plane_intersection(A, B, ToCGALPoint(plane_point), X);
}

//**********************************************************************************
//returnes intersecting polyhedron of two polyhedrons (possibly empty)
Polyhedron Polyhedron_Polyhedron_intersection(Polyhedron A, Polyhedron B, CGALpoint X, CGALpoint centroidA,  CGALpoint centroidB, std::vector<int> &sep_plane){

	Polyhedron Intersection;

	vector<Plane> planes1, planes2;
	vector<CGALpoint> dual_planes;
	Polyhedron::Plane_iterator pi;
	CGALpoint inside(0,0,0);
	
	bool intersection_found = false;
	std::transform( A.facets_begin(), A.facets_end(), A.planes_begin(),Plane_equation());
	std::transform( B.facets_begin(), B.facets_end(), B.planes_begin(),Plane_equation());
	Matrix3r Amatrix;
	Vector3r y;
	// test that X is really inside	
	if(Is_inside_Polyhedron(A,X,DISTANCE_LIMIT) && Is_inside_Polyhedron(B,X,DISTANCE_LIMIT)) {
		intersection_found = true;
		inside = X;
	} else {
		if (!do_intersect(A, B, sep_plane)) return Intersection; 
		//some intersection point
		Real dist_S, dist_T;
		Real lim2 = pow(DISTANCE_LIMIT,2);
		CGALvector d1;
		Real factor = sqrt(DISTANCE_LIMIT * 1.5);
		//test vertices A - not needed, edges are enough
		for (Polyhedron::Vertex_iterator vIter = A.vertices_begin(); vIter != A.vertices_end() && !intersection_found; vIter++){
			d1 = centroidA-vIter->point();
			inside = vIter->point() + d1/sqrt(d1.squared_length())*DISTANCE_LIMIT*20.;
			intersection_found = (Is_inside_Polyhedron(A,inside,DISTANCE_LIMIT) && Is_inside_Polyhedron(B,inside,DISTANCE_LIMIT));		
		}
		//test vertices B - necessary
		for (Polyhedron::Vertex_iterator vIter = B.vertices_begin(); vIter != B.vertices_end() && !intersection_found; vIter++){
			d1 = centroidB-vIter->point();
			inside = vIter->point() + d1/sqrt(d1.squared_length())*DISTANCE_LIMIT*20.;
			intersection_found = (Is_inside_Polyhedron(A,inside,DISTANCE_LIMIT) && Is_inside_Polyhedron(B,inside,DISTANCE_LIMIT));
		}	

		//test edges
		for (Polyhedron::Edge_iterator eIter = A.edges_begin(); eIter != A.edges_end() && !intersection_found; eIter++){
			for (Polyhedron::Facet_iterator fIter = B.facets_begin(); fIter != B.facets_end() && !intersection_found; fIter++){
				dist_S = Oriented_squared_distance(fIter->plane(), eIter->vertex()->point());
				dist_T = Oriented_squared_distance(fIter->plane(), eIter->opposite()->vertex()->point());
				if (dist_S*dist_T >= 0 || std::abs(dist_S)<lim2 || std::abs(dist_T)<lim2) continue;
				inside = eIter->vertex()->point() + (eIter->opposite()->vertex()->point()-eIter->vertex()->point())*sqrt(std::abs(dist_S))/(sqrt(std::abs(dist_S))+sqrt(std::abs(dist_T)));
				// the fact that edge intersects the facet (not only its plane) is not explicitely checked, it sufices to check that the resulting point is inside both polyhedras
				Plane p1 = fIter->plane();
				Plane p2 = eIter->facet()->plane();
				Plane p3 = eIter->opposite()->facet()->plane();
				Amatrix << p1.a(),p1.b(),p1.c(),
				     p2.a(),p2.b(),p2.c(),
				     p3.a(),p3.b(),p3.c();
				y = Vector3r(-p1.d()-factor*sqrt(pow(p1.a(),2)+pow(p1.b(),2)+pow(p1.c(),2)),-p2.d()-factor*sqrt(pow(p2.a(),2)+pow(p2.b(),2)+pow(p2.c(),2)),-p3.d()-factor*sqrt(pow(p3.a(),2)+pow(p3.b(),2)+pow(p3.c(),2)));
				inside = ToCGALPoint(SolveLinSys3x3(Amatrix,y));
				intersection_found = (Is_inside_Polyhedron(A,inside,DISTANCE_LIMIT) && Is_inside_Polyhedron (B,inside,DISTANCE_LIMIT));
			}
		}        
	}
	
	//Polyhedrons do not intersect
	if (!intersection_found)  return Intersection;

	//set the intersection point to origin	
	Transformation transl_back(CGAL::TRANSLATION, inside - CGALpoint(0.,0.,0.));
	Transformation transl(CGAL::TRANSLATION, CGALpoint(0.,0.,0.)-inside);

	std::transform( A.points_begin(), A.points_end(), A.points_begin(), transl);
	std::transform( B.points_begin(), B.points_end(), B.points_begin(), transl);

	//dualize polyhedrons
	std::transform( A.facets_begin(), A.facets_end(), A.planes_begin(),Plane_equation());
	std::transform( B.facets_begin(), B.facets_end(), B.planes_begin(),Plane_equation());
	for(Polyhedron::Plane_iterator pi =A.planes_begin(); pi!=A.planes_end(); ++pi) planes1.push_back(*pi);
	for(Polyhedron::Plane_iterator pi =B.planes_begin(); pi!=B.planes_end(); ++pi) planes2.push_back(*pi);


	//merge planes
	planes1 = MergePlanes(planes1,planes2, MERGE_PLANES_LIMIT);//MERGE_PLANES_LIMIT);
	for(vector<Plane>::iterator pi =planes1.begin(); pi!= planes1.end(); ++pi) dual_planes.push_back(CGALpoint(-pi->a()/pi->d(),-pi->b()/pi->d(),-pi->c()/pi->d()));

 	//compute convex hull of it
	Intersection = ConvexHull(dual_planes);
	if (Intersection.empty())  return Intersection;

	//simplify
	std::transform( Intersection.facets_begin(), Intersection.facets_end(), Intersection.planes_begin(),Plane_equation());
	Intersection = Simplify(Intersection, SIMPLIFY_LIMIT);
	std::transform( Intersection.facets_begin(), Intersection.facets_end(), Intersection.planes_begin(),Plane_equation());
	
	//dualize again
	dual_planes.clear();
	for(Polyhedron::Plane_iterator pi =Intersection.planes_begin(); pi!=Intersection.planes_end(); ++pi) {
		const auto pX = -pi->a()/pi->d();
		const auto pY = -pi->b()/pi->d();
		const auto pZ = -pi->c()/pi->d();
		if (std::isnan(pX) || std::isnan(pY) || std::isnan(pZ)) {
			Polyhedron IntersectionEmpty;
			return IntersectionEmpty;
		} else {
			dual_planes.push_back(CGALpoint(-pi->a()/pi->d(),-pi->b()/pi->d(),-pi->c()/pi->d()));
		}
	}
	//compute convex hull of it
	Intersection = ConvexHull(dual_planes);
	if (Intersection.empty())  return Intersection;
	//return to original position
	std::transform( Intersection.points_begin(), Intersection.points_end(), Intersection.points_begin(), transl_back);
	
	if (Intersection.size_of_facets() < 4) Intersection.clear();
	return Intersection;
}

//**********************************************************************************
Vector3r FromCGALPoint(CGALpoint A){
	return Vector3r(A.x(), A.y(), A.z());
}

//**********************************************************************************
Vector3r FromCGALVector(CGALvector A){
	return Vector3r(A.x(), A.y(), A.z());
}

//**********************************************************************************
CGALpoint ToCGALPoint(Vector3r A){
	return CGALpoint(A[0], A[1], A[2]);
}

//**********************************************************************************
CGALvector ToCGALVector(Vector3r A){
	return CGALvector(A[0], A[1], A[2]);
}

//**********************************************************************************
//new polyhedra
shared_ptr<Body> NewPolyhedra(vector<Vector3r> v, shared_ptr<Material> mat){
	shared_ptr<Body> body(new Body);
	body->material=mat;
	body->shape=shared_ptr<Polyhedra>(new Polyhedra());
	Polyhedra* A = static_cast<Polyhedra*>(body->shape.get());	
	A->v = v;
	A->Initialize();
	body->state->pos= A->GetCentroid();
	body->state->mass=body->material->density*A->GetVolume();
	body->state->inertia =A->GetInertia()*body->material->density;
	body->state->ori = A->GetOri();
	body->bound=shared_ptr<Aabb>(new Aabb);
	body->setAspherical(true);
	return body;
}

//**********************************************************************************
//split polyhedra
shared_ptr<Body> SplitPolyhedra(const shared_ptr<Body>& body, const Vector3r direction, const Vector3r point){
	Scene* scene=Omega::instance().getScene().get();
	const Se3r& se3=body->state->se3; 
	Polyhedra* A = static_cast<Polyhedra*>(body->shape.get());
	State* X = static_cast<State*>(body->state.get());

	const Vector3r OrigPos = X->pos;
	const Vector3r OrigVel = X->vel;
	const Vector3r OrigAngVel = X->angVel;

	//move and rotate CGAL structure Polyhedron
	Matrix3r rot_mat = (se3.orientation).toRotationMatrix();
	Vector3r trans_vec = se3.position;
	Transformation t_rot_trans(
		rot_mat(0,0),rot_mat(0,1),rot_mat(0,2),trans_vec[0],
		rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],
		rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);
 
	//calculate first splitted polyhedrons
	Plane B(ToCGALPoint(point-direction*SPLITTER_GAP), ToCGALVector(direction)); 
	Polyhedron S1 = Polyhedron_Plane_intersection(PA, B, ToCGALPoint(se3.position), B.projection(ToCGALPoint(OrigPos)) - 1E-6*ToCGALVector(direction));

	B = Plane(ToCGALPoint(point+direction*SPLITTER_GAP), ToCGALVector((-1.)*direction));
	Polyhedron S2 = Polyhedron_Plane_intersection(PA, B, ToCGALPoint(se3.position), B.projection(ToCGALPoint(OrigPos)) + 1E-6*ToCGALVector(direction));
	//scene->bodies->erase(body->id);

	//replace original polyhedron
	A->Clear();
	for(Polyhedron::Vertex_iterator vi = S1.vertices_begin(); vi !=  S1.vertices_end(); vi++) A->v.push_back(FromCGALPoint(vi->point()));
	A->Initialize();
	X->pos = A->GetCentroid();
	X->ori = A->GetOri();
	X->mass=body->material->density*A->GetVolume();
	X->refPos[0] = X->refPos[0]+1.;
	X->inertia =A->GetInertia()*body->material->density;
	X->vel = OrigVel + OrigAngVel.cross(X->pos-OrigPos);
	X->angVel = OrigAngVel;
	
	//second polyhedron
	vector<Vector3r> v2;
	for(Polyhedron::Vertex_iterator vi = S2.vertices_begin(); vi !=  S2.vertices_end(); vi++) v2.push_back(FromCGALPoint(vi->point()));
	shared_ptr<Body> BP = NewPolyhedra(v2, body->material);
	BP->shape->color = Vector3r(Real(rand())/RAND_MAX,Real(rand())/RAND_MAX,Real(rand())/RAND_MAX);
	scene->bodies->insert(BP);
	//set proper state variables
	BP->state->vel = OrigVel + OrigAngVel.cross(BP->state->pos-OrigPos);
	BP->state->angVel = OrigAngVel;

	return BP;
}

#endif // YADE_CGAL
