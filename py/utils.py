# encoding: utf-8
#
# utility functions for yade
#
# 2008-2009 © Václav Šmilauer <eudoxos@arcig.cz>

"""Heap of functions that don't (yet) fit anywhere else.

Devs: please DO NOT ADD more functions here, it is getting too crowded!
"""

import math,random
from yade import *
from yade.wrapper import *
from miniEigen import *
try: # use psyco if available
	import psyco
	psyco.full()
except ImportError: pass
import doctest

# c++ implementations for performance reasons
from yade._utils import *

def saveVars(mark='',loadNow=False,**kw):
	"""Save passed variables into the simulation so that it can be recovered when the simulation is loaded again.

	For example, variables a=5, b=66 and c=7.5e-4 are defined. To save those, use::

		>>> from yade import utils
		>>> utils.saveVars('mark',a=1,b=2,c=3,loadNow=True)
		>>> a,b,c
		(1, 2, 3)

	those variables will be save in the .xml file, when the simulation itself is saved. To recover those variables once the .xml is loaded again, use

		>>> utils.loadVars('mark')

	and they will be defined in the __builtin__ namespace (i.e. available from anywhere in the python code).

	If *loadParam*==True, variables will be loaded immediately after saving. That effectively makes *\*\*kw* available in builtin namespace.
	"""
	import cPickle
	Omega().tags['pickledPythonVariablesDictionary'+mark]=cPickle.dumps(kw)
	if loadNow: loadVars(mark)

def loadVars(mark=None):
	"""Load variables from saveVars, which are saved inside the simulation.
	If mark==None, all save variables are loaded. Otherwise only those with
	the mark passed."""
	import cPickle
	import __builtin__
	if mark!=None:
		d=cPickle.loads(Omega().tags['pickledPythonVariablesDictionary'+mark])
		for k in d: __builtin__.__dict__[k]=d[k]
	else: # load everything one by one
		for m in Omega().tags.keys():
			if m.startswith('pickledPythonVariablesDictionary'):
				loadVars(m[len('pickledPythonVariableDictionary')+1:])


def SpherePWaveTimeStep(radius,density,young):
	r"""Compute P-wave critical timestep for a single (presumably representative) sphere, using formula for P-Wave propagation speed $\Delta t_{c}=\frac{r}{\sqrt{E/\rho}}$.
	If you want to compute minimum critical timestep for all spheres in the simulation, use :yref:`yade.utils.PWaveTimeStep` instead.
	
	>>> SpherePWaveTimeStep(1e-3,2400,30e9)
	2.8284271247461903e-07
	"""
	from math import sqrt
	return radius/sqrt(young/density)

def randomColor():
	"Return random Vector3 with each component in interval 0…1 (uniform distribution)"
	return Vector3(random.random(),random.random(),random.random())

def typedEngine(name):
	"""Return first engine from current O.engines, identified by its type (as string). For example:

	>>> from yade import utils
	>>> O.engines=[InsertionSortCollider(),NewtonIntegrator(),GravityEngine()]
	>>> utils.typedEngine("NewtonIntegrator") == O.engines[1]
	True
	"""
	return [e for e in Omega().engines if e.name==name][0]

def downCast(obj,newClassName):
	"""Cast given object to class deriving from the same yade root class and copy all parameters from given object.
	Obj should be up in the inheritance tree, otherwise some attributes may not be defined in the new class."""
	return obj.__class__(newClassName,dict([ (key,obj[key]) for key in obj.keys() ]))


def defaultMaterial():
	"""Return default material, when creating bodies with :yref:`yade.utils.sphere` and friends, material is unspecified and there is no shared material defined yet. By default, this function returns::

		FrictMat(density=1e3,young=1e7,poisson=.3,frictionAngle=.5,label='defaultMat')
	"""
	return FrictMat(density=1e3,young=1e7,poisson=.3,frictionAngle=.5,label='defaultMat')

