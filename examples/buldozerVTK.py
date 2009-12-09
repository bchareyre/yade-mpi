### Simpificated buldozer simulation with VTK recorder
from yade import utils
from numpy import linspace
from numpy import arange
import gts
import itertools
from yade import pack


###Initial Data
numKnifeParts = 10
radiusKnife=1
lengthKnife=2
radiusSph = 0.05
numBoxes = Vector3(8,5,2)
gapBetweenBoxes = 0.05
sizeBox = (lengthKnife-(numBoxes[1]-1)*gapBetweenBoxes)/numBoxes[1]

## PhysicalParameters 
Density=2400
frictionAngle=radians(35)
tc = 0.001
en = 0.3
es = 0.3

### Creating the Buldozer Knife
### from facets, using GTS
Knife=[]
for i in linspace(pi, pi*3/2, num=numKnifeParts, endpoint=True):
	Knife.append(Vector3(radiusKnife*cos(i),0,radiusKnife*sin(i)))
	
KnifeP=[Knife,[p+Vector3(0,lengthKnife,0) for p in Knife]]
KnifePoly=pack.sweptPolylines2gtsSurface(KnifeP,threshold=1e-4)
KnifeIDs=O.bodies.append(pack.gtsSurface2Facets(KnifePoly,color=(1,0,0),wire=False,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters",**utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)))
O.bodies.append(utils.facetBox((0,0,radiusKnife),(lengthKnife*3,lengthKnife*3,lengthKnife),wallMask=16,color=(1,1,1),wire=False,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters",**utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)))


### Creating the material for buldozer
colorsph1=Vector3(120,234,150);
colorsph2=Vector3(0,0,1);

colorsph1.Normalize();
colorsph2.Normalize();
colorSph=colorsph1
for xyz in itertools.product(arange(0,numBoxes[0]),arange(0,numBoxes[1]),arange(0,numBoxes[2])):
	ids_spheres=O.bodies.appendClumped(pack.regularHexa(pack.inEllipsoid((xyz[0]*(sizeBox+gapBetweenBoxes),xyz[1]*(sizeBox+gapBetweenBoxes)+sizeBox*0.5,xyz[2]*(sizeBox+gapBetweenBoxes)-radiusKnife+sizeBox*0.6),(sizeBox/2,sizeBox/2,sizeBox/2)),radius=radiusSph,gap=0,color=colorSph,density=Density,frictionAngle=frictionAngle, physParamsClass="SimpleViscoelasticBodyParameters"))
	for id in ids_spheres[1]:
		s=O.bodies[id]
		p=utils.getViscoelasticFromSpheresInteraction(s.phys['mass'],tc,en,es)
		s.phys['kn'],s.phys['cn'],s.phys['ks'],s.phys['cs']=p['kn'],p['cn'],p['ks'],p['cs']
	if (colorSph==colorsph1):
		colorSph=colorsph2
	else:
		colorSph=colorsph1

O.dt=.2*tc

O.initializers=[ BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]) ]

O.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[InteractingSphere2InteractingSphere4SpheresContactGeometry(), InteractingFacet2InteractingSphere4SpheresContactGeometry()],
		[SimpleViscoelasticRelationships()],
		[ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw()],
	),
	GravityEngine(gravity=[0,0,-9.8]),
	TranslationEngine(translationAxis=[1,0,0],velocity=2,subscribedBodies=KnifeIDs), # Buldozer motion
	NewtonIntegrator(damping=0),
	VTKRecorder(virtPeriod=0.01,fileName='/tmp/buldozer-',recorders=['spheres','facets'])
]

O.saveTmp()
from yade import qt
qt.Controller()
qt.View()
r=qt.Renderer()
r['Light_position']=Vector3(0,0,50)
O.run()
#qt.makeSimulationVideo('/tmp/buldozer.ogg',iterPeriod=1000,fps=30)

