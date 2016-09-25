## CWBoon 2016


#################################### STRESS CONDITIONS #######################################
K0 = 1.0
gravityMultiplier = K0
sigmaZ0 = 0.0 
density = 2700
unitWeight = 9.81*density
initialFriction = 1.0 #14 31 7, 11(1.5) 21(0.5)
boundaryFriction = 35.0
roofID = 0
sideWallID = 0
scaleMass = 1.0
radius = 5.0
diameter = radius*2.0
sigmaV_height = diameter*2.0 
##############################################################################################

from yade import utils
utils.readParamsFromTable(
        K0 = K0,
	boundaryFriction = 1.0*boundaryFriction,
        noTableOk=True,     # use default values if not run in batch
)
if utils.runningInBatch(): from yade.params.table import *


p=BlockGen()
p.minSize = 1.0 #0.5 ori
p.maxRatio = 10.0 #5.0
p.density=2700
p.dampingMomentum =0.5
p.damp3DEC=0.0
p.viscousDamping = 0.0
p.Kn = 5e9
p.Ks= 0.5e9
p.frictionDeg = initialFriction
p.traceEnergy = False
p.defaultDt = 1e-9
p.rForPP = 0.05 #0.0001 ori0.05
p.kForPP = 0.05 
p.gravity = [0,0,9.81]
p.inertiaFactor = 1.0 
p.numberOfGrids=100
p.exactRotation = False
p.shrinkFactor = 1.00 #1.1
p.initialOverlap = 1e-6
'''
p.boundarySizeXmax = 4.0*radius -p.rForPP
p.boundarySizeYmax = 0.5*(1.0-2.0*p.rForPP)	
p.boundarySizeZmax = 4.0*radius-  p.rForPP	
p.boundarySizeXmin = -4.0*radius + p.rForPP	
p.boundarySizeYmin = -0.5*(1.0-2.0*p.rForPP)
'''
p.boundarySizeXmax = 3.5*diameter -p.rForPP
p.boundarySizeYmax = 0.5*(1.0-2.0*p.rForPP)	
p.boundarySizeZmax = 3.5*diameter-  p.rForPP	
p.boundarySizeXmin = -3.5*diameter + p.rForPP	
p.boundarySizeYmin = -0.5*(1.0-2.0*p.rForPP)	
p.boundarySizeZmin = -sigmaV_height - radius + p.rForPP	
p.RForPP = abs(p.boundarySizeZmin)*4
p.persistentPlanes = False
p.jointProbabilistic = True
p.brittleLength = p.minSize*0.3
p.maxClosure = 0.005
p.subdivisionRatio = 0.01
p.opening = True
p.boundaries = False
p.slopeFace = False
p.calJointLength = True
p.twoDimension = True
p.unitWidth2D = p.boundarySizeYmax*2 + 2*p.rForPP
p.calAreaStep = 30
p.useFaceProperties = False
p.neverErase = False 
p.filenameOpening = '/home/booncw/yadeRev/trunk/examples/Tunnel/joints/circleCavern.csv'   # update this to appropriate folder
p.filenameProbabilistic ='/home/booncw/yadeRev/trunk/examples/Tunnel/joints/old/reducedG2/statistics/G3joints155_75_25A.csv'  # update this to appropriate folder
p.directionA=Vector3(1,0,0) #(-0.9858,0, -0.1676)
p.directionB=Vector3(0,1,0)
p.directionC=Vector3(0,0,1) #(-0.865288, 0, 0.501275)
p.load()
import yade.timing
O.timingEnabled = True
yade.timing.reset()
		
calculateTimeStepAfterSupport = False
supportInstalled = False
supportInstalled1 = False
supportInstalled2 = False
supportInstalled4 = False
# material
young = 600.0e6 # [N/m^2]
poisson = 0.6 
frictionAngle=radians(0.0) # [rad]
# append geometry and material
O.materials.append(FrictMat(young=young,poisson=poisson,density=p.density,frictionAngle=frictionAngle, label='frictionless'))

for b in O.bodies:
	b.state.blockedDOFs= 'yXZ' #['y','rx','rz']
	for b in O.bodies:
		posX = b.state.pos[0]
		posZ = b.state.pos[2]
		if posZ >  -1.25*radius  and posZ < -radius and posX > 0.5*radius and posX < 0.5*radius:
			roofID = b.id
		if posZ >  -0.5*radius  and posZ < 0.5*radius and posX > radius and posX < 1.25*radius:
			sideWallID = b.id


oriRoofPos = O.bodies[roofID].state.pos
oriSideWallPos = O.bodies[sideWallID].state.pos


#LATERAL BOUNDARIES
bodyY=[]
bodyY2 = []
number = 200
thickness = 0.3*radius
totalLength = abs(p.boundarySizeZmin)+p.boundarySizeZmax+2.0*p.rForPP
idCountBegin = len(O.bodies)

for i in range(0,number):
	bodyY.append(Body())
	bodyY2.append(Body())
	length = totalLength/number
	wire=False
	color=[0,0,1]
	highlight=False
	kPP = p.kForPP
	rPP = p.rForPP
	RPP = 0.5*length
	aPP = [1,-1,0,0,0,0]
	bPP = [0,0,1,-1,0,0]
	cPP = [0,0,0,0,1,-1]
	dPP = [thickness,thickness,0.5*p.unitWidth2D-rPP,0.5*p.unitWidth2D-rPP,0.5*length-rPP,0.5*length-rPP]
	minmaxAabb = Vector3(1.05*(dPP[0]+rPP),1.05*(dPP[2]-rPP),1.05*(dPP[4]+rPP))
	bodyY[i].shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP, b=bPP, c=cPP, d=dPP,id=idCountBegin+i,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,fixedNormal=True,boundaryNormal=Vector3(1,0,0))
	bodyY2[i].shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP, b=bPP, c=cPP, d=dPP,id=idCountBegin+i+number,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,fixedNormal=True,boundaryNormal=Vector3(-1,0,-0))	
	V = (2*(thickness+rPP))*(length)*(p.unitWidth2D)
	geomInertX = 1.0/12*V*p.density*(p.unitWidth2D**2 + length**2) 
	geomInertY = 1.0/12*V*p.density*((2.0*thickness+2.0*rPP)**2 + length**2) 
	geomInertZ = 1.0/12*V*p.density*((2.0*thickness+2.0*rPP)**2 + p.unitWidth2D**2) 
	utils._commonBodySetup(bodyY[i],V,Vector3(geomInertX,geomInertY,geomInertZ), material='frictionless',pos= [p.boundarySizeXmin-rPP-thickness-rPP,0.0,p.boundarySizeZmin-p.rForPP + 0.5*length + length*i], dynamic=True, fixed=False) 
	bodyY[i].state.pos = [p.boundarySizeXmin-rPP-thickness-rPP,0.0,p.boundarySizeZmin-p.rForPP + 0.5*length + length*i]
	bodyY[i].state.blockedDOFs='zyXZY' #['z','y','rx','rz','ry']
	bodyY[i].shape.isBoundary=True
	bodyY[i].dynamic=True
	bodyY[i].state.mass = p.density*V*scaleMass
	utils._commonBodySetup(bodyY2[i],V,Vector3(geomInertX,geomInertY,geomInertZ), material='frictionless',pos= [p.boundarySizeXmax+ rPP+thickness+rPP,0.0,p.boundarySizeZmin-p.rForPP + 0.5*length + length*i], dynamic=True, fixed=False) 
	bodyY2[i].state.pos = [p.boundarySizeXmax+ rPP+thickness+rPP,0.0,p.boundarySizeZmin-p.rForPP + 0.5*length + length*i]
	bodyY2[i].state.blockedDOFs='zyXZY' #['z','y','rx','rz','ry'] ['z','y','rx','rz','ry']
	bodyY2[i].shape.isBoundary=True
	bodyY2[i].dynamic=True
	bodyY2[i].state.mass = p.density*V*scaleMass
	leftID = O.bodies.append(bodyY[i])
	rightID = O.bodies.append(bodyY2[i])
	Ixx = 1.0/12*p.unitWidth2D*length**3
	Moment = K0*unitWeight*Ixx
	AxialStress = K0*unitWeight*(abs(bodyY2[i].state.pos[2]+ abs(p.boundarySizeZmin) + p.rForPP))
	AxialForce = AxialStress*p.unitWidth2D*length
	O.engines = O.engines[:3]+ [ForceEngine(force=(AxialForce,0,0),ids=[leftID]) ] + O.engines[3:]
	O.engines = O.engines[:3]+ [ForceEngine(force=(-AxialForce,0,0),ids=[rightID]) ] + O.engines[3:]
	#O.engines = O.engines[:3]+ [TorqueEngine(moment=(0,Moment,0),ids=[leftID]) ] + O.engines[3:]
	#O.engines = O.engines[:3]+ [TorqueEngine(moment=(0,-Moment,0),ids=[rightID]) ] + O.engines[3:]



#VERTICAL BOUNDARIES
bBottom = Body()
wire=False
color=[0,0,1]
highlight=False
kPP = p.kForPP
rPP = p.rForPP
RPP = 0.5*p.boundarySizeZmax
aPP = [1,-1,0,0,0,0]
bPP = [0,0,1,-1,0,0]
cPP = [0,0,0,0,1,-1]
dPP = [p.boundarySizeXmax , p.boundarySizeXmax ,0.5*p.unitWidth2D-rPP,0.5*p.unitWidth2D-rPP,thickness,thickness]
minmaxAabb = Vector3(1.05*(dPP[0]+rPP),1.05*(dPP[2]-rPP),1.05*(dPP[4]+rPP))
bBottom.shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP, b=bPP, c=cPP, d=dPP,id=idCountBegin+i+number+2,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,fixedNormal=True,boundaryNormal=Vector3(0,0,-1))	
V = (2*(thickness+rPP))*(p.boundarySizeXmax+rPP)*(p.unitWidth2D)
geomInert = 1/12*(p.unitWidth2D)*(length**3)
geomInertX = 1/12*V*p.density*(p.unitWidth2D**2 + (2.0*thickness+2.0*rPP)**2 ) 
geomInertY = 1/12*V*p.density*((2.0*p.boundarySizeXmax+2.0*rPP)**2 + (2.0*thickness+2.0*rPP)**2 ) 
geomInertZ = 1/12*V*p.density*((2.0*p.boundarySizeXmax+2.0*rPP)**2 + p.unitWidth2D**2) 
utils._commonBodySetup(bBottom,V,Vector3(geomInertX,geomInertY,geomInertZ), material='frictionless',pos= [0.0,0.0,p.boundarySizeZmax+rPP+thickness+rPP], dynamic=True, fixed=True) 
bBottom.state.pos = [0.0,0.0,p.boundarySizeZmax+rPP+thickness+rPP]
bBottom.shape.isBoundary=True
bBottom.state.mass = p.density*V*scaleMass
bBottom.dynamic=False
O.bodies.append(bBottom)




for b in O.bodies:
	b.state.refPos=b.state.pos


for e in O.engines:
	if e.__class__.__name__== 'InteractionLoop':
		contactLaw = e.lawDispatcher.functors[0]
		physDispatcher = e.physDispatcher.functors[0]
	if e.__class__.__name__ == 'NewtonIntegrator':
		newtonIntegrator = e

############################################
#####             Graphs               #####
############################################

from math import *
from yade import plot

def opening(x,y,z):
	d = 5.0
	p1 = -1.0*x - d
	if p1 < 0.0:
		p1 = 0.0
	p2 = 1.0*x - d
	if p2 < 0.0:
		p2 = 0.0
	p3 = 0.5*x - 0.866*z - d
	if p3 < 0.0:
		p3 = 0.0
	p4 = -0.5*x - 0.866*z - d
	if p4 < 0.0:
		p4 = 0.0
	p5 = 0.866*x - 0.5*z - d
	if p5 < 0.0:
		p5 = 0.0
	p6 = -0.866*x - 0.5*z -d
	if p6 < 0.0:
		p6 = 0.0
	p7 = -1.0*z - d 
	if p7 < 0.0:
		p7 = 0.0
	p8 = 1.0*z - 2.5 #base of horseshoe
	if p8 < 0.0:
		p8 = 0.0
	return p1+p2+p3+p4+p5+p6+p7+p8