def _commonBodySetup(b,volume,geomInertia,material,noBound=False,resetState=True):
	"""Assign common body parameters."""
	#if 'physParamsClass' in matKw.keys(): raise ArgumentError("You as passing physParamsClass as argument, but it is no longer used. Use material instead.")
	#if 'materialClass' in matKw.keys(): raise ArgumentError("You as passing materialClass as argument, but it is no longer used. Use material instead.")
	if isinstance(material,int):
		if material<0 and len(O.materials)==0: O.materials.append(defaultMaterial());
		b.mat=O.materials[material]
	elif isinstance(material,str): b.mat=O.materials[material]
	elif isinstance(material,Material): b.mat=material
	elif callable(material): b.mat=material()
	else: raise TypeError("The 'material' argument must be None (for defaultMaterial), string (for shared material label), int (for shared material id) or Material instance.");
	## resets state (!!)
	if resetState: b.state=b.mat.newAssocState()
	mass=volume*b.mat.density
	b.state.mass,b.state.inertia=mass,geomInertia*b.mat.density
	if not noBound: b.bound=Aabb(diffuseColor=[0,1,0])

def sphere(center,radius,dynamic=True,wire=False,color=None,highlight=False,material=-1,mask=1):
	"""Create sphere with given parameters; mass and inertia computed automatically.

	Last assigned material is used by default (*material*=-1), and utils.defaultMaterial() will be used if no material is defined at all.

	:Parameters:
		`center`: Vector3
			center
		`radius`: float
			radius
		`color`: Vector3 or None
			random color will be assigned if None
		`material`: int | string | Material instance | callable returning Material instance
			* if int, O.materials[material] will be used; as a special case, if material==-1 and there is no shared materials defined, utils.defaultMaterial() will be assigned to O.materials[0]
			* if string, it is label of an existing material that will be used
			* if Material instance, this instance will be used
			* if callable, it will be called without arguments; returned Material value will be used (Material factory object, if you like)
		`mask`: integer
			:yref:`Body.mask` for the body

	:return:
		A Body instance with desired characteristics.


	Creating default shared material if none exists neither is given::

		>>> O.reset()
		>>> from yade import utils
		>>> len(O.materials)
		0
		>>> s0=utils.sphere([2,0,0],1)
		>>> len(O.materials)
		1

	Instance of material can be given::

		>>> s1=utils.sphere([0,0,0],1,wire=False,color=(0,1,0),material=ElastMat(young=30e9,density=2e3))
		>>> s1.shape.wire
		False
		>>> s1.shape.color
		Vector3(0,1,0)
		>>> s1.mat.density
		2000.0

	Material can be given by label::

		>>> O.materials.append(FrictMat(young=10e9,poisson=.11,label='myMaterial'))
		1
		>>> s2=utils.sphere([0,0,2],1,material='myMaterial')
		>>> s2.mat.label
		'myMaterial'
		>>> s2.mat.poisson
		0.11

	Finally, material can be a callable object (taking no arguments), which returns a Material instance.
	Use this if you don't call this function directly (for instance, through yade.pack.randomDensePack), passing
	only 1 *material* parameter, but you don't want material to be shared.

	For instance, randomized material properties can be created like this:

		>>> import random
		>>> def matFactory(): return ElastMat(young=1e10*random.random(),density=1e3+1e3*random.random())
		... 
		>>> s3=utils.sphere([0,2,0],1,material=matFactory)
		>>> s4=utils.sphere([1,2,0],1,material=matFactory)

	"""
	b=Body()
	b.shape=Sphere(radius=radius,color=color if color else randomColor(),wire=wire,highlight=highlight)
	V=(4./3)*math.pi*radius**3
	geomInert=(2./5.)*V*radius**2
	_commonBodySetup(b,V,Vector3(geomInert,geomInert,geomInert),material)
	b.state.pos=b.state.refPos=center
	b.dynamic=dynamic
	b.mask=mask
	return b

def box(center,extents,orientation=[1,0,0,0],dynamic=True,wire=False,color=None,highlight=False,material=-1,mask=1):
	"""Create box (cuboid) with given parameters.

	:Parameters:
		`extents`: Vector3
			half-sizes along x,y,z axes
	
	See :yref:`yade.utils.sphere`'s documentation for meaning of other parameters."""
	b=Body()
	b.shape=Box(extents=extents,color=color if color else randomColor(),wire=wire,highlight=highlight)
	V=8*extents[0]*extents[1]*extents[2]
	geomInert=Vector3(4*(extents[1]**2+extents[2]**2),4*(extents[0]**2+extents[2]**2),4*(extents[0]**2+extents[1]**2))
	_commonBodySetup(b,V,geomInert,material)
	b.state.pos=b.state.refPos=center
	b.dynamic=dynamic
	b.mask=mask
	return b

