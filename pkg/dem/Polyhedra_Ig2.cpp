// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

#include"Polyhedra.hpp"
#include"Polyhedra_Ig2.hpp"

#define _USE_MATH_DEFINES

YADE_PLUGIN(/* self-contained in hpp: */ (Ig2_Polyhedra_Polyhedra_PolyhedraGeom) (Ig2_Wall_Polyhedra_PolyhedraGeom) (Ig2_Facet_Polyhedra_PolyhedraGeom)	);

//**********************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedras. */

bool Ig2_Polyhedra_Polyhedra_PolyhedraGeom::go(const shared_ptr<Shape>& cm1,const shared_ptr<Shape>& cm2,const State& state1,const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& interaction){

	//get polyhedras
	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());		
	Polyhedra* B = static_cast<Polyhedra*>(cm2.get());

	bool isNew = !interaction->geom;

	//move and rotate 1st the CGAL structure Polyhedron
	Matrix3r rot_mat = (se31.orientation).toRotationMatrix();
	Vector3r trans_vec = se31.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);
	std::transform( PA.facets_begin(), PA.facets_end(), PA.planes_begin(),Plane_equation());	


	//move and rotate 2nd the CGAL structure Polyhedron
	rot_mat = (se32.orientation).toRotationMatrix();
	trans_vec = se32.position;
	t_rot_trans = Transformation(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PB = B->GetPolyhedron();
	std::transform( PB.points_begin(), PB.points_end(), PB.points_begin(), t_rot_trans);
	std::transform( PB.facets_begin(), PB.facets_end(), PB.planes_begin(),Plane_equation());

	shared_ptr<PolyhedraGeom> bang;
	if (isNew) {
		// new interaction
		bang=shared_ptr<PolyhedraGeom>(new PolyhedraGeom());
		bang->sep_plane.assign(3,0);
		bang->contactPoint = Vector3r(0,0,0);
		bang->isShearNew = true;
		interaction->geom = bang;
	}else{	
		// use data from old interaction
  		bang=YADE_PTR_CAST<PolyhedraGeom>(interaction->geom);
		bang->isShearNew = bang->equivalentPenetrationDepth<=0;
	}


	//find intersection Polyhedra
	Polyhedron Int;
	Int = Polyhedron_Polyhedron_intersection(PA,PB,ToCGALPoint(bang->contactPoint),ToCGALPoint(se31.position),ToCGALPoint(se32.position), bang->sep_plane);	

	//volume and centroid of intersection
	double volume;
	Vector3r centroid;	
	P_volume_centroid(Int, &volume, &centroid);
 	if(isnan(volume) || volume<=1E-25 || volume > min(A->GetVolume(),B->GetVolume())) {bang->equivalentPenetrationDepth=0; 	return true;}
	if ( (!Is_inside_Polyhedron(PA, ToCGALPoint(centroid))) or (!Is_inside_Polyhedron(PB, ToCGALPoint(centroid))))  {bang->equivalentPenetrationDepth=0; return true;}

	//find normal direction
        Vector3r normal = FindNormal(Int, PA, PB);
	if((se32.position-centroid).dot(normal)<0) normal*=-1;	

	//calculate area of projection of Intersection into the normal plane
	//double area = CalculateProjectionArea(Int, ToCGALVector(normal));
	//if(isnan(area) || area<=1E-20) {bang->equivalentPenetrationDepth=0; return true;}
        double area = volume/1E-8;

	// store calculated stuff in bang; some is redundant
	bang->equivalentCrossSection=area;
	bang->contactPoint=centroid;
	bang->penetrationVolume=volume;
	bang->equivalentPenetrationDepth=volume/area;
	bang->precompute(state1,state2,scene,interaction,normal,bang->isShearNew,shift2);
	bang->normal=normal;
	
	/*
	FILE * fin = fopen("Interactions.dat","a");
	fprintf(fin,"************** IDS %d %d **************\n",interaction->id1, interaction->id2);
	fprintf(fin,"volume\t%e\n",volume);	
	fprintf(fin,"centroid\t%e\t%e\t%e\n",centroid[0],centroid[1],centroid[2]);
	PrintPolyhedron2File(PA,fin);
	PrintPolyhedron2File(PB,fin);
	PrintPolyhedron2File(Int,fin);
	fclose(fin);
	*/

	return true;	
}

//**********************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedron and Wall. */

