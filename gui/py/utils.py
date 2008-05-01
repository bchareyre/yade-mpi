# encoding: utf-8
#
# utility functions for yade
#
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>

import math

from yade.wrapper import *

try: # use psyco if available
	import psyco
	psyco.full()
except ImportError: pass

def sphere(radius,center,density=1,young=3e9,poisson=.3,dynamic=True,color=[1,1,1]):
	"""Create default sphere, with given parameters. Physical properties such as mass and inertia are calculated automatically."""
	s=Body()
	s.shape=GeometricalModel('Sphere',{'radius':radius,'diffuseColor':color})
	s.mold=InteractingGeometry('InteractingSphere',{'radius':radius,'diffuseColor':color})
	V=(4./3)*math.pi*radius**3
	inert=(2./5.)*V*density*radius**2
	s.phys=PhysicalParameters('BodyMacroParameters',{'se3':[center[0],center[1],center[2],1,0,0,0],'mass':V*density,'inertia':[inert,inert,inert],'young':young,'poisson':poisson})
	s.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
	s['isDynamic']=dynamic
	return s

def box(extents,center,orientation=[1,0,0,0],density=1,young=3e9,poisson=.3,dynamic=True,color=[1,1,1]):
	"""Create default box (cuboid), with given parameters. Physical properties such as mass and inertia are calculated automatically."""
	b=Body()
	b.shape=GeometricalModel('Box',{'extents':extents,'diffuseColor':color})
	b.mold=InteractingGeometry('InteractingBox',{'extents':extents,'diffuseColor':color})
	mass=8*extents[0]*extents[1]*extents[2]*density
	b.phys=PhysicalParameters('BodyMacroParameters',{'se3':[center[0],center[1],center[2],orientation[0],orientation[1],orientation[2],orientation[3]],'mass':mass,'inertia':[mass*4*(extents[1]**2+extents[2]**2),mass*4*(extents[0]**2+extents[2]**2),mass*4*(extents[0]**2+extents[1]**2)],'young':young,'poisson':poisson})
	b.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
	b['isDynamic']=dynamic
	return b

def negPosExtremes(axis,distFactor=1.1):
	"""Get 2 lists (negative and positive extremes) of sphere ids that are close to the boundary
	in the direction of requested axis (0=x,1=y,2=z).

	distFactor enlarges radius of the sphere, it can be considered 'extremal' even if it doesn't touch the extreme.
	"""
	inf=float('inf')
	extremes=[inf,-inf]
	ret=[[],[]]
	o=Omega()
	for b in o.bodies:
		extremes[1]=max(extremes[1],+b.shape['radius']+b.phys['se3'][axis])
		extremes[0]=min(extremes[0],-b.shape['radius']+b.phys['se3'][axis])
	print extremes
	for b in o.bodies:
		if b.phys['se3'][axis]-b.shape['radius']*distFactor<=extremes[0]: ret[0].append(b.id)
		if b.phys['se3'][axis]+b.shape['radius']*distFactor>=extremes[1]: ret[1].append(b.id)
	return ret

def randomizeColors(onShapes=True,onMolds=False,onlyDynamic=False):
	"""Assign random colors to shape's (GeometricalModel) and/or mold's (InteractingGeometry) diffuseColor.
	
	onShapes and onMolds turn on/off operating on the respective colors.
	If onlyDynamic is true, only dynamic bodies will have the color changed.
	"""
	if not onShapes and not onMolds: return
	o=Omega()
	for b in o.bodies:
		color=(random.random(),random.random(),random.random())
		if onShapes and (b['isDynamic'] or not onlyDynamic): b.shape['diffuseColor']=color
		if onMolds  and (b['isDynamic'] or not onlyDynamic): b.mold['diffuseColor']=color

def runInQtGui(background=True):
	"""Run the current simulation with the QtGUI.
	
	If background is True, the command will be run in background and the saved simulation temporary will not be deleted.

	Note that it runs in separate process, hence no changes will propagate back to this simulation."""
	import os,tempfile,yade.runtime
	[fileobj,filename]=tempfile.mkstemp('.xml','yade')
	Omega().save(filename)
	if background: bg=' &'
	else: bg=''
	os.system(yade.runtime.executable+' -N QtGUI -S "'+filename+'"'+bg)
	if not background: os.remove(filename)

def PWaveTimeStep():
	"""Estimate timestep by the elastic wave propagation speed (p-wave).
	
	Multiply the value returned by some safety factor < 1, since shear waves are not taken into account here."""
	o=Omega()
	dt=float('inf')
	for b in o.bodies:
		if not (b.phys.has_key('young') and b.shape.has_key('radius')): continue
		density=b.phys['mass']/((4./3.)*math.pi*b.shape['radius']**3)
		thisDt=b.shape['radius']/math.sqrt(b.phys['young']/density)
		dt=min(dt,thisDt)
	return dt

def spheresFromFile(filename,**kw):
	"""Load sphere coordinates from file, create spheres, insert them to the simulation.

	filename is the file holding ASCII numbers (at least 4 colums that hold x_center, y_center, z_center, radius).
	All remaining arguments are passed the the yade.utils.sphere function that creates the bodies.
	
	Returns list of body ids that were inserted."""
	ret=[]
	for l in open(filename):
		ss=[float(i) for i in l.split()]
		id=o.bodies.append(sphere(ss[3],[ss[0],ss[2],ss[1]],**kw))
		ret.append(id)
	return ret