def openingBenchmark(x,y,z):
	d = 5.0
	p1 = x - d
	p2 = 0.9914448614*x + 0.1305261922*z - d
	p3 = 0.9659258263*x + 0.2588190451*z -d
	p4 = 0.9238795325*x + 0.3826834324*z -d
	p5 = 0.8660254038*x + 0.5*z - d
	p6 = 0.7933533403*x + 0.608761429*z - d
	p7 = 0.7071067812*x + 0.7071067812*z - d
	p8 = 0.608761429*x + 0.7933533403*z - d
	p9 = 0.5*x + 0.8660254038*z - d
	p10 = 0.3826834324*x + 0.9238795325*z - d
	p11 = 0.2588190451*x + 0.9659258263*z - d
	p12 = 0.1305261922*x + 0.9914448614*z - d
	p13 =  z - d
	p14 = -0.1305261922*x + 0.9914448614*z - d
	p15 =-0.2588190451*x + 0.9659258263*z - d
	p16 =-0.3826834324*x + 0.9238795325*z - d
	p17 =-0.5*x + 0.8660254038*z - d
	p18 =-0.608761429*x + 0.7933533403*z - d
	p19 =-0.7071067812*x + 0.7071067812*z - d
	p20 =-0.7933533403*x + 0.608761429*z - d
	p21 =-0.8660254038*x + 0.5*z - d
	p22 =-0.9238795325*x + 0.3826834324*z - d
	p23 =-0.9659258263*x + 0.2588190451*z - d
	p24 =-0.9914448614*x + 0.1305261922*z - d
	p25 = -x -d
	p26 =-0.9914448614*x  -0.1305261922*z - d
	p27 = -0.9659258263*x -0.2588190451*z - d
	p28 = -0.9238795325*x -0.3826834324*z - d
	p29 = -0.8660254038*x -0.5*z - d
	p30 = -0.7933533403*x -0.608761429*z - d
	p31 = -0.7071067812*x -0.7071067812*z - d
	p32 = -0.608761429*x  -0.7933533403*z - d
	p33 = -0.5*x -0.8660254038*z - d
	p34 = -0.3826834324*x  -0.9238795325*z - d
	p35 = -0.2588190451*x -0.9659258263*z - d
	p36 = -0.1305261922*x -0.9914448614*z - d
	p37 = -z - d
	p38 = 0.1305261922*x -0.9914448614*z - d
	p39 = 0.2588190451*x -0.9659258263*z - d
	p40 = 0.3826834324*x -0.9238795325*z - d
	p41 = 0.5*x -0.8660254038*z - d
	p42 = 0.608761429*x -0.7933533403*z - d
	p43 = 0.7071067812*x -0.7071067812*z - d
	p44 = 0.7933533403*x -0.608761429*z - d
	p45 = 0.8660254038*x -0.5*z -d
	p46 = 0.9238795325*x -0.3826834324*z -d
	p47 = 0.9659258263*x -0.2588190451*z - d
	p48 = 0.9914448614*x -0.1305261922*z - d
	p49 = x - d
	if p1<0.0 and p2<0.0 and p3 < 0.0 and p4<0.0 and p5<0.0 and p6<0.0 and p7<0.0 and p8<0.0 and p9<0.0 and p10<0.0 and p11<0.0 and p12<0.0 and p13<0.0 and p14<0.0 and p15<0.0 and p16<0.0 and p17<0.0 and p18<0.0 and p19<0.0 and p20<0.0 and p21<0.0 and p22<0.0 and p23<0.0 and p24<0.0 and p25<0.0 and p26<0.0 and p27<0.0 and p28<0.0 and p29<0.0 and p30<0.0 and p31<0.0 and p31<0.0 and p31<0.0 and p32<0.0 and p33<0.0 and p34<0.0 and p35<0.0 and p36<0.0 and p37<0.0 and p38<0.0 and p39<0.0 and p40<0.0 and p41<0.0 and p42<0.0 and p43<0.0 and p44<0.0 and p45<0.0 and p46<0.0 and p47<0.0 and p48<0.0 and p49<0.0:
		return 0.0
	else:
		return 1.0


def openingDense(x,y,z):
	d = 5.0
	p1 = z -d
	p2 = 8.7156e-2*x + 9.962e-1*z - d
	p3 = 1.7365e-1*x + 9.848e-1*z - d
	p4 = 2.5882e-1*x + 9.659e-1*z - d
	p5 = 3.4202e-1*x + 9.397e-1*z - d
	p6 = 4.2262e-1*x + 9.064e-1*z - d
	p7 = 5.0e-1*x + 8.66025e-1*z - d
	p8 = 5.7358e-1*x + 8.1915e-1*z - d
	p9 = 6.4279e-1*x + 7.6604e-1*z - d
	p10 = 7.071e-1*x + 7.071e-1*z - d
	p11 = 7.6604e-1*x + 6.4279e-1*z - d
	p12 = 8.1915e-1*x + 5.7258e-1*z - d
	p13 = 8.66025e-1*x + 0.5*z -d
	p14 = 9.063e-1*x + 4.2262e-1*z - d
	p15 = 9.3969e-1*x + 3.4202e-1*z - d
	p16 = 9.65926e-1*x + 2.5882e-1*z - d
	p17 = 9.8481e-1*x + 1.7365e-1*z - d
	p18 = 9.96195e-1*x + 8.71557e-2*z - d
	p19 = 1.0*x - d;
	p20 = 8.7156e-2*x - 9.962e-1*z - d
	p21 = 1.7365e-1*x - 9.848e-1*z - d
	p22 = 2.5882e-1*x - 9.659e-1*z - d
	p23 = 3.4202e-1*x - 9.397e-1*z - d
	p24 = 4.2262e-1*x - 9.064e-1*z - d
	p25 = 5.0e-1*x - 8.66025e-1*z - d
	p26 = 5.7358e-1*x - 8.1915e-1*z - d
	p27 = 6.4279e-1*x - 7.6604e-1*z - d
	p28 = 7.071e-1*x - 7.071e-1*z - d
	p29 = 7.6604e-1*x - 6.4279e-1*z - d
	p30 = 8.1915e-1*x - 5.7258e-1*z - d
	p31 = 8.66025e-1*x - 0.5*z -d
	p32 = 9.063e-1*x - 4.2262e-1*z - d
	p33 = 9.3969e-1*x - 3.4202e-1*z - d
	p34 = 9.65926e-1*x - 2.5882e-1*z - d
	p35 = 9.8481e-1*x - 1.7365e-1*z - d
	p36 = 9.96195e-1*x - 8.71557e-2*z - d
	p37 = -1.0*x - d;
	p38 = -z -d
	p39 = -8.7156e-2*x + 9.962e-1*z - d
	p40 = -1.7365e-1*x + 9.848e-1*z - d
	p41 = -2.5882e-1*x + 9.659e-1*z - d
	p42 = -3.4202e-1*x + 9.397e-1*z - d
	p43 = -4.2262e-1*x + 9.064e-1*z - d
	p44 = -5.0e-1*x + 8.66025e-1*z - d
	p45 = -5.7358e-1*x + 8.1915e-1*z - d
	p46 = -6.4279e-1*x + 7.6604e-1*z - d
	p47 = -7.071e-1*x + 7.071e-1*z - d
	p48 = -7.6604e-1*x + 6.4279e-1*z - d
	p49 = -8.1915e-1*x + 5.7258e-1*z - d
	p50 = -8.66025e-1*x + 0.5*z -d
	p51 = -9.063e-1*x + 4.2262e-1*z - d
	p52 = -9.3969e-1*x + 3.4202e-1*z - d
	p53 = -9.65926e-1*x + 2.5882e-1*z - d
	p54 = -9.8481e-1*x + 1.7365e-1*z - d
	p55 = -9.96195e-1*x + 8.71557e-2*z - d
	p56 = -8.7156e-2*x - 9.962e-1*z - d
	p57 = -1.7365e-1*x - 9.848e-1*z - d
	p58 = -2.5882e-1*x - 9.659e-1*z - d
	p59 = -3.4202e-1*x - 9.397e-1*z - d
	p60 = -4.2262e-1*x - 9.064e-1*z - d
	p61 = -5.0e-1*x - 8.66025e-1*z - d
	p62 = -5.7358e-1*x - 8.1915e-1*z - d
	p63 = -6.4279e-1*x - 7.6604e-1*z - d
	p64 = -7.071e-1*x - 7.071e-1*z - d
	p65 = -7.6604e-1*x - 6.4279e-1*z - d
	p66 = -8.1915e-1*x - 5.7258e-1*z - d
	p67 = -8.66025e-1*x - 0.5*z -d
	p68 = -9.063e-1*x - 4.2262e-1*z - d
	p69 = -9.3969e-1*x - 3.4202e-1*z - d
	p70 = -9.65926e-1*x - 2.5882e-1*z - d
	p71 = -9.8481e-1*x - 1.7365e-1*z - d
	p72 = -9.96195e-1*x - 8.71557e-2*z - d
	if p1<0.0 and p2<0.0 and p3 < 0.0 and p4<0.0 and p5<0.0 and p6<0.0 and p7<0.0 and p8<0.0 and p9<0.0 and p10<0.0 and p11<0.0 and p12<0.0 and p13<0.0 and p14<0.0 and p15<0.0 and p16<0.0 and p17<0.0 and p18<0.0 and p19<0.0 and p20<0.0 and p21<0.0 and p22<0.0 and p23<0.0 and p24<0.0 and p25<0.0 and p26<0.0 and p27<0.0 and p28<0.0 and p29<0.0 and p30<0.0 and p31<0.0 and p31<0.0 and p31<0.0 and p32<0.0 and p33<0.0 and p34<0.0 and p35<0.0 and p36<0.0 and p37<0.0 and p38<0.0 and p39<0.0 and p40<0.0 and p41<0.0 and p42<0.0 and p43<0.0 and p44<0.0 and p45<0.0 and p46<0.0 and p47<0.0 and p48<0.0 and p49<0.0 and p50<0.0 and p51<0.0 and p52<0.0 and p53<0.0 and p54<0.0 and p55<0.0 and p56<0.0 and p57<0.0 and p58<0.0 and p59<0.0 and p60<0.0 and p61<0.0 and p62<0.0 and p63<0.0 and p64<0.0 and p65<0.0 and p66<0.0 and p67<0.0 and p68<0.0 and p69<0.0 and p70<0.0 and p71<0.0 and p72<0.0:
		return 0.0
	else:
		return 1.0


def openingDouble(x,y,z):
	d = 5.0
	p1 = z -10.0
	p2 = 1.0*x - d;
	p3 = 2.5882e-1*x - 9.659e-1*z - d
	p4 = 5.0e-1*x - 8.66025e-1*z - d
	p5 = 7.071e-1*x - 7.071e-1*z - d
	p6 = 8.66025e-1*x - 0.5*z -d
	p7 = 9.65926e-1*x - 2.5882e-1*z - d
	p8 = -1.0*x - d;
	p9 = -2.5882e-1*x - 9.659e-1*z - d
	p10 = -5.0e-1*x - 8.66025e-1*z - d
	p11 = -7.071e-1*x - 7.071e-1*z - d
	p12 = -8.66025e-1*x - 0.5*z -d
	p13 = -9.65926e-1*x - 2.5882e-1*z - d
	if p1<0.0 and p2<0.0 and p3<0.0 and p4<0.0 and p5<0.0 and p6<0.0 and p7<0.0 and p8<0.0 and p9<0.0 and p10<0.0 and p11<0.0 and p12<0.0 and p13<0.0:
		return 0.0
	else:
		return 1.0

def openingUpperLeft(x,y,z):
	d = 5.0
	partition = 0.0 #1.7
	p1 = -1.0*x - d
	if p1 < 0.0:
		p1 = 0.0
	p2 = 1.0*x + partition
	if p2 < 0.0:
		p2 = 0.0
	p3 = -0.5*x - 0.866*z - d
	if p3 < 0.0:
		p3 = 0.0
	p5 = -0.866*x - 0.5*z - d
	if p5 < 0.0:
		p5 = 0.0
	p7 = -1.0*z - d 
	if p7 < 0.0:
		p7 = 0.0
	p8 = 1.0*z + 1.0  #base of horseshoe
	if p8 < 0.0:
		p8 = 0.0
	return p1+p2+p3+p5+p7+p8