def wall(position,axis,sense=0,color=None,material=-1,mask=1):
	"""Return ready-made wall body.

	:Parameters:
		`position`: float or Vector3
			center of the wall. If float, it is the position along given axis, the other 2 components being zero
		`axis`: ∈{0,1,2}
			orientation of the wall normal (0,1,2) for x,y,z (sc. planes yz, xz, xy)
		`sense`: ∈{-1,0,1}
			sense in which to interact (0: both, -1: negative, +1: positive; see Wall reference documentation)
		`mask`: bitmask (as int)
			:yref:`Body.mask`

	See :yref:`yade.utils.sphere`'s documentation for meaning of other parameters."""
	b=Body()
	b.shape=Wall(sense=sense,axis=axis,color=color if color else randomColor())
	_commonBodySetup(b,0,Vector3(0,0,0),material)
	if isinstance(position,(int,long,float)):
		pos2=Vector3(0,0,0); pos2[axis]=position
	else: pos2=position
	b.state.pos=b.state.refPos=pos2
	b.dynamic=False
	b.mask=mask
	return b

def facet(vertices,dynamic=False,wire=True,color=None,highlight=False,noBound=False,material=-1,mask=1):
	"""Create facet with given parameters.

	:Parameters:
		`vertices`: [Vector3,Vector3,Vector3]
			coordinates of vertices in the global coordinate system.
		`wire`: bool
			if True, facets are shown as skeleton; otherwise facets are filled
		`noBound`:
			do not assign Body().bound
		`color`: Vector3 or None
			random color will be assigned if None
		`material`: int | string | Material instance | callable returning Material instance
			* if int, O.materials[material] will be used; as a special case, if material==-1 and there is no shared materials defined, utils.defaultMaterial() will be assigned to O.materials[0]
			* if string, it is label of an existing material that will be used
			* if Material instance, this instance will be used
			* if callable, it will be called without arguments; returned Material value will be used (Material factory object, if you like)
		`mask`: integer
			:yref:`Body.mask` for the body
	
	See :yref:`yade.utils.sphere`'s documentation for meaning of other parameters."""
	b=Body()
	center=inscribedCircleCenter(vertices[0],vertices[1],vertices[2])
	vertices=Vector3(vertices[0])-center,Vector3(vertices[1])-center,Vector3(vertices[2])-center
	b.shape=Facet(color=color if color else randomColor(),wire=wire,highlight=highlight)
	b.shape.vertices=vertices
	#b.shape.postProcessAttributes(True)
	_commonBodySetup(b,0,Vector3(0,0,0),material,noBound=noBound)
	b.state.pos=b.state.refPos=center
	b.dynamic=dynamic
	b.mask=mask
	return b

def facetBox(center,extents,orientation=Quaternion.Identity,wallMask=63,**kw):
	"""
	Create arbitrarily-aligned box composed of facets, with given center, extents and orientation.
	If any of the box dimensions is zero, corresponding facets will not be created. The facets are oriented outwards from the box.

	
	:Parameters:
			`center`: Vector3
				center of the created box
			`extents`: (eX,eY,eZ)
				lengths of the box sides
			`orientation`: quaternion
				orientation of the box
			`wallMask`: bitmask
				determines which walls will be created, in the order -x (1), +x (2), -y (4), +y (8), -z (16), +z (32). The numbers are ANDed; the default 63 means to create all walls;
			`**kw`: (unused keyword arguments)
				passed to utils.facet

	:Returns: list of facets forming the box.
	"""
	
	
	#Defense from zero dimensions
	if (wallMask>63):
		print "wallMask must be 63 or less"
		wallMask=63
	if (extents[0]==0):
		wallMask=1
	elif (extents[1]==0):
		wallMask=4
	elif (extents[2]==0):
		wallMask=16
	if (((extents[0]==0) and (extents[1]==0)) or ((extents[0]==0) and (extents[2]==0)) or ((extents[1]==0) and (extents[2]==0))):
		raise RuntimeError("Please, specify at least 2 none-zero dimensions in extents!");
	# ___________________________
	
	mn,mx=[-extents[i] for i in 0,1,2],[extents[i] for i in 0,1,2]
	def doWall(a,b,c,d):
		return [facet((a,b,c),**kw),facet((a,c,d),**kw)]
	ret=[]

	A=orientation*Vector3(mn[0],mn[1],mn[2])+center
	B=orientation*Vector3(mx[0],mn[1],mn[2])+center
	C=orientation*Vector3(mx[0],mx[1],mn[2])+center
	D=orientation*Vector3(mn[0],mx[1],mn[2])+center
	E=orientation*Vector3(mn[0],mn[1],mx[2])+center
	F=orientation*Vector3(mx[0],mn[1],mx[2])+center
	G=orientation*Vector3(mx[0],mx[1],mx[2])+center
	H=orientation*Vector3(mn[0],mx[1],mx[2])+center
	if wallMask&1:  ret+=doWall(A,D,H,E)
	if wallMask&2:  ret+=doWall(B,F,G,C)
	if wallMask&4:  ret+=doWall(A,E,F,B)
	if wallMask&8:  ret+=doWall(D,C,G,H)
	if wallMask&16: ret+=doWall(A,B,C,D)
	if wallMask&32: ret+=doWall(E,H,G,F)
	return ret
	
