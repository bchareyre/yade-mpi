# encoding: utf-8
#
# utility functions for yade
#
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>

import math,random
from yade.wrapper import *
from miniWm3Wrap import *
try: # use psyco if available
	import psyco
	psyco.full()
except ImportError: pass

# c++ implementations for performance reasons
from yade._utils import *

def saveVars(mark='',loadNow=False,**kw):
	"""Save passed variables into the simulation so that it can be recovered when the simulation is loaded again.

	For example, variables a=5, b=66 and c=7.5e-4 are defined. To save those, use

	 utils.saveVars(a=a,b=b,c=c)

	those variables will be save in the .xml file, when the simulation itself is saved. To recover those variables once
	the .xml is loaded again, use

	 utils.loadVars(mark)

	and they will be defined in the __builtin__ namespace (i.e. available from anywhere in the python code).

	If loadParam==True, variables will be loaded immediately after saving. That effectively makes **kw available in builtin namespace.
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
	"""Compute P-wave critical timestep for a single sphere.
	If you want to compute minimum critical timestep for all spheres in the simulation, use utils.PWaveTimeStep() instead."""
	from math import sqrt
	return radius/sqrt(young/density)

def randomColor(): return [random.random(),random.random(),random.random()]

def typedEngine(name): return [e for e in Omega().engines if e.name==name][0]

def downCast(obj,newClassName):
	"""Cast given object to class deriving from the same yade root class and copy all parameters from given object.
	Obj should be up in the inheritance tree, otherwise some attributes may not be defined in the new class."""
	return obj.__class__(newClassName,dict([ (key,obj[key]) for key in obj.keys() ]))

bodiesPhysDefaults={'young':30e9,'poisson':.3,'frictionAngle':.5236}

def sphere(center,radius,dynamic=True,wire=False,color=None,density=1,highlight=False,physParamsClass='BodyMacroParameters',**physParamsAttr):
	"""Create default sphere, with given parameters. Physical properties such as mass and inertia are calculated automatically."""
	s=Body()
	if not color: color=randomColor()
	pp=bodiesPhysDefaults.copy(); pp.update(physParamsAttr);
	s.shape=GeometricalModel('Sphere',radius=radius,diffuseColor=color,wire=wire,highlight=highlight)
	s.mold=InteractingGeometry('InteractingSphere',radius=radius,diffuseColor=color)
	V=(4./3)*math.pi*radius**3
	inert=(2./5.)*V*density*radius**2
	pp.update({'se3':[center[0],center[1],center[2],1,0,0,0],'refSe3':[center[0],center[1],center[2],1,0,0,0],'mass':V*density,'inertia':[inert,inert,inert]})
	s.phys=PhysicalParameters(physParamsClass)
	s.phys.updateExistingAttrs(pp)
	s.bound=BoundingVolume('AABB',diffuseColor=[0,1,0])
	s['isDynamic']=dynamic
	return s

def box(center,extents,orientation=[1,0,0,0],dynamic=True,wire=False,color=None,density=1,highlight=False,physParamsClass='BodyMacroParameters',**physParamsAttr):
	"""Create default box (cuboid), with given parameters. Physical properties such as mass and inertia are calculated automatically."""
	b=Body()
	if not color: color=randomColor()
	pp=bodiesPhysDefaults.copy(); pp.update(physParamsAttr);
	b.shape=GeometricalModel('Box',extents=extents,diffuseColor=color,wire=wire,highlight=highlight)
	b.mold=InteractingGeometry('InteractingBox',extents=extents,diffuseColor=color)
	mass=8*extents[0]*extents[1]*extents[2]*density
	V=extents[0]*extents[1]*extents[2]
	pp.update({'se3':[center[0],center[1],center[2],orientation[0],orientation[1],orientation[2],orientation[3]],'refSe3':[center[0],center[1],center[2],orientation[0],orientation[1],orientation[2],orientation[3]],'mass':V*density,'inertia':[mass*4*(extents[1]**2+extents[2]**2),mass*4*(extents[0]**2+extents[2]**2),mass*4*(extents[0]**2+extents[1]**2)]})
	b.phys=PhysicalParameters(physParamsClass)
	b.phys.updateExistingAttrs(pp)
	b.bound=BoundingVolume('AABB',diffuseColor=[0,1,0])
	b['isDynamic']=dynamic
	return b

def facet(vertices,dynamic=False,wire=True,color=None,density=1,highlight=False,physParamsClass='BodyMacroParameters',**physParamsAttr):
	"""Create default facet with given parameters. Vertices are given as sequence of 3 3-tuple and they, all in global coordinates."""
	b=Body()
	if not color: color=randomColor()
	pp=bodiesPhysDefaults.copy(); pp.update(physParamsAttr);
	b.shape=GeometricalModel('Facet',diffuseColor=color,wire=wire,highlight=highlight)
	b.mold=InteractingGeometry('InteractingFacet',diffuseColor=color)
	center=inscribedCircleCenter(vertices[0],vertices[1],vertices[2])
	vertices=Vector3(vertices[0])-center,Vector3(vertices[1])-center,Vector3(vertices[2])-center
	b.shape['vertices']=vertices;	b.mold['vertices']=vertices
	pp.update({'se3':[center[0],center[1],center[2],1,0,0,0],'refSe3':[center[0],center[1],center[2],1,0,0,0],'inertia':[0,0,0]})
	b.phys=PhysicalParameters(physParamsClass)
	b.phys.updateExistingAttrs(pp)
	b.bound=BoundingVolume('AABB',diffuseColor=[0,1,0])
	b['isDynamic']=dynamic
	b.mold.postProcessAttributes(True)
	return b

def facetBox(center,extents,orientation=[1,0,0,0],wallMask=63,**kw):
	"""Create arbitrarily-aligned box composed of facets, with given center, extents and orientation. wallMask determines which walls will be created,
	in the order -x (1), +x (2), -y (4), +y (8), -z (16), +z (32). The numbers are ANDed; the default 63 means to create all walls.
	Remaining **kw arguments are passed to utils.facet. The facets are oriented outwards from the box."""
	mn,mx=[-extents[i] for i in 0,1,2],[extents[i] for i in 0,1,2]
	def doWall(a,b,c,d):
		return [facet((a,b,c),**kw),facet((a,c,d),**kw)]
	ret=[]
	qTemp = Quaternion(Vector3(orientation[0],orientation[1],orientation[2]),orientation[3])
	A=qTemp.Rotate(Vector3(mn[0],mn[1],mn[2]))+center
	B=qTemp.Rotate(Vector3(mx[0],mn[1],mn[2]))+center
	C=qTemp.Rotate(Vector3(mx[0],mx[1],mn[2]))+center
	D=qTemp.Rotate(Vector3(mn[0],mx[1],mn[2]))+center
	E=qTemp.Rotate(Vector3(mn[0],mn[1],mx[2]))+center
	F=qTemp.Rotate(Vector3(mx[0],mn[1],mx[2]))+center
	G=qTemp.Rotate(Vector3(mx[0],mx[1],mx[2]))+center
	H=qTemp.Rotate(Vector3(mn[0],mx[1],mx[2]))+center
	if wallMask&1:  ret+=doWall(A,D,H,E)
	if wallMask&2:  ret+=doWall(B,C,G,F)
	if wallMask&4:  ret+=doWall(A,B,F,E)
	if wallMask&8:  ret+=doWall(D,H,G,C)
	if wallMask&16: ret+=doWall(A,D,C,B)
	if wallMask&32: ret+=doWall(E,F,G,H)
	return ret
	
	'''
	mn,mx=[center[i]-extents[i] for i in 0,1,2],[center[i]+extents[i] for i in 0,1,2]
	def doWall(a,b,c,d):
		return [facet((a,b,c),**kw),facet((a,c,d),**kw)]
	ret=[]
	qTemp = Quaternion(Vector3(orientation[0],orientation[1],orientation[2]),orientation[3])
	A=qTemp.Rotate(Vector3(mn[0],mn[1],mn[2]))
	B=qTemp.Rotate(Vector3(mx[0],mn[1],mn[2]))
	C=qTemp.Rotate(Vector3(mx[0],mx[1],mn[2]))
	D=qTemp.Rotate(Vector3(mn[0],mx[1],mn[2]))
	E=qTemp.Rotate(Vector3(mn[0],mn[1],mx[2]))
	F=qTemp.Rotate(Vector3(mx[0],mn[1],mx[2]))
	G=qTemp.Rotate(Vector3(mx[0],mx[1],mx[2]))
	H=qTemp.Rotate(Vector3(mn[0],mx[1],mx[2]))
	if wallMask&1:  ret+=doWall(A,D,H,E)
	if wallMask&2:  ret+=doWall(B,C,G,F)
	if wallMask&4:  ret+=doWall(A,B,F,E)
	if wallMask&8:  ret+=doWall(D,H,G,C)
	if wallMask&16: ret+=doWall(A,D,C,B)
	if wallMask&32: ret+=doWall(E,F,G,H)
	return ret
	'''
	

def aabbWalls(extrema=None,thickness=None,oversizeFactor=1.5,**kw):
	"""return 6 walls that will wrap existing packing;
	extrema are extremal points of the AABB of the packing (will be calculated if not specified)
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
			walls[-1].shape['wire']=True
	return walls