def openingUpperRight(x,y,z):
	d = 5.0
	partition = 0.0 # 1.7
	p1 = 1.0*x - d
	if p1 < 0.0:
		p1 = 0.0
	p2 = -1.0*x + partition
	if p2 < 0.0:
		p2 = 0.0
	p3 = 0.5*x - 0.866*z - d
	if p3 < 0.0:
		p3 = 0.0
	p5 = 0.866*x - 0.5*z - d
	if p5 < 0.0:
		p5 = 0.0
	p7 = -1.0*z - d 
	if p7 < 0.0:
		p7 = 0.0
	p8 = 1.0*z + 1.0  #base of horseshoe
	if p8 < 0.0:
		p8 = 0.0
	return p1+p2+p3+p5+p7+p8


def openingUpperMid(x,y,z):
	d = 5.0
	partition = 1.7
	p2 = 1.0*x - partition
	if p2 < 0.0:
		p2 = 0.0
	p3 = -1.0*x - partition
	if p3 < 0.0:
		p3 = 0.0
	p7 = -1.0*z - d 
	if p7 < 0.0:
		p7 = 0.0
	p8 = 1.0*z +1.0  #base of horseshoe
	if p8 < 0.0:
		p8 = 0.0
	return p2+p3+p7+p8


def openingBtm(x,y,z):
	d = 5.0
	p1 = -1.0*x - d
	if p1 < 0.0:
		p1 = 0.0
	p2 = 1.0*x - d
	if p2 < 0.0:
		p2 = 0.0
	p7 = -1.0*z -1.0
	if p7 < 0.0:
		p7 = 0.0
	p8 = 1.0*z -2.5  #base of horseshoe
	if p8 < 0.0:
		p8 = 0.0
	return p1+p2+p7+p8


def excavate():
	global radius
	global roofID
	global averageMass
	for b in O.bodies:
		posX = b.state.pos[0]
		posY = b.state.pos[1]
		posZ = b.state.pos[2]
		'''
		if opening(posX,posY,posZ) < 1e-4: # and b.shape.erase==True:
			if b.isClumpMember == True and b.shape.erase==True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False and b.shape.erase==True:
				O.bodies.erase(b.id)
		'''
		'''
		if posX**2 + posZ**2 - (1.2*radius)**2 < 0.0: # and b.dynamic == True:
			if b.isClumpMember == True and b.shape.erase==True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False and b.shape.erase==True:
				O.bodies.erase(b.id)
		'''
		if openingDouble(posX,posY,posZ) < 1e-4 and b.dynamic == True: #posX**2 + posZ**2 - (1.0*radius)**2 < 0.0 openingCircle openingDense openingBenchmark
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)

		


def openingCircle(x,y,z):
	d = 5.0
	p1 = -1.0*x - d
	if p1 < 0.0:
		p1 = 0.0
	p2 = 1.0*x - d
	if p2 < 0.0:
		p2 = 0.0
	p3 = 0.5*x - 0.866*z - d
	if p3 < 0.0:
		p3 = 0.0
	p4 = -0.5*x - 0.866*z - d
	if p4 < 0.0:
		p4 = 0.0
	p5 = 0.866*x - 0.5*z - d
	if p5 < 0.0:
		p5 = 0.0
	p6 = -0.866*x - 0.5*z -d
	if p6 < 0.0:
		p6 = 0.0
	p7 = -1.0*z - d 
	if p7 < 0.0:
		p7 = 0.0
	p8 = 1.0*z - 10.0 
	if p8 < 0.0:
		p8 = 0.0
	p9 = 0.5*x + 0.866*z - d
	if p9 < 0.0:
		p9 = 0.0
	p10 = -0.5*x + 0.866*z - d
	if p10 < 0.0:
		p10 = 0.0
	p11 = 0.866*x + 0.5*z -d
	if p11 < 0.0:
		p11 = 0.0
	p12 = -0.866*x + 0.5*z -d
	if p12 < 0.0:
		p12 = 0.0
	return p1+p2+p3+p4+p5+p6+p7+p8+p9+p10+p11+p12


def reduceKni(kni):
	for i in O.interactions:
		i.phys.kn_i = kni
		i.phys.ks_i = kni

	
def pruneSmallBodies(minMass):
	global radius
	for b in O.bodies:
		posX = b.state.pos[0]
		posY = b.state.pos[1]
		posZ = b.state.pos[2]
		if posX**2 + posZ**2 - (1.2*radius)**2 < 0.0 and b.state.mass<minMass and b.dynamic == True:
			#if b.isClump == True:
			#	O.bodies.deleteClumpBody(b)
			#if b.isClump == False and b.isClumpMember == False:
			#	O.bodies.erase(b.id)
			if b.isClump == True:
				isBolt = False
				for i in b.shape.ids:
					if  O.bodies[i].shape.isBolt==True:
						isBolt=True
				if isBolt==False:
					O.bodies.deleteClumpBody(b)
			elif b.isClump == False and b.isClumpMember == False and b.shape.isBolt==False:
				O.bodies.erase(b.id)




def pruneSmallBodiesLining(minMass):
	global radius
	for b in O.bodies:
		posX = b.state.pos[0]
		posY = b.state.pos[1]
		posZ = b.state.pos[2]
		if posX**2 + posZ**2 - (1.2*radius)**2 < 0.0 and b.state.mass<minMass and b.dynamic == True:
			if b.isClump == True :
				for i in b.shape.ids:
					bReplace = Body()
					bReplace.state = O.bodies[i].state
					bReplace.state.ori = O.bodies[i].state.ori
					bReplace.shape = O.bodies[i].shape
					bReplace.shape.erase=True
					bReplace.mat = O.bodies[i].mat
					bReplace.dynamic=False
					O.bodies.append(bReplace)
					O.bodies.deleteClumpMember(b, O.bodies[i])
			elif b.isClump == False and b.isClumpMember == False:
				b.shape.erase = True
				b.dynamic = False

			
def keepBody(bodyID):
	for b in O.bodies:
		if b.id!=bodyID and b.dynamic == True:
			if b.isClump == True:
				O.bodies.deleteClumpBody(b)
			elif b.isClump == False and b.isClumpMember == False:
				O.bodies.erase(b.id)


def assignRoofID(bodyNo):
	global roofID
	global oriRoofPos
	roofID = bodyNo
	oriRoofPos = O.bodies[roofID].state.pos

def assignSideWallID(bodyNo):
	global sideWallID
	global oriSideWallPos
	sideWallID = bodyNo
	oriSideWallPos = O.bodies[sideWallID].state.pos

def stopLateralBodies():
	for b in O.bodies:
		if b.isClump==False: 
			if b.shape.isBoundary==True and b.state.pos[2] > p.boundarySizeZmin -p.rForPP :
				b.dynamic = False


def allowLateralBodies():
	for b in O.bodies:
		if b.isClump==False: 
			if b.shape.isBoundary==True and b.state.pos[0] > p.boundarySizeXmax + p.rForPP :
				b.dynamic = True


def calTimeStep():
	global minTimeStep
	global supportInstalled
	global liningStiffness
	global nodeMass
	boltStiffness = 0.0
	minMass = 1.0e15
	if supportInstalled==True:
		#boltStiffness = max(3.0e9,liningStiffness)
		minMass = nodeMass
	mkratio = 99999999.9
	maxK = 1.0
	for i in O.interactions:
		if i.isReal==True:
			mass1 = 0.0
			mass2 = 0.0
			if O.bodies[i.id1].isClumpMember == False:
				mass1  = O.bodies[i.id1].state.mass
			else:
				mass1 = O.bodies[O.bodies[i.id1].clumpId].state.mass
			if O.bodies[i.id2].isClumpMember == False:
				mass2  = O.bodies[i.id2].state.mass
			else:
				mass2 = O.bodies[O.bodies[i.id2].clumpId].state.mass
			if mass1/i.phys.Knormal_area < mkratio:
				mkratio = mass1/max(i.phys.Knormal_area,boltStiffness)
			if mass2/i.phys.Knormal_area < mkratio:
				mkratio = mass2/max(i.phys.Knormal_area, boltStiffness)
	presentDt = 0.15*sqrt(mkratio) #0.15
	O.dt = presentDt


def reduceFric(fric):
	global boundaryFriction
	fric = max(fric,0.0)
	boundaryFriction = max(fric,0.0)	
	for i in O.interactions:
		if O.bodies[i.id1].shape.isBoundary==False and O.bodies[i.id2].shape.isBoundary==False:
			i.phys.phi_b = fric
			i.phys.phi_r = fric
			i.phys.effective_phi = fric
	for b in O.bodies:
		if b.isClump==False:
			if b.shape.isBoundary == False:
				b.mat.frictionAngle = radians(fric)


def reduceFricGenerate(fric):
	fric = max(fric,0.0)	
	for i in O.interactions:
		if O.bodies[i.id1].shape.isBoundary==False and O.bodies[i.id2].shape.isBoundary==False:
			i.phys.phi_b = fric
			i.phys.phi_r = fric
			i.phys.effective_phi = fric
	for b in O.bodies:
		if b.isClump==False:
			if b.shape.isBoundary == False:
				b.mat.frictionAngle = radians(fric)


def myAddPlotData():
	global boundaryFriction
	global roofID
	global sideWallID
	global oriRoofPos
	global oriSideWallPos
	global radius
	global gravityMultiplier
	roofDisp =  O.bodies[roofID].state.pos[2] #(O.bodies[roofID].state.pos-oriRoofPos).norm() 
	sideWallDisp = O.bodies[sideWallID].state.pos[2] # (O.bodies[sideWallID].state.pos-oriSideWallPos).norm() 
	uf=utils.unbalancedForce()
	KE = utils.kineticEnergy()
	## monitor out of balanced forces for two bodies
	'''
	f1 = O.interactions[63,137].phys.normalForce + O.interactions[63,137].phys.shearForce if O.interactions[63,137].isReal == True else Vector3(0,0,0)
	f2 = O.interactions[63,166].phys.normalForce + O.interactions[63,166].phys.shearForce if O.interactions[63,166].isReal == True else Vector3(0,0,0)
	f3 = O.interactions[63,73].phys.normalForce + O.interactions[63,73].phys.shearForce if O.interactions[63,73].isReal == True else Vector3(0,0,0)
	f4 = O.interactions[63,375].phys.normalForce + O.interactions[63,375].phys.shearForce if O.interactions[63,375].isReal == True else Vector3(0,0,0)
	f5 = O.interactions[63,30].phys.normalForce + O.interactions[63,30].phys.shearForce if O.interactions[63,30].isReal == True else Vector3(0,0,0)
	f6 = O.interactions[63,195].phys.normalForce + O.interactions[63,195].phys.shearForce if O.interactions[63,195].isReal == True else Vector3(0,0,0)
	f7 = O.interactions[63,218].phys.normalForce + O.interactions[63,218].phys.shearForce if O.interactions[63,218].isReal == True else Vector3(0,0,0)
	weight1 = O.bodies[63].state.mass*O.engines[3].gravity
	ub1 = (f1+f2-f3-f4-f5-f6-f7+weight1).norm()/(f1+f2-f3-f4-f5-f6-f7).norm()
	f1 = O.interactions[429,513].phys.normalForce + O.interactions[429,513].phys.shearForce if O.interactions[429,513].isReal == True else Vector3(0,0,0)
	f2 = O.interactions[429,535].phys.normalForce + O.interactions[429,535].phys.shearForce if O.interactions[429,535].isReal == True else Vector3(0,0,0)
	f3 = O.interactions[429,572].phys.normalForce + O.interactions[429,572].phys.shearForce if O.interactions[429,572].isReal == True else Vector3(0,0,0)
	f4 = O.interactions[429,256].phys.normalForce + O.interactions[429,256].phys.shearForce if O.interactions[429,256].isReal == True else Vector3(0,0,0)
	f5 = O.interactions[429,307].phys.normalForce + O.interactions[429,307].phys.shearForce if O.interactions[429,307].isReal == True else Vector3(0,0,0)
	f6 = O.interactions[429,467].phys.normalForce + O.interactions[429,467].phys.shearForce if O.interactions[429,467].isReal == True else Vector3(0,0,0)
	weight2 = O.bodies[429].state.mass*O.engines[3].gravity
	ub2 = (-f1-f2+f3+f4+f5+f6+weight2).norm()/(-f1-f2+f3+f4+f5+f6).norm()
	'''
	ub1=0.0
	ub2=0.0
	plot.addData(timeStep1=O.iter,timeStep2=O.iter,timeStep3=O.iter,timeStep4=O.iter,timeStep5=O.iter,timeStep6=O.iter,timeStep7=O.iter,ub63=ub1,ub429=ub2,unbalancedForce=uf,kineticEn=KE,frictionAngle=boundaryFriction,roofDisplacement=roofDisp,sideWallDisplacement=sideWallDisp)