def facetCylinder(center,radius,height,orientation=Quaternion.Identity,segmentsNumber=10,wallMask=7,closed=1,**kw):
	"""
	Create arbitrarily-aligned cylinder composed of facets, with given center, radius, height and orientation.
	Return List of facets forming the cylinder;
	
	:Parameters:
			`center`: Vector3
				center of the created cylinder
			`radius`: float
				cylinder radius
			`height`: float
				cylinder height
			`orientation`: Quaternion
				orientation of the cylinder
			`segmentsNumber`: int
				number of edges on the cylinder surface (>=5)
			`wallMask`: bitmask
				determines which walls will be created, in the order up (1), down (2), side (4). The numbers are ANDed; the default 7 means to create all walls;
			`**kw`: (unused keyword arguments)
				passed to utils.facet;
	"""
	
	#Defense from zero dimensions
	if (segmentsNumber<3):
		raise RuntimeError("The segmentsNumber should be at least 3");
	if (height<=0):
		raise RuntimeError("The height should have the positive value");
	if (radius<=0):
		raise RuntimeError("The radius should have the positive value");
	if (wallMask>7):
		print "wallMask must be 7 or less"
		wallMask=7
	# ___________________________
	import numpy
	anglesInRad = numpy.linspace(0, 2.0*math.pi, segmentsNumber+1, endpoint=True)
	P1=[]; P2=[]
	P1.append(Vector3(0,0,-height/2))
	P2.append(Vector3(0,0,+height/2))
	
	for i in anglesInRad:
		X=radius*math.cos(i)
		Y=radius*math.sin(i)
		P1.append(Vector3(X,Y,-height/2))
		P2.append(Vector3(X,Y,+height/2))
	for i in range(0,len(P1)):
		P1[i]=orientation*P1[i]+center
		P2[i]=orientation*P2[i]+center
		
	ret=[]
	for i in range(2,len(P1)):
		if wallMask&2:
			ret.append(facet((P1[0],P1[i],P1[i-1]),**kw))
		if wallMask&1:
			ret.append(facet((P2[0],P2[i-1],P2[i]),**kw))
		if wallMask&4:
			ret.append(facet((P1[i],P2[i],P2[i-1]),**kw))
			ret.append(facet((P2[i-1],P1[i-1],P1[i]),**kw))
	return ret
	
	
def aabbWalls(extrema=None,thickness=None,oversizeFactor=1.5,**kw):
	"""Return 6 boxes that will wrap existing packing as walls from all sides;
	extrema are extremal points of the Aabb of the packing (will be calculated if not specified)
	thickness is wall thickness (will be 1/10 of the X-dimension if not specified)
	Walls will be enlarged in their plane by oversizeFactor.
	returns list of 6 wall Bodies enclosing the packing, in the order minX,maxX,minY,maxY,minZ,maxZ.
	"""
	walls=[]
	if not extrema: extrema=aabbExtrema()
	if not thickness: thickness=(extrema[1][0]-extrema[0][0])/10.
	for axis in [0,1,2]:
		mi,ma=extrema
		center=[(mi[i]+ma[i])/2. for i in range(3)]
		extents=[.5*oversizeFactor*(ma[i]-mi[i]) for i in range(3)]
		extents[axis]=thickness/2.
		for j in [0,1]:
			center[axis]=extrema[j][axis]+(j-.5)*thickness
			walls.append(box(center=center,extents=extents,dynamic=False,**kw))
			walls[-1].shape.wire=True
	return walls


def aabbDim(cutoff=0.,centers=False):
	"""Return dimensions of the axis-aligned bounding box, optionally with relative part *cutoff* cut away."""
	a=aabbExtrema(cutoff,centers)
	return (a[1][0]-a[0][0],a[1][1]-a[0][1],a[1][2]-a[0][2])

