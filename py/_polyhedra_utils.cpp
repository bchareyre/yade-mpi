// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL

#include "pkg/dem/Polyhedra.hpp"

#include <core/Scene.hpp>
#include <core/Omega.hpp>
#include <pkg/common/Sphere.hpp>
#include <pkg/common/ElastMat.hpp>
#include <lib/pyutil/doc_opts.hpp>
#include <cmath>

#include <numpy/ndarraytypes.h>

namespace py = boost::python;

//**********************************************************************************
//print polyhedron in basic position
void PrintPolyhedra(const shared_ptr<Shape>& shape){
	Polyhedra* A = static_cast<Polyhedra*>(shape.get());
	Polyhedron PA = A->GetPolyhedron();
	A->Initialize();
	PrintPolyhedron(PA);
}

//**********************************************************************************
//print polyhedron in actual position
void PrintPolyhedraActualPos(const shared_ptr<Shape>& cm1,const State& state1){
	const Se3r& se3=state1.se3; 
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());
	A->Initialize();

	//move and rotate CGAL structure Polyhedron
	Matrix3r rot_mat = (se3.orientation).toRotationMatrix();
	Vector3r trans_vec = se3.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);

	PrintPolyhedron(PA);
}


//**********************************************************************************
//test of polyhedron intersection callable from python shell
bool do_Polyhedras_Intersect(const shared_ptr<Shape>& cm1,const shared_ptr<Shape>& cm2,const State& state1,const State& state2){

	const Se3r& se31=state1.se3; 
	const Se3r& se32=state2.se3;
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());		
	Polyhedra* B = static_cast<Polyhedra*>(cm2.get());

	//move and rotate 1st the CGAL structure Polyhedron
	Matrix3r rot_mat = (se31.orientation).toRotationMatrix();
	Vector3r trans_vec = se31.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);

	//move and rotate 2st the CGAL structure Polyhedron
	rot_mat = (se32.orientation).toRotationMatrix();
	trans_vec = se32.position;
	t_rot_trans = Transformation(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PB = B->GetPolyhedron();
	std::transform( PB.points_begin(), PB.points_end(), PB.points_begin(), t_rot_trans);

	//calculate plane equations
	std::transform( PA.facets_begin(), PA.facets_end(), PA.planes_begin(),Plane_equation());
	std::transform( PB.facets_begin(), PB.facets_end(), PB.planes_begin(),Plane_equation());


	//call test
	return do_intersect(PA,PB);
}

//**********************************************************************************
//determination of critical time step for polyhedrons & spheres (just rough estimation)
Real PWaveTimeStep(){
	const shared_ptr<Scene> _rb=shared_ptr<Scene>();
	shared_ptr<Scene> rb=(_rb?_rb:Omega::instance().getScene());
	Real dt=std::numeric_limits<Real>::infinity();
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || !b->material || !b->shape) continue;
		shared_ptr<Sphere> s=YADE_PTR_DYN_CAST<Sphere>(b->shape);
		shared_ptr<Polyhedra> p=YADE_PTR_DYN_CAST<Polyhedra>(b->shape);
		if(!s && !p) continue;
		if(!p){
			//spheres
			shared_ptr<ElastMat> ebp=YADE_PTR_DYN_CAST<ElastMat>(b->material);
 			if(!ebp) continue;
			Real density=b->state->mass/((4./3.)*Mathr::PI*pow(s->radius,3));
			dt=min(dt,s->radius/sqrt(ebp->young/density));
		}else{
			//polyhedrons
			shared_ptr<PolyhedraMat> ebp=YADE_PTR_DYN_CAST<PolyhedraMat>(b->material);
 			if(!ebp) continue;
			Real density=b->state->mass/p->GetVolume();
			//get equivalent radius and use same equation as for sphere
			Real equi_radius=pow(p->GetVolume()/((4./3.)*Mathr::PI),1./3.);				
			dt=min(dt,equi_radius/sqrt(ebp->young*equi_radius/density));
		}
	}
	if (dt==std::numeric_limits<Real>::infinity()) {
		dt = 1.0;
		LOG_WARN("PWaveTimeStep has not found any suitable spherical or polyhedral body to calculate dt. dt is set to 1.0");
	}
	return dt;
}

