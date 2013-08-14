# -*- coding: utf-8 -*-

from yade import pack, utils, ymport, export

packing='parallellepiped_10'
DFN='persistentPlane30Deg'

############################ material definition
facetMat = O.materials.append(JCFpmMat(type=0,young=1,frictionAngle=radians(1),poisson=0.4,density=1))
def sphereMat(): return JCFpmMat(type=1,young=1,frictionAngle=radians(1),density=1)

############################ Import of the sphere assembly
O.bodies.append(ymport.text(packing+'.spheres',scale=1,shift=Vector3(0,0,0),material=sphereMat)) #(-3,-4,-8)

#### some preprocessing (not mandatory)
dim=utils.aabbExtrema()
xinf=dim[0][0]
xsup=dim[1][0]
yinf=dim[0][1]
ysup=dim[1][1]
zinf=dim[0][2]
zsup=dim[1][2]

R=0
Rmax=0
numSpheres=0.
for o in O.bodies:
 if isinstance(o.shape,Sphere):
   o.shape.color=(0,0,1)
   numSpheres+=1
   R+=o.shape.radius
   if o.shape.radius>Rmax:
     Rmax=o.shape.radius
 else :
   o.shape.color=(0,0,0)
Rmean=R/numSpheres

print 'number of spheres=', numSpheres, ' | Rmean=', Rmean, ' | dim=', dim

############################ import stl file
O.bodies.append(ymport.stl(DFN+'.stl',color=(0.9,0.9,0.9),wire=False,material=facetMat)) 

############################ engines definition
interactionRadius=1.;
O.engines=[

	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=interactionRadius,label='is2aabb'),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=interactionRadius,label='ss2d3dg'),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_JCFpmMat_JCFpmMat_JCFpmPhys(cohesiveTresholdIteration=1,alpha=1,tensileStrength=1e6,cohesion=1e6,jointNormalStiffness=1,jointShearStiffness=1,jointTensileStrength=1e6,jointCohesion=1e6,jointFrictionAngle=1,label='interactionPhys')],
		[Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM(smoothJoint=True,label='interactionLaw')]
	),
	NewtonIntegrator(damping=1)

]

############################ timestep + opening yade windows
O.dt=0.001*utils.PWaveTimeStep()

from yade import qt
v=qt.Controller()
v=qt.View()

############################ Identification spheres on joint
#### color set for particles on joint
jointcolor1=(0,1,0)
jointcolor2=(1,0,0)
jointcolor3=(0,0,1)
jointcolor4=(1,1,1)
jointcolor5=(0,0,0)

#### first step-> find spheres on facet
O.step();

for i in O.interactions:
    ##if not i.isReal : continue
    if isinstance(O.bodies[i.id1].shape,Facet) and isinstance(O.bodies[i.id2].shape,Sphere): 
	vertices=O.bodies[i.id1].shape.vertices
	normalRef=vertices[0].cross(vertices[1]) # defines the normal to the facet normalRef
	nRef=normalRef/(normalRef.norm()) ## normalizes normalRef
	normalFacetSphere=i.geom.normal # geom.normal is oriented from id1 to id2 -> normalFacetSphere from facet (i.id1) to sphere (i.id2)

	if O.bodies[i.id2].mat.onJoint==False : ## particles has not yet been identified as belonging to a joint plane
	    O.bodies[i.id2].mat.onJoint=True
	    O.bodies[i.id2].mat.joint=1
	    O.bodies[i.id2].shape.color=jointcolor1
	    if nRef.dot(normalFacetSphere)>=0 :
		O.bodies[i.id2].mat.jointNormal1=nRef
	    elif nRef.dot(normalFacetSphere)<0 :
		O.bodies[i.id2].mat.jointNormal1=-nRef
	elif O.bodies[i.id2].mat.onJoint==True :  ## particles has already been identified as belonging to, at least, 1 facet
	    if O.bodies[i.id2].mat.joint==1 and ((O.bodies[i.id2].mat.jointNormal1.cross(nRef)).norm()>0.05) : ## particles has already been identified as belonging to only 1 facet
		O.bodies[i.id2].mat.joint=2
		O.bodies[i.id2].shape.color=jointcolor2
		if nRef.dot(normalFacetSphere)>=0 :
		    O.bodies[i.id2].mat.jointNormal2=nRef
		elif nRef.dot(normalFacetSphere)<0 :
		    O.bodies[i.id2].mat.jointNormal2=-nRef
	    elif O.bodies[i.id2].mat.joint==2 and ((O.bodies[i.id2].mat.jointNormal1.cross(nRef)).norm()>0.05) and ((O.bodies[i.id2].mat.jointNormal2.cross(nRef)).norm()>0.05): ## particles has already been identified as belonging to more than 1 facet
		O.bodies[i.id2].mat.joint=3
		O.bodies[i.id2].shape.color=jointcolor3
		if nRef.dot(normalFacetSphere)>=0 :
		    O.bodies[i.id2].mat.jointNormal3=nRef
		elif nRef.dot(normalFacetSphere)<0 :
		    O.bodies[i.id2].mat.jointNormal3=-nRef
	    elif O.bodies[i.id2].mat.joint==3 and ((O.bodies[i.id2].mat.jointNormal1.cross(nRef)).norm()>0.05) and ((O.bodies[i.id2].mat.jointNormal2.cross(nRef)).norm()>0.05) and ((O.bodies[i.id2].mat.jointNormal3.cross(nRef)).norm()>0.05):
		O.bodies[i.id2].mat.joint=4
		O.bodies[i.id2].shape.color=jointcolor5

