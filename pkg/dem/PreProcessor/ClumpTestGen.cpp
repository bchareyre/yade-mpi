// © 2007 Vaclav Smilauer <eudoxos@arcig.cz>

#include"ClumpTestGen.hpp"
#include<yade/pkg-dem/Clump.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-dem/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/pkg-common/StateEngineUnit.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/AABB.hpp>

YADE_PLUGIN((ClumpTestGen));
CREATE_LOGGER(ClumpTestGen);

bool ClumpTestGen::generate()
{
	//Shop::setDefault("param_pythonRunExpr",string("if S.i%50==0 and S.i<1000 and S.i>500:\n\tprint S.i,len(S.sel),B[1].x, B[1].E"));

	rootBody=Shop::rootBody();
	Shop::rootBodyActors(rootBody);
	// clumps do not need to subscribe currently (that will most likely change, though)
	rootBody->engines.push_back(shared_ptr<ClumpMemberMover>(new ClumpMemberMover));
	

	shared_ptr<Scene> oldRootBody=Omega::instance().getScene();
	Omega::instance().setScene(rootBody);

	shared_ptr<Body> ground=Shop::box(Vector3r(0,0,-1),Vector3r(3,3,.2));
	ground->isDynamic=false;
	// revert random colors for this single case...
	#ifdef YADE_GEOMETRICALMODEL
		ground->geometricalModel->diffuseColor=Vector3r(.6,.6,.6);
	#endif
	ground->interactingGeometry->diffuseColor=Vector3r(.3,.3,.3);
	rootBody->bodies->insert(ground);

	vector<Vector3r> relPos; vector<Real> radii; Vector3r clumpPos;

	// standalone (non-clump!) sphere as well
	shared_ptr<Body> sphere=Shop::sphere(Vector3r(0,0,0),.5);
	rootBody->bodies->insert(sphere);
	// one-sphere clump
	clumpPos=Vector3r(-2,0,0);
	relPos.push_back(Vector3r(0,0,0)); radii.push_back(.5);
	createOneClump(rootBody,clumpPos,relPos,radii);
	relPos.clear(); radii.clear();
	// two-sphere clump
	clumpPos=Vector3r(2,0,0);
	relPos.push_back(Vector3r(0,-.5,.5)); radii.push_back(.5);
	relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
	createOneClump(rootBody,clumpPos,relPos,radii);
	relPos.clear(); radii.clear();
	// three-sphere slump
	clumpPos=Vector3r(0,2,0);
	relPos.push_back(Vector3r(0,-.5,.5)); radii.push_back(.5);
	relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
	relPos.push_back(Vector3r(.5,0,0)); radii.push_back(.5);
	createOneClump(rootBody,clumpPos,relPos,radii);
	relPos.clear(); radii.clear();
	// four-sphere slump
	clumpPos=Vector3r(0,-2,0);
	relPos.push_back(Vector3r(0,0,0)); radii.push_back(.5);
	relPos.push_back(Vector3r(.5,0,0)); radii.push_back(.5);
	relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
	relPos.push_back(Vector3r(0,0,.5)); radii.push_back(.5);
	createOneClump(rootBody,clumpPos,relPos,radii);
	relPos.clear(); radii.clear();

	// restore Omega
	Omega::instance().setScene(oldRootBody);
	
	message="OK";
	return true;
}

/*! \brief Generate clump of spheres, the result will be inserted into rootBody.
 *
 * Attention here: clump's id must be greater than id of any of its constituents; therefore
 *   1. create bodies that will be clumped, add them to bodies (at that moment they get their id) and save their ids in clumpMembers
 *   2. create (empty) clump and add it to bodies
 *   3. add bodies to be clumped to the clump
 *   4. call Clump::updateProperties to get physical properties physically right (inertia, position, orientation, mass, ...).
 *
 * @param clumpPos Center of the clump (not necessarily centroid); serves merely as reference for sphere positions.
 * @param relPos Relative positions of individual spheres' centers.
 * @param radii Radii of composing spheres. Must have the same length as relPos.
 * @param rootBody clumpedBodies.
 */
void ClumpTestGen::createOneClump(shared_ptr<Scene>& rootBody, Vector3r clumpPos, vector<Vector3r> relPos, vector<Real> radii)
{
	assert(relPos.size()==radii.size());
	list<body_id_t> clumpMembers;	
	for(size_t i=0; i<relPos.size(); i++){
		shared_ptr<Body> sphere=Shop::sphere(clumpPos+relPos[i],radii[i]);
		body_id_t lastId=(body_id_t)rootBody->bodies->insert(sphere);
		clumpMembers.push_back(lastId);
		LOG_TRACE("Generated (not yet) clumped sphere #"<<lastId);
	}
	shared_ptr<Clump> clump=shared_ptr<Clump>(new Clump());
	shared_ptr<Body> clumpAsBody=static_pointer_cast<Body>(clump);
	clump->isDynamic=true;
	rootBody->bodies->insert(clumpAsBody);
	FOREACH(body_id_t id, clumpMembers){
		clump->add(id);
	}
	clump->updateProperties(false);
}



YADE_REQUIRE_FEATURE(PHYSPAR);