//**********************************************************************************
//returns approximate sieve size of polyhedron
Real SieveSize(const shared_ptr<Shape>& cm1){
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());
	
	double phi = M_PI/4.;	
	double x,y;
	double minx = 0, maxx = 0, miny = 0, maxy = 0;
	
	for (vector<Vector3r>::iterator i=A->v.begin(); i!=A->v.end(); ++i){
		x = cos(phi)*(*i)[1]  + sin(phi)*(*i)[2];
		y = -sin(phi)*(*i)[1] + cos(phi)*(*i)[2];
		minx = min(minx,x);
		maxx = max(maxx,x);
		miny = min(miny,y);
		maxy = max(maxy,y);
	}	
	
	return max(maxx-minx, maxy-miny);
}


//**********************************************************************************
//returns approximate size of polyhedron
Vector3r SizeOfPolyhedra(const shared_ptr<Shape>& cm1){
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());
	
	double minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	
	for (vector<Vector3r>::iterator i=A->v.begin(); i!=A->v.end(); ++i){
		minx = min(minx,(*i)[0]);
		maxx = max(maxx,(*i)[0]);
		miny = min(miny,(*i)[1]);
		maxy = max(maxy,(*i)[1]);
		minz = min(minz,(*i)[2]);
		maxz = max(maxz,(*i)[2]);
	}	
	
	return Vector3r(maxx-minx, maxy-miny, maxz-minz);
}

//**********************************************************************************
//save sieve curve points into a file
void SieveCurve(){
	const shared_ptr<Scene> _rb=shared_ptr<Scene>();
	shared_ptr<Scene> rb=(_rb?_rb:Omega::instance().getScene());
	std::vector< std::pair<double,double> > sieve_volume;
	double total_volume = 0;
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || !b->shape) continue;
		shared_ptr<Polyhedra> p=YADE_PTR_DYN_CAST<Polyhedra>(b->shape);
		if(p){
			sieve_volume.push_back(std::pair<double,double>(SieveSize(p),p->GetVolume()));
			total_volume += p->GetVolume();
		}
	}
	
	std::sort(sieve_volume.begin(), sieve_volume.end()) ;
	double cumul_vol = 0;

	ofstream myfile;
  	myfile.open ("sieve_curve.dat");
	for(std::vector< std::pair<double,double> > :: iterator i = sieve_volume.begin(); i != sieve_volume.end(); ++i) {
		cumul_vol += i->second/total_volume;
		myfile << i->first << "\t" << cumul_vol << endl;
	}
  	myfile.close();
}

//**********************************************************************************
//save size of polyhedrons into a file
void SizeRatio(){
	const shared_ptr<Scene> _rb=shared_ptr<Scene>();
	shared_ptr<Scene> rb=(_rb?_rb:Omega::instance().getScene());
	ofstream myfile;
  	myfile.open ("sizes.dat");
	FOREACH(const shared_ptr<Body>& b, *rb->bodies){
		if(!b || !b->shape) continue;
		shared_ptr<Polyhedra> p=YADE_PTR_DYN_CAST<Polyhedra>(b->shape);
		if(p){
			myfile << SizeOfPolyhedra(p) << endl; 
		}
	}
  	myfile.close();
}

//**********************************************************************************
//returns max coordinates
Vector3r MaxCoord(const shared_ptr<Shape>& cm1,const State& state1){
	const Se3r& se3=state1.se3; 
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());

	//move and rotate CGAL structure Polyhedron
	Matrix3r rot_mat = (se3.orientation).toRotationMatrix();
	Vector3r trans_vec = se3.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);
	
	Vector3r maxccord = trans_vec;
	for(Polyhedron::Vertex_iterator vi = PA.vertices_begin(); vi != PA.vertices_end(); ++vi){	
		if (vi->point()[0]>maxccord[0]) maxccord[0]=vi->point()[0];
		if (vi->point()[1]>maxccord[1]) maxccord[1]=vi->point()[1];
		if (vi->point()[2]>maxccord[2]) maxccord[2]=vi->point()[2];
	}
	
	return maxccord;
}