#### second step -> find spheres interacting with spheres on facet (could be executed in the same timestep as step 1?)
for j in O.interactions:
    #if not i.isReal : continue
    if isinstance(O.bodies[j.id1].shape,Facet) and isinstance(O.bodies[j.id2].shape,Sphere):
	vertices=O.bodies[j.id1].shape.vertices
	normalRef=vertices[0].cross(vertices[1]) # defines the normal to the facet normalRef
	nRef=normalRef/(normalRef.norm()) ## normalizes normalRef
	if ((O.bodies[j.id2].mat.jointNormal1.cross(nRef)).norm()<0.05) :
	    jointNormalRef=O.bodies[j.id2].mat.jointNormal1
	elif ((O.bodies[j.id2].mat.jointNormal2.cross(nRef)).norm()<0.05) :
	    jointNormalRef=O.bodies[j.id2].mat.jointNormal2
	elif ((O.bodies[j.id2].mat.jointNormal3.cross(nRef)).norm()<0.05) :
	    jointNormalRef=O.bodies[j.id2].mat.jointNormal3
	else : continue
	facetCenter=O.bodies[j.id1].state.pos
	#### seek for each sphere interacting with the identified sphere i.id2
	for n in O.interactions.withBody(j.id2) :
	    if n.id1==j.id2 and isinstance(O.bodies[n.id2].shape,Sphere): 
		facetSphereDir=(O.bodies[n.id2].state.pos-facetCenter)
		if O.bodies[n.id2].mat.onJoint==True :
		    if O.bodies[n.id2].mat.joint==3 and ((O.bodies[n.id2].mat.jointNormal1.cross(jointNormalRef)).norm()>0.05) and ((O.bodies[n.id2].mat.jointNormal2.cross(jointNormalRef)).norm()>0.05) and ((O.bodies[n.id2].mat.jointNormal3.cross(jointNormalRef)).norm()>0.05):
			O.bodies[n.id2].mat.joint=4
			O.bodies[n.id2].shape.color=jointcolor5
		    elif O.bodies[n.id2].mat.joint==2 and ((O.bodies[n.id2].mat.jointNormal1.cross(jointNormalRef)).norm()>0.05) and ((O.bodies[n.id2].mat.jointNormal2.cross(jointNormalRef)).norm()>0.05):
			O.bodies[n.id2].mat.joint=3
			if facetSphereDir.dot(jointNormalRef)>=0:
			    O.bodies[n.id2].mat.jointNormal3=jointNormalRef
			elif facetSphereDir.dot(jointNormalRef)<0:
			    O.bodies[n.id2].mat.jointNormal3=-jointNormalRef 
		    elif O.bodies[n.id2].mat.joint==1 and ((O.bodies[n.id2].mat.jointNormal1.cross(jointNormalRef)).norm()>0.05) :
			O.bodies[n.id2].mat.joint=2
			if facetSphereDir.dot(jointNormalRef)>=0:
			    O.bodies[n.id2].mat.jointNormal2=jointNormalRef
			elif facetSphereDir.dot(jointNormalRef)<0:
			    O.bodies[n.id2].mat.jointNormal2=-jointNormalRef
		elif  O.bodies[n.id2].mat.onJoint==False :
		    O.bodies[n.id2].mat.onJoint=True
		    O.bodies[n.id2].mat.joint=1
		    O.bodies[n.id2].shape.color=jointcolor4
		    if facetSphereDir.dot(jointNormalRef)>=0:
			O.bodies[n.id2].mat.jointNormal1=jointNormalRef
		    elif facetSphereDir.dot(jointNormalRef)<0:
			O.bodies[n.id2].mat.jointNormal1=-jointNormalRef

	    elif n.id2==j.id2 and isinstance(O.bodies[n.id1].shape,Sphere): 
		facetSphereDir=(O.bodies[n.id1].state.pos-facetCenter)
		if O.bodies[n.id1].mat.onJoint==True :
		    if O.bodies[n.id1].mat.joint==3 and ((O.bodies[n.id1].mat.jointNormal1.cross(jointNormalRef)).norm()>0.05) and ((O.bodies[n.id1].mat.jointNormal2.cross(jointNormalRef)).norm()>0.05) and ((O.bodies[n.id1].mat.jointNormal3.cross(jointNormalRef)).norm()>0.05):
			O.bodies[n.id1].mat.joint=4
			O.bodies[n.id1].shape.color=jointcolor5
		    elif O.bodies[n.id1].mat.joint==2 and ((O.bodies[n.id1].mat.jointNormal1.cross(jointNormalRef)).norm()>0.05) and ((O.bodies[n.id1].mat.jointNormal2.cross(jointNormalRef)).norm()>0.05):
			O.bodies[n.id1].mat.joint=3
			if facetSphereDir.dot(jointNormalRef)>=0:
			    O.bodies[n.id1].mat.jointNormal3=jointNormalRef
			elif facetSphereDir.dot(jointNormalRef)<0:
			    O.bodies[n.id1].mat.jointNormal3=-jointNormalRef
		    elif O.bodies[n.id1].mat.joint==1 and ((O.bodies[n.id1].mat.jointNormal1.cross(jointNormalRef)).norm()>0.05) :
			O.bodies[n.id1].mat.joint=2
			if facetSphereDir.dot(jointNormalRef)>=0:
			    O.bodies[n.id1].mat.jointNormal2=jointNormalRef
			elif facetSphereDir.dot(jointNormalRef)<0:
			    O.bodies[n.id1].mat.jointNormal2=-jointNormalRef
		elif  O.bodies[n.id1].mat.onJoint==False :
		    O.bodies[n.id1].mat.onJoint=True
		    O.bodies[n.id1].mat.joint=1
		    O.bodies[n.id1].shape.color=jointcolor4
		    if facetSphereDir.dot(jointNormalRef)>=0:
			O.bodies[n.id1].mat.jointNormal1=jointNormalRef
		    elif facetSphereDir.dot(jointNormalRef)<0:
			O.bodies[n.id1].mat.jointNormal1=-jointNormalRef


#### for visualization:
#bj=0
#vert=(0.,1.,0.)
#hor=(0.,1.,1.)
#for o in O.bodies:
    #if o.mat.onJoint==True : # or o.shape.name=='Facet':
	##if  o.shape.name=='Facet':
	    ##o.shape.wire=True
	##o.state.pos+=(0,50,0)
	##bj+=1
	#if o.mat.jointNormal1.dot(hor)>0 :
	    ##o.state.pos+=(0,50,0)
	    #o.shape.color=jointcolor1
	#elif o.mat.jointNormal1.dot(hor)<0 :
	    ##o.state.pos+=(0,55,0)
	    #o.shape.color=jointcolor2
	#if o.mat.type>2 :
	    #bj+=1
	    #o.shape.color=jointcolor5
	    ##print o.mat.jointNormal.dot(vert)


#### Save text file with informations on each sphere
export.text(packing+'_'+DFN+'.spheres')
export.textExt(packing+'_'+DFN+'_jointedPM.spheres',format='jointedPM')

O.wait()
