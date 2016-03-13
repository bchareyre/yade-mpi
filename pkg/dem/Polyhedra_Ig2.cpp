// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

#include "Polyhedra_Ig2.hpp"

YADE_PLUGIN(/* self-contained in hpp: */ (Ig2_Polyhedra_Polyhedra_PolyhedraGeom) (Ig2_Wall_Polyhedra_PolyhedraGeom) (Ig2_Facet_Polyhedra_PolyhedraGeom) (Ig2_Sphere_Polyhedra_ScGeom) );

//**********************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedras. */

bool Ig2_Polyhedra_Polyhedra_PolyhedraGeom::go(
		const shared_ptr<Shape>& shape1,
		const shared_ptr<Shape>& shape2,
		const State& state1,
		const State& state2,
		const Vector3r& shift2,
		const bool& force,
		const shared_ptr<Interaction>& interaction) {
	//get polyhedras
	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;
	Polyhedra* A = static_cast<Polyhedra*>(shape1.get());
	Polyhedra* B = static_cast<Polyhedra*>(shape2.get());

	bool isNew = !interaction->geom;

	//move and rotate 1st the CGAL structure Polyhedron
	Matrix3r rot_mat = (se31.orientation).toRotationMatrix();
	Vector3r trans_vec = se31.position;
	
	Transformation t_rot_trans(
		rot_mat(0,0), rot_mat(0,1), rot_mat(0,2), trans_vec[0],
		rot_mat(1,0), rot_mat(1,1), rot_mat(1,2), trans_vec[1],
		rot_mat(2,0), rot_mat(2,1), rot_mat(2,2), trans_vec[2], 1.
	);
	
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);
	
	std::transform( PA.facets_begin(), PA.facets_end(), PA.planes_begin(),Plane_equation());	
	
	//move and rotate 2nd the CGAL structure Polyhedron
	rot_mat = (se32.orientation).toRotationMatrix();
	trans_vec = se32.position + shift2;
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
	Int = Polyhedron_Polyhedron_intersection(PA,PB,ToCGALPoint(bang->contactPoint),ToCGALPoint(se31.position),ToCGALPoint(se32.position+shift2), bang->sep_plane);	
	
	//volume and centroid of intersection
	Real volume;
	Vector3r centroid;	
	P_volume_centroid(Int, &volume, &centroid);
	
 	if(isnan(volume) || volume<=1E-25 || volume > min(A->GetVolume(),B->GetVolume())) {
		bang->equivalentPenetrationDepth=0;
		bang->penetrationVolume=min(A->GetVolume(),B->GetVolume());
		bang->normal = (A->GetVolume()>B->GetVolume() ? 1 : -1)*(se32.position+shift2-se31.position);
		return true;
	}
	
	if ((!Is_inside_Polyhedron(PA, ToCGALPoint(centroid))) or 
			(!Is_inside_Polyhedron(PB, ToCGALPoint(centroid)))) {
				bang->equivalentPenetrationDepth=0;
				return true;
		}
	
	//find normal direction
	Vector3r normal = FindNormal(Int, PA, PB);
	
	if((se32.position+shift2-centroid).dot(normal)<0) normal*=-1;	
	
	//calculate area of projection of Intersection into the normal plane
	//Real area = CalculateProjectionArea(Int, ToCGALVector(normal));
	//if(isnan(area) || area<=1E-20) {bang->equivalentPenetrationDepth=0; return true;}
	//Real area = volume/1E-8;
	Real area = std::pow(volume,2./3.);
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

bool Ig2_Polyhedra_Polyhedra_PolyhedraGeom::goReverse(
		const shared_ptr<Shape>& shape1,
		const shared_ptr<Shape>& shape2,
		const State& state1,
		const State& state2,
		const Vector3r& shift2,
		const bool& force,
		const shared_ptr<Interaction>& c) {
			return go(shape1,shape2,state2,state1,-shift2,force,c);
		}
//**********************************************************************************
/*! Create Polyhedra (collision geometry) from colliding Polyhedron and Wall. */

