# -*- coding: utf-8 -*-

# J. Duriez duriez@geo.hmg.inpg.fr
# "PreProcessor" script to define a numerical model for simple shear box
# In order to illustrate various Kinem...Engine which allow to perform different loadings on the box
# There is here one § for each Kinem...Engine, comment/uncomment them to observe what you want

#NB : the run of the script is paused at each plot window (so that there is time to observe it). Type "Return" in the Yade terminal to resume


from yade import plot
from yade.pack import *


O=Omega() 

#Def of the material which will be used
O.materials.append(NormalInelasticMat(density=2600,young=4.0e9,poisson=.04,frictionAngle=.6,coeff_dech=3.0,label='Materiau1'))

#Def of dimensions of the box
#length=5
#height=1
#width=2
length=0.1
height=0.02
width=0.04
thickness=0.001

porosity = 0.55
nSpheres=800
rMean = pow( ((1-porosity) * length * height * width) / (nSpheres * 4.0/3.0 * pi) , 1.0/3.0 )

#Definition of bodies constituing the numerical model : six boxes corresponding to sids of the simple shear box, containing a particle samples
pred=inAlignedBox((0,0,-0.02),(0.1,0.02,0.02))

leftBox = utils.box( center=(-thickness/2.0,(height)/2.0,0), extents=(thickness/2.0,5*(height/2.0+thickness),width/2.0) ,fixed=True,wire=True)

lowBox = utils.box( center=(length/2.0,-thickness/2.0,0), extents=(length/2.0,thickness/2.0,width/2.0) ,fixed=True,wire=True)

rightBox = utils.box( center=(length+thickness/2.0,height/2.0,0), extents=(thickness/2.0,5*(height/2.0+thickness),width/2.0) ,fixed=True,wire=True)

upBox = utils.box( center=(length/2.0,height+thickness/2.0,0), extents=(length/2.0,thickness/2.0,width/2.0) ,fixed=True,wire=True)

behindBox = utils.box( center=(length/2.0,height/2.0,-width/2.0-thickness/2.0), extents=(2.5*length/2.0,height/2.0+thickness,thickness/2.0), fixed=True,wire=True)

inFrontBox = utils.box( center=(length/2.0,height/2.0,width/2.0+thickness/2.0), extents=(2.5*length/2.0,height/2.0+thickness,thickness/2.0), fixed=True,wire=True)

O.bodies.append([leftBox,lowBox,rightBox,upBox,behindBox,inFrontBox])

#memoizeDb='/tmp/simpleshear-triax-packings.sqlite'
#ListSph=randomDensePack(pred,radius=0.002,rRelFuzz=0.15,memoDbg=True,memoizeDb=memoizeDb)
#ListSph=randomDensePack(pred,radius=0.002,rRelFuzz=0.15,memoDbg=True,memoizeDb=memoizeDb,spheresInCell=100)
#O.bodies.append(ListSph)

sp=yade._packSpheres.SpherePack()
sp.makeCloud(Vector3(0,0.0,-width/2.0),Vector3(length,height,width/2.0),rMean,.15)
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])


#---- Def of the engines ----#

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom6D(),Ig2_Box_Sphere_ScGeom6D()],
		[Ip2_2xNormalInelasticMat_NormalInelasticityPhys()],
		[Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity()]
	),
	NewtonIntegrator(damping=.1)
	,PyRunner(iterPeriod=50,command='defData()')
	]

def defData():
	plot.addData(fy=O.forces.f(3)[1], # vertical component of the force sustained by the upper side of the shear box
		    fx=O.forces.f(3)[0], # horizontal component of the force sustained by the upper side of the shear box
		    step=O.iter,
		    gamma=O.bodies[3].state.pos[0] - length/2.0,
		    u=O.bodies[3].state.pos[1] - (height+thickness/2.0)
		    )

fy=O.forces.f(3)[1] # The fy above exists only as a label in plot.data


from yade import qt
qt.View()
#O.save("SimpleShearReady.xml")
#O.saveTmp("InitialState")


#---- Compression ----
O.engines = O.engines+[KinemCTDEngine(compSpeed=0.5,sigma_save=(),temoin_save=(),targetSigma=40000.0,LOG=False)]

O.dt=.4*utils.PWaveTimeStep()
print ''
print 'Be patient, running in progress (the shear box is being compressed)'
O.run(14000,True)
plot.plots={'step':('fy',)}
plot.plot(subPlots=False)
print 'Plotting curve (fy = normal force). Type Return to go ahead'
print ''
raw_input()
plot.plots={'u':('fy',)}
plot.plot(subPlots=False)
print 'Plotting curve (u = normal displacement). Type Return to go ahead'
print ''
raw_input()
#O.save('FinComp.xml')


#---- Shear at constant normal displacement ----
nCycShear = 20000

O.engines=O.engines[:5]+[KinemCNDEngine(shearSpeed=(length/7.0)/(nCycShear*O.dt),gamma_save=(),temoin_save=(),gammalim=length/7.0,LOG=False)]

print 'Be patient, running in progress (the shear box is being sheared)'
O.run(int(1.15*nCycShear),True)
plot.plots={'step':('gamma',)}
plot.plot(subPlots=False)
print 'Plotting curve (gamma = tangential displacement). Type Return to go ahead'
print ''
raw_input()
plot.plots={'gamma':('fx','fy',)}
plot.plot(subPlots=False)
print 'Plotting curve (fx = tangential force). Type Return to go ahead'
print ''
raw_input()


#---- A re-compression, from this initial sheared state ----
O.engines=O.engines[:5]+[KinemCTDEngine(compSpeed=0.5,sigma_save=(),temoin_save=(),targetSigma=80000.0,LOG=False)]
print 'Be patient, running in progress (the sample is being again compressed, from this sheared state)'
O.run(10000,True)
plot.plots={'u':('fx','fy',)}
plot.plot(subPlots=False)
print 'Plotting curve. Type Return to go ahead'
print ''
raw_input()
print 'End of script'

##---- Shear at constant normal load/stress ----
#nCycShear = 20000
#O.engines=O.engines[:6]+[KinemCNLEngine(shearSpeed=(length/10.0)/(nCycShear*O.dt),gamma_save=(),temoin_save=(),gammalim=length/10.0,LOG=False)]
#O.run(int(1.15*nCycShear),True)
#plot.plots={'step':('gamma','u',)}
#plot.plot()
#raw_input()
#plot.plots={'gamma':('fx','fy',)}
#plot.plot()
#raw_input()


#---- Shear at constant normal stifness ----
#nCycShear = 20000
#O.engines=O.engines[:6]+[KinemCNSEngine(shearSpeed=(length/10.0)/(nCycShear*O.dt),gammalim=length/10.0,LOG=False,KnC=1)]
#O.run(int(1.15*nCycShear),True)
#plot.plots={'step':('gamma','u',)}
#plot.plot()
#raw_input()
#plot.plots={'gamma':('fx','fy',)}
#plot.plot()
#raw_input()
#plot.plots={'u':('fy',)}
#plot.plot()
#raw_input()


