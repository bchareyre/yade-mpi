########################################## encoding: utf-8 #######################################
from yade.gridpfacet import *
from yade import pack,plot
import math
import random as rand
import numpy as np
from scipy.stats import gaussian_kde

# Time step
O.dt=1e-6

# Skeleton creation
#title='FILETAIEAIE'
#folder=os.path.dirname(sys.argv[0])+"Results/"+str(title)+'/'
#if os.path.exists(folder)==False:
    #os.mkdir(folder)
    #os.mkdir(folder+'VTK/')



                # Geometry and date #
##########################################  BASSIN #######################################
young=6e8
color=[0.,1.,1.]
RBASSIN=0.01                            # Rudius Pfacet
Base=3.78;base=3.78;Hight=2.;AB=(Base-base)/2 ;Long=10. # Channel dimensions
frictvirtual=30                         # virtual friction angle (Only in the area where the mesh is placed)
frictreal=20                            # friction angle

slope_channel = math.radians(31)    #Slope of the channe, in radian
length_channel = 90 #Length of the channe, in m
width_channel = 4   #Width of the channel, in m
height_channel = 2  #Height of the channel, in m
gravityVector = Vector3(0.,-9.81*sin(slope_channel),-9.81*cos(slope_channel))   #Gravity vector to consider a channel inclined with slope angle 'slope'


##########################################  Geometry and date FENCE PARAMETERS #######################################
#CenterCoord = [0.,1.01*Hight,Long/3.] #Coordonnees du centre du filet

Nx=12.          #24-> 15.12m        # Loops in X
Nz=18.          #27-> 4.9m      # Loops/2 in z
Rfilet=0.008/2                      # Mesh radius
young_wire=60e9                     # Young
young_Conn=60e9
frict_wire=10.  #10                 # friction angle
frict_Con=10.   #30

Lx=0.63/2                           # Length of one loop
Lz=0.35/2                           # Hight of one loop
Lz1=Lz/2
Lx1=Lx*4/5
CenterCoord = [Base/2,Long/2.,((Nz+1) * Lz/2)/2]    # Center

    ########### BOUCLE ############
ouv = 0.04 /2                       # Oppening of boucle
Rman = 0.005 /2                     # Boucle section radius
decman = ouv/2+Rman+Rfilet              # Gap

########################################## CABLE #######################################
# Initial vaule of pretension the legth of the vector depending on the number of cables
PreTimput=[0,0,0]
# Value of pretension
ForcePretini=[4000.,4000.,4000.]

ForcePret=[10000.,10000.,10000.]
# If braided=> 1
Braid=[0,1,1]
# position of the cables lookingfor: z_cable ###REFERENCE MESH 0 Zmax=(Nz+1) * Lz/2
z_cable=[(Nz+1)*Lz/2,1.5/2,0.]

# Cable radius
Rcable=0.012/2

########################################## FREINS #######################################

Rfrein = 0.012/2                # Cable radius
Ffrein = 176e3                  # Force frein to stard (the maximun force is define inside de function)

########################################## LAVES TORRENTIELLES PARAMETERS #######################################

rb=.5
young_sph=10e8
spIds=[]
vel_front=Vector3(0.,0.,0.)
### Engines need to be defined first since the function gridConnection creates the interaction

########################################## MATERIAL #######################################

    ################### SCRIPT NICO ##################
# Channel
chanFrictAng =  70.

#Fluid
fluidDensity =  1200.   #Density of the fluid, in kg/m3, used for the computation of the dragforce
m_viscosity =   2   #Test avec 0.022

# Debris
diameterPart =  0.2 #Diameter of the particles, in m
m_roughness =   0.01    #Fraction on the radius
sphDensity =    2500
sphYoung =  1e7
sphPoisson =    0.2
sphFrictAng =   35.

O.materials.append(
        FrictMat(
            density=sphDensity,
            frictionAngle=math.radians(sphFrictAng),
            poisson=sphPoisson,
            young=sphYoung,
            label='Frict'
        ))

O.materials.append(
                FrictMat(
                        density=2500,
                        frictionAngle=math.radians(chanFrictAng),
                        poisson=0.2,
                        young=1e7,
                        label='FrictBase'
                ))

O.materials.append(
                FrictMat(
                        density=2500,
                        frictionAngle=math.radians(0.),
                        poisson=0.2,
                        young=1e7,
                        label='FrictWall'
                ))

O.materials.append(
                FrictMat(
                        density=2500,
                        frictionAngle=math.radians(20.),
                        poisson=0.2,
                        young=1e7,
                        label='FrictRigid'
                ))

    ###################### BASIN #####################

# Virtual
O.materials.append( CohFrictMat( young=young,poisson=0.3,density=2650,frictionAngle=radians(frictvirtual),normalCohesion=3e100,shearCohesion=3e100,momentRotationLaw=True,label='gridNodeMat1' ) )  # material to create the gridConnections
O.materials.append( FrictMat( young=young,poisson=0.3,density=2650,frictionAngle=radians(frictvirtual),label='pFacetMat1' ) )  # material for general interactions
# Real
O.materials.append( CohFrictMat( young=young,poisson=0.3,density=2650,frictionAngle=radians(frictreal),normalCohesion=3e100,shearCohesion=3e100,momentRotationLaw=True,label='gridNodeMat' ) )  # material to create the gridConnections
O.materials.append( FrictMat( young=young,poisson=0.3,density=2650,frictionAngle=radians(frictreal),label='pFacetMat' ) )  # material for general interactions

    ###################### FENCE #####################

# Stress–strain curve
E1_1 = 500e6
E2_1 = 1460e6
Ef_1 = 5869.565e6
Defini_1 = 0.0001
Def1_1 = 0.060
Def2_1 = 0.015+Def1_1
Deff_1 = 0.041+Def2_1
SSVal_Simple=[(Defini_1,2*Defini_1*Ef_1),(Def1_1,Def1_1*E1_1),(Def2_1,Def2_1*E2_1),(Deff_1,2*Deff_1*Ef_1)]#[(0.0001,5.09e6),(0.030,50e6),(0.043,155e6),(0.067,1250e6)]

O.materials.append( WireMat( young=young_wire,poisson=0.3,frictionAngle=radians(frict_wire),density=7850,diameter=2*Rfilet,isDoubleTwist=False,label='gridNodeNetPackSimple',strainStressValues=SSVal_Simple) ) # Material to create the GridNode

SSVal_Double=[(0.0001,5.09e6),(0.020,50e6),(0.035,250e6),(0.088,2000e6)]
SSVal_Double=SSVal_Simple

O.materials.append( WireMat( young=young_wire,poisson=0.3,frictionAngle=radians(frict_wire),density=7850,diameter=2*Rfilet,isDoubleTwist=False,label='gridNodeNetPackDouble',strainStressValues=SSVal_Simple) ) # Material to create the GridNode

O.materials.append(FrictMat( young=young_Conn,poisson=0.3,density=7850,frictionAngle=radians(frict_Con), label='CohMat'))#normalCohesion=200e6,isCohesive=True,shearCohesion=200e2,momentRotationLaw=False, label='CohMat') ) # Material to create the iteraction (Gridconection)

######################################### ENGINES #######################################

# Definition of simulation's variable
Factor = 1.5    #Factor ratio for the distance detection of distant interactions
theta_method = 0.55 #1 : Euler Backward, 0.5 : trapezoidal rule, 0 : not used, 0.55 : suggested optimum
resolution = 1      #Change normal component resolution method, 0: Iterative exact resolution (theta method, linear contact), 1: Newton-Rafson dimentionless resolution (theta method, linear contact), 2: Newton-Rafson with nonlinear surface deflection (Hertzian-like contact)
NewtonRafsonTol = 1e-6  #Precision of the resolution of Newton-Rafson's method
vel_front = Vector3(0.,0.,0.)

O.engines=[
    ForceResetter(),
    InsertionSortCollider([
        Bo1_Sphere_Aabb(aabbEnlargeFactor=Factor, label="aabb"),
        Bo1_GridConnection_Aabb(),
        Bo1_Wall_Aabb(),
        Bo1_Box_Aabb()],
        verletDist=-0.1,
        allowBiggerThanPeriod=False
    ),
    InteractionLoop([
        Ig2_Sphere_Sphere_ScGeom(),
        Ig2_GridNode_GridNode_GridNodeGeom6D(),
        Ig2_GridConnection_GridConnection_GridCoGridCoGeom(),
        Ig2_Sphere_GridConnection_ScGridCoGeom(),
        Ig2_Sphere_Sphere_ScGeom6D(interactionDetectionFactor=Factor,label="Ig2"),
        Ig2_Box_Sphere_ScGeom6D(),
    ],
    [
        Ip2_WireMat_WireMat_WirePhys(),
        Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=False),

        Ip2_FrictMat_FrictMat_FrictPhys(),
        Ip2_FrictMat_FrictMat_LubricationPhys(eta=m_viscosity,eps=m_roughness,label="Phys_Lub"),
    ],
    [
        Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),
        Law2_ScGeom_FrictPhys_CundallStrack(),
        Law2_ScGridCoGeom_FrictPhys_CundallStrack(),
        Law2_ScGeom_WirePhys_WirePM(),
        Law2_GridCoGridCoGeom_FrictPhys_CundallStrack(),
        Law2_ScGeom_ImplicitLubricationPhys(
                        activateNormalLubrication=True,
                        activateTangencialLubrication=True,
                        activateTwistLubrication=True,
                        activateRollLubrication=True,
                        theta=theta_method,
                        resolution=resolution,
                        warnedOnce=True,
                        maxSubSteps=20,
                        NewtonRafsonTol=NewtonRafsonTol,
                        debug=False)

    ]
    ),
    #DragEngine(Rho=1200,Cd=0.47,V=vel_front, label='Drag', dead=True),
    NewtonIntegrator(gravity=gravityVector,damping=0.3,label='newton'),
    PyRunner(command='Tension()',iterPeriod=700,label='measureTension',dead=True), # Measure the tension