def aabbExtrema2d(pts):
	"""Return 2d bounding box for a sequence of 2-tuples."""
	inf=float('inf')
	min,max=[inf,inf],[-inf,-inf]
	for pt in pts:
		if pt[0]<min[0]: min[0]=pt[0]
		elif pt[0]>max[0]: max[0]=pt[0]
		if pt[1]<min[1]: min[1]=pt[1]
		elif pt[1]>max[1]: max[1]=pt[1]
	return tuple(min),tuple(max)

def perpendicularArea(axis):
	"""Return area perpendicular to given axis (0=x,1=y,2=z) generated by bodies
	for which the function consider returns True (defaults to returning True always)
	and which is of the type :yref:`Sphere`.
	"""
	ext=aabbExtrema()
	other=((axis+1)%3,(axis+2)%3)
	return (ext[1][other[0]]-ext[0][other[0]])*(ext[1][other[1]]-ext[0][other[1]])

def fractionalBox(fraction=1.,minMax=None):
	"""retrurn (min,max) that is the original minMax box (or aabb of the whole simulation if not specified)
	linearly scaled around its center to the fraction factor"""
	if not minMax: minMax=aabbExtrema()
	half=[.5*(minMax[1][i]-minMax[0][i]) for i in [0,1,2]]
	return (tuple([minMax[0][i]+(1-fraction)*half[i] for i in [0,1,2]]),tuple([minMax[1][i]-(1-fraction)*half[i] for i in [0,1,2]]))


def randomizeColors(onlyDynamic=False):
	"""Assign random colors to :yref:`Shape::color`.

	If onlyDynamic is true, only dynamic bodies will have the color changed.
	"""
	for b in O.bodies:
		color=(random.random(),random.random(),random.random())
		if b.dynamic or not onlyDynamic: b.shape.color=color

def avgNumInteractions(cutoff=0.):
	"""Return average numer of interactions per particle, also known as *coordination number*.

	:param cutoff: cut some relative part of the sample's bounding box away.
	"""
	nums,counts=bodyNumInteractionsHistogram(aabbExtrema(cutoff))
	return sum([nums[i]*counts[i] for i in range(len(nums))])/(1.*sum(counts))

def plotNumInteractionsHistogram(cutoff=0.):
	"Plot histogram with number of interactions per body, optionally cutting away *cutoff* relative axis-aligned box from specimen margin."
	nums,counts=bodyNumInteractionsHistogram(aabbExtrema(cutoff))
	import pylab
	pylab.bar(nums,counts)
	pylab.title('Number of interactions histogram, average %g (cutoff=%g)'%(avgNumInteractions(cutoff),cutoff))
	pylab.xlabel('Number of interactions')
	pylab.ylabel('Body count')
	pylab.show()

def plotDirections(aabb=(),mask=0,bins=20,numHist=True):
	"""Plot 3 histograms for distribution of interaction directions, in yz,xz and xy planes and
	(optional but default) histogram of number of interactions per body."""
	import pylab,math
	from yade import utils
	for axis in [0,1,2]:
		d=utils.interactionAnglesHistogram(axis,mask=mask,bins=bins,aabb=aabb)
		fc=[0,0,0]; fc[axis]=1.
		subp=pylab.subplot(220+axis+1,polar=True);
		# 1.1 makes small gaps between values (but the column is a bit decentered)
		pylab.bar(d[0],d[1],width=math.pi/(1.1*bins),fc=fc,alpha=.7,label=['yz','xz','xy'][axis])
		#pylab.title(['yz','xz','xy'][axis]+' plane')
		pylab.text(.5,.25,['yz','xz','xy'][axis],horizontalalignment='center',verticalalignment='center',transform=subp.transAxes,fontsize='xx-large')
	if numHist:
		pylab.subplot(224,polar=False)
		nums,counts=utils.bodyNumInteractionsHistogram(aabb if len(aabb)>0 else utils.aabbExtrema())
		avg=sum([nums[i]*counts[i] for i in range(len(nums))])/(1.*sum(counts))
		pylab.bar(nums,counts,fc=[1,1,0],alpha=.7,align='center')
		pylab.xlabel('Interactions per body (avg. %g)'%avg)
		pylab.axvline(x=avg,linewidth=3,color='r')
		pylab.ylabel('Body count')
	pylab.show()