plot.plots={'timeStep1':('unbalancedForce'),'timeStep2':('kineticEn'),'timeStep3':('frictionAngle'),'timeStep4':('roofDisplacement'),'timeStep5':('sideWallDisplacement'),'timeStep6':('ub63'),'timeStep7':('ub429')}



def saveToFile():
	global K0
	os. makedirs('saveData/'+ O.tags['d.id']+'K'+str(K0))
	O.save('saveData/'+O.tags['d.id']+'K'+str(K0)+'/Tunnel'+'.yade.gz')
	return 	

def saveData():
	global K0
	plot.saveDataTxt('saveData/'+O.tags['d.id']+'K'+str(K0)+'/plot'+'.txt')


stage1=False
stage2=False
stage3=False
stage4 = False

def stopfunction():
	global gravityMultiplier
	global boundaryFriction
	global initialFriction
	global newtonIntegrator
	global roofID
	global supportInstalled
	global stage1
	global stage2
	global stage3
	global stage4
	global bodyY
	global calculateTimeStepAfterSupport
	if O.iter > 3500 and bodyY[0].dynamic ==True: #13000
		O.pause()
		stopLateralBodies()
		O.run()
	elif O.iter > 4000 and bodyY[0].dynamic==False and supportInstalled == False: #15000
		O.pause()
		reduceFric(boundaryFriction)
		supportInstalled=True
		excavate()
		pruneSmallBodies(200.0) #200 #400
		O.step()
		for b in O.bodies:
			b.state.refPos=b.state.pos
		newtonIntegrator.damping = 0.0
		physDispatcher.viscousDamping = 0.8
		for i in O.interactions:
			i.phys.viscousDamping = 0.8
		assignSideWallID(roofID)
		#calTimeStep()
		#O.dt = 5.6e-5 #5.6e-5 #2.25e-5 #5.6e-5 #1.8e-5 #
		installBolts()
		#lining.openingCreated = True
		printBolt(1)
		vtkRecorderTunnel.iterPeriod=1
		lining.vtkIteratorInterval=1
		O.step()
		for b in O.bodies:
			if b.isClump == False:
				if b.shape.isLining == True:
					b.state.blockedDOFs='yXZ' #['y','rx','rz']
		vtkRecorderTunnel.iterPeriod=2500
		#lining.vtkIteratorInterval=2500
		printBolt(1000)
		O.engines = O.engines+ [PyRunner(command='printSupportForces()',iterPeriod=500,label='supportRecorder')]
		if calculateTimeStepAfterSupport == False:
			calculateTimeStepAfterSupport = True
			calTimeStep()
		O.run()
	
		



def goToNextStage():
	global boundaryFriction
	global roofID
	global sideWallID
	uf=utils.unbalancedForce()
	KE = 0.0
	normKE = 0.0
	for b in O.bodies:
		if b.isClumpMember==False and b.dynamic==True:
			KE = KE+0.5*b.state.mass*b.state.vel.squaredNorm()
			normKE = normKE+0.5*b.state.mass
	normalizedKE = KE/normKE
	if normalizedKE < 1e-4:
	#if uf<0.01:
		reduceFric(boundaryFriction - 0.5)



O.dt = 5.94e-5 #8e-8 #11.2e-5 
O.step()
#O.step()
#calTimeStep()
O.engines=O.engines+[PyRunner(iterPeriod=10,command='myAddPlotData()')]
#O.engines=O.engines+[PyRunner(iterPeriod=1,command='calTimeStep()',label='timeStepCalculator')]
O.engines = O.engines+ [PyRunner(command='stopfunction()',iterPeriod=50,label='checker')]



O.engines = O.engines + [PotentialBlockVTKRecorderTunnel(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',iterPeriod=5000,sampleX=40,sampleY=1,sampleZ=40,maxDimension=0.05,twoDimension=True,REC_INTERACTION=True,REC_VELOCITY=True, label='vtkRecorderTunnel')]
#O.engines = O.engines + [PotentialBlockVTKRecorder(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/TunnelClump',iterPeriod=1,sampleX=40,sampleY=1,sampleZ=40,maxDimension=0.05,twoDimension=True,label='vtkRecorder' )]

#O.engines=O.engines+[PBDisplayVTKtunnel(iterPeriod=1,contactPtSize=1,cameraPosition=Vector3(0,50,0),maxDimension=0.3,displayCentroid=True,displayID=True)]


Pult = 180e9
Sult = 180e9
Kaxial = 0.2e9 #1.5e9 0.2
Kshear = 0.0
boltLen = 4.0
openingRadius = 5.0
boltDiameter = 0.034
tension = 0.0
EAbolt = 102000000.0
smoothOut = False

O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='Minus22_5',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.9239,0,0.3827), normalStiffness=Kaxial, shearStiffness=Kshear, preTension=tension, axialMax=Pult,shearMax=Sult,openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltMinus225' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='Minus11_25',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.9808,0,0.1951), normalStiffness=Kaxial, shearStiffness=Kshear, preTension=tension,axialMax=Pult,shearMax=Sult,openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut, label='boltMinus11_25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='0',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-1,0,-0.0), normalStiffness=Kaxial, shearStiffness=Kshear,preTension =tension, axialMax=Pult,shearMax=Sult, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut, label='bolt0' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='11_25',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.9808,0,-0.1951), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt11_25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='22_5',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.9239,0.0,-0.3827), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt225' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='33_75',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.8315,0,-0.5556), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt33_75' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='45',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.7071,0.0,-0.7071), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension ,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt45')] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='56_25',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.5556,0,-0.8315), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt56_25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='67_5',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.3827,0.0,-0.9239), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension ,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt675')] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='78_75',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.1951,0,-0.9808),normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt78_75' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='90',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0,0,-1.0), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt90' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='101_25',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.1951,0,-0.9808), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt101_25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='112_5',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.3827,0,-0.9239), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt113' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='123_75',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.5556,0,-0.8315), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt123_75' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='135',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.7071,0,-0.7071), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt135' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='146_25',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.8315,0,-0.5556), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt146_25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='157_5',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.9239,0,-0.3827), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt158' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='168_75',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.9808,0,-0.1951), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt168_75' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='180',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(1,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt180' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='191_25',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.9808,0,0.1951), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt191_25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='202_5',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.9239,0.0,0.3827), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult,preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt2025' )] + O.engines[3:]

O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='L1',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.08716,0,-0.9962), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter ,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltL1' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='L2',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.2924,0,-0.9563), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltL2' )] + O.engines[3:]

O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='R1',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.08716,0,-0.9962), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut ,label='boltR1' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='R2',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.2924,0,-0.9563), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltR2' )] + O.engines[3:]


O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='15',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.96593,0,-0.25882), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt15' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='30',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.866,0,-0.5), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt30' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='60',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.5,0,-0.866), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt60' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='75',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.2588,0,-0.96593), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt75' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='105',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.2588,0,-0.96593), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt105' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='120',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.5,0,-0.866), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt120' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='150',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.866,0,-0.5), normalStiffness=Kaxial, shearStiffness=Kshear, axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt150' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='165',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.96593,0,-0.25882), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt165' )] + O.engines[3:]

O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.996194698091746,0,-0.0871557427476582), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='10',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.984807753012208,0,-0.17364817766693), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt10' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='20',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.939692620785908,0,-0.342020143325669), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt20' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='25',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.90630778703665,0,-0.422618261740699), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt25' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='35',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.819152044288992,0,-0.573576436351046), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt35' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='40',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.766044443118978,0,-0.642787609686539), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt40' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='50',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.642787609686539,0,-0.766044443118978), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt50' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='55',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.573576436351046,0,-0.819152044288992), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt55' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='65',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.422618261740699,0,-0.90630778703665), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt65' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='70',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.342020143325669,0,-0.939692620785908), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt70' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='80',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.17364817766693,0,-0.984807753012208), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt80' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='85',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.0871557427476584,0,-0.996194698091746), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt85' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='95',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.0871557427476582,0,-0.996194698091746), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt95' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='100',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.17364817766693,0,-0.984807753012208), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt100' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='110',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.342020143325669,0,-0.939692620785908), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt110' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='115',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.422618261740699,0,-0.90630778703665), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt115' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='125',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.573576436351046,0,-0.819152044288992), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt125' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='130',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.642787609686539,0,-0.766044443118978), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt130' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='140',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.766044443118978,0,-0.642787609686539), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt140' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='145',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.819152044288992,0,-0.573576436351046), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt145' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='155',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.90630778703665,0,-0.422618261740699), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt155' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='160',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.939692620785908,0,-0.342020143325669), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt160' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='170',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.984807753012208,0,-0.173648177666931), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt170' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='175',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.996194698091746,0,-0.0871557427476582), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt175' )] + O.engines[3:]

O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='m5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.996194698091746,0,0.0871557427476582), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltm5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='m10',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.984807753012208,0,0.17364817766693), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltm10' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='m15',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.96593,0,0.25882), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltm15' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='mEast5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.996194698091746,0,0.0871557427476582), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltmEast5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='mEast10',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.984807753012208,0,0.17364817766693), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltmEast10' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='mEast15',iterPeriod=1, vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.96593,0,0.25882), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult,preTension=tension,openingRad=openingRadius, halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltmEast15' )] + O.engines[3:]

O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='m12_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.9763,0,0.21644), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltm12_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='m7_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.99144,0,0.130526), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltm7_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='m2_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.99905,0,0.043619), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='boltm2_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='2_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.99905,0,-0.043619), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt2_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='7_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.99144,0,-0.13053), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt7_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='12_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.9763,0,-0.21644), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt12_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='17_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.95372,0,-0.30071), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt17_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='27_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.88701,0,-0.46175), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt27_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='32_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.84339,0,-0.5373), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt32_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='37_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.79335,0,-0.60876), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt37_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='42_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.73728,0,-0.67559), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt42_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='47_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.67559,0,-0.73728), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt47_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='52_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.60876,0,-0.79335), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt52_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='57_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.5373,0,-0.84339), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt57_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='62_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.46175,0,-0.88701), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt62_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='72_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.30071,0,-0.95372), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt72_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='77_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.21644,0,-0.9763), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt77_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='82_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.13053,0,-0.99144), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt82_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='87_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(-0.04362,0,-0.99905), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt87_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='92_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.043619,0,-0.99905), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt92_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='97_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.130526,0,-0.99144), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt97_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='102_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.21644,0,-0.9763), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt102_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='107_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.300706,0,-0.95372), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt107_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='117_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.461749,0,-0.88701), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt117_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='122_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.5374,0,-0.84339), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt122_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='127_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.608761,0,-0.79335), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt127_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='132_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.67559,0,-0.73728), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt132_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='137_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.737277,0,-0.67559), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt137_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='142_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.793353,0,-0.60876), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt142_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='147_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.843391,0,-0.5373), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt147_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='152_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.887011,0,-0.46175), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt152_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='162_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.953717,0,-0.30071), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt162_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='167_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.976296,0,-0.21644), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt167_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='172_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.991445,0,-0.13053), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt172_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='177_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.999048,0,-0.043619), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt177_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='182_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.999048,0,0.043619), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt182_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='187_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.991445,0,0.130526), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt187_5' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='192_5',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,0), boltLength= radius+boltLen, boltDirection=Vector3(0.976296,0,0.21644), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='bolt192_5' )] + O.engines[3:]