#   PyRunner(command='StaTension()',iterPeriod=5,label='stabtension',dead=True), # Stabalize after tension
    PyRunner(command='StaCable()',iterPeriod=700,label='cableengine'),  # mesh placed
    PyRunner(command='gravityDeposit()',iterPeriod=500,label='gravDep',dead=False),
    PyRunner(command='Fx, Fy, Fz=Force_wall()',iterPeriod=1000,label='FWall',dead=True),
    PyRunner(command='deadZone()',iterPeriod=1000,label='deadZone_engine',dead=True),
    PyRunner(command='max_pos_x,height_front, vel_front=front()',iterPeriod=1000,label='dista',dead=True),
    PyRunner(command='vel_profil()',iterPeriod=1000,label='velProfil',dead=True),
    PyRunner(command='shearLub_tot,shearCon_tot=bedshear_total()',iterPeriod=1000,label='bedshear_tot_engine',dead=True),
#   PyRunner(command='shearLub1,shearCon1=bedshear_at_x(0.15*length)',iterPeriod=1000,label='bedshear_1_engine',dead=True),
#    PyRunner(command='shearLub2,shearCon2=bedshear_at_x(30)',iterPeriod=1000,label='bedshear_2_engine',dead=True),
#    PyRunner(command='shearLub3,shearCon3=bedshear_at_x(50)',iterPeriod=1000,label='bedshear_3_engine',dead=True),
#    PyRunner(command='shearLub4,shearCon4=bedshear_at_x(75)',iterPeriod=1000,label='bedshear_4_engine',dead=True),
    #VTKRecorder(fileName=folder+'/VTK/vtk-',virtPeriod=0.1),
]

#Drag.ids=spIds
#Drag.Rho=fluidDensity
#Drag.Cd=0.47

collider.avoidSelfInteractionMask = 2

######################################### CONSTRUCTION DU BASSIN #######################################

boxColor = (104/255., 111/255., 140/255.)
chanBack = O.bodies.append(aabbWalls([(0,length_channel/3.,0),(0,length_channel/3.,height_channel)],thickness=0.0,material='FrictWall',oversizeFactor=1.0,color=boxColor))
chanS1 = O.bodies.append(aabbWalls([(0,length_channel/3.,0),(0,0,height_channel)],thickness=0.0,material='FrictWall',oversizeFactor=1.0,color=boxColor))
chanB = O.bodies.append(aabbWalls([(0,length_channel/3.,0),(width_channel,0,0)],thickness=0.0,material='FrictBase',oversizeFactor=1.0,color=boxColor))
chanS2 = O.bodies.append(aabbWalls([(width_channel,0,0),(width_channel,length_channel/3.,height_channel)],thickness=0.0,material='FrictWall',oversizeFactor=1.0,color=boxColor))
chanBottom = O.bodies.append(aabbWalls([(0,0,0),(width_channel,0,height_channel)],thickness=0.0,material='FrictWall',oversizeFactor=1.0,color=boxColor))


DepBox = O.bodies.append(aabbWalls([(0.,length_channel/3.,-0.1),(width_channel,length_channel/3.-0.1*length_channel,2*height_channel)],thickness=0.0,material='FrictWall',oversizeFactor=1.0,color=boxColor))

######################################### AJOUT ECOULEMENT #######################################

partCloud = pack.SpherePack()
partCloud.makeCloud(minCorner=(0.,length_channel/3.-length_channel/10./2.,0.),maxCorner=(width_channel,length_channel/3.-0.095*length_channel,height_channel*2.), rMean=diameterPart/2.0)
spIds = partCloud.toSimulation(material='Frict',color=[205./255,175./255,149./255])
allSp = spIds

######################################### CONSTRUCTION DU MAILLAGE #######################################

    ##################### TAILLE REELLE #####################

Lxreel = Nx * Lx
Lzreel = (Nz+1) * Lz/2
print 'Longueur réelle X = ', Lxreel
print 'Longueur réelle Z = ', Lzreel
Agraphes = []

hz = Nz+1                   # Number of heights
xstart = CenterCoord[0] - (Nx*Lx/2)         # Coordinates of the left inf angle
y = CenterCoord[1]
zstart = CenterCoord[2]- ((hz*0.5*Lz)/2)

if (Nz%2==0):                   # Numbers of loops z
    kz = int(Nz/2 + 1)
else:
    kz = int(ceil(Nz/2))


Nx = int(Nx)

NetPack = []                    # All point of the mesh
ItLeft = []                 # Left side
ItRight = []                    # Right side
ItCenter = []

for i in range(Nx+1):               # Loops x
    x = xstart + Lx*i
        for j in range(kz):         # loops z
        z = zstart + Lz*j
################################### ACCROCHE COTE GAUCHE ######################################
    # Note: the radius is 1.01*Rfilet and 0.99*Rfilet because in that way when you create a grid connection betwen two GridNode with diferent characteristics it  takes the inf of the bigest radius
        if i == 0:
                    NetPack += [O.bodies.append( gridNode([x+Lx/4,y,z+Lz/2],1.01*Rfilet, color=[0,0,1],material='gridNodeNetPackDouble',wire=False,highlight=False,fixed=False))]
                    ItLeft.append(NetPack[-1])
                    Agraphes.append(NetPack[-1])
                    if j != 0:
                        NetPack += [ O.bodies.append( gridNode([x,y,z],0.99*Rfilet, color=[1,0,0],material='gridNodeNetPackSimple',wire=False,highlight=False,fixed=False))]
                ItLeft.append(NetPack[-1])

    ##################### ACCROCHE COTE DROIT #####################
        if i == Nx:
                    NetPack += [ O.bodies.append( gridNode([x-Lx/4,y,z],1.01*Rfilet, color=[0,0,1],material='gridNodeNetPackDouble',wire=False,highlight=False,fixed=False))]
                    Agraphes.append(NetPack[-1])
                    ItRight.append(NetPack[-1])
                    if j != kz-1:
                        NetPack += [ O.bodies.append( gridNode([x,y,z+Lz/2],0.99*Rfilet, color=[1,0,0],material='gridNodeNetPackSimple',wire=False,highlight=False,fixed=False))]
                        ItRight.append(NetPack[-1])

                    if j == kz-1:
                        if Nz%2 != 0:
                            NetPack +=[ O.bodies.append( gridNode([x,y,z+Lz/2],0.99*Rfilet, color=[1,0,0],material='gridNodeNetPackSimple',wire=False,highlight=False,fixed=False))]
                            ItRight.append(NetPack[-1])
                            NetPack +=[ O.bodies.append( gridNode([x-Lx/4,y,z+Lz],1.01*Rfilet, color=[0,0,1],material='gridNodeNetPackDouble',wire=False,highlight=False,fixed=False))]
                            Agraphes.append(NetPack[-1])
                            ItRight.append(NetPack[-1])

    ##################### RESTE MAILLAGE #####################
                if i!= 0 and i!= Nx:
                    NetPack += [ O.bodies.append( gridNode([x-Lx/4,y,z],1.01*Rfilet, color=[0,0,1],material='gridNodeNetPackDouble',wire=False,highlight=False,fixed=False))]
                    Agraphes.append(NetPack[-1])
                    ItCenter.append(NetPack[-1])

                    NetPack +=[ O.bodies.append( gridNode([x,y,z+Lz/4],0.99*Rfilet, color=[0,1,0],material='gridNodeNetPackSimple',wire=False,highlight=False,fixed=False))]
                    ItCenter.append(NetPack[-1])
                    NetPack += [ O.bodies.append( gridNode([x+Lx/4,y,z+Lz/2],1.01*Rfilet, color=[0,0,1],material='gridNodeNetPackDouble',wire=False,highlight=False,fixed=False))]
                    Agraphes.append(NetPack[-1])
                    ItCenter.append(NetPack[-1])

                    if j != kz-1:
                        NetPack += [ O.bodies.append( gridNode([x,y,z+3*Lz/4],0.99*Rfilet, color=[0,1,0],material='gridNodeNetPackSimple',wire=False,highlight=False,fixed=False))]
                        ItCenter.append(NetPack[-1])
                    if j == kz-1:
                        if  Nz%2!=0:
                             NetPack += [ O.bodies.append( gridNode([x,y,z+3*Lz/4],0.99*Rfilet, color=[0,1,0],material='gridNodeNetPackSimple',wire=False,highlight=False,fixed=False))]
                             ItCenter.append(NetPack[-1])
                             NetPack += [ O.bodies.append( gridNode([x-Lx/4,y,z+Lz],1.01*Rfilet, color=[0,1,0],material='gridNodeNetPackDouble',wire=False,highlight=False,fixed=False))]
                             Agraphes.append(NetPack[-1])
                             ItCenter.append(NetPack[-1])

