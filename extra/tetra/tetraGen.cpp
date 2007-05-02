
#include<yade/core/MetaBody.hpp>
#include<yade/extra/Shop.hpp>

#include"TetraTestGen.hpp"

char* yadePluginClasses[]={
        "TetraTestGen",
        NULL /*sentinel*/
};

string TetraTestGen::generate()
{
	//Shop::setDefault("param_pythonRunExpr",string("if S.i%50==0 and S.i<1000 and S.i>500:\n\tprint S.i,len(S.sel),B[1].x, B[1].E"));

	rootBody=Shop::rootBody();
	Shop::rootBodyActors(rootBody);
	
	#if 0
	shared_ptr<MetaBody> oldRootBody=Omega::instance().getRootBody();
	Omega::instance().setRootBody(rootBody);
	#endif

	vector<Vector3r> v;

	v.push_back(Vector3r(1,1,0));
	v.push_back(Vector3r(0,-1,0));
	v.push_back(Vector3r(-1,1,0));
	v.push_back(Vector3r(0,0,-2));
	shared_ptr<Body> ground=Shop::tetra(v);
	ground->isDynamic=false;


	for(size_t i=0; i<gridSize[0]; i++) for(size_t j=0; j<gridSize[1];j++) for(size_t k=0; k<gridSize[2];k++){
		Vector3r center=1.0*Vector3r(i-gridSize[0]/2.,j-gridSize[1]/2.,k-gridSize[2]/2.);
		double size=.2;
		#define __VERTEX(x,y,z) center+size*(Vector3r(x,y,z)+Vector3r(Mathr::IntervalRandom(-.5,.5),Mathr::IntervalRandom(-.5,.5),Mathr::IntervalRandom(-.5,.5)))
			v[0]=__VERTEX(1,1,0);
			v[1]=__VERTEX(0,-1,0);
			v[2]=__VERTEX(-1,0,0);
			v[3]=__VERTEX(0,0,2);
		#undef __VERTEX
		shared_ptr<Body> t=Shop::tetra(v);
		rootBody->bodies->insert(ground);
	}

	#if 0
	Omega::instance().setRootBody(oldRootBody);
	#endif
	
	return "OK";
}