def aabbDim(cutoff=0.,centers=False):
	"""return dimensions of the bounding box, optionally cut."""
	a=aabbExtrema(cutoff,centers)
	return (a[1][0]-a[0][0],a[1][1]-a[0][1],a[1][2]-a[0][2])

def aabbExtrema2d(pts):
	"""return 2d bounding box for a sequence of 2-tuples"""
	inf=float('inf')
	min,max=[inf,inf],[-inf,-inf]
	for pt in pts:
		if pt[0]<min[0]: min[0]=pt[0]
		elif pt[0]>max[0]: max[0]=pt[0]
		if pt[1]<min[1]: min[1]=pt[1]
		elif pt[1]>max[1]: max[1]=pt[1]
	return tuple(min),tuple(max)

def perpendicularArea(axis):
	"""return area perpendicular to given axis (0=x,1=y,2=z) generated by bodies
	for which the function consider returns True (defaults to returning True always)
	and which is of the type "Sphere"
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


def spheresFromFile(filename,scale=1.,wenjieFormat=False,**kw):
	"""Load sphere coordinates from file, create spheres, insert them to the simulation.

	filename is the file holding ASCII numbers (at least 4 colums that hold x_center, y_center, z_center, radius).
	All remaining arguments are passed the the yade.utils.sphere function that creates the bodies.

	wenjieFormat will skip all lines that have exactly 5 numbers and where the 4th one is exactly 1.0 -
	this was used by a fellow developer called Wenjie to mark box elements.
	
	Returns list of body ids that were inserted into simulation."""
	o=Omega()
	ret=[]
	for l in open(filename):
		ss=[float(i) for i in l.split()]
		if wenjieFormat and len(ss)==5 and ss[4]==1.0: continue
		id=o.bodies.append(sphere([scale*ss[0],scale*ss[1],scale*ss[2]],scale*ss[3],**kw))
		ret.append(id)
	return ret

def spheresToFile(filename,consider=lambda id: True):
	"""Save sphere coordinates into ASCII file; the format of the line is: x y z r.
	Non-spherical bodies are silently skipped.
	
	Returns number of spheres that were written."""
	o=Omega()
	out=open(filename,'w')
	count=0
	for b in o.bodies:
		if not b.shape or not b.shape.name=='Sphere' or not consider(b.id): continue
		out.write('%g\t%g\t%g\t%g\n'%(b.phys.pos[0],b.phys.pos[1],b.phys.pos[2],b.shape['radius']))
		count+=1
	out.close()
	return count

def avgNumInteractions(cutoff=0.):
	nums,counts=bodyNumInteractionsHistogram(aabbExtrema(cutoff))
	return sum([nums[i]*counts[i] for i in range(len(nums))])/(1.*sum(counts))

def plotNumInteractionsHistogram(cutoff=0.):
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


def import_stl_geometry(file, young=30e9,poisson=.3,color=[0,1,0],frictionAngle=0.5236,wire=True,noBoundingVolume=False,noInteractingGeometry=False,physParamsClass='BodyMacroParameters',**physParamsAttr):
	""" Import geometry from stl file, create facets and return list of their ids."""
	imp = STLImporter()
	imp.wire = wire
	imp.open(file)
	o=Omega()
	begin=len(o.bodies)
	for i in xrange(imp.number_of_facets):
		b=Body()
		b['isDynamic']=False
		pp={'se3':[0,0,0,1,0,0,0],'young':young,'poisson':poisson,'frictionAngle':frictionAngle}
		pp.update(physParamsAttr)
		b.phys=PhysicalParameters(physParamsClass)
		b.phys.updateExistingAttrs(pp)
		if not noBoundingVolume:
			b.bound=BoundingVolume('AABB',diffuseColor=[0,1,0])
		o.bodies.append(b)
	imp.import_geometry(o.bodies,begin,noInteractingGeometry)
	imported=range(begin,begin+imp.number_of_facets)
	for i in imported:
		if not noInteractingGeometry:
			o.bodies[i].mold.postProcessAttributes(True)
		o.bodies[i].shape['diffuseColor']=color
	return imported

def encodeVideoFromFrames(frameSpec,out,renameNotOverwrite=True,fps=24):
	"""Create .ogg video from external image files.
	
	@param frameSpec If string, wildcard in format understood by GStreamer's multifilesrc plugin (e.g. '/tmp/frame-%04d.png'). If list or tuple, filenames to be encoded in given order.
	@param out file to save video into
	@param renameNotOverwrite if True, existing same-named video file will have ~[number] appended; will be overwritten otherwise.
	@param fps Frames per second.
	"""
	import pygst,sys,gobject,os,tempfile,shutil
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
			os.symlink(frame,os.path.join(tmpDir,'%07d'%no))
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


def readParamsFromTable(tableFileLine=None,noTableOk=False,unknownOk=False,**kw):
	"""
	Read parameters from a file and assign them to __builtin__ variables.

	tableFile is a text file (with one value per blank-separated columns)
	tableLine is number of line where to get the values from

		The format of the file is as follows (commens starting with # and empty lines allowed)
		
		# commented lines allowed anywhere
		name1 name2 … # first non-blank line are column headings
		val1  val2  … # 1st parameter set
		val2  val2  … # 2nd 
		…

	The name `description' is special and is assigned to Omega().tags['description']

	assigns Omega().tags['params']="name1=val1,name2=val2,…"
	
	assigns Omega().tags['defaultParams']="unassignedName1=defaultValue1,…"

	saves all parameters (default as well as settable) using saveVars('table')

	return value is the number of assigned parameters.
	"""
	o=Omega()
	tagsParams=[]
	dictDefaults,dictParams={},{}
	import os, __builtin__,re
	if not tableFileLine and not os.environ.has_key('PARAM_TABLE'):
		if not noTableOk: raise EnvironmentError("PARAM_TABLE is not defined in the environment")
		o.tags['line']='l!'
	else:
		if not tableFileLine: tableFileLine=os.environ['PARAM_TABLE']
		env=tableFileLine.split(':')
		tableDesc=None
		tableFile,tableLine=env[0],env[1]
		if len(env)>2: tableDesc=env[3]
		o.tags['line']='l'+tableLine
		# the empty '#' line to make line number 1-based
		ll=[l for l in ['#']+open(tableFile).readlines()]; values=ll[int(tableLine)].split('#')[0].split()
		names=None
		for l in ll:
			if not re.match(r'^\s*(#.*)?$',l): names=l.split(); break
		if not names: raise RuntimeError("No non-blank line (colum headings) found.");
		assert(len(names)==len(values))
		if 'description' in names: O.tags['description']=values[names.index('description')]
		else:
			bangCols=[i for i,h in enumerate(names) if h[-1]=='!']
			if len(bangCols)==0: bangCols=range(len(names))
			for i in range(len(names)):
				if names[i][-1]=='!': names[i]=names[i][:-1] # strip trailing !
			O.tags['description']=','.join(names[col]+'='+('%g'%values[col] if isinstance(values[col],float) else str(values[col])) for col in bangCols).replace("'",'').replace('"','')
		for i in range(len(names)):
			if names[i]=='description': continue
			if names[i] not in kw.keys():
				if (not unknownOk) and names[i][0]!='!': raise NameError("Parameter `%s' has no default value assigned"%names[i])
			else: kw.pop(names[i])
			if names[i][0]!='!':
				exec('%s=%s'%(names[i],values[i])) in __builtins__; tagsParams+=['%s=%s'%(names[i],values[i])]; dictParams[names[i]]=values[i]
	defaults=[]
	for k in kw.keys():
		exec("%s=%s"%(k,repr(kw[k]))) in __builtins__
		defaults+=["%s=%s"%(k,kw[k])]; dictDefaults[k]=kw[k]
	o.tags['defaultParams']=",".join(defaults)
	o.tags['params']=",".join(tagsParams)
	dictParams.update(dictDefaults); saveVars('table',**dictParams)
	return len(tagsParams)

def ColorizedVelocityFilter(isFilterActivated=True,autoScale=True,minValue=0,maxValue=0,posX=0,posY=0.2,width=0.05,height=0.5,title='Velocity, m/s'):
    f = DeusExMachina('ColorizedVelocityFilter',isFilterActivated=isFilterActivated,autoScale=autoScale,minValue=minValue,maxValue=maxValue,posX=posX,posY=posY,width=width,height=height,title=title)
    O.engines+=[f]
    return f

def ColorizedTimeFilter(point=[0,0,0],normal=[0,1,0],isFilterActivated=True,autoScale=True,minValue=0,maxValue=0,posX=0,posY=0.2,width=0.05,height=0.5,title='Time, m/s'):
    f = DeusExMachina('ColorizedTimeFilter',point=point,normal=normal,isFilterActivated=isFilterActivated,autoScale=autoScale,minValue=minValue,maxValue=maxValue,posX=posX,posY=posY,width=width,height=height,title=title)
    O.engines+=[f]
    return f

def PythonRunnerFilter(command='pass',isFilterActivated=True):
    f = DeusExMachina('PythonRunnerFilter',command=command,isFilterActivated=isFilterActivated)
    O.engines+=[f]
    return f

def replaceCollider(colliderEngine):
	"""Replaces collider (Collider) engine with the engine supplied. Raises error if no collider is in engines."""
	colliderIdx=-1
	for i,e in enumerate(O.engines):
		if O.isChildClassOf(e.name,"Collider"):
			colliderIdx=i
			break
	if colliderIdx<0: raise RuntimeError("No Collider found within O.engines.")
	O.engines=O.engines[:colliderIdx]+[colliderEngine]+O.engines[colliderIdx+1:]


def procStatus(name):
	import os
	for l in open('/proc/%d/status'%os.getpid()):
		if l.split(':')[0]==name: return l
	raise "No such line in /proc/[pid]/status: "+name
def vmData():
	l=procStatus('VmData'); ll=l.split(); assert(ll[2]=='kB')
	return int(ll[1])

def uniaxialTestFeatures(filename=None,areaSections=10,**kw):
	"""Get some data about the current packing useful for uniaxial test:
	
	1. Find the dimensions that is the longest (uniaxial loading axis)
	2. Find the minimum cross-section area of the speciment by examining several (areaSections)
		sections perpendicular to axis, computing area of the convex hull for each one. This will
		work also for non-prismatic specimen.
	3. Find the bodies that are on the negative/positive boundary, to which the straining condition
		should be applied.

	@param filename if given, spheres will be loaded from this file (ASCII format); if not, current simulation will be used.
	@param areaSection number of section that will be used to estimate cross-section

	Returns dictionary with keys 'negIds', 'posIds', 'axis', 'area'.
	"""
	if filename: ids=spheresFromFile(filename,**kw)
	else: ids=[b.id for b in O.bodies]
	mm,mx=aabbExtrema()
	dim=aabbDim(); axis=list(dim).index(max(dim)) # list(dim) for compat with python 2.5 which didn't have index defined for tuples yet (appeared in 2.6 first)
	import numpy
	areas=[approxSectionArea(coord,axis) for coord in numpy.linspace(mm[axis],mx[axis],num=10)[1:-1]]
	negIds,posIds=negPosExtremeIds(axis=axis,distFactor=2.2)
	return {'negIds':negIds,'posIds':posIds,'axis':axis,'area':min(areas)}

def NormalRestitution2DampingRate(en):
        """Compute the normal damping rate as a function of the normal coefficient of restitution.
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