def encodeVideoFromFrames(frameSpec,out,renameNotOverwrite=True,fps=24):
	"""Create .ogg video from external image files.

	:parameters:
		`frameSpec`: wildcard | sequence of filenames
			If string, wildcard in format understood by GStreamer's multifilesrc plugin (e.g. '/tmp/frame-%04d.png'). If list or tuple, filenames to be encoded in given order.
			
			.. warning::
				GStreamer is picky about the wildcard; if you pass a wrong one, if will not complain, but silently stall.
		`out`: filename
			file to save video into
		`renameNotOverwrite`: bool
			if True, existing same-named video file will have ~[number] appended; will be overwritten otherwise.
		`fps`:
			Frames per second.
	"""
	import pygst,sys,gobject,os,tempfile,shutil,os.path
	pygst.require("0.10")
	import gst
	if renameNotOverwrite and os.path.exists(out):
		i=0;
		while(os.path.exists(out+"~%d"%i)): i+=1
		os.rename(out,out+"~%d"%i); print "Output file `%s' already existed, old file renamed to `%s'"%(out,out+"~%d"%i)
	if frameSpec.__class__==list or frameSpec.__class__==tuple:
		# create a new common prefix, symlink given files to prefix-%05d.png in their order, adjust wildcard, go ahead.
		tmpDir=tempfile.mkdtemp()
		for no,frame in enumerate(frameSpec):
			os.symlink(os.path.abspath(frame),os.path.join(tmpDir,'%07d'%no))
		wildcard=os.path.join(tmpDir,'%07d')
	else:
		tmpDir=None; wildcard=frameSpec
	print "Encoding video from %s to %s"%(wildcard,out)
	pipeline=gst.parse_launch('multifilesrc location="%s" index=0 caps="image/png,framerate=\(fraction\)%d/1" ! pngdec ! ffmpegcolorspace ! theoraenc sharpness=2 quality=63 ! oggmux ! filesink location="%s"'%(wildcard,fps,out))
	bus=pipeline.get_bus()
	bus.add_signal_watch()
	mainloop=gobject.MainLoop();
	bus.connect("message::eos",lambda bus,msg: mainloop.quit())
	pipeline.set_state(gst.STATE_PLAYING)
	mainloop.run()
	pipeline.set_state(gst.STATE_NULL); pipeline.get_state()
	# remove symlinked frames, if any
	if tmpDir: shutil.rmtree(tmpDir)


def replaceCollider(colliderEngine):
	"""Replaces collider (Collider) engine with the engine supplied. Raises error if no collider is in engines."""
	colliderIdx=-1
	for i,e in enumerate(O.engines):
		if O.isChildClassOf(e.name,"Collider"):
			colliderIdx=i
			break
	if colliderIdx<0: raise RuntimeError("No Collider found within O.engines.")
	O.engines=O.engines[:colliderIdx]+[colliderEngine]+O.engines[colliderIdx+1:]

def _procStatus(name):
	import os
	for l in open('/proc/%d/status'%os.getpid()):
		if l.split(':')[0]==name: return l
	raise "No such line in /proc/[pid]/status: "+name
def vmData():
	"Return memory usage data from Linux's /proc/[pid]/status, line VmData."
	l=_procStatus('VmData'); ll=l.split(); assert(ll[2]=='kB')
	return int(ll[1])

def uniaxialTestFeatures(filename=None,areaSections=10,axis=-1,**kw):
	"""Get some data about the current packing useful for uniaxial test:

#. Find the dimensions that is the longest (uniaxial loading axis)

#. Find the minimum cross-section area of the specimen by examining several (areaSections) sections perpendicular to axis, computing area of the convex hull for each one. This will work also for non-prismatic specimen.

#. Find the bodies that are on the negative/positive boundary, to which the straining condition should be applied.

:parameters:
	`filename`:
		if given, spheres will be loaded from this file (ASCII format); if not, current simulation will be used.
	`areaSection`:
		number of section that will be used to estimate cross-section
	`axis`:
		if given, force strained axis, rather than computing it from predominant length

:return: dictionary with keys 'negIds', 'posIds', 'axis', 'area'.

.. warning::
	The function :yref:`yade.utils.approxSectionArea` uses convex hull algorithm to find the area, but the implementation is reported to be *buggy* (bot works in some cases). Always check this number, or fix the convex hull algorithm (it is documented in the source, see :ysrc:`py/_utils.cpp`).

	"""
	if filename: ids=spheresFromFile(filename,**kw)
	else: ids=[b.id for b in O.bodies]
	mm,mx=aabbExtrema()
	dim=aabbDim();
	if axis<0: axis=list(dim).index(max(dim)) # list(dim) for compat with python 2.5 which didn't have index defined for tuples yet (appeared in 2.6 first)
	assert(axis in (0,1,2))
	import numpy
	areas=[approxSectionArea(coord,axis) for coord in numpy.linspace(mm[axis],mx[axis],num=10)[1:-1]]
	negIds,posIds=negPosExtremeIds(axis=axis,distFactor=2.2)
	return {'negIds':negIds,'posIds':posIds,'axis':axis,'area':min(areas)}