//**********************************************************************************
//returns min coordinates
Vector3r MinCoord(const shared_ptr<Shape>& cm1,const State& state1){
	const Se3r& se3=state1.se3; 
	Polyhedra* A = static_cast<Polyhedra*>(cm1.get());

	//move and rotate CGAL structure Polyhedron
	Matrix3r rot_mat = (se3.orientation).toRotationMatrix();
	Vector3r trans_vec = se3.position;
	Transformation t_rot_trans(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2), trans_vec[0],rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),trans_vec[1],rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),trans_vec[2],1.);
	Polyhedron PA = A->GetPolyhedron();
	std::transform( PA.points_begin(), PA.points_end(), PA.points_begin(), t_rot_trans);
	
	Vector3r minccord = trans_vec;
	for(Polyhedron::Vertex_iterator vi = PA.vertices_begin(); vi != PA.vertices_end(); ++vi){	
		if (vi->point()[0]<minccord[0]) minccord[0]=vi->point()[0];
		if (vi->point()[1]<minccord[1]) minccord[1]=vi->point()[1];
		if (vi->point()[2]<minccord[2]) minccord[2]=vi->point()[2];
	}
	
	return minccord;
}


//**********************************************************************************
//generate "packing" of non-overlapping polyhedrons
vector<Vector3r> fillBox_cpp(Vector3r minCoord, Vector3r maxCoord, Vector3r sizemin, Vector3r sizemax, Vector3r ratio, int seed, shared_ptr<Material> mat){	
	vector<Vector3r> v;
	Polyhedra trialP;
	Polyhedron trial, trial_moved;
	srand(seed);
	int it = 0;
	vector<Polyhedron> polyhedrons;
	vector<vector<Vector3r> > vv;
	Vector3r position;
	bool intersection;
	int count = 0;
	
	bool fixed_ratio = 0;
	if (ratio[0] > 0 && ratio[1] > 0 && ratio[2]>0){
		fixed_ratio = 1;
		sizemax[0] = min(min(sizemax[0]/ratio[0],  sizemax[1]/ratio[1]),  sizemax[2]/ratio[2]);
		sizemin[0] = max(max(sizemin[0]/ratio[0],  sizemin[1]/ratio[1]),  sizemin[2]/ratio[2]);
	}

	//it - number of trials to make packing possibly more/less dense
	Vector3r random_size;
	while (it<1000){
		it = it+1;
		if (it == 1){	
			trialP.Clear();		       
			trialP.seed = rand();
			if(fixed_ratio)  trialP.size = (rand()*(sizemax[0]-sizemin[0])/RAND_MAX + sizemin[0])*ratio;			
			else  trialP.size = Vector3r(rand()*(sizemax[0]-sizemin[0]),rand()*(sizemax[1]-sizemin[1]),rand()*(sizemax[2]-sizemin[2]))/RAND_MAX + sizemin;			
			trialP.Initialize();
			trial = trialP.GetPolyhedron();	
			Matrix3r rot_mat = (trialP.GetOri()).toRotationMatrix();
			Transformation t_rot(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2),rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),1.);	
			std::transform( trial.points_begin(), trial.points_end(), trial.points_begin(), t_rot);			
		}		
		position = Vector3r(rand()*(maxCoord[0]-minCoord[0]),rand()*(maxCoord[1]-minCoord[1]),rand()*(maxCoord[2]-minCoord[2]))/RAND_MAX + minCoord;

		//move CGAL structure Polyhedron
		Transformation transl(CGAL::TRANSLATION, ToCGALVector(position));
		trial_moved = trial;		
		std::transform( trial_moved.points_begin(), trial_moved.points_end(), trial_moved.points_begin(), transl);
		//calculate plane equations
		std::transform( trial_moved.facets_begin(), trial_moved.facets_end(), trial_moved.planes_begin(),Plane_equation());	

		intersection = false;	
		//call test with boundary
		for(Polyhedron::Vertex_iterator vi = trial_moved.vertices_begin(); (vi !=  trial_moved.vertices_end()) && (!intersection); vi++){
			intersection = (vi->point().x()<minCoord[0]) || (vi->point().x()>maxCoord[0]) || (vi->point().y()<minCoord[1]) || (vi->point().y()>maxCoord[1]) || (vi->point().z()<minCoord[2]) || (vi->point().z()>maxCoord[2]);
		}
		//call test with other polyhedrons	
		for(vector<Polyhedron>::iterator a = polyhedrons.begin(); (a != polyhedrons.end()) && (!intersection); a++){	
			intersection = do_intersect(*a,trial_moved);
		        if (intersection) break;
		}
		if (!intersection){
			polyhedrons.push_back(trial_moved);
			v.clear();
			for(Polyhedron::Vertex_iterator vi = trial_moved.vertices_begin(); vi !=  trial_moved.vertices_end(); vi++){
				v.push_back(FromCGALPoint(vi->point()));
			}
			vv.push_back(v);
			it = 0;
			count ++;

		}
	}
	cout << "generated " << count << " polyhedrons"<< endl;

	//can't be used - no information about material
	Scene* scene=Omega::instance().getScene().get();
	for(vector<vector<Vector3r> >::iterator p=vv.begin(); p!=vv.end(); ++p){
		shared_ptr<Body> BP = NewPolyhedra(*p, mat);
		BP->shape->color = Vector3r(double(rand())/RAND_MAX,double(rand())/RAND_MAX,double(rand())/RAND_MAX);
		scene->bodies->insert(BP);
	}
	return v;
} 

