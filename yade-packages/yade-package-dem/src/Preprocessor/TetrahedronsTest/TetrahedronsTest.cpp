/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TetrahedronsTest.hpp"

#include "BodyMacroParameters.hpp"
#include "ElasticContactLaw.hpp"
#include "SDECLinkGeometry.hpp"
#include "MacroMicroElasticRelationships.hpp"
#include "SDECTimeStepper.hpp"
#include "SwiftPolyhedronProximityModeler.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-package-common/Box.hpp>
#include <yade/yade-package-common/AABB.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/SAPCollider.hpp>
#include <yade/yade-package-common/PersistentSAPCollider.hpp>
#include <yade/yade-lib-serialization/IOFormatManager.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-package-common/BoundingVolumeMetaEngine.hpp>
#include <yade/yade-package-common/InteractionDescriptionSet2AABB.hpp>
#include <yade/yade-package-common/MetaInteractingGeometry.hpp>

#include <yade/yade-package-common/PhysicalActionContainerReseter.hpp>
#include <yade/yade-package-common/PhysicalActionContainerInitializer.hpp>

#include <yade/yade-package-common/PhysicalActionDamper.hpp>
#include <yade/yade-package-common/PhysicalActionApplier.hpp>

#include <yade/yade-package-common/CundallNonViscousForceDamping.hpp>
#include <yade/yade-package-common/CundallNonViscousMomentumDamping.hpp>
#include <yade/yade-package-common/GravityEngine.hpp>

#include <yade/yade-package-common/InteractionPhysicsMetaEngine.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-package-common/InteractingBox.hpp>
#include <yade/yade-package-common/InteractingSphere.hpp>
#include <yade/yade-package-common/PhysicalParametersMetaEngine.hpp>

#include <yade/yade-package-common/BodyRedirectionVector.hpp>
#include <yade/yade-package-common/InteractionVecSet.hpp>
#include <yade/yade-package-common/PhysicalActionVectorVector.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

TetrahedronsTest::TetrahedronsTest () : FileGenerator()
{
	nbTetrahedrons = Vector3r(1,1,1);
	minRadius = 5;
	maxRadius = 5;
	groundSize = Vector3r(50,5,50);
	dampingForce = 0.3;
	dampingMomentum = 0.3;
	timeStepUpdateInterval = 300;
	sphereYoungModulus   = 15000000.0;
	//sphereYoungModulus   = 10000;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density = 2600;
	rotationBlocked = false;
	gravity = Vector3r(0,-9.81,0);
	disorder = 0.2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

TetrahedronsTest::~TetrahedronsTest ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::postProcessAttributes(bool)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbTetrahedrons);
	REGISTER_ATTRIBUTE(minRadius);
	REGISTER_ATTRIBUTE(maxRadius);
	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(disorder);
	REGISTER_ATTRIBUTE(groundSize);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(rotationBlocked);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string TetrahedronsTest::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);
	createActors(rootBody);
	positionRootBody(rootBody);

////////////////////////////////////
///////// Container
	
	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->volatileInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->actionParameters		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);
		
////////////////////////////////////
///////// ground

	shared_ptr<Body> ground;
	createBox(ground, Vector3r(0,0,0), groundSize);
	rootBody->bodies->insert(ground);