def NormalRestitution2DampingRate(en):
	r"""Compute the normal damping rate as a function of the normal coefficient of restitution $e_n$. For $e_n\in\langle0,1\rangle$ damping rate equals
	
	.. math:: -\frac{\log e_n}{\sqrt{e_n^2+\pi^2}}

	"""
	if en == 0.0: return 0.999999999
	if en == 1.0: return 0.0
	from math import sqrt,log,pi
	ln_en = math.log(en)
	return (-ln_en/math.sqrt((math.pow(ln_en,2) + math.pi*math.pi)))

def xMirror(half):
	"""Mirror a sequence of 2d points around the x axis (changing sign on the y coord).
The sequence should start up and then it will wrap from y downwards (or vice versa).
If the last point's x coord is zero, it will not be duplicated."""
	return list(half)+[(x,-y) for x,y in reversed(half[:-1] if half[-1][0]==0 else half)]

#############################
##### deprecated functions


def _deprecatedUtilsFunction(old,new):
	"Wrapper for deprecated functions, example below."
	import warnings
	warnings.warn('Function utils.%s is deprecated, use %s instead.'%(old,new),stacklevel=2,category=DeprecationWarning)

# example of _deprecatedUtilsFunction usage:
#
# def import_mesh_geometry(*args,**kw):
#    "|ydeprecated|"
#    _deprecatedUtilsFunction('import_mesh_geometry','yade.import.gmsh')
#    import yade.ymport
#    return yade.ymport.stl(*args,**kw)


class TableParamReader():
	"""Class for reading simulation parameters from text file.
	
Each parameter is represented by one column, each parameter set by one line. Colums are separated by blanks (no quoting).

First non-empty line contains column titles (without quotes).
You may use special column named 'description' to describe this parameter set;
if such colum is absent, description will be built by concatenating column names and corresponding values (``param1=34,param2=12.22,param4=foo``)

* from columns ending in ``!`` (the ``!`` is not included in the column name)
* from all columns, if no columns end in ``!``.

Empty lines within the file are ignored (although counted); ``#`` starts comment till the end of line. Number of blank-separated columns must be the same for all non-empty lines.

A special value ``=`` can be used instead of parameter value; value from the previous non-empty line will be used instead (works recursively).

This class is used by :yref:`yade.utils.readParamsFromTable`.
	"""
	def __init__(self,file):
		"Setup the reader class, read data into memory."
		import re
		# read file in memory, remove newlines and comments; the [''] makes lines 1-indexed
		ll=[re.sub('\s*#.*','',l[:-1]) for l in ['']+open(file,'r').readlines()]
		# usable lines are those that contain something else than just spaces
		usableLines=[i for i in range(len(ll)) if not re.match(r'^\s*(#.*)?$',ll[i])]
		headings=ll[usableLines[0]].split()
		# use all values of which heading has ! after its name to build up the description string
		# if there are none, use all columns
		if not 'description' in headings:
			bangHeads=[h[:-1] for h in headings if h[-1]=='!'] or headings
			headings=[(h[:-1] if h[-1]=='!' else h) for h in headings]
		usableLines=usableLines[1:] # and remove headinds from usableLines
		values={}
		for l in usableLines:
			val={}
			for i in range(len(headings)):
				val[headings[i]]=ll[l].split()[i]
			values[l]=val
		lines=values.keys(); lines.sort()
		# replace '=' by previous value of the parameter
		for i,l in enumerate(lines):
			for j in values[l].keys():
				if values[l][j]=='=':
					try:
						values[l][j]=values[lines[i-1]][j]
					except IndexError,KeyError:
						raise RuntimeError("The = specifier on line %d refers to nonexistent value on previous line?"%l)
		#import pprint; pprint.pprint(headings); pprint.pprint(values)
		# add descriptions, but if they repeat, append line number as well
		if not 'description' in headings:
			descs=set()
			for l in lines:
				dd=','.join(head.replace('!','')+'='+('%g'%values[head] if isinstance(values[l][head],float) else str(values[l][head])) for head in bangHeads).replace("'",'').replace('"','')
				if dd in descs: dd+='__line=%d__'%l
				values[l]['description']=dd
				descs.add(dd)
		self.values=values
				
	def paramDict(self):
		"""Return dictionary containing data from file given to constructor. Keys are line numbers (which might be non-contiguous and refer to real line numbers that one can see in text editors), values are dictionaries mapping parameter names to their values given in the file. The special value '=' has already been interpreted, ``!`` (bangs) (if any) were already removed from column titles, ``description`` column has already been added (if absent)."""
		return self.values
		