//**************************************************************************
/* Generate truncated icosahedron*/
vector<Vector3r> TruncIcosaHedPoints(Vector3r radii){
    vector<Vector3r> v;

    double p = (1.+sqrt(5.))/2.;
    Vector3r f,c,b;
    f = radii/sqrt(9.*p + 1.);
    vector<Vector3r> A,B;
    A.push_back(Vector3r(0.,1.,3.*p)); A.push_back(Vector3r(2.,1.+2.*p,p)); A.push_back(Vector3r(1.,2.+p,2.*p));
    for(int i=0;i<(int) A.size() ;i++){
        B.clear();
	c = Vector3r(A[i][0]*f[0],A[i][1]*f[1],A[i][2]*f[2]);
	B.push_back(c); B.push_back(Vector3r(c[1],c[2],c[0])); B.push_back(Vector3r(c[2],c[0],c[1]));
        for(int j=0;j<(int) B.size() ;j++){
		b = B[j];
		v.push_back(b);
		if (b[0] != 0.){ 
			v.push_back(Vector3r(-b[0], b[1], b[2]));	
			if (b[1] != 0.){ 
				v.push_back(Vector3r(-b[0],-b[1], b[2]));
				if (b[2] != 0.) v.push_back(Vector3r(-b[0],-b[1],-b[2]));
			}
			if (b[2] != 0.) v.push_back(Vector3r(-b[0], b[1],-b[2]));
		}
		if (b[1] != 0.){ 
			v.push_back(Vector3r( b[0],-b[1], b[2]));
			if (b[2] != 0.)	v.push_back(Vector3r( b[0],-b[1],-b[2]));
		}
		if (b[2] != 0.)	v.push_back(Vector3r( b[0], b[1],-b[2]));
	}
    }
    return v;
}

//**************************************************************************
/* Generate SnubCube*/
vector<Vector3r> SnubCubePoints(Vector3r radii){
    vector<Vector3r> v;

    double c1 = 0.337754;
    double c2 = 1.14261;
    double c3 = 0.621226;
    Vector3r f,b;
    f = radii/1.3437133737446;
    vector<Vector3r> A;
    A.push_back(Vector3r(c2,c1,c3)); A.push_back(Vector3r(c1,c3,c2)); A.push_back(Vector3r(c3,c2,c1)); 
    A.push_back(Vector3r(-c1,-c2,-c3)); A.push_back(Vector3r(-c2,-c3,-c1)); A.push_back(Vector3r(-c3,-c1,-c2));
    for(int i=0;i<(int) A.size();i++){
	b = Vector3r(A[i][0]*f[0],A[i][1]*f[1],A[i][2]*f[2]);
	v.push_back(b);
	v.push_back(Vector3r(-b[0],-b[1], b[2]));
	v.push_back(Vector3r(-b[0], b[1],-b[2]));
	v.push_back(Vector3r( b[0],-b[1],-b[2]));
    }
    return v;
}