bool Ig2_Wall_Polyhedra_PolyhedraGeom::go(const shared_ptr<Shape>& cm1,const shared_ptr<Shape>& cm2,const State& state1,const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& interaction){

	const int& PA(cm1->cast<Wall>().axis); 
	const int& sense(cm1->cast<Wall>().sense);
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;	
	Polyhedra* B = static_cast<Polyhedra*>(cm2.get());

	bool isNew = !interaction->geom;

	//move and rotate also the CGAL structure Polyhedron
	Matrix3r rot_mat = (se32.orientation).toRotationMatrix();
	Vector3r trans_vec = se32.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PB = B->GetPolyhedron();
	std::transform( PB.points_begin(), PB.points_end(), PB.points_begin(), t_rot_trans);
	std::transform( PB.facets_begin(), PB.facets_end(), PB.planes_begin(),Plane_equation());
	
	//move wall
	Vector3r normal = Vector3r(0,0,0);
	normal[PA] = sense;
	CGALvector CGALnormal = CGALvector(normal[0],normal[1],normal[2]);
        Plane A = Plane(CGALpoint(se31.position[0],se31.position[1],se31.position[2]),CGALvector(normal[0],normal[1],normal[2]));

	shared_ptr<PolyhedraGeom> bang;
	if (isNew) {
		// new interaction
		bang=shared_ptr<PolyhedraGeom>(new PolyhedraGeom());
		bang->contactPoint = Vector3r(0,0,0);
		bang->isShearNew = true;
		interaction->geom = bang;
	}else{	
		// use data from old interaction
  		bang=YADE_PTR_CAST<PolyhedraGeom>(interaction->geom);
		bang->isShearNew = bang->equivalentPenetrationDepth<=0;
	}

	//find intersection Polyhedra
	Polyhedron Int;
	Int = Polyhedron_Plane_intersection(PB,A,ToCGALPoint(se32.position),ToCGALPoint(bang->contactPoint));

	//volume and centroid of intersection
	double volume;
	Vector3r centroid;	
	P_volume_centroid(Int, &volume, &centroid);
	if(isnan(volume) || volume<=1E-25 || volume > B->GetVolume())  {bang->equivalentPenetrationDepth=0; return true;}
	if (!Is_inside_Polyhedron(PB, ToCGALPoint(centroid)))  {bang->equivalentPenetrationDepth=0; return true;}

	//calculate area of projection of Intersection into the normal plane
        double area = volume/1E-8;
	//double area = CalculateProjectionArea(Int, CGALnormal);
	//if(isnan(area) || area<=1E-20) {bang->equivalentPenetrationDepth=0; return true;}

	// store calculated stuff in bang; some is redundant
	bang->equivalentCrossSection=area;
	bang->contactPoint=centroid;
	bang->penetrationVolume=volume;
	bang->equivalentPenetrationDepth=volume/area;
	bang->precompute(state1,state2,scene,interaction,normal,isNew,shift2);
	bang->normal=FromCGALVector(CGALnormal);

	return true;
}

//**********************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedron and Facet. */

