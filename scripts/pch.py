#!/usr/bin/python

from sys import *
import os,re
from string import *

incDir='../build-trunk/include/yade-trunk/yade'
pchDir='./pch/'
srcRoot='./'
incModules={
	'lib-base':0, 'lib-computational-geometry':0, 'lib-factory':0, 'lib-loki':0, 'lib-miniWm3':0, 'lib-multimethods':0, 'lib-opengl':0, 'lib-serialization':0, 'lib-serialization-bin':0, 'lib-serialization-qt':0, 'lib-serialization-xml':0,
	'core':1,
	'pkg-common':2,
	'pkg-dem':3,
	'pkg-realtime-rigidbody':3,
	'pkg-lattice':3,
	'pkg-mass-spring':3,
	'pkg-fem':3,
	'gui-cmd':3,
	'gui-qt3':3, 
	'extra':4}

from os.path import sep
import shutil

inclusionGuards={'GLEngineEditor.hpp': 'GL_ENGINE_EDITOR_HPP', 'InteractionPhysicsEngineUnit.hpp': 'INTERACTIONPHYSICS_ENGINEUNIT_HPP', 'SpheresContactGeometry.hpp': 'SPHERESCONTACTGEOMETRY_HPP', 'GLWindowsManager.hpp': 'GLWINDOWSMANAGER_HPP', 'QtSimulationPlayer.hpp': 'QTSIMULATIONPLAYER_HPP', 'NewtonsMomentumLaw.hpp': 'NEWTONSMOMENTUMLAW_HPP', 'PhysicalParameters.hpp': 'PHYSICALPARAMETERS_HPP', 'ParticleParameters.hpp': 'PARTICLEPARAMETERS_HPP', 'HangingCloth.hpp': 'HANGINGCLOTH_HPP', 'GLDrawInteractionGeometryFunctor.hpp': 'GLDRAW_INTERACTION_GEOMETRY_FUNCTOR_HPP', 'GeometricalModel.hpp': 'GEOMETRICALMODEL_HPP', 'SDECImpactTest.hpp': 'SDECIMPORT_HPP', 'Funnel.hpp': 'FUNNEL_HPP', 'PhysicalActionContainerIteratorPointer.hpp': 'PHYSICALACTIONCONTAINERITERATORPOINTER_HPP', 'ClosestFeatures.hpp': 'CLOSESTSFEATURES_HPP', 'CapillaryStressRecorder.hpp': 'CAPILLARY_STRESS_RECORDER_HPP', 'InteractionGeometryEngineUnit.hpp': 'INTERACTIONGEOMETRYFUNCTOR_HPP', 'Functor.hpp': 'FUNCTOR_INC_', 'BINFormatManager.hpp': 'BINFORMATMANAGER_HPP', 'SDECMovingWall.hpp': 'SDEC_MOVING_WALL_HPP', 'BroadInteractor.hpp': 'BROADINTERACTOR_HPP', 'InteractingBox.hpp': 'INTERACTION_BOX_HPP', 'BodyContainerIterator.hpp': 'BODYCONTAINERITERATOR_HPP', 'InteractionGeometry.hpp': 'INTERACTIONGEOMETRY_HPP', 'LineSegment.hpp': 'LINE_SEGMENT_HPP', 'GLDrawShadowVolumeFunctor.hpp': 'GLDRAWSHADOWVOLUMEFUNCTOR_HPP', 'ClassFactory.hpp': 'CLASSFACTORY_HPP', 'GLDrawLatticeBeamState.hpp': 'GLDRAW_LATTICE_BEAM_STATE_HPP', 'TypeManip.hpp': 'TYPEMANIP_INC_', 'OpenGLRenderingEngine.hpp': 'OPENGLRENDERINGENGINE_HPP', 'Sphere.hpp': 'SPHERE_HPP', 'TranslationEngine.hpp': 'TRANSLATOR_HPP', 'Tetrahedron.hpp': 'TETRAHEDRON_HPP', 'GLDrawSimpleElasticInteraction.hpp': 'GLDRAW_SIMPLE_ELASTIC_INTERACTION_HPP', 'AveragePositionRecorder.hpp': 'AVERAGE_POSISTION_RECORDER_HPP', 'EmptyType.hpp': 'EMPTYTYPE_INC_', 'BoxStack.hpp': 'BOXSTACK_HPP', 'QtSphericalDEM.hpp': 'QTSPHERICALDEM_HPP', 'CohesiveFrictionalContactLaw.hpp': 'COHESIVE_FRICTIONAL_CONTACT_LAW_HPP', 'QtMetaDispatchingEngineProperties.hpp': 'QTMETADISPATCHINGENGINEPROPERTIES_HPP', 'CapillaryRecorder.hpp': 'CAPILLARY_RECORDER_HPP', 'GLDrawBoxShadowVolume.hpp': 'GLDRAWBOXSHADOWVOLUME_HPP', 'FundamentalHandler.tpp': '__FUNDAMENTALHANDLER_H__', 'ElasticBodySimpleRelationship.hpp': 'ELASTICBODYSIMPLERELATIONSHIP_HPP', 'GeometricalModelEngineUnit.hpp': 'GEOMETRICAL_MODEL_ENGINE_UNIT_HPP', 'MovingSupport.hpp': 'NO_SHEAR_PLANE', 'QtGUIGenerator.hpp': 'QTGUIGENERATOR_HPP', 'LatticeLaw.hpp': 'LATTICELAW_HPP', 'Archive.hpp': 'ARCHIVECONTENT_HPP', 'InteractionGeometryMetaEngine.hpp': 'INTERACTION_GEOMETRY_METAENGINE_HPP', 'GLSimulationPlayerViewer.hpp': 'SIMULATIONVIEWER_HPP', 'DataRecorder.hpp': 'DATARECORDER_HPP', 'SphericalDEMSimulator.hpp': 'SPHERICALDEMSIMULATOR_HPP', 'Preferences.hpp': 'PREFERENCES_HPP', 'PersistentAloneSAPCollider.hpp': 'PERSISTENTSAPCOLLIDER_HPP', 'GLDrawInteractingMyTetrahedron.hpp': 'GLDRAWINTERACTINGMYTETRAHEDRON_HPP', 'MacroMicroElasticRelationshipsWater.hpp': 'SDECLINEARCONTACTMODEL_HPP', 'ElasticCohesiveLaw.hpp': 'ELASTICCOHESIVELAW_HPP', 'MyTetrahedronLaw.hpp': 'MYTETRAHEDRONLAW_HPP', 'IOManagerExceptions.hpp': 'IOMANAGEREXCEPTIONS_HPP', 'Engine.hpp': 'ENGINE_HPP', 'QtFileGenerator.hpp': 'QTFILEGENERATOR_HPP', 'PositionOrientationRecorder.hpp': 'POSITIONORIENTATIONRECORDER_HPP', 'StandAloneSimulator.hpp': 'STANDALONESIMULATOR_HPP', 'SimulationFlow.hpp': 'SIMULATION_FLOW_HPP', 'TriaxialStateRecorder.hpp': 'TRIAXIAL_STATE_RECORDER_HPP', 'Indexable.hpp': 'INDEXABLE_HPP', 'FEMTetrahedronData.hpp': 'LATTICEBEAMPARAMETERS_HPP', 'FunctorWrapper.hpp': 'DYNLIB_LAUNCHER_HPP', 'MetaDispatchingEngine.hpp': 'METADISPATCHINGENGINE_HPP', 'ThreadWorker.hpp': 'THREAD_WORKER_HPP', 'CundallNonViscousMomentumDamping.hpp': 'ACTION_MOMENTUM_DAMPING_HPP', 'GLDrawBox.hpp': 'GLDRAWBOX_HPP', 'SimpleElasticInteraction.hpp': 'SIMPLEELASTICINTERACTION_HPP', 'QtGUIPreferences.hpp': 'QTGUIPREFERENCES_HPP', 'InteractionContainerIteratorPointer.hpp': 'INTERACTIONCONTAINERITERATORPOINTER_HPP', 'BodyRedirectionVectorIterator.hpp': 'BODYREDIRECTIONVECTORITERATOR_HPP', 'MultiTypeHandler.tpp': '__MULTITYPEHANDLER_H__', 'FEMLaw.hpp': 'FEMLAW_HPP', 'MetaInteractingGeometry.hpp': 'METAINTERACTINGGEOMETRY_HPP', 'InteractingGeometry.hpp': 'INTERACTING_GEOMETRY_HPP', 'GLDrawBoundingSphere.hpp': 'GLDRAWBOUNDINGSPHERE_HPP', 'PhysicalActionApplierUnit.hpp': 'PHYSICALACTIONAPPLIERUNIT_HPP', 'TetraTestGen.hpp': 'TETRATESTGEN_HPP', 'GLDrawStateFunctor.hpp': 'GLDRAW_STATE_FUNCTOR_HPP', 'TriaxialTest.hpp': 'SDECIMPORT_HPP', 'GLViewer.hpp': 'GLVIEWER_HPP', 'yadeWm3Extra.hpp': 'YADE_GEOM_UTILS_HPP', 'InteractionHashMapIterator.hpp': 'INTERACTIONHASHMAPITERATOR_HPP', 'InteractionPhysics.hpp': 'INTERACTIONPHYSICS_HPP', 'CohesiveTriaxialTest.hpp': 'COHESIVE_TRIAXIAL_TEST_HPP', 'VelocityRecorder.hpp': 'VELOCITY_RECORDER_HPP', 'TetrahedronsTest.hpp': 'TETRAHEDRONSTEST_HPP', 'SerializableTypes.hpp': '__SERIALIZABLETYPES_HPP__', 'GLDrawRigidBodyState.hpp': 'GLDRAWRIGIDBODYSTATE_HPP', 'SerializationExceptions.hpp': 'SERIALIZATIONEXCEPTIONS_HPP', 'GLDrawInteractingSphere.hpp': '__GLDRAWINTERACTIONSPHERE_HPP__', 'RotatingBox.hpp': 'ROTATINGBOX_HPP', 'GLDrawMetaInteractingGeometry.hpp': 'GLDRAWCOLLISIONGEOMETRYSET_HPP', 'NullGUI.hpp': 'NULLGUI_HPP', 'TypeTraits.hpp': 'TYPETRAITS_INC_', 'BodyRedirectionVector.hpp': 'BODYREDIRECTIONVECTOR_HPP', 'InteractingGeometryEngineUnit.hpp': 'INTERACTING_GEOMETRY_ENGINE_UNIT_HPP', 'FileDialog.hpp': 'FILEDIALOG_HPP', 'SDECSpheresPlane.hpp': 'SDEC_SPHERES_PLANE_HPP', 'Clump.hpp': 'CLUMP_HPP', 'PythonRecorder.hpp': 'PYTHON_RECORDER_HPP', 'FEMSetTextLoader.hpp': 'FEM_SET_TEXT_LOADER_HPP', 'DisplacementEngine.hpp': 'DISPLACEMENTENGINE_HPP', 'CapillaryCohesiveLaw.hpp': 'CAPILLARY_COHESIVE_LAW_HPP', 'SerializableSingleton.hpp': 'SERIALIZABLESINGLETON_HPP', 'WallStressRecorder.hpp': 'WALL_STRESS_RECORDER_HPP', 'CohesiveFrictionalContactInteraction.hpp': 'COHESIVE_FRICTIONAL_CONTACT_PARAMETERS_HPP', 'GlobalStiffnessCounter.hpp': 'GLOBALSTIFFNESSCOUNTER_HPP', 'GLDrawInteractingGeometryFunctor.hpp': 'GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP', 'SimulationControllerUpdater.hpp': 'SIMULATIONCONTROLLERUPDATER_HPP', 'SimulationController.hpp': 'SIMULATIONCONTROLLER_HPP', 'SAPCollider.hpp': 'SAPCOLLIDER_HPP', 'SphericalDEM.hpp': 'SPHERICALDEM_HPP', 'Quadrilateral.hpp': 'QUADRILATERAL_HPP', 'yadeExceptions.hpp': 'YADE_EXCEPTIONS_HPP', 'GLDrawParticleState.hpp': 'GLDRAWPARTICLESTATE_HPP', 'TriaxialStressController.hpp': 'TRIAXIAL_STRESS_CONTROLLER_HPP', 'SDECLinkPhysics.hpp': 'SDECLINKPHYSICS_HPP', 'DistantPersistentSAPCollider.hpp': '__DISTANTPERSISTENTSAPCOLLIDER_HPP__', 'InteractingSphere.hpp': 'INTERACTIONSPHERE_HPP', 'SpringPhysics.hpp': 'SPRINGPHYSICS_HPP', 'Contact.hpp': 'CONTACT_HPP', 'PhysicalActionDamper.hpp': 'PHYSICALACTIONDAMPER_HPP', 'KnownFundamentalsHandler.tpp': 'KNOWNFUNDAMENTALSHANDLER_HPP', 'GLDrawGeometricalModelFunctor.hpp': 'GLDRAWGEOMETRICALMODELFUNCTOR_HPP', 'CapillaryPressureEngine.hpp': 'CAPILLARY_PRESSURE_ENGINE_HPP', 'NullType.hpp': 'NULLTYPE_INC_', 'PhysicalAction.hpp': 'PHYSICALACTION_HPP', 'Tetra.hpp': 'TETRA_HPP', 'GLDrawInteractionPhysicsFunctor.hpp': 'GLDRAW_INTERACTION_PHYSICS_FUNCTOR_HPP', 'PersistentSAPCollider.hpp': '__PERSISTENTSAPCOLLIDER_HPP__', 'GeometricalModelMetaEngine.hpp': 'GEOMETRICAL_MODEL_DISPATCHER_HPP', 'RenderingEngine.hpp': 'RENDERINGENGINE_HPP', 'Factorable.hpp': 'FACTORABLE_HPP', 'IOFormatManager.tpp': 'IOMANAGER_TPP', 'OpenGLWrapper.hpp': 'OPENGLWRAPPER_HPP', 'ElasticBodyParameters.hpp': 'ELASTICBODYPARAMETERS_HPP', 'GLDrawLatticeSetGeometry.hpp': 'GLDRAW_LATTICE_SET_GEOMETRY_HPP', 'InteractionContainerIterator.hpp': 'INTERACTIONCONTAINERITERATOR_HPP', 'QtGUI.hpp': 'QTGUI_HPP', 'MetaEngine.hpp': 'METAENGINE_HPP', 'InteractionOfMyTetrahedron.hpp': 'INTERACTIONOFMYTETRAHEDRON_HPP', 'AABB.hpp': 'AABB_HPP', 'BoundingVolume.hpp': 'BOUNDINGVOLUME_HPP', 'InteractionVecSetIterator.hpp': 'INTERACTIONVECSETITERATOR_HPP', 'PhysicalActionVectorVectorIterator.hpp': 'PHYSICALACTIONVECTORVECTORITERATOR_HPP', 'MacroMicroElasticRelationships.hpp': 'SDECLINEARCONTACTMODEL_HPP', 'GLDrawQuadrilateral.hpp': 'GLDRAW_QUADRILATERAL_HPP', 'DynLibManager.hpp': 'DYNLIBMANAGER_HPP', 'BodyAssocVector.hpp': 'BODYASSOCVEC_HPP', 'Typelist.hpp': 'TYPELIST_INC_', 'PointerHandler.tpp': '__POINTERHANDLER_H__', 'GLDrawLatticeInteractingGeometry.hpp': 'GLDRAW_LATTICE_INTERACTING_GEOMETRY_HPP', 'Box.hpp': 'BOX_HPP', 'ResultantForceEngine.hpp': 'RESULTANT_FORCE_ENGINE_HPP', 'XMLFormatManager.hpp': 'XMLFORMATMANAGER_HPP', 'GLDrawSDECLinkGeometry.hpp': 'GLDRAW_SPHERES_LINK_CONTACT_GEOMETRY_HPP', 'InteractionVecSet.hpp': 'INTERACTIONVECSET_HPP', 'FrontEnd.hpp': 'FRONTEND_HPP', 'ArchiveTypes.hpp': '__ARCHIVESTYPES_HPP__', 'Logging.hpp': 'LOGGING_HPP', 'MetaBody.hpp': 'METABODY_HPP', 'BodyContainerIteratorPointer.hpp': 'BODYCONTAINERITERATORPOINTER_HPP', 'ContactStressRecorder.hpp': 'CONTACT_STRESS_RECORDER_HPP', 'IOFormatManager.hpp': 'IOFORMATMANAGER_HPP', 'FrictionLessElasticContactLaw.hpp': 'FRICTIONLESSELASTICCONTACTLAW_HPP', 'ForceEngine.hpp': 'FORCE_ENGINE_HPP', 'GLDrawSphereShadowVolume.hpp': 'GLDRAWSPHERESHADOWVOLUME_HPP', 'BodyMacroParameters.hpp': 'BODYMACROPARAMETERS_HPP', 'GLWindow.hpp': 'GLWINDOW_HPP', 'PhysicalActionContainer.hpp': 'PHYSICALACTIONCONTAINER_HPP', 'RigidBodyParameters.hpp': 'RIGIDBODYPARAMETERS_HPP', 'GeometricalModelForceColorizer.hpp': 'GEOMETRICAL_MODEL_FORCE_COLORIZER_HPP', 'BodyAssocVectorIterator.hpp': 'BODYASSOCVECTORITERATOR', 'SpringGeometry.hpp': 'SPRINGGEOMETRY_HPP', 'ElasticContactLaw.hpp': 'ELASTIC_CONTACT_LAW_HPP', 'SDECLinkGeometry.hpp': 'SDECLINKGEOMETRY_HPP', 'GLDrawSphere.hpp': 'GLDRAWSPHERE_HPP', 'PhysicalActionApplier.hpp': 'PHYSICALACTIONAPPLIER_HPP', 'PhysicalActionContainerIterator.hpp': 'PHYSICALACTIONCONTAINERITERATOR_HPP', 'CohesiveFrictionalBodyParameters.hpp': 'COHESIVEFRICTIONALBODYPARAMETERS_HPP', 'GLDrawInteractingBox.hpp': 'GLDRAWINTERACTIONBOX_HPP', 'InteractionSolver.hpp': 'INTERACTIONSOLVER_HPP', 'GlobalStiffnessTimeStepper.hpp': 'STIFFNESS_MATRIX_TIME_STEPPER_HPP', 'InteractionContainer.hpp': 'INTERACTIONCONTAINER_HPP', 'Body.hpp': 'BODY_HPP', 'GLDrawClosestFeatures.hpp': 'GLDRAW_CLOSEST_FEATURES_HPP', 'FpsTracker.hpp': 'FPSTRACKER_HPP', 'ElasticContactInteraction.hpp': 'ELASTIC_CONTACT_PARAMETERS_HPP', 'ThreePointBending.hpp': 'LINKEDSPHERES_HPP', 'NonLocalInitializer.hpp': 'NONLOCALINITIALIZER_HPP', 'ContainerHandler.tpp': '__CONTAINERHANDLER_H__', 'GLDrawAABB.hpp': 'GLDRAWAABB_HPP', 'XMLSaxParser.hpp': 'XMLSAXPARSER_HPP', 'TimeStepper.hpp': 'TIMESTEPPER_HPP', 'MassSpringLaw.hpp': 'MASSSPRINGLAW_HPP', 'EngineUnit.hpp': 'ENGINEUNIT_HPP', 'ModifiedTriaxialTest.hpp': 'SDECIMPORT_HPP', 'QtEngineEditor.hpp': 'QTENGINEEDITOR_HPP', 'BoundingVolumeEngineUnit.hpp': 'BOUNDINGVOLUMEFACTORY_HPP', 'GlobalStiffness.hpp': 'GLOBALSTIFFNESSMATRIX_HPP', 'MultiMethodsExceptions.hpp': 'MULTIMETHODSEXCEPTIONS_HPP', 'Momentum.hpp': 'MOMENTUM_HPP', 'PhysicalActionVectorVector.hpp': 'PHYSICALACTIONVECTORVECTOR_HPP', 'QtCodeGenerator.hpp': 'QTCODEGENERATOR_HPP', 'GLTextLabel.hpp': 'GLTEXTLABEL_HPP', 'YadeQtMainWindow.hpp': 'YADEQTMAINWINDOW_HPP', 'NonLocalDependency.hpp': 'NONLOCALDEPENDENCY_HPP', 'Force.hpp': 'ACTIONFORCE_HPP', 'ParticleSetParameters.hpp': 'PARTICLESETPARAMETERS_HPP', 'TriaxialCompressionEngine.hpp': 'TRIAXIALCOMPRESSIONENGINE_HPP', 'CundallNonViscousForceDamping.hpp': 'ACTION_FORCE_DAMPING_HPP', 'DynLibDispatcher.hpp': 'DYNLIB_DISPATCHER_HPP', 'FileGenerator.hpp': 'FILEGENERATOR_HPP', 'Omega.hpp': 'OMEGA_HPP', 'SimpleElasticRelationships.hpp': 'SIMPLECONTACTMODEL_HPP', 'RotationEngine.hpp': 'ROTATIONENGINE_HPP', 'InteractingMyTetrahedron.hpp': 'INTERACTING_MY_TETRAHEDRON_HPP', 'GLDrawLineSegment.hpp': 'GLDRAW_LINE_SEGMENT_HPP', 'cmdGui.hpp': 'CMDGUI_HPP', 'LatticeBeamAngularSpring.hpp': 'LATTICEBEAMANGULARSPRING_HPP', 'ForceRecorder.hpp': 'FORCE_RECORDER_HPP', 'QtPreferencesEditor.hpp': 'QTPREFERENCESEDITOR_HPP', 'GLDrawSpheresContactGeometry.hpp': 'GLDRAW_SPHERES_CONTACT_GEOMETRY_HPP', 'LatticeInteractingGeometry.hpp': 'LATTICE_INTERACTINGGEOMETRY_HPP', 'StandAloneEngine.hpp': 'STANDALONEENGINE_HPP', 'GLDrawBoundingVolumeFunctor.hpp': 'GLDRAWBOUNDINGVOLUMEFUNCTOR_HPP', 'NewtonsForceLaw.hpp': 'NEWTONSFORCELAW_HPP', 'BoundingSphere.hpp': 'BOUNDINGSPHERE_HPP', 'MeasurePoisson.hpp': 'POISSON_RECORDER_HPP', 'DeusExMachina.hpp': 'KINEMATICENGINE_HPP', 'CapillaryParameters.hpp': 'CAPILLARY_PARAMETERS_HPP', 'AssocVector.hpp': 'ASSOCVECTOR_INC_', 'PhysicalActionDamperUnit.hpp': 'PHYSICALACTIONDAMPERUNIT_HPP', 'Interaction.hpp': 'INTERACTION_HPP', 'CohesiveFrictionalRelationships.hpp': 'COHESIVEFRICTIONALCONTACTMODEL_HPP', 'BoundingVolumeMetaEngine.hpp': 'BOUNDINGVOLUMEUPDATOR_HPP', 'InteractionHashMap.hpp': 'INTERACTIONHASHMAP_HPP', 'SDECLinkedSpheres.hpp': 'LINKEDSPHERES_HPP', 'BodyContainer.hpp': 'BODYCONTAINER_HPP', 'ElasticCriterionTimeStepper.hpp': 'ELASTIC_CRITERION_TIME_STEPPER_HPP'}