#Cavern
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+1m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,1.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip1m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+2m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,2.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip2m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+3m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,3.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip3m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+4m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,4.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip4m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+5m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,5.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip5m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-1m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,1.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin1m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-2m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,2.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin2m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-3m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,3.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin3m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-4m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,4.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin4m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-5m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,5.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin5m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+6m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,6.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip6m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+7m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,7.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip7m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+8m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,8.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip8m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori+9m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,9.0), boltLength= radius+boltLen, boltDirection=Vector3(1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horip9m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-6m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,6.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin6m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-7m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,7.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin7m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-8m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,8.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin8m' )] + O.engines[3:]
O.engines = O.engines[:3]+  [RockBolt(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='hori-9m',iterPeriod=1,
vtkIteratorInterval=5000,startingPoint = Vector3(0,0,9.0), boltLength= radius+boltLen, boltDirection=Vector3(-1.0,0,0.0), normalStiffness=Kaxial, shearStiffness=Kshear,axialMax=Pult,shearMax=Sult, preTension=tension, openingRad=openingRadius,halfActiveLength=boltDiameter,axialStiffness=EAbolt,useMidPoint=smoothOut,label='Horin9m' )] + O.engines[3:]


liningDensity = 2500.0
liningThickness= 0.2 #0.3
nodeNumber=100
nodeNumberDouble=100.0
nodeMass = liningDensity*(pi*radius**2 - pi*(radius-liningThickness)**2)/nodeNumberDouble
secondMomentI = 1.0/12.0*1.0*liningThickness**3
arcLength = 2.0*pi*radius/nodeNumberDouble
searchRadius=radius
Emodulus=25000000000.0
axialStiffness = Emodulus*liningThickness
bendingStiffness = 1.0*Emodulus*secondMomentI
liningStiffness = 5.0*max(axialStiffness/arcLength,12.0*bendingStiffness/arcLength**3) #30.0*
tempExpansion = 0.0 #12.0e-6*20.0
O.engines = O.engines[:3]+  [RockLiningGlobal(fileName='/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/Tunnel',name='Lining',iterPeriod=1, vtkIteratorInterval=5000,contactLength=arcLength, liningThickness=liningThickness,startingPoint = Vector3(0,0,0), density=liningDensity, ElasticModulus=Emodulus, Inertia=secondMomentI, Area=liningThickness*1.0, totalNodes=nodeNumber, lumpedMass=nodeMass, interfaceStiffness=10000000,interfaceCohesion=0.0,interfaceTension=0.0, expansionFactor=tempExpansion,initOverlap= 0.00, interfaceFriction=30.0, axialMax=Pult,shearMax=Sult,openingRad=searchRadius,EA=axialStiffness,EI=bendingStiffness,label='lining' )] + O.engines[3:]




def printSupportForces():
	record(O.iter)
	#recordLining(O.iter)

def installBolts():
	global supportInstalled
	supportInstalled = True
	bolt0.openingCreated=True
	bolt180.openingCreated=True
	'''bolt225.openingCreated=True
	bolt45.openingCreated=True
	bolt675.openingCreated=True
	bolt90.openingCreated=True
	bolt113.openingCreated=True
	bolt135.openingCreated=True
	bolt158.openingCreated=True
	#boltMinus225.openingCreated=True
	#boltMinus11_25.openingCreated=True
	bolt11_25.openingCreated=True
	bolt33_75.openingCreated=True
	bolt56_25.openingCreated=True
	bolt78_75.openingCreated=True
	bolt101_25.openingCreated=True
	bolt123_75.openingCreated=True
	bolt146_25.openingCreated=True
	bolt168_75.openingCreated=True  '''
	#bolt191_25.openingCreated=True
	#bolt2025.openingCreated=True
	#boltL1.openingCreated=True
	#boltL2.openingCreated=True
	#boltR1.openingCreated=True
	#boltR2.openingCreated=True
	#bolt15.openingCreated=True
	#bolt30.openingCreated=True
	#bolt75.openingCreated=True
	#bolt105.openingCreated=True
	#bolt165.openingCreated=True
	#bolt5.openingCreated=True
	#bolt10.openingCreated=True
	bolt20.openingCreated=True
	#bolt25.openingCreated=True
	#bolt35.openingCreated=True
	bolt40.openingCreated=True
	#bolt50.openingCreated=True
	#bolt55.openingCreated=True
	#bolt65.openingCreated=True
	bolt60.openingCreated=True
	#bolt70.openingCreated=True
	bolt80.openingCreated=True
	#bolt85.openingCreated=True
	#bolt90.openingCreated=True
	#bolt95.openingCreated=True
	bolt100.openingCreated=True
	#bolt110.openingCreated=True
	#bolt115.openingCreated=True
	#bolt125.openingCreated=True
	bolt120.openingCreated=True
	#bolt130.openingCreated=True
	bolt140.openingCreated=True
	#bolt145.openingCreated=True
	#bolt150.openingCreated=True
	#bolt155.openingCreated=True
	bolt160.openingCreated=True
	#bolt170.openingCreated=True
	#bolt175.openingCreated=True
	#Horip1m.openingCreated=True
	Horip2m.openingCreated=True
	#Horip3m.openingCreated=True
	Horip4m.openingCreated=True
	#Horip5m.openingCreated=True
	#Horin1m.openingCreated=True
	Horin2m.openingCreated=True
	#Horin3m.openingCreated=True
	Horin4m.openingCreated=True
	#Horin5m.openingCreated=True
	Horip6m.openingCreated=True
	#Horip7m.openingCreated=True
	Horip8m.openingCreated=True
	#Horip9m.openingCreated=True
	Horin6m.openingCreated=True
	#Horin7m.openingCreated=True
	Horin8m.openingCreated=True
	#Horin9m.openingCreated=True

'''
	boltm5.openingCreated=True
	boltm10.openingCreated=True
	boltm15.openingCreated=True
	boltmEast5.openingCreated=True
	boltmEast10.openingCreated=True
	boltmEast15.openingCreated=True
	boltm12_5.openingCreated=True
	boltm7_5.openingCreated=True
	boltm2_5.openingCreated=True
	bolt2_5.openingCreated=True
	bolt7_5.openingCreated=True
	bolt12_5.openingCreated=True
	bolt17_5.openingCreated=True
	bolt27_5.openingCreated=True
	bolt32_5.openingCreated=True
	bolt37_5.openingCreated=True
	bolt42_5.openingCreated=True
	bolt47_5.openingCreated=True
	bolt52_5.openingCreated=True
	bolt57_5.openingCreated=True
	bolt62_5.openingCreated=True
	bolt72_5.openingCreated=True
	bolt77_5.openingCreated=True
	bolt82_5.openingCreated=True
	bolt87_5.openingCreated=True
	bolt92_5.openingCreated=True
	bolt97_5.openingCreated=True
	bolt102_5.openingCreated=True
	bolt107_5.openingCreated=True
	bolt117_5.openingCreated=True
	bolt122_5.openingCreated=True
	bolt127_5.openingCreated=True
	bolt132_5.openingCreated=True
	bolt137_5.openingCreated=True
	bolt142_5.openingCreated=True
	bolt147_5.openingCreated=True
	bolt152_5.openingCreated=True
	bolt162_5.openingCreated=True
	bolt167_5.openingCreated=True
	bolt172_5.openingCreated=True
	bolt177_5.openingCreated=True
	bolt182_5.openingCreated=True
	bolt187_5.openingCreated=True
	bolt192_5.openingCreated=True
'''

def installBolts1():
	global supportInstalled1
	supportInstalled1 = True
	bolt225.openingCreated=True
	bolt45.openingCreated=True
	bolt675.openingCreated=True
	bolt33_75.openingCreated=True
	bolt56_25.openingCreated=True
	bolt78_75.openingCreated=True
	#bolt90.openingCreated=True


def installBolts2():
	global supportInstalled2
	supportInstalled2 = True
	bolt90.openingCreated=True
	bolt113.openingCreated=True
	bolt135.openingCreated=True
	bolt158.openingCreated=True
	bolt101_25.openingCreated=True
	bolt123_75.openingCreated=True
	bolt146_25.openingCreated=True
	
	

def installBolts4():
	global supportInstalled4
	supportInstalled4 = True
	bolt11_25.openingCreated=True
	bolt168_75.openingCreated=True
	bolt0.openingCreated=True
	bolt180.openingCreated=True
	boltL1.openingCreated=True
	boltL2.openingCreated=True
	boltR1.openingCreated=True
	boltR2.openingCreated=True

def printBolt(interval):
	boltMinus225.vtkIteratorInterval=interval
	bolt0.vtkIteratorInterval=interval
	bolt225.vtkIteratorInterval=interval
	bolt45.vtkIteratorInterval=interval
	bolt675.vtkIteratorInterval=interval
	bolt90.vtkIteratorInterval=interval
	bolt113.vtkIteratorInterval=interval
	bolt135.vtkIteratorInterval=interval
	bolt158.vtkIteratorInterval=interval
	bolt180.vtkIteratorInterval=interval
	bolt2025.vtkIteratorInterval=interval
	boltMinus11_25.vtkIteratorInterval=interval
	bolt11_25.vtkIteratorInterval=interval
	bolt33_75.vtkIteratorInterval=interval
	bolt56_25.vtkIteratorInterval=interval
	bolt78_75.vtkIteratorInterval=interval
	bolt101_25.vtkIteratorInterval=interval
	bolt123_75.vtkIteratorInterval=interval
	bolt146_25.vtkIteratorInterval=interval
	bolt168_75.vtkIteratorInterval=interval
	bolt191_25.vtkIteratorInterval=interval
	boltL1.vtkIteratorInterval=interval
	boltL2.vtkIteratorInterval=interval
	boltR1.vtkIteratorInterval=interval
	boltR2.vtkIteratorInterval=interval
	bolt15.vtkIteratorInterval=interval
	bolt30.vtkIteratorInterval=interval
	bolt60.vtkIteratorInterval=interval
	bolt75.vtkIteratorInterval=interval
	bolt105.vtkIteratorInterval=interval
	bolt120.vtkIteratorInterval=interval
	bolt150.vtkIteratorInterval=interval
	bolt165.vtkIteratorInterval=interval
	bolt5.vtkIteratorInterval=interval
	bolt10.vtkIteratorInterval=interval
	bolt20.vtkIteratorInterval=interval
	bolt25.vtkIteratorInterval=interval
	bolt35.vtkIteratorInterval=interval
	bolt40.vtkIteratorInterval=interval
	bolt50.vtkIteratorInterval=interval
	bolt55.vtkIteratorInterval=interval
	bolt65.vtkIteratorInterval=interval
	bolt70.vtkIteratorInterval=interval
	bolt80.vtkIteratorInterval=interval
	bolt85.vtkIteratorInterval=interval
	bolt95.vtkIteratorInterval=interval
	bolt100.vtkIteratorInterval=interval
	bolt110.vtkIteratorInterval=interval
	bolt115.vtkIteratorInterval=interval
	bolt125.vtkIteratorInterval=interval
	bolt130.vtkIteratorInterval=interval
	bolt140.vtkIteratorInterval=interval
	bolt145.vtkIteratorInterval=interval
	bolt155.vtkIteratorInterval=interval
	bolt160.vtkIteratorInterval=interval
	bolt170.vtkIteratorInterval=interval
	bolt175.vtkIteratorInterval=interval
	bolt5.vtkIteratorInterval=interval
	bolt10.vtkIteratorInterval=interval
	bolt20.vtkIteratorInterval=interval
	bolt25.vtkIteratorInterval=interval
	bolt35.vtkIteratorInterval=interval
	bolt40.vtkIteratorInterval=interval
	bolt50.vtkIteratorInterval=interval
	bolt55.vtkIteratorInterval=interval
	bolt65.vtkIteratorInterval=interval
	bolt70.vtkIteratorInterval=interval
	bolt80.vtkIteratorInterval=interval
	bolt85.vtkIteratorInterval=interval
	bolt95.vtkIteratorInterval=interval
	bolt100.vtkIteratorInterval=interval
	bolt110.vtkIteratorInterval=interval
	bolt115.vtkIteratorInterval=interval
	bolt125.vtkIteratorInterval=interval
	bolt130.vtkIteratorInterval=interval
	bolt140.vtkIteratorInterval=interval
	bolt145.vtkIteratorInterval=interval
	bolt155.vtkIteratorInterval=interval
	bolt160.vtkIteratorInterval=interval
	bolt170.vtkIteratorInterval=interval
	bolt175.vtkIteratorInterval=interval
	boltm5.vtkIteratorInterval=interval
	boltm10.vtkIteratorInterval=interval
	boltm15.vtkIteratorInterval=interval
	boltmEast5.vtkIteratorInterval=interval
	boltmEast10.vtkIteratorInterval=interval
	boltmEast15.vtkIteratorInterval=interval
	boltm12_5.vtkIteratorInterval=interval
	boltm7_5.vtkIteratorInterval=interval
	boltm2_5.vtkIteratorInterval=interval
	bolt2_5.vtkIteratorInterval=interval
	bolt7_5.vtkIteratorInterval=interval
	bolt12_5.vtkIteratorInterval=interval
	bolt17_5.vtkIteratorInterval=interval
	bolt27_5.vtkIteratorInterval=interval
	bolt32_5.vtkIteratorInterval=interval
	bolt37_5.vtkIteratorInterval=interval
	bolt42_5.vtkIteratorInterval=interval
	bolt47_5.vtkIteratorInterval=interval
	bolt52_5.vtkIteratorInterval=interval
	bolt57_5.vtkIteratorInterval=interval
	bolt62_5.vtkIteratorInterval=interval
	bolt72_5.vtkIteratorInterval=interval
	bolt77_5.vtkIteratorInterval=interval
	bolt82_5.vtkIteratorInterval=interval
	bolt87_5.vtkIteratorInterval=interval
	bolt92_5.vtkIteratorInterval=interval
	bolt97_5.vtkIteratorInterval=interval
	bolt102_5.vtkIteratorInterval=interval
	bolt107_5.vtkIteratorInterval=interval
	bolt117_5.vtkIteratorInterval=interval
	bolt122_5.vtkIteratorInterval=interval
	bolt127_5.vtkIteratorInterval=interval
	bolt132_5.vtkIteratorInterval=interval
	bolt137_5.vtkIteratorInterval=interval
	bolt142_5.vtkIteratorInterval=interval
	bolt147_5.vtkIteratorInterval=interval
	bolt152_5.vtkIteratorInterval=interval
	bolt162_5.vtkIteratorInterval=interval
	bolt167_5.vtkIteratorInterval=interval
	bolt172_5.vtkIteratorInterval=interval
	bolt177_5.vtkIteratorInterval=interval
	bolt182_5.vtkIteratorInterval=interval
	bolt187_5.vtkIteratorInterval=interval
	bolt192_5.vtkIteratorInterval=interval
	Horip1m.vtkIteratorInterval=interval
	Horip2m.vtkIteratorInterval=interval
	Horip3m.vtkIteratorInterval=interval
	Horip4m.vtkIteratorInterval=interval
	Horip5m.vtkIteratorInterval=interval
	Horin1m.vtkIteratorInterval=interval
	Horin2m.vtkIteratorInterval=interval
	Horin3m.vtkIteratorInterval=interval
	Horin4m.vtkIteratorInterval=interval
	Horin5m.vtkIteratorInterval=interval
	Horip6m.vtkIteratorInterval=interval
	Horip7m.vtkIteratorInterval=interval
	Horip8m.vtkIteratorInterval=interval
	Horip9m.vtkIteratorInterval=interval
	Horin6m.vtkIteratorInterval=interval
	Horin7m.vtkIteratorInterval=interval
	Horin8m.vtkIteratorInterval=interval
	Horin9m.vtkIteratorInterval=interval


f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltMinus225.txt','w')
f.write('boltMinus225f\tboltMinus225axial\tboltMinus225shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt0.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt225.txt','w')
f.write('bolt225f\tbolt225axial\tbolt225shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt45.txt','w')
f.write('bolt45f\tbolt45axial\tbolt45shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt675.txt','w')
f.write('bolt675f\tbolt675axial\tbolt675shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt90.txt','w')
f.write('bolt90f\tbolt90axial\tbolt90shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt113.txt','w')
f.write('bolt113f\tbolt113axial\tbolt113shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt135.txt','w')
f.write('bolt135f\tbolt135axial\tbolt135shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt158.txt','w')
f.write('bolt158f\tbolt158axial\tbolt158shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt180.txt','w')
f.write('bolt180f\tbolt180axial\tbolt180shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt2025.txt','w')
f.write('bolt2025f\tbolt2025axial\tbolt2025shear\truptured\tnodePosFrCentre\n')
f.close()

f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltMinus11_25.txt','w')
f.write('boltMinus11_25f\tboltMinus11_25axial\tboltMinus11_25shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt11_25.txt','w')
f.write('bolt11_25f\tbolt11_25axial\tbolt11_25shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt33_75.txt','w')
f.write('bolt33_75f\tbolt33_75axial\tbolt33_75shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt56_25.txt','w')
f.write('bolt56_25f\tbolt56_25axial\tbolt56_25shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt78_75.txt','w')
f.write('bolt78_75f\tbolt78_75axial\tbolt78_75shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt101_25.txt','w')
f.write('bolt101_25f\tbolt101_25axial\tbolt101_25shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt123_75.txt','w')
f.write('bolt123_75f\tbolt123_75axial\tbolt123_75shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt146_25.txt','w')
f.write('bolt146_25f\tbolt146_25axial\tbolt146_25shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt168_75.txt','w')
f.write('bolt168_75f\tbolt168_75axial\tbolt168_75shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt191_25.txt','w')
f.write('bolt191_25f\tbolt191_25axial\tbolt191_25shear\truptured\tnodePosFrCentre\n')
f.close()

f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt15.txt','w')
f.write('bolt15f\tbolt15axial\tbolt15shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt30.txt','w')
f.write('bolt30f\tbolt305axial\tbolt30shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt60.txt','w')
f.write('bolt60f\tbolt60axial\tbolt60shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt75.txt','w')
f.write('bolt75f\tbolt75axial\tbolt75shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt105.txt','w')
f.write('bolt105f\tbolt105axial\tbolt105shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt120.txt','w')
f.write('bolt120f\tbolt120axial\tbolt120shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt150.txt','w')
f.write('bolt150f\tbolt150axial\tbolt150shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt165.txt','w')
f.write('bolt165f\tbolt165axial\tbolt165shear\truptured\tnodePosFrCentre\n')
f.close()

f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltL1.txt','w')
f.write('boltL1f\tboltL1axial\tboltL1shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltL2.txt','w')
f.write('boltL2f\tboltL2axial\tboltL2shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltR1.txt','w')
f.write('boltR1f\tboltR1axial\tboltR1shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltR2.txt','w')
f.write('boltR2f\tboltR2axial\tboltR2shear\truptured\tnodePosFrCentre\n')
f.close()

f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt5.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt10.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt20.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt25.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt35.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt40.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt50.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt55.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt65.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt70.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt80.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt85.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt95.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt100.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt110.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt115.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt125.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt130.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt140.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt145.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt155.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt160.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt170.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt175.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltm5.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltm10.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltm15.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltmEast5.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltmEast10.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltmEast15.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+1m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+2m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+3m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+4m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+5m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-1m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-2m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-3m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-4m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-5m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+6m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+7m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+8m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+9m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-6m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-7m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-8m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()
f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-9m.txt','w')
f.write('bolt0f\tbolt0axial\tbolt0shear\truptured\tnodePosFrCentre\n')
f.close()


f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/lining.txt','w')
f.write('liningRefAngle\tliningAxial\tliningShear\tliningMoment\tsigmaMax\tsigmaMin\tdisplacement\tnormalPressure\ttotalPressure\n')
f.close()


def recordLining(stageNo):
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/lining.txt','a')	
	count = 0
	nodeNo = lining.totalNodes
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(lining.refAngle[count]/pi*180.0)+'\t'+str(lining.axialForces[count])+'\t'+str(lining.shearForces[count])+'\t'+str(lining.moment[count])+'\t'+str(lining.sigmaMax[count])+'\t'+str(lining.sigmaMin[count])+'\t'+str(lining.displacement[count])+'\t'+str(lining.radialDisplacement[count])+'\t'+str(O.bodies[lining.blockIDs[count]].shape.liningNormalPressure.norm())+'\t'+str(O.bodies[lining.blockIDs[count]].shape.liningTotalPressure.norm())+'\n')
		count = count+1
	f.close()