bool Ig2_Facet_Polyhedra_PolyhedraGeom::go(const shared_ptr<Shape>& cm1,const shared_ptr<Shape>& cm2,const State& state1,const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& interaction){


	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;
	Facet*   A = static_cast<Facet*>(cm1.get());	
	Polyhedra* B = static_cast<Polyhedra*>(cm2.get());

	bool isNew = !interaction->geom;

	//move and rotate 1st the CGAL structure Polyhedron
	Matrix3r rot_mat = (se32.orientation).toRotationMatrix();
	Vector3r trans_vec = se32.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PB = B->GetPolyhedron();
	std::transform( PB.points_begin(), PB.points_end(), PB.points_begin(), t_rot_trans);
	std::transform( PB.facets_begin(), PB.facets_end(), PB.planes_begin(),Plane_equation());

	//move and rotate facet
	vector<CGALpoint> v;
	v.resize(6);
	for (int i=0; i<3; i++) v[i] = ToCGALPoint(se31.orientation*A->vertices[i] + se31.position); // vertices in global coordinates

	//determine 
	CGALpoint f_center((v[0].x()+v[1].x()+v[2].x())/3.,(v[0].y()+v[1].y()+v[2].y())/3.,(v[0].z()+v[1].z()+v[2].z())/3.);
	CGALvector f_normal = CGAL::cross_product((v[1]-v[0]),(v[2]-v[0]));

	//chage normal + change order of vertices to get outwarding facet normal for initial tetrahedron
 	if ((ToCGALPoint(se32.position)-f_center)*(f_normal) < 0) {
		f_normal = (-1)*f_normal;
  		CGALpoint help;
		help = v[0];
		v[0]=v[1];
		v[1]=help;	
	}

	double f_area = sqrt(f_normal.squared_length());
	for (int i=3; i<6; i++) v[i] = v[i-3]-f_normal/f_area*0.05*sqrt(f_area); // vertices in global coordinates

	Polyhedron PA;
	//use convex hull	
	//CGAL::convex_hull_3(v.begin(), v.end(), PA);

	//construct polyhedron directly
	Polyhedron::Halfedge_iterator hei = PA.make_tetrahedron(v[4],v[1],v[0],v[2]);
	hei = PA.split_vertex(hei, hei->next()->opposite());
	hei->vertex()->point() = v[3];	
	hei = PA.split_facet(hei->next()->next()->next(),hei->next());
	hei = PA.split_vertex(hei, hei->next_on_vertex()->next_on_vertex());
	hei->vertex()->point() = v[5];		

	shared_ptr<PolyhedraGeom> bang;
	if (isNew) {
		// new interaction
		bang=shared_ptr<PolyhedraGeom>(new PolyhedraGeom());
		bang->sep_plane.assign(3,0);
		bang->contactPoint = Vector3r(0,0,0);
		bang->isShearNew = true;
		interaction->geom = bang;
	}else{	
		// use data from old interaction
  		bang=YADE_PTR_CAST<PolyhedraGeom>(interaction->geom);
		bang->isShearNew = bang->equivalentPenetrationDepth<=0;
	}

	//find intersection Polyhedra
	Polyhedron Int;
	Int = Polyhedron_Polyhedron_intersection(PA,PB,ToCGALPoint(bang->contactPoint),ToCGALPoint(se31.position),ToCGALPoint(se32.position), bang->sep_plane);	

	//volume and centroid of intersection
	double volume;
	Vector3r centroid;	
	P_volume_centroid(Int, &volume, &centroid);
 	if(isnan(volume) || volume<=1E-25 || volume > B->GetVolume()) {bang->equivalentPenetrationDepth=0; return true;}
	if (!Is_inside_Polyhedron(PB, ToCGALPoint(centroid)))  {bang->equivalentPenetrationDepth=0; return true;}

	//find normal direction
	Vector3r normal = FindNormal(Int, PA, PB);
	if((se32.position-centroid).dot(normal)<0) normal*=-1;

	//calculate area of projection of Intersection into the normal plane
        double area = volume/1E-8;
	//double area = CalculateProjectionArea(Int, ToCGALVector(normal));
	//if(isnan(area) || area<=1E-20) {bang->equivalentPenetrationDepth=0; return true;}
		
	// store calculated stuff in bang; some is redundant
	bang->equivalentCrossSection=area;
	bang->contactPoint=centroid;
	bang->penetrationVolume=volume;
	bang->equivalentPenetrationDepth=volume/area;
	bang->precompute(state1,state2,scene,interaction,normal,bang->isShearNew,shift2);
	bang->normal=normal;

	return true;	
}

//**********************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedron and Wall. */

/*
bool Ig2_Sphere_Polyhedra_PolyhedraGeom::go(const shared_ptr<Shape>& cm1,const shared_ptr<Shape>& cm2,const State& state1,const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& interaction){

	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;
	Polyhedra* B = static_cast<Polyhedra*>(cm2.get());
	const Real& r=cm1->cast<Sphere>().radius;

	//cout << "Sphere x Polyhedra" << endl;

	bool isNew = !interaction->geom;

	//move and rotate 1st the CGAL structure Polyhedron
	Matrix3r rot_mat = (se32.orientation).toRotationMatrix();
	Vector3r trans_vec = se32.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PB = B->GetPolyhedron();
	std::transform( PB.points_begin(), PB.points_end(), PB.points_begin(), t_rot_trans);
	std::transform( PB.facets_begin(), PB.facets_end(), PB.planes_begin(),Plane_equation());	
	
	shared_ptr<PolyhedraGeom> bang;
	if (isNew) {
		// new interaction
		bang=shared_ptr<PolyhedraGeom>(new PolyhedraGeom());
		bang->isShearNew = true;
		interaction->geom = bang;
	}else{	
		// use data from old interaction
  		bang=YADE_PTR_CAST<PolyhedraGeom>(interaction->geom);
	}

	//volume and centroid of intersection
	double volume, area;
        CGALvector normalCGAL;
	CGALpoint centroidCGAL=ToCGALPoint(se32.position);
	Sphere_Polyhedron_intersection(PB, r, ToCGALPoint(se31.position), centroidCGAL,  volume, normalCGAL, area);
 	if(isnan(volume) || volume<=1E-25 || volume > B->GetVolume()) {bang->equivalentPenetrationDepth=0; return true;}
	Vector3r centroid = FromCGALPoint(centroidCGAL);
	Vector3r normal = FromCGALVector(normalCGAL);

	

	// store calculated stuff in bang; some is redundant
	bang->equivalentCrossSection=area;
	bang->contactPoint=centroid;
	bang->penetrationVolume=volume;
	bang->equivalentPenetrationDepth=volume/area;
	bang->precompute(state1,state2,scene,interaction,normal,bang->isShearNew,shift2);
	bang->normal=normal;

	return true;	
}
*/

#endif // YADE_CGAL
