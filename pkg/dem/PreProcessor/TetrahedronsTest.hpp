/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TETRAHEDRONSTEST_HPP
#define TETRAHEDRONSTEST_HPP

#include<yade/core/FileGenerator.hpp>
#include<yade/pkg-common/Tetrahedron.hpp>

/*! 
 * \brief This class derived from FileGenerator is what can be called an 
 *
 *    int main(argc, argv**);
 *
 *  of the simulation. This is where the execution flow control, and the data
 *  are put together to make a sensible, working simulation.
 *
 *
 * So the main loop, Body->engines:
 *
 *  - with class InteractingMyTetrahedron2AABB registered in TetrahedronsTest inside an MetaEngine called
 *     boundigVolumeDispatcher yade will automatically create AABB from InteractingMyTetrahedron
 *
 *  - with AABB available, the PersistentSAPCollider will be able to detect that two bodies are interacting
 *
 *  - if interaction is detected, an appropriate EngineUnit will be called, in
 *    this small example I have just two of them:
 *
 *        InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron
 *        InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron
 *
 *    depending on between what kind of body there is an interaction.
 *
 *    They will create for us a class InteractionOfMyTetrahedron
 *
 *  - For any interaction we need to calculate the physics of interaction,
 *    class SimpleElasticInteraction. It is created by ElasticBodySimpleRelationship
 *    inside MetaEngine interactionPhysicsDispatcher
 *
 *  - with all that info available it is possibel to calculate PhysicalActions: a Force and a Momentum,
 *    class MyTetrahedronLaw is doing the job
 *
 *  - then we can apply gravityCondition (an additional force directed downwards)
 *
 *  - apply damping on Force and Momentum (you can remove that part, because a
 *    realistic scientific calculation should not need to use damping ;)
 *    this is done by actionDampingDispatcher
 *
 *  - then we can apply those PhysicalActions on the Body, thus calculate new
 *    body's acceleration and angularAcceleration using NewtonsLaw. class applyActionDispatcher does just that
 *
 *  - and finally the new body's position is integrated using leapfrog integration
 *
 *
 *
 * There is also a secondary loop Body->initializers, which is called only for the first time, before the simulation.
 * It is used to build BoundingVolume of bodies and InteractingMyTetrahedron from the GeometricalModel (Tetrahedron)\
 *
 *
 * additionally physicalActionInitializer and PhysicalActionContainerReseter
 * are used on each iteration to reset calculated forces and momentums back to
 * zero (otherwise they could accumulate from one iteration to another, a thing
 * we don't want to happen :)
 *
 *
 */

class TetrahedronsTest : public FileGenerator
{
	private :
		Vector3r	 nbTetrahedrons
				,groundSize
				,gravity;

		Real		 minSize
				,density
				,maxSize
				,dampingForce
				,disorder
				,dampingMomentum
				,youngModulus;

		int		 timeStepUpdateInterval;
		bool		 rotationBlocked;

		void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createTetrahedron(shared_ptr<Body>& body, int i, int j, int k);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);
		
		void makeTet(shared_ptr<Tetrahedron>& tet, Real radius);

	public :
		TetrahedronsTest ();
		~TetrahedronsTest ();
		string generate();

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(TetrahedronsTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(TetrahedronsTest,false);

#endif // TETRAHEDRONSTEST_HPP