bool Ig2_Wall_Polyhedra_PolyhedraGeom::go(const shared_ptr<Shape>& shape1,const shared_ptr<Shape>& shape2,
		const State& state1,const State& state2, const Vector3r& shift2, const bool& force,
		const shared_ptr<Interaction>& interaction) {
	const int& PA(shape1->cast<Wall>().axis); 
	const int& sense(shape1->cast<Wall>().sense);
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;	
	Polyhedra* B = static_cast<Polyhedra*>(shape2.get());

	bool isNew = !interaction->geom;

	//move and rotate also the CGAL structure Polyhedron
	Matrix3r rot_mat = (se32.orientation).toRotationMatrix();
	Vector3r trans_vec = se32.position;
	
	Transformation t_rot_trans(
		rot_mat(0,0),rot_mat(0,1),rot_mat(0,2),trans_vec[0],
		rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],
		rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PB = B->GetPolyhedron();
	std::transform( PB.points_begin(), PB.points_end(), PB.points_begin(), t_rot_trans);
	std::transform( PB.facets_begin(), PB.facets_end(), PB.planes_begin(),Plane_equation());
	
	//move wall
	Vector3r normal = Vector3r(0,0,0);
	if (sense != 0) {
		normal[PA] = sense;
	} else {
		normal[PA] = 1;
		Real dot = normal.dot(se32.position-se31.position);
		if (dot<1) { normal[PA] = -1; }
	}
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
	Real volume;
	Vector3r centroid;
	P_volume_centroid(Int, &volume, &centroid);
	if(isnan(volume) || volume<=1E-25 || volume > B->GetVolume())  {bang->equivalentPenetrationDepth=0; return true;}
	if (!Is_inside_Polyhedron(PB, ToCGALPoint(centroid)))  {bang->equivalentPenetrationDepth=0; return true;}

	//calculate area of projection of Intersection into the normal plane
	Real area = volume/1E-8;
	//Real area = CalculateProjectionArea(Int, CGALnormal);
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

bool Ig2_Facet_Polyhedra_PolyhedraGeom::go(const shared_ptr<Shape>& shape1,const shared_ptr<Shape>& shape2,
																					 const State& state1,const State& state2,
																					 const Vector3r& shift2, const bool& force,
																					 const shared_ptr<Interaction>& interaction){
	
	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;
	Facet*   A = static_cast<Facet*>(shape1.get());	
	Polyhedra* B = static_cast<Polyhedra*>(shape2.get());
	
	bool isNew = !interaction->geom;
	
	//move and rotate 1st the CGAL structure Polyhedron
	Matrix3r rot_mat = (se32.orientation).toRotationMatrix();
	Vector3r trans_vec = se32.position;
	Transformation t_rot_trans(
		rot_mat(0,0),rot_mat(0,1),rot_mat(0,2),trans_vec[0],
		rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],
		rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
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
	
	Real f_area = sqrt(f_normal.squared_length());
	for (int i=3; i<6; i++) v[i] = v[i-3]-f_normal/f_area*0.05*sqrt(f_area); // vertices in global coordinates

	Polyhedron PA;

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
	} else {
		// use data from old interaction
		bang=YADE_PTR_CAST<PolyhedraGeom>(interaction->geom);
		bang->isShearNew = bang->equivalentPenetrationDepth<=0;
	}
	
	//find intersection Polyhedra
	Polyhedron Int;
	Int = Polyhedron_Polyhedron_intersection(PA,PB,ToCGALPoint(bang->contactPoint),ToCGALPoint(se31.position),ToCGALPoint(se32.position), bang->sep_plane);	
	
	//volume and centroid of intersection
	Real volume;
	Vector3r centroid;
	P_volume_centroid(Int, &volume, &centroid);
	if(isnan(volume) || volume<=1E-25 || volume > B->GetVolume()) {bang->equivalentPenetrationDepth=0; return true;}
	if (!Is_inside_Polyhedron(PB, ToCGALPoint(centroid)))  {bang->equivalentPenetrationDepth=0; return true;}
	
	//find normal direction
	Vector3r normal = FindNormal(Int, PA, PB);
	if((se32.position-centroid).dot(normal)<0) normal*=-1;
	
	//calculate area of projection of Intersection into the normal plane
	Real area = volume/1E-8;
	
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
/*! Create Polyhedra (collision geometry) from colliding Polyhedron and Sphere. */

bool Ig2_Sphere_Polyhedra_ScGeom::go(const shared_ptr<Shape>& shape1,const shared_ptr<Shape>& shape2,const State& state1,const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& interaction){

	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;

	Sphere* A = static_cast<Sphere*>(shape1.get());	
	Real radius = A->radius;
	Real r2 = radius*radius;
	Polyhedra* B = static_cast<Polyhedra*>(shape2.get());

	bool isNew = !interaction->geom;
	shared_ptr<ScGeom> geom;

	// use polyhedron surface triangulation
	const std::vector<int> faceTri = B->GetSurfaceTriangulation();
	// get vertices in global coordinate system
	std::vector<Vector3r> pts(B->v);
	for (unsigned int i=0; i<pts.size(); i++) pts[i] = se32.position + se32.orientation*pts[i];

	//******************************************************************************
	// find the closest point of polyhedron to the sphere center, see following paper for notation
	// http://automatica.dei.unipd.it/public/Schenato/PSC/2010_2011/gruppo4-Building_termo_identification/IdentificazioneTermodinamica20062007/eggpsc/Simulatore%20termico/Generazione%20Mesh/1995%20-%203D%20Distance%20from%20a%20Point%20to%20a%20Triangle.pdf
	bool isInside = true; // if center is inside polyhedron
	const Vector3r& p0 = se31.position;
	const Vector3r& center = p0;
	Vector3r closest; // closest point to be found
	Real dst2min = DBL_MAX; // minimal squared distance (large number initially)
	// auxiliary value
	Vector3r p1,p2,p3,e1,e2,e3,e,n,p0a,p10,p20,p30,v1,v2,v3,pa,pb,r,p0aa,pp,ppp;
  pp = Vector3r::Zero();
	Real dst,t,en,o1,o2,o3,dst2,edst2,edst2min;
	PointTriangleRelation rel=none,relTemp=none,relTemp2;
	for (unsigned int i=0; i<faceTri.size()/3; i++) { // iterate over all triangles...
		// triangle vertices
		p1 = pts[faceTri[3*i+0]];
		p2 = pts[faceTri[3*i+1]];
		p3 = pts[faceTri[3*i+2]];
		// triangle edges vectors
		e1 = p2-p1;
		e2 = p3-p2;
		e3 = p1-p3;
		n = (e1.cross(-e3)).normalized(); // tirangle outer normal
		dst = (p0-p1).dot(n); // oriented distance of p0 to triangle plane
		if (dst>0) isInside = false; // p0 lies in positive halfspace of triangle, cannot be inside
		p0a = p0 - dst*n; // p0 projected to triangle plane
		p10 = p0a-p1;
		p20 = p0a-p2;
		p30 = p0a-p3;
		// inside/outside with respect to individual edges
		o1 = (p10.cross(p20)).dot(n);
		o2 = (p20.cross(p30)).dot(n);
		o3 = (p30.cross(p10)).dot(n);
		if (o1>0 && o2>0 && o3>0) { // p0a is inside triangle
			pp = p0a;
			relTemp = inside;
		} else {
			edst2min = DBL_MAX;
			for (int j=0; j<3; j++) { // iterate over 3 edges end find the closest point of them
				pa = j==0? p1 : (j==1? p2 : p3);
				pb = j==0? p2 : (j==1? p3 : p1);
				r = (((pb-p0a).cross(pa-p0a)).cross(pb-pa)).normalized();
				p0aa = p0a + (pa-p0a).dot(r)*r; // projection of p0a on the edge
				e = pb-pa;
				en = e.norm();
				e /= en;
				t = (p0aa-pa).dot(e)/en; // parameter of edge line
				if      (t<0.) { ppp = pa; relTemp2=vertex; }
				else if (t>1.) { ppp = pb; relTemp2=vertex; }
				else { ppp = p0aa; relTemp2=edge; }
				edst2 = (ppp-p0).squaredNorm();
				if (edst2 < edst2min) {
					edst2min = edst2;
					pp = ppp;
					relTemp = relTemp2;
				}
			}
		}
		dst2 = (pp-p0).squaredNorm();
		if (dst2 < dst2min) { // dst2 is now the minimal squared distance and pp is now the closest found point so far
			dst2min = dst2;
			closest = pp;
			rel = relTemp;
		}
	}
	//******************************************************************************

	if (isNew && !(isInside || dst2min<=r2) ) return false;

	if (isNew) {
		geom = shared_ptr<ScGeom>(new ScGeom());
		geom->radius1 = geom->radius2 = radius;
		interaction->geom = geom;
	} else {
		geom = YADE_PTR_CAST<ScGeom>(interaction->geom);
	}

	Real penetrationDepth;
	Vector3r normal,contactPoint;
	if (isInside) { // some artificial tricks for unlikely case
		normal = se32.position - center;
		dst = normal.norm();
		penetrationDepth = radius;
		normal /= dst;
		contactPoint = center + normal*(.5*radius);
	} else { 
		if (rel==none) { LOG_FATAL("TODO"); }
		Real coeff = rel==edge? edgeCoeff : (rel==vertex? vertexCoeff : 1.0);
		normal = closest - center;
		dst = normal.norm();
		normal /= dst;
		penetrationDepth = radius - dst;
		contactPoint = center + normal*(radius-.5*penetrationDepth);
		penetrationDepth *= coeff;
	}
	geom->normal = normal;
	geom->penetrationDepth = penetrationDepth;
	geom->contactPoint = contactPoint;

	geom->precompute(state1,state2,scene,interaction,normal,isNew,shift2,false/*avoidGranularRatcheting only for sphere-sphere*/);

	return true;
}

#endif // YADE_CGAL