//**************************************************************************
/* Generate ball*/
vector<Vector3r> BallPoints(Vector3r radii, int NumFacets,int seed){
        vector<Vector3r> v;
	if (NumFacets == 60) v = TruncIcosaHedPoints(radii);
	if (NumFacets == 24) v = SnubCubePoints(radii);
	else{
		double inc = Mathr::PI * (3. - pow(5.,0.5));
    		double off = 2. / double(NumFacets);
		double y,r,phi;
        	for(int k=0; k<NumFacets; k++){
	            y = double(k) * off - 1. + (off / 2.);
        	    r = pow(1. - y*y,0.5);
        	    phi = double(k) * inc;
        	    v.push_back(Vector3r(cos(phi)*r*radii[0], y*radii[1], sin(phi)*r*radii[2]));
		}
	}

	// randomly rotate
        srand(seed);
	Quaternionr Rot(double(rand())/RAND_MAX,double(rand())/RAND_MAX,double(rand())/RAND_MAX,double(rand())/RAND_MAX);
	Rot.normalize();
	for(int i=0; i< (int) v.size();i++) {
		v[i] = Rot*(Vector3r(v[i][0],v[i][1],v[i][2]));
	}
        return v;
}

//**********************************************************************************
//generate "packing" of non-overlapping balls
vector<Vector3r> fillBoxByBalls_cpp(Vector3r minCoord, Vector3r maxCoord, Vector3r sizemin, Vector3r sizemax, Vector3r ratio, int seed, shared_ptr<Material> mat, int NumPoints){	
	vector<Vector3r> v;
	Polyhedra trialP;
	Polyhedron trial, trial_moved;
	srand(seed);
	int it = 0;
	vector<Polyhedron> polyhedrons;
	vector<vector<Vector3r> > vv;
	Vector3r position;
	bool intersection;
	int count = 0;
	Vector3r radii;

	
	bool fixed_ratio = 0;
	if (ratio[0] > 0 && ratio[1] > 0 && ratio[2]>0){
		fixed_ratio = 1;
		sizemax[0] = min(min(sizemax[0]/ratio[0],  sizemax[1]/ratio[1]),  sizemax[2]/ratio[2]);
		sizemin[0] = max(max(sizemin[0]/ratio[0],  sizemin[1]/ratio[1]),  sizemin[2]/ratio[2]);
	}

	fixed_ratio = 1; //force spherical

	//it - number of trials to make packing possibly more/less dense
	Vector3r random_size;
	while (it<1000){
		it = it+1;
		if (it == 1){	
			if (fixed_ratio) {
				double rrr = (rand()*(sizemax[0]-sizemin[0])/RAND_MAX + sizemin[0])/2.;
				radii = Vector3r(rrr,rrr,rrr);
			}else  {
				radii = Vector3r(rand()*(sizemax[0]-sizemin[0])/2.,rand()*(sizemax[1]-sizemin[1])/2.,rand()*(sizemax[2]-sizemin[2])/2.)/RAND_MAX + sizemin/2.;
			}				
			trialP.v = BallPoints(radii,NumPoints,rand());
			trialP.Initialize();
			trial = trialP.GetPolyhedron();	
			Matrix3r rot_mat = (trialP.GetOri()).toRotationMatrix();
			Transformation t_rot(rot_mat(0,0),rot_mat(0,1),rot_mat(0,2),rot_mat(1,0),rot_mat(1,1),rot_mat(1,2),rot_mat(2,0),rot_mat(2,1),rot_mat(2,2),1.);	
			std::transform( trial.points_begin(), trial.points_end(), trial.points_begin(), t_rot);			
		}		
		position = Vector3r(rand()*(maxCoord[0]-minCoord[0]),rand()*(maxCoord[1]-minCoord[1]),rand()*(maxCoord[2]-minCoord[2]))/RAND_MAX + minCoord;

		//move CGAL structure Polyhedron
		Transformation transl(CGAL::TRANSLATION, ToCGALVector(position));
		trial_moved = trial;		
		std::transform( trial_moved.points_begin(), trial_moved.points_end(), trial_moved.points_begin(), transl);
		//calculate plane equations
		std::transform( trial_moved.facets_begin(), trial_moved.facets_end(), trial_moved.planes_begin(),Plane_equation());	

		intersection = false;	
		//call test with boundary
		for(Polyhedron::Vertex_iterator vi = trial_moved.vertices_begin(); (vi !=  trial_moved.vertices_end()) && (!intersection); vi++){
			intersection = (vi->point().x()<minCoord[0]) || (vi->point().x()>maxCoord[0]) || (vi->point().y()<minCoord[1]) || (vi->point().y()>maxCoord[1]) || (vi->point().z()<minCoord[2]) || (vi->point().z()>maxCoord[2]);
		}
		//call test with other polyhedrons	
		for(vector<Polyhedron>::iterator a = polyhedrons.begin(); (a != polyhedrons.end()) && (!intersection); a++){	
			intersection = do_intersect(*a,trial_moved);
		        if (intersection) break;
		}
		if (!intersection){
			polyhedrons.push_back(trial_moved);
			v.clear();
			for(Polyhedron::Vertex_iterator vi = trial_moved.vertices_begin(); vi !=  trial_moved.vertices_end(); vi++){
				v.push_back(FromCGALPoint(vi->point()));
			}
			vv.push_back(v);
			it = 0;
			count ++;

		}
	}
	cout << "generated " << count << " polyhedrons"<< endl;

	//can't be used - no information about material
	Scene* scene=Omega::instance().getScene().get();
	for(vector<vector<Vector3r> >::iterator p=vv.begin(); p!=vv.end(); ++p){
		shared_ptr<Body> BP = NewPolyhedra(*p, mat);
		BP->shape->color = Vector3r(double(rand())/RAND_MAX,double(rand())/RAND_MAX,double(rand())/RAND_MAX);
		scene->bodies->insert(BP);
	}
	return v;
} 

