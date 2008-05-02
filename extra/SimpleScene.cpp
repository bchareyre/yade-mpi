bool SimpleScene::generate(){
	message="";
	rootBody=Shop::rootBody();

	/* initializers */
		rootBody->initializers.clear();

		shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
			physicalActionInitializer->physicalActionNames.push_back("Force");
			physicalActionInitializer->physicalActionNames.push_back("Momentum");
			rootBody->initializers.push_back(physicalActionInitializer);
	
		shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
			boundingVolumeDispatcher->add(new InteractingSphere2AABB);
			boundingVolumeDispatcher->add(new InteractingBox2AABB);
			boundingVolumeDispatcher->add(new MetaInteractingGeometry2AABB);
			rootBody->initializers.push_back(boundingVolumeDispatcher);

	/* engines */
		rootBody->engines.clear();

		rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
			rootBody->engines.push_back(boundingVolumeDispatcher);

		shared_ptr<PersistentSAPCollider> collider(new PersistentSAPCollider);
			rootBody->engines.push_back(collider);

		shared_ptr<InteractionGeometryMetaEngine> igeomDispatcher(new InteractionGeometryMetaEngine);
			igeomDispatcher->add(new InteractingSphere2InteractingSphere4SpheresContactGeometry);
			igeomDispatcher->add(new InteractingBox2InteractingSphere4SpheresContactGeometry);
			rootBody->engines.push_back(igeomDispatcher);

		shared_ptr<InteractionPhysicsMetaEngine> iphysDispatcher(new InteractionPhysicsMetaEngine);
			shared_ptr<SimpleElasticRelationships> ssr(new SimpleElasticRelationships);
			iphysDispatcher->add(ssr);
			rootBody->engines.push_back(iphysDispatcher);

		shared_ptr<ElasticContactLaw> ecl(new ElasticContactLaw);
			rootBody->engines.push_back(ecl);

		shared_ptr<GravityEngine> ge(new GravityEngine);
			ge->gravity=Vector3r(0,0,-9.81);
			rootBody->engines.push_back(ge);


		shared_ptr<PhysicalActionDamper> dampingDispatcher(new PhysicalActionDamper);
			shared_ptr<CundallNonViscousForceDamping> forceDamper(new CundallNonViscousForceDamping);
			forceDamper->damping=0.2;
			dampingDispatcher->add(forceDamper);
			shared_ptr<CundallNonViscousMomentumDamping> momentumDamper(new CundallNonViscousMomentumDamping);
			momentumDamper->damping=0.2;
			dampingDispatcher->add(momentumDamper);
			rootBody->engines.push_back(dampingDispatcher);


		shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
			applyActionDispatcher->add(new NewtonsForceLaw);
			applyActionDispatcher->add(new NewtonsMomentumLaw);
			rootBody->engines.push_back(applyActionDispatcher);

		shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
			positionIntegrator->add(new LeapFrogPositionIntegrator);
			rootBody->engines.push_back(positionIntegrator);

		shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
			orientationIntegrator->add(new LeapFrogOrientationIntegrator);
			rootBody->engines.push_back(orientationIntegrator);


}
