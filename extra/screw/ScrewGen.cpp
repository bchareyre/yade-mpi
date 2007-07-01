#include"ScrewGen.hpp"
#include<yade/extra/Clump.hpp>
#include<yade/extra/Shop.hpp>

char* yadePluginClasses[]={
	"ScrewGen",
	NULL /*sentinel*/
};

CREATE_LOGGER(ScrewGen);

ScrewGen::ScrewGen(){
	n_tip=5;
	shaft_density=2;
	x_tip=0;
	x_tip_shaft=2;
	x_shaft_end=8;
	r_shaft=1;

	thread_x_per_rad=.5;
	thread_sphere_r=.2;
	thread_sphere_density=2;

}


bool ScrewGen::generate(){
	// basic things
	rootBody=Shop::rootBody();
	Shop::rootBodyActors(rootBody);
	// clumps do not need to subscribe currently (that will most likely change, though)
	rootBody->engines.push_back(shared_ptr<ClumpMemberMover>(new ClumpMemberMover));


	Omega::instance().setRootBody(rootBody);


	// ground
	shared_ptr<Body> ground=Shop::box(Vector3r(5,0,-2),Vector3r(5,5,.2));
	ground->isDynamic=false;
	// revert random colors for this single case...
	ground->geometricalModel->diffuseColor=Vector3r(.6,.6,.6);
	ground->interactingGeometry->diffuseColor=Vector3r(.3,.3,.3);
	rootBody->bodies->insert(ground);


	// SCREW
	shared_ptr<Clump> screw=shared_ptr<Clump>(new Clump());
	shared_ptr<Body> screwAsBody(static_pointer_cast<Body>(screw));
	rootBody->bodies->insert(screwAsBody);
	screw->isDynamic=true;

	// tip
	for(unsigned i=1; i<n_tip; i++){
		Vector3r C=Vector3r(x_tip+i*((x_tip_shaft-x_tip)/n_tip),0,0);
		Real r=r_shaft*(i/((double)n_tip));
		shared_ptr<Body> sphere=Shop::sphere(C,r);
		body_id_t lastId=rootBody->bodies->insert(sphere);
		screw->add(lastId);
		LOG_INFO("Sphere at "<<C<<", radius "<<r);
	}
	// shaft
	unsigned n_shaft=(unsigned)((x_shaft_end-x_tip_shaft)/(r_shaft/shaft_density));
	for(unsigned i=1; i<n_shaft; i++){
		Vector3r C=Vector3r(x_tip_shaft+i*((x_shaft_end-x_tip_shaft)/n_shaft),0,0);
		Real r=r_shaft;
		shared_ptr<Body> sphere=Shop::sphere(C,r);
		body_id_t lastId=rootBody->bodies->insert(sphere);
		screw->add(lastId);
		LOG_INFO("Sphere at "<<C<<", radius "<<r);
	}
	LOG_INFO("Tip and shaft done");

	//thread
	// how many radians do we have to turn to cover the shaft
	Real total_angle=(x_shaft_end-x_tip_shaft)/thread_x_per_rad;
	if(false){ //create thread as spheres
		Real thread_n_spheres_per_rad=(r_shaft+thread_sphere_r)/(thread_sphere_r/thread_sphere_density);
		for(unsigned i=0; i<thread_n_spheres_per_rad*total_angle; i++){
			Real angle=(1./thread_n_spheres_per_rad)*i;
			Vector3r C(x_tip_shaft+thread_x_per_rad*angle,sin(angle)*(r_shaft+thread_sphere_r),cos(angle)*(r_shaft+thread_sphere_r));
			shared_ptr<Body> sphere=Shop::sphere(C,thread_sphere_r);
			body_id_t lastId=rootBody->bodies->insert(sphere);
			screw->add(lastId);
			LOG_INFO("Sphere at "<<C<<", radius "<<thread_sphere_r);
		}
	} else { // create thread as tetrahedra
		Real thread_n_tetra_per_rad=(r_shaft+2*thread_sphere_r)/(thread_sphere_r); // unit density
		for(unsigned i=0; i<thread_n_tetra_per_rad*total_angle; i++){
			Real angle=(1./thread_n_tetra_per_rad)*i;
			Real half_angle_step=.5/thread_n_tetra_per_rad;
			Vector3r vx[4];
			// tetrahedron that has 2 vertices on the thread top
			vx[0]=Vector3r(x_tip_shaft+thread_x_per_rad*(angle-half_angle_step),sin(angle-half_angle_step)*(r_shaft+2*thread_sphere_r),cos(angle-half_angle_step)*(r_shaft+2*thread_sphere_r));
			vx[1]=Vector3r(x_tip_shaft+thread_x_per_rad*(angle+half_angle_step),sin(angle+half_angle_step)*(r_shaft+2*thread_sphere_r),cos(angle+half_angle_step)*(r_shaft+2*thread_sphere_r));
			//Vector3r thread_normal_on _shaft_surf=
			vx[2]=Vector3r(x_tip_shaft+thread_x_per_rad*(angle)-thread_sphere_r,sin(angle)*(r_shaft),cos(angle)*(r_shaft));
			vx[3]=Vector3r(x_tip_shaft+thread_x_per_rad*(angle)+thread_sphere_r,sin(angle)*(r_shaft),cos(angle)*(r_shaft));
			shared_ptr<Body> tetra=Shop::tetra(vx);
			body_id_t lastId=rootBody->bodies->insert(tetra);
			screw->add(lastId);

			// bottom filler: 2 points on lower x (2,3,1 coincident)
			vx[0]=Vector3r(x_tip_shaft+thread_x_per_rad*(angle+2*half_angle_step)-thread_sphere_r,sin(angle+2*half_angle_step)*(r_shaft),cos(angle+2*half_angle_step)*(r_shaft));
			tetra=Shop::tetra(vx);
			lastId=rootBody->bodies->insert(tetra);
			screw->add(lastId);

			// bottom filler: 2 points on greater x (2,3,1 coincident)
			vx[0]=Vector3r(x_tip_shaft+thread_x_per_rad*(angle+2*half_angle_step)+thread_sphere_r,sin(angle+2*half_angle_step)*(r_shaft),cos(angle+2*half_angle_step)*(r_shaft));
			tetra=Shop::tetra(vx);
			lastId=rootBody->bodies->insert(tetra);
			screw->add(lastId);

			LOG_INFO("Thread top tetra with vertices "<<vx[0]<<", "<<vx[1]<<", "<<vx[2]<<", "<<vx[3]);
		}
	}

	screw->updateProperties(false);

	message="OK.";
	return true;
}