if __name__=="__main__":
	tryTable="""head1 important2! !OMP_NUM_THREADS! abcd
	1 1.1 1.2 1.3
	'a' 'b' 'c' 'd'  ### comment
	
	# empty line
	1 = = g
"""
	file='/tmp/try-tbl.txt'
	f=open(file,'w')
	f.write(tryTable)
	f.close()
	from pprint import *
	pprint(TableParamReader(file).paramDict())
	
def readParamsFromTable(tableFileLine=None,noTableOk=False,unknownOk=False,**kw):
	"""
	Read parameters from a file and assign them to __builtin__ variables.

	The format of the file is as follows (commens starting with # and empty lines allowed)::
		
		# commented lines allowed anywhere
		name1 name2 … # first non-blank line are column headings
					# empty line is OK, with or without comment
		val1  val2  … # 1st parameter set
		val2  val2  … # 2nd 
		…

	Assigned tags:

	* *description* column is assigned to Omega().tags['description']; this column is synthesized if absent (see :yref:`yade.utils.TableParamReader`)
	* Omega().tags['params']="name1=val1,name2=val2,…"
	* Omega().tags['defaultParams']="unassignedName1=defaultValue1,…"

	All parameters (default as well as settable) are saved using saveVars('table').

	:parameters:
		`tableFile`:
			text file (with one value per blank-separated columns)
		`tableLine`:
			number of line where to get the values from.
		`noTableOk`: bool
			do not raise exception if the file cannot be open; use default values
		`unknownOk`: bool
			do not raise exception if unknown column name is found in the file; assign it as well

	:return:
		number of assigned parameters.

	"""
	tagsParams=[]
	dictDefaults,dictParams={},{}
	import os, __builtin__,re
	if not tableFileLine and not os.environ.has_key('PARAM_TABLE'):
		if not noTableOk: raise EnvironmentError("PARAM_TABLE is not defined in the environment")
		O.tags['line']='l!'
	else:
		if not tableFileLine: tableFileLine=os.environ['PARAM_TABLE']
		env=tableFileLine.split(':')
		tableFile,tableLine=env[0],int(env[1])
		allTab=TableParamReader(tableFile).paramDict()
		if not allTab.has_key(tableLine): raise RuntimeError("Table %s doesn't contain valid line number %d"%(tableFile,tableLine))
		vv=allTab[tableLine]
		O.tags['line']='l%d'%tableLine
		O.tags['description']=vv['description']
		# assign values specified in the table to python vars
		# !something cols are skipped, those are env vars we don't treat at all (they are contained in description, though)
		for col in vv.keys():
			if col=='description' or col[0]=='!': continue
			if col not in kw.keys() and (not unknownOk): raise NameError("Parameter `%s' has no default value assigned"%names[i])
			if vv[col]=='*': vv[col]=kw[col] # use default value for * in the table
			elif col in kw.keys(): kw.pop(col) # remove the var from kw, so that it contains only those that were default at the end of this loop
			print 'ASSIGN',col,vv[col]
			exec('%s=%s'%(col,vv[col])) in __builtins__; tagsParams+=['%s=%s'%(col,vv[col])]; dictParams[col]=vv[col]
	# assign remaining (default) keys to python vars
	defaults=[]
	for k in kw.keys():
		exec("%s=%s"%(k,repr(kw[k]))) in __builtins__
		defaults+=["%s=%s"%(k,kw[k])]; dictDefaults[k]=kw[k]
	O.tags['defaultParams']=",".join(defaults)
	O.tags['params']=",".join(tagsParams)
	dictParams.update(dictDefaults); saveVars('table',**dictParams)
	return len(tagsParams)