##################### Détermination des coordonnees maximales et minimales ####################

xmax = CenterCoord[0]
xmin = CenterCoord[0]
zmax = CenterCoord[2]
zmin = CenterCoord[2]
id1 = NetPack[1]
xmax = O.bodies[id1].state.pos[0]
xmin = O.bodies[id1].state.pos[0]
zmax = O.bodies[id1].state.pos[2]
zmin = O.bodies[id1].state.pos[2]
for i in NetPack:
    id1 = i
    if xmax < O.bodies[i].state.pos[0]:
            xmax = O.bodies[i].state.pos[0]
        if zmax < O.bodies[i].state.pos[2]:
            zmax = O.bodies[i].state.pos[2]
        Idzmax=i
        if xmin > O.bodies[i].state.pos[0]:
            xmin = O.bodies[i].state.pos[0]
        if zmin > O.bodies[i].state.pos[2]:
            zmin = O.bodies[i].state.pos[2]
        Idzmin=i

print 'xmin =', xmin
print 'xmax =', xmax
print 'zmin =', zmin
print 'zmax =', zmax

#### y=mx+n Equation of channel slope
m=(-Hight)/(AB-xmin)
n=zmax-m*xmin

    ##################### DEFINITION INERACTIONS #####################

Pos1 = lambda x: O.bodies[x].state.pos[2]   # To sort the list
Pos0 = lambda x: O.bodies[x].state.pos[0]
list.sort(ItLeft, key = Pos1)
list.sort(ItRight, key = Pos1)
list.sort(Agraphes, key = Pos0)
list.sort(Agraphes, key = Pos1)


for i,j in zip(ItLeft[:-1], ItLeft[1:]):#it gauche
    O.bodies.append( gridConnection(i,j,Rfilet,material='CohMat',color=[1,2,1],mask=3))
for i,j in zip(ItCenter[:-1], ItCenter[1:]):#it centre
    if O.bodies[i].state.pos[2] != zmax:
        O.bodies.append( gridConnection(i,j,Rfilet,material='CohMat',color=[1,2,1],mask=3))
for i,j in zip(ItRight[:-1], ItRight[1:]):#it Droite
        O.bodies.append( gridConnection(i,j,Rfilet,material='CohMat',color=[1,2,1],mask=3))

for i,j in zip(ItCenter[:-2], ItCenter[2:]) :#it centre verticale
        if O.bodies[i].state.pos[0] == O.bodies[j].state.pos[0]:
                O.bodies.append( gridConnection(i,j,Rfilet,material='CohMat',color=[1,2,1],mask=3))
for i, j in zip(Agraphes[:-Nx], Agraphes[Nx:]):
        O.bodies.append( gridConnection(i,j,Rfilet,material='CohMat',color=[1,2,1],mask=3))

############################# NOEUDS Sup et Inf ##################################

Netsup = []         # To create the Boucle
Netinf =[]          # To have a list with the inf points
for i in O.bodies:
    if zmax == i.state.pos[2]:
        Netsup += [i.id]
    if zmin == i.state.pos[2]:
        Netinf += [i.id]
side=[]
side+=ItLeft + ItRight



########################################## Function CABLE #######################################

def CABLEc(cablez,Braid,ForcePret) :
## cablex,cablez are the initial point cable in the final position. If Braid=1 is true if Braid =0 => False

##### Parameters
    Young_cab=60e9
    Young_cabco=Young_cab
    strenruptc=1400e9           #A.Trad & D.Betrand If 360e3 Error=> Ask
    densityc=7850.
    frottement=10.
    shearCohc = 1e100
    Rcable=0.012/2
    dec_cable=2*Rcable+Rfilet       # Gap between net and cable

########################################## Material #######################################

    O.materials.append(CohFrictMat(young=Young_cab,poisson=0.3,density=densityc,frictionAngle=radians(frottement),normalCohesion=pi*strenruptc,isCohesive=True,shearCohesion=shearCohc,momentRotationLaw=False, alphaKr = 1.0,label='cablemat'))

    O.materials.append(FrictMat(young=Young_cabco,poisson=0.3,density=densityc,frictionAngle=radians(frottement),label='cablecomat'))

########################################## CÂBLE #######################################
    cable=[]
    cablecon=[]
    deplx=0.    # To create the Pretension
    deply=0.
    deplz=0.
    if  Braid==1:   # If Braid=1 the cable is braided in other case used Boucle (The boucle can only be used on the upper cable)
        posz=cablez/(Lz/2)  # Position related with loops
### to have all the time contact with the basin, its take the point after and add the last segment
        if posz!=0 and posz <= (Nz+1)*0.99:
            if 0.<=posz-int(posz)<=0.5:
                posz=int(posz)+0.5
            if 0.5 < posz-int(posz)<1:
                posz=int(posz)+1.5
        posx=((posz*(Lz/2)-n)/m+4*Rfrein+2*RBASSIN)/Lx
        if int(posx)< 1:
            if 0<posx-int(posx) < 0.25:
                posx1=int(posx)+0.25
                print '1'
            if 0.25 <= posx-int(posx) < 0.5:
                posx1=int(posx)+0.75
                print '2'
            if 0.5 <= posx-int(posx) < 1:
                posx1=int(posx)+1.75
                print '3'
        else:
            if 0.<=posx-int(posx) < 0.5:
                posx1=int(posx)+0.75
                print '4'
            if 0.5 <= posx-int(posx) < 1:
                posx1=int(posx)+1.25
                print '5'

        zCa=posz*(Lz/2)
        inipoint=Vector3(posx1*Lx+xmin,CenterCoord[1],posz*(Lz/2))
        finpoint=Vector3((Nx-posx1)*Lx+xmin,CenterCoord[1],posz*(Lz/2))
        Long_cable = finpoint-inipoint
        lfinal = abs(Long_cable)
        direction=Long_cable/abs(Long_cable)
        kx=int(lfinal/(Lx/16))
        print posx, posx1, posz
        if round(posz,1) == float(Nz+1):    # Sup Normally with boucle
            if Nz%2==0:
# create all the points that make up the cable
                cable.append(O.bodies.append(gridNode([(posx*Lx),CenterCoord[1],posz*(Lz/2)-2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                cable.append(O.bodies.append(gridNode([inipoint[0]+dec_cable/5,inipoint[1]-dec_cable,inipoint[2]-2.25*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                for i in range(kx+1):
                    if i!=0 and i != kx:
                        if i%2==0:
                            x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
                            y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                            z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx-2.25*Rcable
                            cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                        else:
                            x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
                            y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                            z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx-2.25*Rcable
                            cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                    else:
                        x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
                        y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                        z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx-2.25*Rcable
                        cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))

                cable.append(O.bodies.append(gridNode([((Nx-posx)*Lx),CenterCoord[1],posz*(Lz/2)-2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))

### Pretension
#           Long_cable=( O.bodies[cable[-1]].state.pos- O.bodies[cable[0]].state.pos)
#           lfinal = abs(Long_cable)
#           linit = Young_cabco*lfinal/((ForcePret/(pi*Rcable*Rcable))+Young_cabco)
#           deltaL=lfinal-linit
#           if ForcePret!= 0:
#               for k in cable:
#                   i=O.bodies[k]
#                   if Long_cable[0] != 0:
#                       deplx = ((direction*deltaL)[0]/(direction*linit)[0])*(i.state.pos[0]-O.bodies[cable[0]].state.pos[0])
#                   if Long_cable[1] != 0:
#                       deply = ((direction*deltaL)[1]/(direction*linit)[1])*(i.state.pos[1]-O.bodies[cable[0]].state.pos[1])
#                   if Long_cable[2] != 0:
#                       deplz = ((direction*deltaL)[2]/(direction*linit)[2])*(i.state.pos[2]-O.bodies[cable[0]].state.pos[2])
#                   i.state.pos=Vector3(i.state.pos[0]+deplx,i.state.pos[1]+deply,i.state.pos[2]+deplz)

## Braid
                for i in range((len(cable))/8):
                    for j in range(8):
                        if i%2!=0:
                            if j<7:
                                x = O.bodies[cable[2+j+i*8]].state.pos[0]
                                y = O.bodies[cable[2+j+i*8]].state.pos[1] -(dec_cable/(8))*j
                                z = O.bodies[cable[2+j+i*8]].state.pos[2]
                                O.bodies[cable[2+j+i*8]].state.pos=(x,y,z)
                            if 7<=j<8:
                                x = O.bodies[cable[2+j+i*8]].state.pos[0]
                                y = O.bodies[cable[2+j+i*8]].state.pos[1] -(1.2*dec_cable/(8))*j
                                z = O.bodies[cable[2+j+i*8]].state.pos[2]
                                O.bodies[cable[2+j+i*8]].state.pos=(x,y,z)
                        else:
                            if j<2 :
                                x = O.bodies[cable[2+j+i*8]].state.pos[0]
                                y = O.bodies[cable[2+j+i*8]].state.pos[1] +(1.2*dec_cable)*j
                                z = O.bodies[cable[2+j+i*8]].state.pos[2]
                                O.bodies[cable[2+j+i*8]].state.pos=(x,y,z)
                            if i==0 and j==0:
                                x = O.bodies[cable[2+j+i*8]].state.pos[0] +dec_cable/5
                                y = O.bodies[cable[2+j+i*8]].state.pos[1]
                                z = O.bodies[cable[2+j+i*8]].state.pos[2] -dec_cable/10
                                O.bodies[cable[2+j+i*8]].state.pos=(x,y,z)
                            if 2<=j<8:
                                x = O.bodies[cable[2+j+i*8]].state.pos[0]
                                y = O.bodies[cable[2+j+i*8]].state.pos[1] -(dec_cable/(8))*j+dec_cable
                                z = O.bodies[cable[2+j+i*8]].state.pos[2]
                                O.bodies[cable[2+j+i*8]].state.pos=(x,y,z)
                O.bodies[cable[0]].state.pos=(O.bodies[cable[-0]].state.pos[0]+dec_cable/1.4,O.bodies[cable[0]].state.pos[1]-1.4*dec_cable,O.bodies[cable[0]].state.pos[2])
                O.bodies[cable[-1]].state.pos=(O.bodies[cable[-1]].state.pos[0],O.bodies[cable[-1]].state.pos[1]-dec_cable,O.bodies[cable[-1]].state.pos[2])

            else:
                cable.append(O.bodies.append(gridNode([(posx*Lx),CenterCoord[1],posz*(Lz/2)-2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                for i in range(kx+1):
                    if i!=0 and i != kx:
                        if i%2==0:
                            x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
                            y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                            z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx-2.25*Rcable
                            cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                        else:
                            x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
                            y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                            z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx-2.25*Rcable
                            cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                    else:
                        x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx

                        y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                        z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx-2.25*Rcable
                        cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))

                cable.append(O.bodies.append(gridNode([((Nx-posx)*Lx),CenterCoord[1],posz*(Lz/2)-2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                for i in range((len(cable))/8):
                    for j in range(8):
                        if i%2!=0:
                            if j==0:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0] +dec_cable/5
                                y = O.bodies[cable[1+j+i*8]].state.pos[1]
                                z = O.bodies[cable[1+j+i*8]].state.pos[2] -dec_cable/10
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                            if 0<j<=7:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(dec_cable/(8))*j+dec_cable
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                        else:
                            if j<7:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(dec_cable/(8))*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                            if j==7 :
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(1.2*dec_cable)
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                O.bodies[cable[-1]].state.pos=(O.bodies[cable[-1]].state.pos[0],O.bodies[cable[-1]].state.pos[1]+dec_cable,O.bodies[cable[-1]].state.pos[2])
            for i,j in zip(cable[:-1],cable[1:]):
                cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0],mask=3) ) )
            O.bodies[cable[0]].state.blockedDOFs = 'xyzXYZ'
            O.bodies[cable[-1]].state.blockedDOFs = 'xyzXYZ'
            O.bodies[cable[0]].shape.color=[0.5,.75,1]
            O.bodies[cable[-1]].shape.color=[0.5,.75,1]

        if posz!=0 and  round(posz,1) != float(Nz+1): # Cent Normally braided
# create all the points that make up the cable
            cable.append(O.bodies.append(gridNode([(posx*Lx),CenterCoord[1],posz*(Lz/2)],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
            print O.bodies[-1].state.pos
            for i in range(kx+1):
                x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
                y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
                z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx
                cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
            cable.append(O.bodies.append(gridNode([((Nx-posx)*Lx),CenterCoord[1],posz*(Lz/2)],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
### To do the first Tension
#           for i,j in zip(cable[:-1],cable[1:]):
#               cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0],mask=3) ) )
###  Pretension
#           Long_cable=( O.bodies[cable[-1]].state.pos- O.bodies[cable[0]].state.pos)
#
#           lfinal = abs(Long_cable)
#           linit = Young_cabco*lfinal/((ForcePret/(pi*Rcable*Rcable))+Young_cabco)
#           deltaL=lfinal-linit
#           if ForcePret!= 0:
#               for k in cable:
#                   i=O.bodies[k]
#                   if Long_cable[0] != 0:
#                       deplx = ((direction*deltaL)[0]/(direction*linit)[0])*(i.state.pos[0]-O.bodies[cable[0]].state.pos[0])
#                   if Long_cable[1] != 0:
#                       deply = ((direction*deltaL)[1]/(direction*linit)[1])*(i.state.pos[1]-O.bodies[cable[0]].state.pos[1])
#                   if Long_cable[2] != 0:
#                       deplz = ((direction*deltaL)[2]/(direction*linit)[2])*(i.state.pos[2]-O.bodies[cable[0]].state.pos[2])
#                   i.state.pos=Vector3(i.state.pos[0]+deplx,i.state.pos[1]+deply,i.state.pos[2]+deplz)
## Braid
            for i in range((len(cable)-1)/8):
                for j in range(8):
                    if i%2!=0:
                        if j<4:
                            x = O.bodies[cable[1+j+i*8]].state.pos[0]
                            y = O.bodies[cable[1+j+i*8]].state.pos[1] -(2*dec_cable/(8))*j
                            z = O.bodies[cable[1+j+i*8]].state.pos[2]
                            O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                        else:
                            x = O.bodies[cable[1+j+i*8]].state.pos[0]
                            y = O.bodies[cable[1+j+i*8]].state.pos[1] +(2*dec_cable/(8))*j-2*dec_cable
                            z = O.bodies[cable[1+j+i*8]].state.pos[2]
                            O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                    else:
                        if j<4:
                            x = O.bodies[cable[1+j+i*8]].state.pos[0]
                            y = O.bodies[cable[1+j+i*8]].state.pos[1] +(2*dec_cable/(8))*j
                            z = O.bodies[cable[1+j+i*8]].state.pos[2]
                            O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                        else:
                            x = O.bodies[cable[1+j+i*8]].state.pos[0]
                            y = O.bodies[cable[1+j+i*8]].state.pos[1] -(2*dec_cable/(8))*j+2*dec_cable
                            z = O.bodies[cable[1+j+i*8]].state.pos[2]
                            O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
            O.bodies[cable[0]].state.pos=(O.bodies[cable[-0]].state.pos[0],O.bodies[cable[0]].state.pos[1]-dec_cable,O.bodies[cable[0]].state.pos[2])
            O.bodies[cable[1]].state.pos=(O.bodies[cable[1]].state.pos[0],O.bodies[cable[0]].state.pos[1],O.bodies[cable[1]].state.pos[2])
            O.bodies[cable[-1]].state.pos=(O.bodies[cable[-1]].state.pos[0],O.bodies[cable[-1]].state.pos[1]-dec_cable,O.bodies[cable[-1]].state.pos[2])
            O.bodies[cable[-2]].state.pos=(O.bodies[cable[-2]].state.pos[0],O.bodies[cable[-1]].state.pos[1],O.bodies[cable[-2]].state.pos[2])

            for i,j in zip(cable[:-1],cable[1:]):
                cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0], mask=3) ) )
        if posz ==0:    # Inf normaly braided

# create all the points that make up the cable
            cable.append(O.bodies.append(gridNode([(posx*Lx),CenterCoord[1],posz*(Lz/2)++2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))

            if posx1-int(posx1)== 0.25:
                for i in range(kx+1):
                    x = inipoint[0] + (direction*lfinal)[0]*i/kx
                    y = inipoint[1] + (direction*lfinal)[1]*i/kx
                    z = inipoint[2] + (direction*lfinal)[2]*i/kx+2.25*Rcable
                    cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                cable.append(O.bodies.append(gridNode([((Nx-posx)*Lx+xmin),CenterCoord[1],posz*(Lz/2)+2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
### To do the first Tension
#               for i,j in zip(cable[:-1],cable[1:]):
#                   cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0],mask=3) ) )
### Pretension
#               Long_cable=( O.bodies[cable[-1]].state.pos- O.bodies[cable[0]].state.pos)
#               lfinal = abs(Long_cable)
#               linit = Young_cabco*lfinal/((ForcePret/(pi*Rcable*Rcable))+Young_cabco)
#               deltaL=lfinal-linit
#               if ForcePret!= 0:
#                   for k in cable:
#                       i=O.bodies[k]
#                       if Long_cable[0] != 0:
#                           deplx = ((direction*deltaL)[0]/(direction*linit)[0])*(i.state.pos[0]-O.bodies[cable[0]].state.pos[0])
#                       if Long_cable[1] != 0:
#                           deply = ((direction*deltaL)[1]/(direction*linit)[1])*(i.state.pos[1]-O.bodies[cable[0]].state.pos[1])
#                       if Long_cable[2] != 0:
#                           deplz = ((direction*deltaL)[2]/(direction*linit)[2])*(i.state.pos[2]-O.bodies[cable[0]].state.pos[2])
#                       i.state.pos=Vector3(i.state.pos[0]+deplx,i.state.pos[1]+deply,i.state.pos[2]+deplz)
### Braid
                for i in range((len(cable))/8):
                    for j in range(8):
                        if i%2!=0:
                            if j<2:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] +(1.2*dec_cable)*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                            if 2<=j<8:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(dec_cable/(8))*j+dec_cable
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                        else:
                            if j<7:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(dec_cable/(8))*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                            if 7<=j<8:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(1.2*dec_cable/(8))*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)

                O.bodies[cable[0]].state.pos=(O.bodies[cable[-0]].state.pos[0],O.bodies[cable[0]].state.pos[1]-dec_cable,O.bodies[cable[0]].state.pos[2])
                O.bodies[cable[-2]].state.pos=(O.bodies[cable[-2]].state.pos[0],O.bodies[cable[-2]].state.pos[1]+dec_cable/3,O.bodies[cable[-2]].state.pos[2]+dec_cable/5)
                O.bodies[cable[-1]].state.pos=(O.bodies[cable[-1]].state.pos[0],O.bodies[cable[-1]].state.pos[1]+dec_cable,O.bodies[cable[-1]].state.pos[2])
            if posx1-int(posx1)== 0.75:
                for i in range(kx+1):
                    x = inipoint[0] + (direction*lfinal)[0]*i/kx
                    y = inipoint[1] + (direction*lfinal)[1]*i/kx
                    z = inipoint[2] + (direction*lfinal)[2]*i/kx+2.25*Rcable
                    cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
                cable.append(O.bodies.append(gridNode([((Nx-posx)*Lx),CenterCoord[1],posz*(Lz/2)+2.1*Rcable],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
### To do the first Tension
#               for i,j in zip(cable[:-1],cable[1:]):
#                   cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0],mask=3) ) )
### Pretension
#               Long_cable=( O.bodies[cable[-1]].state.pos- O.bodies[cable[0]].state.pos)
#               lfinal = abs(Long_cable)
#               linit = Young_cabco*lfinal/((ForcePret/(pi*Rcable*Rcable))+Young_cabco)
#               deltaL=lfinal-linit
#               if ForcePret!= 0:
#                   for k in cable:
#                       i=O.bodies[k]
#                       if Long_cable[0] != 0:
#                           deplx = ((direction*deltaL)[0]/(direction*linit)[0])*(i.state.pos[0]-O.bodies[cable[0]].state.pos[0])
#                       if Long_cable[1] != 0:
#                           deply = ((direction*deltaL)[1]/(direction*linit)[1])*(i.state.pos[1]-O.bodies[cable[0]].state.pos[1])
#                       if Long_cable[2] != 0:
#                           deplz = ((direction*deltaL)[2]/(direction*linit)[2])*(i.state.pos[2]-O.bodies[cable[0]].state.pos[2])
#                       i.state.pos=Vector3(i.state.pos[0]+deplx,i.state.pos[1]+deply,i.state.pos[2]+deplz)
## Braid
                for i in range((len(cable))/8):
                    for j in range(8):
                        if i%2!=0:
                            if j<7:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(dec_cable/(8))*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                            if 7<=j<8:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(1.2*dec_cable/(8))*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                        else:
                            if j<2:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] +(1.2*dec_cable)*j
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)
                            if 2<=j<8:
                                x = O.bodies[cable[1+j+i*8]].state.pos[0]
                                y = O.bodies[cable[1+j+i*8]].state.pos[1] -(dec_cable/(8))*j+dec_cable
                                z = O.bodies[cable[1+j+i*8]].state.pos[2]
                                O.bodies[cable[1+j+i*8]].state.pos=(x,y,z)

                O.bodies[cable[0]].state.pos=(O.bodies[cable[-0]].state.pos[0],O.bodies[cable[0]].state.pos[1]-dec_cable,O.bodies[cable[0]].state.pos[2])
                O.bodies[cable[1]].state.pos=(O.bodies[cable[1]].state.pos[0]-dec_cable/5,O.bodies[cable[0]].state.pos[1]-dec_cable/2,O.bodies[cable[1]].state.pos[2])
                O.bodies[cable[2]].state.pos=(O.bodies[cable[2]].state.pos[0]-dec_cable/1.5,O.bodies[cable[2]].state.pos[1],O.bodies[cable[2]].state.pos[2])
                O.bodies[cable[-1]].state.pos=(O.bodies[cable[-1]].state.pos[0],O.bodies[cable[-1]].state.pos[1]-dec_cable,O.bodies[cable[-1]].state.pos[2])
            for i,j in zip(cable[:-1],cable[1:]):
                cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0],mask=3) ) )

            O.bodies[cable[0]].state.blockedDOFs = 'xyzXYZ'
            O.bodies[cable[-1]].state.blockedDOFs = 'xyzXYZ'

            O.bodies[cable[0]].shape.color=[0.5,.75,1]
            O.bodies[cable[-1]].shape.color=[0.5,.75,1]
    else:
        if cablez==zmax:
            zCa=cablez+decman
            xCa=(zCa-n)*(Lz/2)/m+xmin+4*Rfrein+2*RBASSIN
            inipoint=Vector3(xCa,CenterCoord[1],zCa)
            finpoint=Vector3(Base-xCa,CenterCoord[1],zCa)
            print inipoint
        else:
            posz=cablez/(Lz/2)  # Position related with loops
            posx=(cablez-n)/m/Lx
            if posz!=0 and posz <= (Nz+1)*0.99:
                if 0.<=posz-int(posz)<=0.5:
                    posz=int(posz)+0.5
                if 0.5 < posz-int(posz)<1:
                    posz=int(posz)-0.5
            zCa=posz*(Lz/2)
            xCa=(posz*(Lz/2)-n)/m+xmin+4*Rfrein+2*RBASSIN
            inipoint=Vector3(xCa,CenterCoord[1]-decman,zCa)
            finpoint=Vector3(Base-xCa,CenterCoord[1]-decman,zCa)
            print inipoint
        Long_cable = finpoint-inipoint
        lfinal = abs(Long_cable)
        linit = Young_cabco*lfinal/((ForcePret/(pi*Rcable*Rcable))+Young_cabco)
        deltaL=lfinal-linit
        direction=Long_cable/abs(Long_cable)
        kx=int(lfinal/(Lx/16))
# create all the points that make up the cable
        for i in range(kx+1):
            x = inipoint[0] + (direction*abs(Long_cable))[0]*i/kx
            y = inipoint[1] + (direction*abs(Long_cable))[1]*i/kx
            z = inipoint[2] + (direction*abs(Long_cable))[2]*i/kx
            cable.append(O.bodies.append(gridNode([x,y,z],Rcable,wire=False,fixed=False,material='cablemat',color=[1,01,0])))
        for i,j in zip(cable[:-1],cable[1:]):
            cablecon.append( O.bodies.append(gridConnection(i,j,Rcable,wire=False, material = 'cablecomat',color=[1,01,0],mask=3) ) )
### Pretension
        if ForcePret!= 0:
            for k in cable:
                i=O.bodies[k]
                if Long_cable[0] != 0:
                    deplx = ((direction*deltaL)[0]/(direction*linit)[0])*(i.state.pos[0]-O.bodies[cable[0]].state.pos[0])
                if Long_cable[1] != 0:
                    deply = ((direction*deltaL)[1]/(direction*linit)[1])*(i.state.pos[1]-O.bodies[cable[0]].state.pos[1])
                if Long_cable[2] != 0:
                    deplz = ((direction*deltaL)[2]/(direction*linit)[2])*(i.state.pos[2]-O.bodies[cable[0]].state.pos[2])
                i.state.pos=Vector3(i.state.pos[0]+deplx,i.state.pos[1]+deply,i.state.pos[2]+deplz)
    return (cable, cablecon, zCa)

#######################################################################################################################################

### Creacion de los cables ###


## Position z cables ##
#z_cable=[zmax,6.5,zmax/1.5,zmax/2,zmax/2.2,zmin]

CABLELIST=[] ### TODA LA INF DEL CABLE
for i in range(len(z_cable)):
    CABLELIST.append(CABLEc(z_cable[i],Braid[i],ForcePretini[i]))

CABLE=[]    ##### Contiene Gridnode y GridConnection
Z_real=[]   ##### Contiene Z real de los cables
Cable_iniend=[] ##### Contiene estremos del cable
for i in range(len(CABLELIST)):
    CABLE+=CABLELIST[i][0]+CABLELIST[i][1]
    Z_real.append(CABLELIST[i][2])
    Cable_iniend.append([CABLELIST[i][0][0],CABLELIST[i][0][-1]])

#################################### Function Frein #######################################

def Freins(Pos1, Pos2, Fdec,radius):

    colorf=[0,1,0]
    Asf = pi*pow(radius,2)
# Stress–strain curve
    Ffrein = Fdec
    E1f = 60e9              #module du cable
    sig1f = Ffrein/Asf
    def1f = sig1f/E1f

    E2f = 8e5
    def2f = 104             #Calibrated
    sig2f = sig1f+(def2f-def1f)*E2f

    sigruptf = 383e3/(Asf)          #1400e6 (Experimental date)
    defruptf = def2f + (sigruptf-sig2f)/E1f
    strainStressValuesf=[(def1f,sig1f),(def2f,sig2f),(defruptf,sigruptf)]
    densityf = 7850
    youngf = strainStressValuesf[0][1] / strainStressValuesf[0][0]
    poissonf = 0.3

    freinmat = O.materials.append(WireMat(young=youngf, poisson=poissonf,frictionAngle=radians(10),density=densityf,isDoubleTwist=False,diameter=2*radius,strainStressValues=strainStressValuesf))

    frein = []
    frein.extend([
    O.bodies.append(utils.sphere(Pos1,radius,wire=False,fixed=False,material=freinmat,color=colorf,mask=3)),
    O.bodies.append(utils.sphere(Pos2,radius,wire=False,fixed=False,material=freinmat,color=colorf,mask=3))])
    createInteraction(frein[0],frein[1])

    return (frein)

#################### Creacion de frenos ################
Frein=[] #### Contiene frenos lista de frenos por cable [[c1fr1],[c1fr2],[c2fr1],etc]
for i in  range(len( Cable_iniend)):
    Frein.append(Freins ( [m+n for m,n in zip(O.bodies[Cable_iniend[i][0]].state.pos,[-2*Rcable,0,0])],[m+n for m,n in zip(O.bodies[Cable_iniend[i][0]].state.pos,[-4*Rcable,0,0])],Ffrein,Rfrein))
    Frein.append(Freins ( [m+n for m,n in zip(O.bodies[Cable_iniend[i][-1]].state.pos,[+2*Rcable,0,0])],[m+n for m,n in zip(O.bodies[Cable_iniend[i][-1]].state.pos,[+4*Rcable,0,0])],Ffrein,Rfrein))


################################### MANILLE ##################################

def Manille_Vert(Rman, ouv, posX, posY, posZ, base):

    youngb =5e8         #A.Trad & D.Betrand:  105GPa * Cylinder area / Steel area
    strenruptb=100e9    #FORCE #A.Trad & D.Betrand
    densityb=7850.
    poissonb = 0.3
    shearCohb = 1e100
    frottement = 10     ### Ask

    bouclemat = O.materials.append(CohFrictMat(young=youngb,poisson=poissonb,density=densityb,frictionAngle=radians(frottement),normalCohesion=pi*strenruptb,shearCohesion=shearCohb,momentRotationLaw=True))
    bouclecomat = O.materials.append(FrictMat(young=youngb,poisson=poissonb,density=densityb,frictionAngle=radians(frottement)))
    """
    Maillage d'une manille orientée verticalement et attachée

    Rman = rayon de la section d'acier
    ouv = ouverture interieure de la manille, de bord à bord
    posX, Y, Z = position du centre de la manille
    base = sphere à laquelle la manille est attachée
    bouclemat = matériaux pour les noeud
    bouclecomat = matériaux pour les connexion
    numclump = numero du noeud qui est clump (entre 0 et 3)
    """
    colorb=[1,0,0]
    boucle = []
    dec = Rman + ouv/2      # Gap
    wire = True
    boucle.extend([
        O.bodies.append(gridNode([posX,posY,posZ+dec],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
        O.bodies.append(gridNode([posX,posY+dec,posZ],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
        O.bodies.append(gridNode([posX,posY,posZ-dec],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
        O.bodies.append(gridNode([posX,posY-dec,posZ],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)) ])

    conboucle=[]
    for i,j in zip(boucle[:-1],boucle[1:]):
        conboucle.append( O.bodies.append( gridConnection(i,j,Rman,wire=wire, material = bouclecomat,color=colorb,mask=3) ) )
    conboucle.append( O.bodies.append( gridConnection(boucle[0],boucle[-1],Rman,wire=wire, material = bouclecomat,color=colorb,mask=3) ) )

        ############ CLUMP ############
    clumping = []
    clumping.extend([boucle[0],boucle[1],boucle[2],boucle[3],base])
    clumps = O.bodies.clump(clumping)

    return boucle,conboucle,clumps
def Manille_frein(Rman, ouv, posX, posY, posZ,Dir):

    youngb =5e8         #A.Trad & D.Betrand:  105GPa * Cylinder area / Steel area
    strenruptb=100e9    #FORCE #A.Trad & D.Betrand
    densityb=7850.
    poissonb = 0.3
    shearCohb = 1e100
    frottement = 10     ### Ask

    bouclemat = O.materials.append(CohFrictMat(young=youngb,poisson=poissonb,density=densityb,frictionAngle=radians(frottement),normalCohesion=pi*strenruptb,shearCohesion=shearCohb,momentRotationLaw=True))
    bouclecomat = O.materials.append(FrictMat(young=youngb,poisson=poissonb,density=densityb,frictionAngle=radians(frottement)))
    """
    Maillage d'une manille orientée verticalement et attachée

    Rman = rayon de la section d'acier
    ouv = ouverture interieure de la manille, de bord à bord
    posX, Y, Z = position du centre de la manille
    base = sphere à laquelle la manille est attachée
    bouclemat = matériaux pour les noeud
    bouclecomat = matériaux pour les connexion
    numclump = numero du noeud qui est clump (entre 0 et 3)
    Dir direction du body
    """
    colorf=[0,1,0]
    Asf = pi*pow(Rfrein,2)
# Stress–strain curve
    E1f = 60e9              #module du cable
    sig1f = Ffrein/Asf
    def1f = sig1f/E1f

    E2f = 8e5
    def2f = 104             #Calibrated
    sig2f = sig1f+(def2f-def1f)*E2f

    sigruptf =1400e6/(Asf)          #1400e6 (Experimental date)
    defruptf = def2f + (sigruptf-sig2f)/E1f
    strainStressValuesf=[(def1f,sig1f),(def2f,sig2f),(defruptf,sigruptf)]
    densityf = 7850
    youngf = strainStressValuesf[0][1] / strainStressValuesf[0][0]
    poissonf = 0.3

    freinmat = O.materials.append(WireMat(young=youngf, poisson=poissonf,frictionAngle=radians(10),density=densityf,isDoubleTwist=False,diameter=2*Rfrein,strainStressValues=strainStressValuesf))

    colorb=[1,0,0]
    boucle = []
    dec = Rman + ouv/2      # Gap
    wire = True
    if Dir=='Left':
        boucle.extend([
            O.bodies.append(gridNode([posX-dec*cos(2*Lz/Lx),posY,posZ+dec*sin(2*Lz/Lx)],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
            O.bodies.append(gridNode([posX,posY+dec,posZ],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
            O.bodies.append(gridNode([posX+dec*cos(2*Lz/Lx),posY,posZ-dec*sin(2*Lz/Lx)],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
            O.bodies.append(gridNode([posX,posY-dec,posZ],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)) ])
    else:
        boucle.extend([
            O.bodies.append(gridNode([posX+dec*cos(2*Lz/Lx),posY,posZ+dec*sin(2*Lz/Lx)],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
            O.bodies.append(gridNode([posX,posY+dec,posZ],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
            O.bodies.append(gridNode([posX-dec*cos(2*Lz/Lx),posY,posZ-dec*sin(2*Lz/Lx)],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)),
            O.bodies.append(gridNode([posX,posY-dec,posZ],Rman,wire=wire,fixed=False,material=bouclemat,color=colorb)) ])

    frein = []
    if Dir=='Left':
        frein.extend([
        O.bodies.append(utils.sphere([O.bodies[boucle[0]].state.pos[0],O.bodies[boucle[0]].state.pos[1],O.bodies[boucle[0]].state.pos[2]*1.001],Rman,wire=False,fixed=False,material=freinmat,color=colorf,mask=3)),
        O.bodies.append(utils.sphere([i+j for i,j in zip(O.bodies[boucle[0]].state.pos,[-dec*cos(2*Lz/Lx),0,dec*sin(2*Lz/Lx)])],Rman,wire=False,fixed=False,material=freinmat,color=colorf,mask=3))])
        O.bodies[-1].shape.color.color=[0.5,0.5,0.5]
    else:
        frein.extend([
        O.bodies.append(utils.sphere([O.bodies[boucle[0]].state.pos[0],O.bodies[boucle[0]].state.pos[1],O.bodies[boucle[0]].state.pos[2]*1.001],Rman,wire=False,fixed=False,material=freinmat,color=colorf,mask=3)),
        O.bodies.append(utils.sphere([i+j for i,j in zip(O.bodies[boucle[0]].state.pos,[dec*cos(2*Lz/Lx),0,dec*sin(2*Lz/Lx)])],Rman,wire=False,fixed=False,material=freinmat,color=colorf,mask=3))])
    createInteraction(frein[0],frein[1])

    conboucle=[]
    for i,j in zip(boucle[:-1],boucle[1:]):
        conboucle.append( O.bodies.append( gridConnection(i,j,Rman,wire=wire, material = bouclecomat,color=colorb,mask=3) ) )
    conboucle.append( O.bodies.append( gridConnection(boucle[0],boucle[-1],Rman,wire=wire, material = bouclecomat,color=colorb,mask=3) ) )

        ############ CLUMP ############
    clumping = []
    clumping.extend([boucle[0],boucle[1],boucle[2],boucle[3],frein[0]])
    clumps = O.bodies.clump(clumping)
    TTT=frein[1]
    return boucle,conboucle,clumps,TTT
boucle,conboucle,shackles=[],[],[]

# Creacion de las manillas para todos los elementos
for i in range(len(Braid)):
    if Braid[i]==0:
        if Z_real[i]>=zmax:
            for j in range(len(Netsup)):
                if j==0 or j==len(Netsup)-1:
                    if j==0:
                        for k in CABLELIST[0][0]:
                            if O.bodies[k].state.pos[0]-Lx/32<O.bodies[Netsup[j]].state.pos[0]<O.bodies[k].state.pos[0]+Lx/32:
                                PPP=[O.bodies.clump([Netsup[j],k]),[Netsup[j],k]]
                    else:
                        for k in CABLELIST[0][0]:
                            if O.bodies[k].state.pos[0]-Lx/32<O.bodies[Netsup[j]].state.pos[0]<O.bodies[k].state.pos[0]+Lx/32:
                                TTT=[O.bodies.clump([Netsup[j],k]),[Netsup[j],k]]
                else:
                    posX = O.bodies[Netsup[j]].state.pos[0]
                    aa,bb,cc = Manille_Vert(Rman = Rman, ouv = ouv, posX = posX, posY = CenterCoord[1], posZ = Z_real[i], base = Netsup[j])
                    boucle.append(aa)
                    conboucle.append(bb)
                    shackles.append(cc)
        else:
            for j in NetPack:
                if round(Z_real[i],4) == round(O.bodies[j].state.pos[2],4) and O.bodies[Cable_iniend[i][0]].state.pos[0]<=O.bodies[j].state.pos[0]<=O.bodies[Cable_iniend[i][-1]].state.pos[0]:
                    posX = O.bodies[j].state.pos[0]
                    aa,bb,cc = Manille_Vert(Rman = Rman, ouv = ouv, posX = posX, posY = CenterCoord[1]-decman, posZ = Z_real[i], base = j)
                    boucle.append(aa)
                    conboucle.append(bb)
                    shackles.append(cc)
N_Braid=0.
for i in Braid:
    N_Braid+=i
if N_Braid<len(Braid):
    shackles_la=[PPP[0],TTT[0]]

################################### Iteraction Frein-Cable #######################################

C_F=[]
clumpCF=[]
for i in  range(len( Cable_iniend)):
    C_F+=[O.bodies.clump([Cable_iniend[i][0],Frein[i*2][0]])]
    C_F+=[O.bodies.clump([Cable_iniend[i][-1],Frein[i*2+1][0]])]
    clumpCF+=[Cable_iniend[i][0],Frein[i*2][0],Cable_iniend[i][-1],Frein[i*2+1][0]]

##############################################################################################
################################### CONDITIONS LIMITES #######################################
#####################################################################   #########################

velz=-0.75
vely=-0.25
# Blocked external point breaks

for i in range(len( Frein)):
    O.bodies[Frein[i][-1]].state.blockedDOFs = 'xyzXYZ'

for i in C_F:       # Fixing Iteraction Frein-Cable
    O.bodies[i].state.blockedDOFs = 'xyzXYZ'

## Blocked shackles

if N_Braid<len(Braid):
    for i in shackles:
        O.bodies[i].state.blockedDOFs = 'xyzXYZ'
## Blocked cables
if N_Braid<len(Braid):
    for i in CABLE:
        if not i in clumpCF or i in PPP[1] or i in TTT[1]:
            O.bodies[i].state.blockedDOFs = 'xyzXYZ'
else:
    for i in CABLE:
        if not i in clumpCF:
            O.bodies[i].state.blockedDOFs = 'xyzXYZ'

if N_Braid<len(Braid):
    for i in shackles_la:
        O.bodies[i].state.blockedDOFs = 'xyzXYZ'


########################## Functions ##############################

#########################
## FUNCTION DEFINITION ##
#########################

def gravityDeposit():
    if O.iter>5000 and kineticEnergy()<10 and unbalancedForce()<1 and MeshStabilized:
        #O.pause()
        for i in DepBox:
            O.bodies.erase(i)
        gravDep.dead=True
        #Drag.dead=False
        dista.dead=False
        velProfil.dead=False
        bedshear_tot_engine.dead=False
        bedshear_1_engine.dead=False
        bedshear_2_engine.dead=False
        bedshear_3_engine.dead=False
        bedshear_4_engine.dead=False
        newton.damping=0.
        FWall.dead=False
#       O.save("deposit-mono5cm.yade")
        print "Gravity deposit finished & gate opened"

def addPack(minX,minY,minZ,maxX,maxY,maxZ,num):
    global spIds
    PackAdd = pack.SpherePack()
    PackAdd.makeCloud(minCorner=(minX,minY,minZ),maxCorner=(maxX,maxY,maxZ),rMean=diameterPart/2.,num=num)
    spIdsAdd=PackAdd.toSimulation(material=mat,color=[205./255,175./255,149./255])
    spIds=spIds+spIdsAdd
    #Drag.ids=spIds

xs=np.linspace(0,length_channel,int(length_channel)*10)
zs=np.linspace(0,height_channel,int(height_channel)*100)
max_pos_x, height_front, vel_front, max_vel_front=0.,0.,Vector3(0.,0.,0.),0.
def front():
    global max_vel_front
    #fvel=open(folder+"front.txt","a")
    #fvel3D=open(folder+"front3D.txt","a")
#   for i in spIds:
#       O.bodies[i].shape.color=[205./255,175./255,149./255]
    list_x,list_z=[],[]
    for i in spIds:
        list_x.append(O.bodies[i].state.pos[0])
        list_z.append(O.bodies[i].state.pos[2])
    density_x = gaussian_kde(list_x)
    density_x.covariance_factor = lambda : .05
    density_x._compute_covariance()
    density_z = gaussian_kde(list_z)
    density_z.covariance_factor = lambda : .05
    density_z._compute_covariance()

    max_density_x=max(density_x(xs))
    max_pos_x=xs[max([i for i,x in enumerate(density_x(xs)) if x>0.5*max_density_x])]

    height=max([x for x in zs if 0.95>density_z.integrate_box_1d(0,x)])+diameterPart/2.

    vel_front,a=Vector3(0.,0.,0.),0
    for i in spIds:
        if abs(O.bodies[i].state.pos[0] - max_pos_x) <=0.5:
#           O.bodies[i].shape.color=[0./255,0./255,255./255]
            vel_front+=O.bodies[i].state.vel
            a+=1
    if a!=0:
        vel_front=vel_front/float(a)
        #Drag.V=vel_front

    max_vel_front=max(max_vel_front,vel_front[0])

    #fvel.write(str(O.time)+";"+str(max_pos_x)+";"+str(height)+";"+str(vel_front[0])+"\n")
    #fvel3D.write(str(O.time)+";"+str(vel_front[0])+";"+str(vel_front[1])+";"+str(vel_front[2])+"\n")
    #fvel.close()
    #fvel3D.close()

    return max_pos_x,height, vel_front

def distance(a,b):
    return sqrt((a.state.pos[0]-b.state.pos[0])**2+(a.state.pos[1]-b.state.pos[1])**2+(a.state.pos[2]-b.state.pos[2])**2)

deadZoneIds=[]
def deadZone():
    global spIds
    for i in spIds:
        if (O.bodies[i].state.vel[0]/float(max_vel_front)) < 0.05:
            spIds.remove(i)
            deadZoneIds.append(i)
    if spIds==[]:
        dista.dead=True
        velProfil.dead=True
    #Drag.ids=spIds

def vel_profil():
    #velocity_profil=open(folder+"velocity_profil.txt","a") #ecriture dans un fichier velocity_profil.txt
    #velocity_profil.write(str(O.time)+";"+str(max_pos_x)+";")
    l=[]
    for i in spIds:
        if max_pos_x-1.2 <= O.bodies[i].state.pos[0] <= max_pos_x-0.8:
            l.append(i)

    list_vel_x,list_vel_y,list_vel_z="0","0","0"
    for h in range(int(height_front/diameterPart)):
        vel_x,vel_y,vel_z,a=0.,0.,0.,0
        for i in l:
            if h*diameterPart <= O.bodies[i].state.pos[2] <= (h+1)*diameterPart:
                vel_x+=O.bodies[i].state.vel[0]
                vel_y+=O.bodies[i].state.vel[1]
                vel_z+=O.bodies[i].state.vel[2]
                a+=1
        if a!=0:
            list_vel_x+=":"+str(vel_x/a)
            list_vel_y+=":"+str(vel_y/a)
            list_vel_z+=":"+str(vel_z/a)
    #velocity_profil.write(str(height_front)+";"+str(list_vel_x)+";"+str(list_vel_y)+";"+str(list_vel_z)+"\n")
    #velocity_profil.close()

FtotX,FtotY,FtotZ,tF=[],[],[],[]
def Force_wall():
    #f=open(folder+"Force_wall.txt","a")
    ### Calculate each component of the total NORMAL force between impacting particles and the wall:
    ### Diviser le mur en multiple tranche

    Fx=0
    Fy=0
    Fz=0
    for i in O.interactions:
        if i.id1 == rigWall[0]:
            deadZone_engine.dead=False
            Fx+= i.phys.normalForce[0]
            Fy+= i.phys.normalForce[1]
            Fz+= i.phys.normalForce[2]
    FtotX.append(Fx)
    FtotY.append(Fy)
    FtotZ.append(Fz)
    tF.append(O.time)
    #f.write(str(O.time)+";"+str(Fx)+";"+str(Fy)+";"+str(Fz)+"\n")
    #f.close()
    return Fx, Fy, Fz

shearLub_tot,shearCon_tot=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
def bedshear_total():
    shearLub,shearCon=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
    #fbedshear=open(folder+"bedshear.txt","a")
    for i in O.interactions:
        if i.id1 in chanB:
            shearLub+=i.phys.shearLubricationForce
            shearCon+=i.phys.shearContactForce
    #fbedshear.write(str(O.time)+";"+str(shearLub[0])+";"+str(shearLub[1])+";"+str(shearLub[2])+";"+str(shearCon[0])+";"+str(shearCon[1])+";"+str(shearCon[2])+"\n")
    #fbedshear.close()
    return shearLub,shearCon

shearLub1,shearCon1=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
shearLub2,shearCon2=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
shearLub3,shearCon3=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
shearLub4,shearCon4=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
shearLub5,shearCon5=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
def bedshear_at_x(x):
    #fbedshear=open(folder+"bedshear_at_"+str(x)+".txt","a")
    shearLub,shearCon=Vector3(0.,0.,0.),Vector3(0.,0.,0.)
    for i in O.interactions:
        if (i.id1 in chanB) and (x-0.5 <= O.bodies[i.id2].state.pos[0] <= x+0.5):
            shearLub+=i.phys.shearLubricationForce
            shearCon+=i.phys.shearContactForce
    #fbedshear.write(str(O.time)+";"+str(shearLub[0])+";"+str(shearLub[1])+";"+str(shearLub[2])+";"+str(shearCon[0])+";"+str(shearCon[1])+";"+str(shearCon[2])+"\n")
    #fbedshear.close()
    return shearLub,shearCon

Epot,Ekin,Etot=0.,0.,0.
def Energy():
    Epot,Ekin,Etot=0.,0.,0.
    for i in spIds:
        p=Vector3(length_channel-O.bodies[i].state.pos[0],O.bodies[i].state.pos[1],O.bodies[i].state.pos[2])
        Epot+=O.bodies[i].state.mass*Vector3.dot(gravityVector,p)
        Ekin+=0.5*O.bodies[i].state.mass*O.bodies[i].state.vel.norm()**2
    return Epot,Ekin,Epot+Ekin


a,b,c=0,0,0
ww,zz=0,0



Elem_conct=0
def Element():
    global Elem_conct
    for i in side:
        El=O.bodies[i].intrs()
        for j in El:
            for k in Basin:
                if j.id1==k or j.id2==k:
                    Elem_conct+=1
    return Elem_conct



MeshStabilized=False
def StaCable():
    global a,CABLE,Frein,C_F,shackles,shackles_la,PreTimput,MeshStabilized#,infTen,infTen2
    # Equilibrum
    if unbalancedForce()<0.0006 and a==0 :
        # move cable
        for i in CABLE:
            O.bodies[i].state.vel=(0.,vely,velz)
        ## Move breaks external
        for i in range(len( Frein)):
            O.bodies[Frein[i][-1]].state.vel=(0.,vely,velz)
            O.bodies[Frein[i][-1]].shape.color=[1,0,1]
            O.bodies[Frein[i][-1]].state.blockedDOFs = 'xyzXYZ'
        ## Move breaks-Cable
        for i in C_F:
            O.bodies[i].state.vel=(0.,vely,velz)
        ## Move shackles
        if N_Braid<len(Braid):
            for i in shackles:
                O.bodies[i].state.vel=(0.,vely,velz)
            for i in shackles_la:
                O.bodies[i].state.vel=(0.,vely,velz)
        print ('Equilibrium', int(O.realtime/60), 'min', int(O.realtime-int(O.realtime/60)*60), 's')
        a=1
    # movement of all the cables
    if O.bodies[Idzmax].state.pos[2]<=0 and a==1:
    #Mesh placed, removing of the velocity and fixing the break
        ## move cable
        if N_Braid<len(Braid):
            for i in CABLE:
                if not i in clumpCF or i in PPP[1] or i in TTT[1]:
                    O.bodies[i].state.blockedDOFs = ''
        else:
            for i in CABLE:
                if not i in clumpCF:
                    O.bodies[i].state.blockedDOFs = ''

        ## Move breaks external
        for i in range(len( Frein)):
            O.bodies[Frein[i][-1]].state.vel=(0.,0.,0.)
            O.bodies[Frein[i][-1]].shape.color=[1,0,1]
            O.bodies[Frein[i][-1]].state.blockedDOFs = 'xyz'
        ## Move breaks-Cable
        for i in C_F:
            O.bodies[i].state.vel=(0.,0.,0.)
            O.bodies[i].state.blockedDOFs = ''
        ## Move shackles
        if  N_Braid<len(Braid):
            for i in shackles:
                O.bodies[i].state.vel=(0.,0.,0.)
                O.bodies[j].state.blockedDOFs = ''
            for i in shackles_la:
                O.bodies[i].state.blockedDOFs = ''
                O.bodies[i].state.vel=(0.,0.,0.)
        a=2
        print ('Mesh placed', int(O.realtime/60), 'min', int(O.realtime-int(O.realtime/60)*60), 's')

        measureTension.dead=False
#       cableengine.iterPeriod=250
    if unbalancedForce()<0.04 and a==2 : # unbalancedForce stabilizes the tensions of the cables
        for i in range(len(PreTimput)):
            PreTimput[i]=Preimput(CABLELIST[i][0])
            PreTimput[i]=PreTension(CABLELIST[i][0],ForcePret[i],PreTimput[i])

################################ ESTA CONDICION HAY QUE PENSAR COMO HACERLA INDEPENDIENTEMENTE DE N° CABLES) ######
        if (0.999*ForcePret[0] <= PreTimput[0] <= 1.001*ForcePret[0]) and (0.999*ForcePret[1] <= PreTimput[1] <= 1.001*ForcePret[1])and (0.999*ForcePret[1] <= PreTimput[1] <= 1.001*ForcePret[1]) and (0.999*ForcePret[2] <= PreTimput[2] <= 1.001*ForcePret[2]) and (0.999*ForcePret[3] <= PreTimput[3] <= 1.001*ForcePret[3]) and (0.999*ForcePret[4] <= PreTimput[4] <= 1.001*ForcePret[4]):
            print ('Tension', int(O.realtime/60) ,'min', int(O.realtime-int(O.realtime/60)*60), 's')
            a=3
#           O.save('VERSOYEN03.yade.gz')
#           infTen=infT(infTen)
    if unbalancedForce()<0.04 and a==3:
        # Removing damping
        newton.damping=0
#       infTen2=infT(infTen2)
        print ('placed', int(O.realtime/60), 'min', int(O.realtime-int(O.realtime/60)*60) ,'s')
        a=4
        MeshStabilized=True
#       O.bodies[IdSphere].state.vel[1]=-1
#       makeVideo(snapshot.snapshots,'3D.mpeg',fps=10,bps=10000)
        #O.pause()

#       Elem_conctac=Element()

def PreTension(cable,PreTREAl,PreTimput):
    for i,j in zip(cable[:-1],cable[1:]):
            Lseg = abs(O.bodies[j].state.pos-O.bodies[i].state.pos)
            if PreTimput < 0.5* PreTREAl :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp+Lseg*0.0003 #PreTREAl*3.3e-8#0.000003
            if 0.5* PreTREAl <= PreTimput < 0.80* PreTREAl :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp+Lseg*0.000006   #PreTREAl*3.3e-8#0.000003
        if  0.80* PreTREAl < PreTimput < 0.99* PreTREAl :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp+Lseg*0.0000001
            if 0.99* PreTREAl <= PreTimput <= PreTREAl*0.999  :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp+Lseg*0.00000002#PreTREAl*1.67e-10
            if 1.001*PreTREAl <= PreTimput <= 1.05*PreTREAl  :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp-Lseg*0.00000002#PreTREAl*2*3.33e-11
            if 1.05*PreTREAl < PreTimput  <= 1.10*PreTREAl :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp-Lseg*0.0000005#PreTREAl*3.33e-8
            if 1.10*PreTREAl < PreTimput :
                O.interactions[i,j].phys.unp = O.interactions[i,j].phys.unp-Lseg*0.000015#PreTREAl*3.33e-8
#       if  0.9999*PreTREAl <= PreTimput <= 1.0001*PreTREAl:
#       print 'tensil'
    obj1=O.bodies[cable[-1]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreTimput=(obj1[0].phys.normalForce+obj1[0].phys.shearForce).norm()
    return PreTimput

def Preimput(cable):
    obj1=O.bodies[cable[-1]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreTimput=(obj1[0].phys.normalForce+obj1[0].phys.shearForce).norm()
    return PreTimput

def Tension():
    obj1=O.bodies[Cable_iniend[0][0]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreT_UpL=(obj1[0].phys.normalForce).norm()
    obj11=O.bodies[Cable_iniend[0][1]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreT_UpR=(obj11[0].phys.normalForce).norm()
    obj2=O.bodies[Cable_iniend[1][0]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreT_CenL=(obj2[0].phys.normalForce).norm()
    obj22=O.bodies[Cable_iniend[1][1]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreT_CenR=(obj22[0].phys.normalForce).norm()
    obj3=O.bodies[Cable_iniend[2][0]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreT_DownL=(obj3[0].phys.normalForce).norm()
    obj33=O.bodies[Cable_iniend[2][1]].intrs() # MEASURE IN THE CENTER OF ITS HIGHT
    PreT_DownR=(obj33[0].phys.normalForce).norm()
    plot.addData(i=O.iter, ii=O.iter, PreT_Up_left=PreT_UpL,PreT_Cen_left=PreT_CenL,PreT_Down_left=PreT_DownL,PreT_Up_right=PreT_UpR,PreT_Cen_right=PreT_CenR,PreT_Down_right=PreT_DownR, unbalanced=unbalancedForce(), **O.energy)

Tension()
#plot.plots={'i':('PreT_Up_left','PreT_Up_right','PreT_Cen_left','PreT_Cen_right','PreT_Down_left','PreT_Down_right'),'ii':('unbalanced',None,O.energy.keys)}  #display the graph with the indicated legends
#plot.plot(subPlots=True) # show the previously described graph on the screen, and update while the simulation runs


O.trackEnergy=True