///////// tetrahedrons

	for(int i=0;i<nbTetrahedrons[0];i++)
		for(int j=0;j<nbTetrahedrons[1];j++)
			for(int k=0;k<nbTetrahedrons[2];k++)
			{
				shared_ptr<Body> tet;
				createTetrahedron(tet,i,j,k);
				rootBody->bodies->insert(tet);
			}
	
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::createTetrahedron(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Tetrahedron> tet(new Tetrahedron);
	shared_ptr<PolyhedralSweptSphere> pss(new PolyhedralSweptSphere);
	
	Quaternionr q = Quaternionr::IDENTITY;
	//q.fromAxisAngle( Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom()),Mathr::symmetricRandom());
	//q.normalize();
	
	Vector3r position		= Vector3r(i,j,k)*(5*maxRadius*1.1) // this formula is crazy !!
					  - Vector3r( nbTetrahedrons[0]/2*(2*maxRadius*1.1) , -9-maxRadius*2 , nbTetrahedrons[2]/2*(2*maxRadius*1.1) )
					  + Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom())*disorder*maxRadius;
	
	Real radius 			= (Mathr::intervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);

	loadTRI(tet,"../data/tetra1.tri");
	tet->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	tet->wire			= false;
	tet->visible			= true;
	tet->shadowCaster		= false;
	
	pss->radius			= 3;
	pss->diffuseColor		= Vector3f(0.8,0.3,0.3);
	pss->vertices.clear();
	pss->vertices.push_back(tet->v1);
	pss->vertices.push_back(tet->v2);
	pss->vertices.push_back(tet->v3);
	pss->vertices.push_back(tet->v4);
	pss->faces.clear();
	vector<int> face;
	face.resize(3);
	face[0] = 2;
	face[1] = 1;
	face[2] = 0;
	pss->faces.push_back(face);
	face[0] = 3;
	face[1] = 2;
	face[2] = 0;
	pss->faces.push_back(face);
	face[0] = 1;
	face[1] = 3;
	face[2] = 0;
	pss->faces.push_back(face);
	face[0] = 1;
	face[1] = 2;
	face[2] = 3;
	pss->faces.push_back(face);
	
	body->interactionGeometry	= pss;
	body->geometricalModel		= tet;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<PolyhedralSweptSphere> pss(new PolyhedralSweptSphere);
	
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
	//physics->mass			= 0;
	//physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	pss->diffuseColor		= Vector3f(1,0,0);
	pss->vertices.clear();
	
	pss->vertices.push_back(Vector3r(-extents[0],-extents[1],-extents[2]));
	pss->vertices.push_back(Vector3r(extents[0],-extents[1],-extents[2]));
	pss->vertices.push_back(Vector3r(extents[0],extents[1],-extents[2]));
	pss->vertices.push_back(Vector3r(-extents[0],extents[1],-extents[2]));
	
	pss->vertices.push_back(Vector3r(-extents[0],-extents[1],extents[2]));
	pss->vertices.push_back(Vector3r(extents[0],-extents[1],extents[2]));
	pss->vertices.push_back(Vector3r(extents[0],extents[1],extents[2]));
	pss->vertices.push_back(Vector3r(-extents[0],extents[1],extents[2]));

	pss->faces.clear();
	vector<int> face;
	face.resize(4);
	face[0] = 0;
	face[1] = 4;
	face[2] = 7;
	face[3] = 3;			//	      7         4
	pss->faces.push_back(face);	//	3         0
	face[0] = 1;			//	
	face[1] = 2;			//
	face[2] = 6;			//
	face[3] = 5;			//	       6         5
	pss->faces.push_back(face);	//	2         1
	face[0] = 2;
	face[1] = 3;
	face[2] = 7;
	face[3] = 6;
	pss->faces.push_back(face);
	face[0] = 0;
	face[1] = 1;
	face[2] = 5;
	face[3] = 4;
	pss->faces.push_back(face);
	face[0] = 4;
	face[1] = 5;
	face[2] = 6;
	face[3] = 7;
	pss->faces.push_back(face);
	face[0] = 3;
	face[1] = 2;
	face[2] = 1;
	face[3] = 0;
	pss->faces.push_back(face);
	pss->radius = 3;
	body->boundingVolume		= aabb;
	body->interactionGeometry	= pss;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> actionParameterInitializer(new PhysicalActionContainerInitializer);
	actionParameterInitializer->actionParameterNames.push_back("Force");
	actionParameterInitializer->actionParameterNames.push_back("Momentum");
	
	shared_ptr<SwiftPolyhedronProximityModeler> swiftPolyhedronProximityModeler(new SwiftPolyhedronProximityModeler);

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("PolyhedralSweptSphere","AABB","PolyhedralSweptSphere2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABB");
	
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add("Force","RigidBodyParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","RigidBodyParameters","NewtonsForceLaw");
	applyActionDispatcher->add("Momentum","RigidBodyParameters","NewtonsMomentumLaw");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");
 	

	shared_ptr<SDECTimeStepper> sdecTimeStepper(new SDECTimeStepper);
	sdecTimeStepper->sdecGroupMask = 1;
	sdecTimeStepper->timeStepUpdateInterval = timeStepUpdateInterval;

	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->actors.push_back(sdecTimeStepper);
	rootBody->actors.push_back(boundingVolumeDispatcher);	
	rootBody->actors.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->actors.push_back(swiftPolyhedronProximityModeler);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new ElasticContactLaw));
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	if(!rotationBlocked)
		rootBody->actors.push_back(orientationIntegrator);
	
	rootBody->initializers.clear();
	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::positionRootBody(shared_ptr<MetaBody>& rootBody) 
{
	rootBody->isDynamic		= false;
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class PhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r(0,0,0);
	physics->acceleration			= Vector3r::ZERO;
		
	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor			= Vector3f(0,0,1);
	
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->interactionGeometry		= dynamic_pointer_cast<InteractingGeometry>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 		= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void TetrahedronsTest::loadTRI(shared_ptr<Tetrahedron>& tet, const string& fileName)
{
	ifstream f(fileName.c_str());
	
	string type;
	int nbFaces;
	int nbVertices;
	f >> type;
	f >> nbVertices;
	f >> nbFaces;
	
	Vector3r v;
	vector<int> face;
	vector<vector<int> > faces;
	
// 	for(int i=0 ; i<nbVertices ; i++)
// 	{
// 		f >> tet->v[0] >> v[1] >> v[2];
// 		tet->vertices.push_back(v);	
// 	}
	f >> tet->v1[0] >> tet->v1[1] >> tet->v1[2];
	f >> tet->v2[0] >> tet->v2[1] >> tet->v2[2];
	f >> tet->v3[0] >> tet->v3[1] >> tet->v3[2];
	f >> tet->v4[0] >> tet->v4[1] >> tet->v4[2];

	face.resize(3);
	faces.clear();
	for(int i=0 ; i<nbFaces ; i++)
	{
		
		f >> face[0] >> face[1] >> face[2];
		//faces.push_back(face);	
	}	
	
	f.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// /*
// void Viewer::polyhedron2PolyhedralSS(const shared_ptr<PointCloud>& poly, shared_ptr<PolyhedralSS>& ss)
// {
// 	ss->vertices = poly->vertices;
// 	ss->se3	     = poly->se3;
// 	ss->radius = 0.2;
// 	
// 	shared_ptr<IFSPolyhedron> ifsp = dynamic_pointer_cast<IFSPolyhedron>(poly);
// 	if (ifsp)
// 	{
// 		ss->faces    = ifsp->faces;
// 		FIXME : not working yet !
// 		for(unsigned int i=0 ; i<ss->faces.size() ; i++)
// 		{	
// 			Vector3r v1 = ss->vertices[ss->faces[i][0]];
// 			Vector3r v2 = ss->vertices[ss->faces[i][1]];
// 			Vector3r v3 = ss->vertices[ss->faces[i][2]];
// 			Vector3r n = (v1-v2).unitCross(v1-v3);
// 			ss->vertices[ss->faces[i][0]] -= n*ss->radius;
// 			ss->vertices[ss->faces[i][1]] -= n*ss->radius;
// 			ss->vertices[ss->faces[i][2]] -= n*ss->radius;
// 			
// 		}
// 	}
// 		
// 	shared_ptr<Tetrahedron> tet = dynamic_pointer_cast<Tetrahedron>(poly);
// 	if (tet)
// 	{
// 		ss->faces.clear();
// 		
// 		vector<int> face;
// 		face.resize(3);
// 		
// 		face[0] = 0;
// 		face[1] = 1;
// 		face[2] = 2;
// 		ss->faces.push_back(face);
// 		
// 		face[0] = 0;
// 		face[1] = 2;
// 		face[2] = 3;
// 		ss->faces.push_back(face);
// 		
// 		face[0] = 0;
// 		face[1] = 3;
// 		face[2] = 1;
// 		ss->faces.push_back(face);
// 		
// 		face[0] = 3;
// 		face[1] = 2;
// 		face[2] = 1;
// 		ss->faces.push_back(face);
// 	}
// 	
// 	ss->computeEdges();
// }*/