# must modify these by hand
skipEndifs={'Omega.hpp':1,'yadeWm3Extra.hpp':2,'Logging.hpp':1,'DynLibManager.hpp':1,'TypeManip.hpp':1,'PythonRecorder.hpp':1}


def grepInc(path,f,module):
	fullF=path+sep+f
	print fullF
	if 1:
		bcUp='/tmp/'+f+'~~'
		shutil.move(fullF,bcUp);
		f2=open(fullF,'w')
	else: bcUp=fullF
	if inclusionGuards.has_key(f): guard=inclusionGuards[f]
	else: guard=None
	toSkip=0
	if f in skipEndifs.keys(): toSkip=skipEndifs[f]
	ifndefLevel=0
	seenIncMod=[]
	for l in open(bcUp):
		m=re.match('^#include<yade/([^/]*)/(.*)>.*$',l)
		if m:
			incMod=m.group(1)
			assert(incMod in incModules.keys())
			assert(incModules[incMod]<=incModules[module])
			if incMod!=module:
				if incMod not in seenIncMod:
					l='#include<yade/'+incMod+'.hh>\n'
					seenIncMod.append(incMod)
				else: l=''
		if guard:
			m=re.match('^#ifndef\s+'+guard+'\s*$',l)
			if m:
				l='#pragma once\n'; ifndefLevel+=1
			m=re.match('^#define\s+'+guard+'\s*$',l)
			if m: l=''
			m=re.match('^#endif.*$',l)
			if m:
				if toSkip>0: toSkip-=1
				else:
					l=''; ifndefLevel-=1
		f2.write(l);
	assert(ifndefLevel==0)
	f2.close()

		#if m and guard==None:
		#	guard=m.group(1)
		#	#print f,guard
		#m=re.match('^#define ([A-Z_]*)$',l)
		#if m and guard!=None:
		#	guardDefined=True
		#m=re.match('#endif.*',l)
		#if m and guard!=None and guardDefined:
		#	inclusionGuards[f]=guard

moduleHeaders={}
for root, dirs, files in os.walk(srcRoot,topdown=True):
	for d in ('.svn'): ## skip all files that are not part of sources in the proper sense!
		try: dirs.remove(d)
		except ValueError: pass
	for f in files:
		if f.split('.')[-1] in ('hpp','inl','ipp','tpp','h','mcr','cpp','cc','C'):
			m=re.match('^.*?'+sep+'((extra|core)|((gui|lib|pkg)'+sep+'.*?))(|'+sep+'.*)$',root)
			module=m.group(1).replace(sep,'-')
			# add headers to encompassing header file
			if f.split('.')[-1] in ('hpp','h'):
				if moduleHeaders.has_key(module): moduleHeaders[module].append(f)
				else: moduleHeaders[module]=[f]
			assert(module in incModules.keys())
			grepInc(root,f,module)

for module in moduleHeaders.keys():
	f=open(pchDir+sep+module+'.hh','w')
	for hh in moduleHeaders[module]:
		f.write('#include<yade/'+module+'/'+hh+'>\n')