//**********************************************************************************
//split polyhedra
void Split(const shared_ptr<Body> body, Vector3r direction, Vector3r point){
	SplitPolyhedra(body, direction, point);
}

//**********************************************************************************
//distace of point from a plane (squared) with sign
double Oriented_squared_distance2(Plane P, CGALpoint x){
	double h = P.a()*x.x()+P.b()*x.y()+P.c()*x.z()+P.d();
	return ((h>0.)-(h<0.))*pow(h,2)/(CGALvector(P.a(),P.b(),P.c())).squared_length();
}

//**********************************************************************************
bool convexHull(vector<Vector3r> points){	
	vector<CGALpoint> pointsCGAL;
	for(int i=0;i<(int) points.size() ;i++) {
		pointsCGAL.push_back(ToCGALPoint(points[i]));
	}
	Polyhedron P;
	CGAL::convex_hull_3(pointsCGAL.begin(), pointsCGAL.end(), P);
	return true;
} 

BOOST_PYTHON_MODULE(_polyhedra_utils){
	YADE_SET_DOCSTRING_OPTS;
	py::def("PrintPolyhedra",PrintPolyhedra,"Print list of vertices sorted according to polyhedrons facets.");
	py::def("PrintPolyhedraActualPos",PrintPolyhedraActualPos,"Print list of vertices sorted according to polyhedrons facets.");
	py::def("PWaveTimeStep",PWaveTimeStep,"Get timestep accoring to the velocity of P-Wave propagation; computed from sphere radii, rigidities and masses.");
	py::def("do_Polyhedras_Intersect",do_Polyhedras_Intersect,"check polyhedras intersection");
	py::def("fillBox_cpp",fillBox_cpp,"Generate non-overlaping polyhedrons in box");
	py::def("fillBoxByBalls_cpp",fillBoxByBalls_cpp,"Generate non-overlaping 'spherical' polyhedrons in box");
	py::def("MinCoord",MinCoord,"returns min coordinates");
	py::def("MaxCoord",MaxCoord,"returns max coordinates");
	py::def("SieveSize",SieveSize,"returns approximate sieve size of polyhedron");
	py::def("SieveCurve",SieveCurve,"save sieve curve coordinates into file");
	py::def("SizeOfPolyhedra",SizeOfPolyhedra,"returns max, middle an min size in perpendicular directions");
	py::def("SizeRatio",SizeRatio,"save sizes of polyhedra into file");
	py::def("convexHull",convexHull,"TODO");
	py::def("Split",Split,"split polyhedron perpendicularly to given direction through given point");
}

#endif // YADE_CGAL