def record(stageNo):
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltMinus225.txt','a')	
	count = 0
	nodeNo = len(boltMinus225.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltMinus225.forces[count])+'\t'+str(boltMinus225.axialForces[count])+'\t'+str(boltMinus225.shearForces[count])+'\t'+str(boltMinus225.ruptured[count])+'\t'+str(boltMinus225.distanceFrCentre[count])+'\t'+str(boltMinus225.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt0.txt','a')	
	count = 0
	nodeNo = len(bolt0.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt0.forces[count])+'\t'+str(bolt0.axialForces[count])+'\t'+str(bolt0.shearForces[count])+'\t'+str(bolt0.ruptured[count])+'\t'+str(bolt0.distanceFrCentre[count])+'\t'+str(bolt0.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt225.txt','a')	
	count = 0
	nodeNo = len(bolt225.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt225.forces[count])+'\t'+str(bolt225.axialForces[count])+'\t'+str(bolt225.shearForces[count])+'\t'+str(bolt225.ruptured[count])+'\t'+str(bolt225.distanceFrCentre[count])+'\t'+str(bolt225.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt45.txt','a')	
	count = 0
	nodeNo = len(bolt45.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt45.forces[count])+'\t'+str(bolt45.axialForces[count])+'\t'+str(bolt45.shearForces[count])+'\t'+str(bolt45.ruptured[count])+'\t'+str(bolt45.distanceFrCentre[count])+'\t'+str(bolt45.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt675.txt','a')	
	count = 0
	nodeNo = len(bolt675.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt675.forces[count])+'\t'+str(bolt675.axialForces[count])+'\t'+str(bolt675.shearForces[count])+'\t'+str(bolt675.ruptured[count])+'\t'+str(bolt675.distanceFrCentre[count])+'\t'+str(bolt675.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt90.txt','a')	
	count = 0
	nodeNo = len(bolt90.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt90.forces[count])+'\t'+str(bolt90.axialForces[count])+'\t'+str(bolt90.shearForces[count])+'\t'+str(bolt90.ruptured[count])+'\t'+str(bolt90.distanceFrCentre[count])+'\t'+str(bolt90.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt113.txt','a')	
	count = 0
	nodeNo = len(bolt113.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt113.forces[count])+'\t'+str(bolt113.axialForces[count])+'\t'+str(bolt113.shearForces[count])+'\t'+str(bolt113.ruptured[count])+'\t'+str(bolt113.distanceFrCentre[count])+'\t'+str(bolt113.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt135.txt','a')	
	count = 0
	nodeNo = len(bolt135.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt135.forces[count])+'\t'+str(bolt135.axialForces[count])+'\t'+str(bolt135.shearForces[count])+'\t'+str(bolt135.ruptured[count])+'\t'+str(bolt135.distanceFrCentre[count])+'\t'+str(bolt135.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt158.txt','a')	
	count = 0
	nodeNo = len(bolt158.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt158.forces[count])+'\t'+str(bolt158.axialForces[count])+'\t'+str(bolt158.shearForces[count])+'\t'+str(bolt158.ruptured[count])+'\t'+str(bolt158.distanceFrCentre[count])+'\t'+str(bolt158.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt180.txt','a')	
	count = 0
	nodeNo = len(bolt180.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt180.forces[count])+'\t'+str(bolt180.axialForces[count])+'\t'+str(bolt180.shearForces[count])+'\t'+str(bolt180.ruptured[count])+'\t'+str(bolt180.distanceFrCentre[count])+'\t'+str(bolt180.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt2025.txt','a')	
	count = 0
	nodeNo = len(bolt2025.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt2025.forces[count])+'\t'+str(bolt2025.axialForces[count])+'\t'+str(bolt2025.shearForces[count])+'\t'+str(bolt2025.ruptured[count])+'\t'+str(bolt2025.distanceFrCentre[count])+'\t'+str(bolt2025.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltMinus11_25.txt','a')	
	count = 0
	nodeNo = len(boltMinus11_25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltMinus11_25.forces[count])+'\t'+str(boltMinus11_25.axialForces[count])+'\t'+str(boltMinus11_25.shearForces[count])+'\t'+str(boltMinus11_25.ruptured[count])+'\t'+str(boltMinus11_25.distanceFrCentre[count])+'\t'+str(boltMinus11_25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt11_25.txt','a')	
	count = 0
	nodeNo = len(bolt11_25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt11_25.forces[count])+'\t'+str(bolt11_25.axialForces[count])+'\t'+str(bolt11_25.shearForces[count])+'\t'+str(bolt11_25.ruptured[count])+'\t'+str(bolt11_25.distanceFrCentre[count])+'\t'+str(bolt11_25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt33_75.txt','a')	
	count = 0
	nodeNo = len(bolt33_75.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt33_75.forces[count])+'\t'+str(bolt33_75.axialForces[count])+'\t'+str(bolt33_75.shearForces[count])+'\t'+str(bolt33_75.ruptured[count])+'\t'+str(bolt33_75.distanceFrCentre[count])+'\t'+str(bolt33_75.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt56_25.txt','a')	
	count = 0
	nodeNo = len(bolt56_25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt56_25.forces[count])+'\t'+str(bolt56_25.axialForces[count])+'\t'+str(bolt56_25.shearForces[count])+'\t'+str(bolt56_25.ruptured[count])+'\t'+str(bolt56_25.distanceFrCentre[count])+'\t'+str(bolt56_25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt78_75.txt','a')	
	count = 0
	nodeNo = len(bolt78_75.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt78_75.forces[count])+'\t'+str(bolt78_75.axialForces[count])+'\t'+str(bolt78_75.shearForces[count])+'\t'+str(bolt78_75.ruptured[count])+'\t'+str(bolt78_75.distanceFrCentre[count])+'\t'+str(bolt78_75.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt101_25.txt','a')	
	count = 0
	nodeNo = len(bolt101_25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt101_25.forces[count])+'\t'+str(bolt101_25.axialForces[count])+'\t'+str(bolt101_25.shearForces[count])+'\t'+str(bolt101_25.ruptured[count])+'\t'+str(bolt101_25.distanceFrCentre[count])+'\t'+str(bolt101_25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt123_75.txt','a')	
	count = 0
	nodeNo = len(bolt123_75.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt123_75.forces[count])+'\t'+str(bolt123_75.axialForces[count])+'\t'+str(bolt123_75.shearForces[count])+'\t'+str(bolt123_75.ruptured[count])+'\t'+str(bolt123_75.distanceFrCentre[count])+'\t'+str(bolt123_75.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt146_25.txt','a')	
	count = 0
	nodeNo = len(bolt146_25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt146_25.forces[count])+'\t'+str(bolt146_25.axialForces[count])+'\t'+str(bolt146_25.shearForces[count])+'\t'+str(bolt146_25.ruptured[count])+'\t'+str(bolt146_25.distanceFrCentre[count])+'\t'+str(bolt146_25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt168_75.txt','a')	
	count = 0
	nodeNo = len(bolt168_75.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt168_75.forces[count])+'\t'+str(bolt168_75.axialForces[count])+'\t'+str(bolt168_75.shearForces[count])+'\t'+str(bolt168_75.ruptured[count])+'\t'+str(bolt168_75.distanceFrCentre[count])+'\t'+str(bolt168_75.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt191_25.txt','a')	
	count = 0
	nodeNo = len(bolt191_25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt191_25.forces[count])+'\t'+str(bolt191_25.axialForces[count])+'\t'+str(bolt191_25.shearForces[count])+'\t'+str(bolt191_25.ruptured[count])+'\t'+str(bolt191_25.distanceFrCentre[count])+'\t'+str(bolt191_25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltL1.txt','a')	
	count = 0
	nodeNo = len(boltL1.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltL1.forces[count])+'\t'+str(boltL1.axialForces[count])+'\t'+str(boltL1.shearForces[count])+'\t'+str(boltL1.ruptured[count])+'\t'+str(boltL1.distanceFrCentre[count])+'\t'+str(boltL1.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltL2.txt','a')	
	count = 0
	nodeNo = len(boltL2.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltL2.forces[count])+'\t'+str(boltL2.axialForces[count])+'\t'+str(boltL2.shearForces[count])+'\t'+str(boltL2.ruptured[count])+'\t'+str(boltL2.distanceFrCentre[count])+'\t'+str(boltL2.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltR1.txt','a')	
	count = 0
	nodeNo = len(boltR1.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltR1.forces[count])+'\t'+str(boltR1.axialForces[count])+'\t'+str(boltR1.shearForces[count])+'\t'+str(boltR1.ruptured[count])+'\t'+str(boltR1.distanceFrCentre[count])+'\t'+str(boltR1.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltR2.txt','a')	
	count = 0
	nodeNo = len(boltR2.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltR2.forces[count])+'\t'+str(boltR2.axialForces[count])+'\t'+str(boltR2.shearForces[count])+'\t'+str(boltR2.ruptured[count])+'\t'+str(boltR2.distanceFrCentre[count])+'\t'+str(boltR2.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt15.txt','a')	
	count = 0
	nodeNo = len(bolt15.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt15.forces[count])+'\t'+str(bolt15.axialForces[count])+'\t'+str(bolt15.shearForces[count])+'\t'+str(bolt15.ruptured[count])+'\t'+str(bolt15.distanceFrCentre[count])+'\t'+str(bolt15.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt30.txt','a')	
	count = 0
	nodeNo = len(bolt30.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt30.forces[count])+'\t'+str(bolt30.axialForces[count])+'\t'+str(bolt30.shearForces[count])+'\t'+str(bolt30.ruptured[count])+'\t'+str(bolt30.distanceFrCentre[count])+'\t'+str(bolt30.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt60.txt','a')	
	count = 0
	nodeNo = len(bolt60.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt60.forces[count])+'\t'+str(bolt60.axialForces[count])+'\t'+str(bolt60.shearForces[count])+'\t'+str(bolt60.ruptured[count])+'\t'+str(bolt60.distanceFrCentre[count])+'\t'+str(bolt60.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt75.txt','a')	
	count = 0
	nodeNo = len(bolt75.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt75.forces[count])+'\t'+str(bolt75.axialForces[count])+'\t'+str(bolt75.shearForces[count])+'\t'+str(bolt75.ruptured[count])+'\t'+str(bolt75.distanceFrCentre[count])+'\t'+str(bolt75.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt105.txt','a')	
	count = 0
	nodeNo = len(bolt105.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt105.forces[count])+'\t'+str(bolt105.axialForces[count])+'\t'+str(bolt105.shearForces[count])+'\t'+str(bolt105.ruptured[count])+'\t'+str(bolt105.distanceFrCentre[count])+'\t'+str(bolt105.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt120.txt','a')	
	count = 0
	nodeNo = len(bolt120.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt120.forces[count])+'\t'+str(bolt120.axialForces[count])+'\t'+str(bolt120.shearForces[count])+'\t'+str(bolt120.ruptured[count])+'\t'+str(bolt120.distanceFrCentre[count])+'\t'+str(bolt120.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt150.txt','a')	
	count = 0
	nodeNo = len(bolt150.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt150.forces[count])+'\t'+str(bolt150.axialForces[count])+'\t'+str(bolt150.shearForces[count])+'\t'+str(bolt150.ruptured[count])+'\t'+str(bolt150.distanceFrCentre[count])+'\t'+str(bolt150.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt165.txt','a')	
	count = 0
	nodeNo = len(bolt165.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt165.forces[count])+'\t'+str(bolt165.axialForces[count])+'\t'+str(bolt165.shearForces[count])+'\t'+str(bolt165.ruptured[count])+'\t'+str(bolt165.distanceFrCentre[count])+'\t'+str(bolt165.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt5.txt','a')	
	count = 0
	nodeNo = len(bolt5.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt5.forces[count])+'\t'+str(bolt5.axialForces[count])+'\t'+str(bolt5.shearForces[count])+'\t'+str(bolt5.ruptured[count])+'\t'+str(bolt5.distanceFrCentre[count])+'\t'+str(bolt5.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt10.txt','a')	
	count = 0
	nodeNo = len(bolt10.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt10.forces[count])+'\t'+str(bolt10.axialForces[count])+'\t'+str(bolt10.shearForces[count])+'\t'+str(bolt10.ruptured[count])+'\t'+str(bolt10.distanceFrCentre[count])+'\t'+str(bolt10.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt20.txt','a')	
	count = 0
	nodeNo = len(bolt20.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt20.forces[count])+'\t'+str(bolt20.axialForces[count])+'\t'+str(bolt20.shearForces[count])+'\t'+str(bolt20.ruptured[count])+'\t'+str(bolt20.distanceFrCentre[count])+'\t'+str(bolt20.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt25.txt','a')	
	count = 0
	nodeNo = len(bolt25.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt25.forces[count])+'\t'+str(bolt25.axialForces[count])+'\t'+str(bolt25.shearForces[count])+'\t'+str(bolt25.ruptured[count])+'\t'+str(bolt25.distanceFrCentre[count])+'\t'+str(bolt25.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt35.txt','a')	
	count = 0
	nodeNo = len(bolt35.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt35.forces[count])+'\t'+str(bolt35.axialForces[count])+'\t'+str(bolt35.shearForces[count])+'\t'+str(bolt35.ruptured[count])+'\t'+str(bolt35.distanceFrCentre[count])+'\t'+str(bolt35.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt40.txt','a')	
	count = 0
	nodeNo = len(bolt40.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt40.forces[count])+'\t'+str(bolt40.axialForces[count])+'\t'+str(bolt40.shearForces[count])+'\t'+str(bolt40.ruptured[count])+'\t'+str(bolt40.distanceFrCentre[count])+'\t'+str(bolt40.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt50.txt','a')	
	count = 0
	nodeNo = len(bolt50.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt50.forces[count])+'\t'+str(bolt50.axialForces[count])+'\t'+str(bolt50.shearForces[count])+'\t'+str(bolt50.ruptured[count])+'\t'+str(bolt50.distanceFrCentre[count])+'\t'+str(bolt50.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt55.txt','a')	
	count = 0
	nodeNo = len(bolt55.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt55.forces[count])+'\t'+str(bolt55.axialForces[count])+'\t'+str(bolt55.shearForces[count])+'\t'+str(bolt55.ruptured[count])+'\t'+str(bolt55.distanceFrCentre[count])+'\t'+str(bolt55.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt65.txt','a')	
	count = 0
	nodeNo = len(bolt65.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt65.forces[count])+'\t'+str(bolt65.axialForces[count])+'\t'+str(bolt65.shearForces[count])+'\t'+str(bolt65.ruptured[count])+'\t'+str(bolt65.distanceFrCentre[count])+'\t'+str(bolt65.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt70.txt','a')	
	count = 0
	nodeNo = len(bolt70.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt70.forces[count])+'\t'+str(bolt70.axialForces[count])+'\t'+str(bolt70.shearForces[count])+'\t'+str(bolt70.ruptured[count])+'\t'+str(bolt70.distanceFrCentre[count])+'\t'+str(bolt70.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt80.txt','a')	
	count = 0
	nodeNo = len(bolt80.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt80.forces[count])+'\t'+str(bolt80.axialForces[count])+'\t'+str(bolt80.shearForces[count])+'\t'+str(bolt80.ruptured[count])+'\t'+str(bolt80.distanceFrCentre[count])+'\t'+str(bolt80.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt85.txt','a')	
	count = 0
	nodeNo = len(bolt85.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt85.forces[count])+'\t'+str(bolt85.axialForces[count])+'\t'+str(bolt85.shearForces[count])+'\t'+str(bolt85.ruptured[count])+'\t'+str(bolt85.distanceFrCentre[count])+'\t'+str(bolt85.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt95.txt','a')	
	count = 0
	nodeNo = len(bolt95.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt95.forces[count])+'\t'+str(bolt95.axialForces[count])+'\t'+str(bolt95.shearForces[count])+'\t'+str(bolt95.ruptured[count])+'\t'+str(bolt95.distanceFrCentre[count])+'\t'+str(bolt95.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt100.txt','a')	
	count = 0
	nodeNo = len(bolt100.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt100.forces[count])+'\t'+str(bolt100.axialForces[count])+'\t'+str(bolt100.shearForces[count])+'\t'+str(bolt100.ruptured[count])+'\t'+str(bolt100.distanceFrCentre[count])+'\t'+str(bolt100.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt110.txt','a')	
	count = 0
	nodeNo = len(bolt110.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt110.forces[count])+'\t'+str(bolt110.axialForces[count])+'\t'+str(bolt110.shearForces[count])+'\t'+str(bolt110.ruptured[count])+'\t'+str(bolt110.distanceFrCentre[count])+'\t'+str(bolt110.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt115.txt','a')	
	count = 0
	nodeNo = len(bolt115.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt115.forces[count])+'\t'+str(bolt115.axialForces[count])+'\t'+str(bolt115.shearForces[count])+'\t'+str(bolt115.ruptured[count])+'\t'+str(bolt115.distanceFrCentre[count])+'\t'+str(bolt115.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt125.txt','a')	
	count = 0
	nodeNo = len(bolt125.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt125.forces[count])+'\t'+str(bolt125.axialForces[count])+'\t'+str(bolt125.shearForces[count])+'\t'+str(bolt125.ruptured[count])+'\t'+str(bolt125.distanceFrCentre[count])+'\t'+str(bolt125.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt130.txt','a')	
	count = 0
	nodeNo = len(bolt130.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt130.forces[count])+'\t'+str(bolt130.axialForces[count])+'\t'+str(bolt130.shearForces[count])+'\t'+str(bolt130.ruptured[count])+'\t'+str(bolt130.distanceFrCentre[count])+'\t'+str(bolt130.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt140.txt','a')	
	count = 0
	nodeNo = len(bolt140.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt140.forces[count])+'\t'+str(bolt140.axialForces[count])+'\t'+str(bolt140.shearForces[count])+'\t'+str(bolt140.ruptured[count])+'\t'+str(bolt140.distanceFrCentre[count])+'\t'+str(bolt140.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt145.txt','a')	
	count = 0
	nodeNo = len(bolt145.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt145.forces[count])+'\t'+str(bolt145.axialForces[count])+'\t'+str(bolt145.shearForces[count])+'\t'+str(bolt145.ruptured[count])+'\t'+str(bolt145.distanceFrCentre[count])+'\t'+str(bolt145.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt155.txt','a')	
	count = 0
	nodeNo = len(bolt155.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt155.forces[count])+'\t'+str(bolt155.axialForces[count])+'\t'+str(bolt155.shearForces[count])+'\t'+str(bolt155.ruptured[count])+'\t'+str(bolt155.distanceFrCentre[count])+'\t'+str(bolt155.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt160.txt','a')	
	count = 0
	nodeNo = len(bolt160.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt160.forces[count])+'\t'+str(bolt160.axialForces[count])+'\t'+str(bolt160.shearForces[count])+'\t'+str(bolt160.ruptured[count])+'\t'+str(bolt160.distanceFrCentre[count])+'\t'+str(bolt160.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt170.txt','a')	
	count = 0
	nodeNo = len(bolt170.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt170.forces[count])+'\t'+str(bolt170.axialForces[count])+'\t'+str(bolt170.shearForces[count])+'\t'+str(bolt170.ruptured[count])+'\t'+str(bolt170.distanceFrCentre[count])+'\t'+str(bolt170.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/bolt175.txt','a')	
	count = 0
	nodeNo = len(bolt175.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(bolt175.forces[count])+'\t'+str(bolt175.axialForces[count])+'\t'+str(bolt175.shearForces[count])+'\t'+str(bolt175.ruptured[count])+'\t'+str(bolt175.distanceFrCentre[count])+'\t'+str(bolt175.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltm5.txt','a')	
	count = 0
	nodeNo = len(boltm5.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltm5.forces[count])+'\t'+str(boltm5.axialForces[count])+'\t'+str(boltm5.shearForces[count])+'\t'+str(boltm5.ruptured[count])+'\t'+str(boltm5.distanceFrCentre[count])+'\t'+str(boltm5.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltm10.txt','a')	
	count = 0
	nodeNo = len(boltm10.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltm10.forces[count])+'\t'+str(boltm10.axialForces[count])+'\t'+str(boltm10.shearForces[count])+'\t'+str(boltm10.ruptured[count])+'\t'+str(boltm10.distanceFrCentre[count])+'\t'+str(boltm10.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltm15.txt','a')	
	count = 0
	nodeNo = len(boltm15.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltm15.forces[count])+'\t'+str(boltm15.axialForces[count])+'\t'+str(boltm15.shearForces[count])+'\t'+str(boltm15.ruptured[count])+'\t'+str(boltm15.distanceFrCentre[count])+'\t'+str(boltm15.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltmEast5.txt','a')	
	count = 0
	nodeNo = len(boltmEast5.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltmEast5.forces[count])+'\t'+str(boltmEast5.axialForces[count])+'\t'+str(boltmEast5.shearForces[count])+'\t'+str(boltmEast5.ruptured[count])+'\t'+str(boltmEast5.distanceFrCentre[count])+'\t'+str(boltmEast5.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltmEast10.txt','a')	
	count = 0
	nodeNo = len(boltmEast10.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltmEast10.forces[count])+'\t'+str(boltmEast10.axialForces[count])+'\t'+str(boltmEast10.shearForces[count])+'\t'+str(boltmEast10.ruptured[count])+'\t'+str(boltmEast10.distanceFrCentre[count])+'\t'+str(boltmEast10.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/boltmEast15.txt','a')	
	count = 0
	nodeNo = len(boltmEast15.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(boltmEast15.forces[count])+'\t'+str(boltmEast15.axialForces[count])+'\t'+str(boltmEast15.shearForces[count])+'\t'+str(boltmEast15.ruptured[count])+'\t'+str(boltmEast15.distanceFrCentre[count])+'\t'+str(boltmEast15.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+1m.txt','a')	
	count = 0
	nodeNo = len(Horip1m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip1m.forces[count])+'\t'+str(Horip1m.axialForces[count])+'\t'+str(Horip1m.shearForces[count])+'\t'+str(Horip1m.ruptured[count])+'\t'+str(Horip1m.distanceFrCentre[count])+'\t'+str(Horip1m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+2m.txt','a')	
	count = 0
	nodeNo = len(Horip2m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip2m.forces[count])+'\t'+str(Horip2m.axialForces[count])+'\t'+str(Horip2m.shearForces[count])+'\t'+str(Horip2m.ruptured[count])+'\t'+str(Horip2m.distanceFrCentre[count])+'\t'+str(Horip2m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+3m.txt','a')	
	count = 0
	nodeNo = len(Horip3m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip3m.forces[count])+'\t'+str(Horip3m.axialForces[count])+'\t'+str(Horip3m.shearForces[count])+'\t'+str(Horip3m.ruptured[count])+'\t'+str(Horip3m.distanceFrCentre[count])+'\t'+str(Horip3m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+4m.txt','a')	
	count = 0
	nodeNo = len(Horip4m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip4m.forces[count])+'\t'+str(Horip4m.axialForces[count])+'\t'+str(Horip4m.shearForces[count])+'\t'+str(Horip4m.ruptured[count])+'\t'+str(Horip4m.distanceFrCentre[count])+'\t'+str(Horip4m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+5m.txt','a')	
	count = 0
	nodeNo = len(Horip5m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip5m.forces[count])+'\t'+str(Horip5m.axialForces[count])+'\t'+str(Horip5m.shearForces[count])+'\t'+str(Horip5m.ruptured[count])+'\t'+str(Horip5m.distanceFrCentre[count])+'\t'+str(Horip5m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-1m.txt','a')	
	count = 0
	nodeNo = len(Horin1m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin1m.forces[count])+'\t'+str(Horin1m.axialForces[count])+'\t'+str(Horin1m.shearForces[count])+'\t'+str(Horin1m.ruptured[count])+'\t'+str(Horin1m.distanceFrCentre[count])+'\t'+str(Horin1m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-2m.txt','a')	
	count = 0
	nodeNo = len(Horin2m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin2m.forces[count])+'\t'+str(Horin2m.axialForces[count])+'\t'+str(Horin2m.shearForces[count])+'\t'+str(Horin2m.ruptured[count])+'\t'+str(Horin2m.distanceFrCentre[count])+'\t'+str(Horin2m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-3m.txt','a')	
	count = 0
	nodeNo = len(Horin3m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin3m.forces[count])+'\t'+str(Horin3m.axialForces[count])+'\t'+str(Horin3m.shearForces[count])+'\t'+str(Horin3m.ruptured[count])+'\t'+str(Horin3m.distanceFrCentre[count])+'\t'+str(Horin3m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-4m.txt','a')	
	count = 0
	nodeNo = len(Horin4m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin4m.forces[count])+'\t'+str(Horin4m.axialForces[count])+'\t'+str(Horin4m.shearForces[count])+'\t'+str(Horin4m.ruptured[count])+'\t'+str(Horin4m.distanceFrCentre[count])+'\t'+str(Horin4m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-5m.txt','a')	
	count = 0
	nodeNo = len(Horin5m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin5m.forces[count])+'\t'+str(Horin5m.axialForces[count])+'\t'+str(Horin5m.shearForces[count])+'\t'+str(Horin5m.ruptured[count])+'\t'+str(Horin5m.distanceFrCentre[count])+'\t'+str(Horin5m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-6m.txt','a')	
	count = 0
	nodeNo = len(Horin6m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin6m.forces[count])+'\t'+str(Horin6m.axialForces[count])+'\t'+str(Horin6m.shearForces[count])+'\t'+str(Horin6m.ruptured[count])+'\t'+str(Horin6m.distanceFrCentre[count])+'\t'+str(Horin6m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-7m.txt','a')	
	count = 0
	nodeNo = len(Horin7m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin7m.forces[count])+'\t'+str(Horin7m.axialForces[count])+'\t'+str(Horin7m.shearForces[count])+'\t'+str(Horin7m.ruptured[count])+'\t'+str(Horin7m.distanceFrCentre[count])+'\t'+str(Horin7m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-8m.txt','a')	
	count = 0
	nodeNo = len(Horin8m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin8m.forces[count])+'\t'+str(Horin8m.axialForces[count])+'\t'+str(Horin8m.shearForces[count])+'\t'+str(Horin8m.ruptured[count])+'\t'+str(Horin8m.distanceFrCentre[count])+'\t'+str(Horin8m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori-9m.txt','a')	
	count = 0
	nodeNo = len(Horin9m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horin9m.forces[count])+'\t'+str(Horin9m.axialForces[count])+'\t'+str(Horin9m.shearForces[count])+'\t'+str(Horin9m.ruptured[count])+'\t'+str(Horin9m.distanceFrCentre[count])+'\t'+str(Horin9m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+6m.txt','a')	
	count = 0
	nodeNo = len(Horip6m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip6m.forces[count])+'\t'+str(Horip6m.axialForces[count])+'\t'+str(Horip6m.shearForces[count])+'\t'+str(Horip6m.ruptured[count])+'\t'+str(Horip6m.distanceFrCentre[count])+'\t'+str(Horip6m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+7m.txt','a')	
	count = 0
	nodeNo = len(Horip7m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip7m.forces[count])+'\t'+str(Horip7m.axialForces[count])+'\t'+str(Horip7m.shearForces[count])+'\t'+str(Horip7m.ruptured[count])+'\t'+str(Horip7m.distanceFrCentre[count])+'\t'+str(Horip7m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+8m.txt','a')	
	count = 0
	nodeNo = len(Horip8m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip8m.forces[count])+'\t'+str(Horip8m.axialForces[count])+'\t'+str(Horip8m.shearForces[count])+'\t'+str(Horip8m.ruptured[count])+'\t'+str(Horip8m.distanceFrCentre[count])+'\t'+str(Horip8m.displacements)+'\n')
		count = count+1
	f.close()
	f = open('/home/booncw/yadeRev/trunk/examples/Tunnel/vtk/hori+9m.txt','a')	
	count = 0
	nodeNo = len(Horip9m.forces)
	f.write('End of excavation stage no: '+str(stageNo)+'\n')
	while count < nodeNo:
		f.write(str(Horip9m.forces[count])+'\t'+str(Horip9m.axialForces[count])+'\t'+str(Horip9m.shearForces[count])+'\t'+str(Horip9m.ruptured[count])+'\t'+str(Horip9m.distanceFrCentre[count])+'\t'+str(Horip9m.displacements)+'\n')
		count = count+1
	f.close()
	
#O.engines=O.engines+[PyRunner(iterPeriod=200,command='goToNextStage()')]
#yade-batch -j1 --job-threads=2 EarthPressureCoefficient30deg.table TunnelK0.py

O.run(12000)

'''
O.saveTmp()
if utils.runningInBatch():
	O.run()
	#O.wait()
	#quit()
else:
	O.run(15010)
'''
