# encoding: utf-8
#
# utility functions for yade
#
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>

#from yade._utils import *

import math,random
from yade.wrapper import *
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

def sphere(center,radius,density=1,young=30e9,poisson=.3,frictionAngle=0.5236,dynamic=True,wire=False,color=None,physParamsClass='BodyMacroParameters',physParamsAttr={}):
	"""Create default sphere, with given parameters. Physical properties such as mass and inertia are calculated automatically."""
	s=Body()
	if not color: color=randomColor()
	s.shape=GeometricalModel('Sphere',{'radius':radius,'diffuseColor':color,'wire':wire,'visible':True})
	s.mold=InteractingGeometry('InteractingSphere',{'radius':radius,'diffuseColor':color})
	V=(4./3)*math.pi*radius**3
	inert=(2./5.)*V*density*radius**2
	pp={'se3':[center[0],center[1],center[2],1,0,0,0],'refSe3':[center[0],center[1],center[2],1,0,0,0],'mass':V*density,'inertia':[inert,inert,inert],'young':young,'poisson':poisson,'frictionAngle':frictionAngle}
	pp.update(physParamsAttr)
	s.phys=PhysicalParameters(physParamsClass)
	for k in [attr for attr in pp.keys() if attr in s.phys.keys()]:
		s.phys[k]=pp[k]
	s.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
	s['isDynamic']=dynamic
	return s

def box(center,extents,orientation=[1,0,0,0],density=1,young=30e9,poisson=.3,frictionAngle=0.5236,dynamic=True,wire=False,color=None,physParamsClass='BodyMacroParameters'):
	"""Create default box (cuboid), with given parameters. Physical properties such as mass and inertia are calculated automatically."""
	b=Body()
	if not color: color=randomColor()
	b.shape=GeometricalModel('Box',{'extents':extents,'diffuseColor':color,'wire':wire,'visible':True})
	b.mold=InteractingGeometry('InteractingBox',{'extents':extents,'diffuseColor':color})
	mass=8*extents[0]*extents[1]*extents[2]*density
	b.phys=PhysicalParameters(physParamsClass,{'se3':[center[0],center[1],center[2],orientation[0],orientation[1],orientation[2],orientation[3]],'refSe3':[center[0],center[1],center[2],orientation[0],orientation[1],orientation[2],orientation[3]],'mass':mass,'inertia':[mass*4*(extents[1]**2+extents[2]**2),mass*4*(extents[0]**2+extents[2]**2),mass*4*(extents[0]**2+extents[1]**2)],'young':young,'poisson':poisson,'frictionAngle':frictionAngle})
	b.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
	b['isDynamic']=dynamic
	return b

def facet(vertices,young=30e9,poisson=.3,frictionAngle=0.5236,dynamic=False,wire=True,color=None,physParamsClass='BodyMacroParameters',physParamsAttr={}):
	"""Create default facet with given parameters."""
	b=Body()
	if not color: color=randomColor()
	b.shape=GeometricalModel('Facet',{'diffuseColor':color,'wire':wire,'visible':True})
	b.mold=InteractingGeometry('InteractingFacet',{'diffuseColor':color})
	center=inscribedCircleCenter(list(vertices[0]),list(vertices[1]),list(vertices[2]))
	vertices=map(lambda a,b:map(lambda x,y:x-y,a,b),vertices,[center,center,center]) 
	vStr='['+' '.join(['{%g %g %g}'%(v[0],v[1],v[2]) for v in vertices])+']'
	b.shape.setRaw('vertices',vStr)
	b.mold.setRaw('vertices',vStr)
	pp={'se3':[center[0],center[1],center[2],1,0,0,0],'refSe3':[center[0],center[1],center[2],1,0,0,0],'young':young,'poisson':poisson,'frictionAngle':frictionAngle}
	pp.update(physParamsAttr)
	b.phys=PhysicalParameters(physParamsClass)
	for k in [attr for attr in pp.keys() if attr in b.phys.keys()]:
		b.phys[k]=pp[k]
	b.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
	b['isDynamic']=dynamic
	b.mold.postProcessAttributes()
	return b

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
			walls[-1].shape['visible']=True
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
	Non-spherical bodies are silently skipped."""
	o=Omega()
	out=open(filename,'w')
	for b in o.bodies:
		if not b.shape or not b.shape.name=='Sphere' or not consider(b.id): continue
		out.write('%g\t%g\t%g\t%g\n'%(b.phys['se3'][0],b.phys['se3'][1],b.phys['se3'][2],b.shape['radius']))
	out.close()

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


def import_stl_geometry(file, young=30e9,poisson=.3,color=[0,1,0],frictionAngle=0.5236,wire=True,noBoundingVolume=False,noInteractingGeometry=False,physParamsClass='BodyMacroParameters',physParamsAttr={}):
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
		for k in [attr for attr in pp.keys() if attr in b.phys.keys()]:
			b.phys[k]=pp[k]
		if not noBoundingVolume:
			b.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
		o.bodies.append(b)
	imp.import_geometry(o.bodies,begin,noInteractingGeometry)
	imported=range(begin,begin+imp.number_of_facets)
	for i in imported:
		if not noInteractingGeometry:
			o.bodies[i].mold.postProcessAttributes()
		o.bodies[i].shape['diffuseColor']=color
	return imported

def encodeVideoFromFrames(wildcard,out,renameNotOverwrite=True,fps=24):
	import pygst,sys,gobject,os
	pygst.require("0.10")
	import gst
	if renameNotOverwrite and os.path.exists(out):
		i=0;
		while(os.path.exists(out+"~%d"%i)): i+=1
		os.rename(out,out+"~%d"%i); print "Output file `%s' already existed, old file renamed to `%s'"%(out,out+"~%d"%i)
	print "Encoding video from %s to %s"%(wildcard,out)
	pipeline=gst.parse_launch('multifilesrc location="%s" index=0 caps="image/png,framerate=\(fraction\)%d/1" ! pngdec ! ffmpegcolorspace ! theoraenc sharpness=2 quality=63 ! oggmux ! filesink location="%s"'%(wildcard,fps,out))
	bus=pipeline.get_bus()
	bus.add_signal_watch()
	mainloop=gobject.MainLoop();
	bus.connect("message::eos",lambda bus,msg: mainloop.quit())
	pipeline.set_state(gst.STATE_PLAYING)
	mainloop.run()
	pipeline.set_state(gst.STATE_NULL); pipeline.get_state()

def readParamsFromTable(tableFileLine=None,noTableOk=False,**kw):
	"""
	Read parameters from a file and assign them to __builtin__ variables.

	tableFile is a text file (with one value per blank-separated columns)
	tableLine is number of line where to get the values from

		The format of the file is as follows (commens starting with # and empty lines allowed
		
		name1 name2 … # 0th line
		val1  val2  … # 1st line
		val2  val2  … # 2nd line
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
	import os, __builtin__
	if not tableFileLine and not os.environ.has_key('PARAM_TABLE'):
		if not noTableOk: raise EnvironmentError("PARAM_TABLE is not defined in the environment")
		o.tags['line']='l!'
	else:
		if not tableFileLine: tableFileLine=os.environ['PARAM_TABLE']
		tableFile,tableLine=tableFileLine.split(':')
		o.tags['line']='l'+tableLine
		ll=[l.split('#')[0] for l in ['']+open(tableFile).readlines()]; names=ll[1].split(); values=ll[int(tableLine)].split()
		assert(len(names)==len(values))
		for i in range(len(names)):
			if names[i]=='description': o.tags['description']=values[i]
			else:
				if names[i] not in kw.keys(): raise NameError("Parameter `%s' has no default value assigned"%names[i])
				kw.pop(names[i])
				eq="%s=%s"%(names[i],values[i])
				exec('__builtin__.%s=%s'%(names[i],values[i])); tagsParams+=['%s=%s'%(names[i],values[i])]; dictParams[names[i]]=values[i]
	defaults=[]
	for k in kw.keys():
		exec("__builtin__.%s=%s"%(k,kw[k]))
		defaults+=["%s=%s"%(k,kw[k])]; dictDefaults[k]=kw[k]
	o.tags['defaultParams']=",".join(defaults)
	o.tags['params']=",".join(tagsParams)
	dictParams.update(dictDefaults); saveVars('table',**dictParams)
	return len(tagsParams)


def basicDEMEngines(interPhysics='SimpleElasticRelationships',constitutiveLaw='ElasticContactLaw',gravity=None,damping=.4):
	"""Set basic set of DEM engines and initializers.
	
	interPhysics and constitutiveLaw specify class of respective engines to use instead of defaults.

	Gravity can be list or tuple to specify numeric value, it can also be an object that will be inserted into
	engines, however. By default, no gravity is applied.
	"""
	O.initializers=[
		StandAloneEngine('PhysicalActionContainerInitializer'),
		MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('InteractingFacet2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
	O.engines=[
		StandAloneEngine('PhysicalActionContainerReseter'),
		MetaEngine('BoundingVolumeMetaEngine',[
			EngineUnit('InteractingSphere2AABB'),
			EngineUnit('InteractingBox2AABB'),
			EngineUnit('InteractingFacet2AABB'),
			EngineUnit('MetaInteractingGeometry2AABB')
		]),
		StandAloneEngine('PersistentSAPCollider'),
		MetaEngine('InteractionGeometryMetaEngine',[
			EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
			EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry'),
			EngineUnit('InteractingBox2InteractingSphere4SpheresContactGeometry')
		]),
		MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
		StandAloneEngine('ElasticContactLaw'),
	]
	if gravity:
		if islist(gravity) or istuple(gravity):
			O.engines=O.engines+[DeusExMachina('GravityEngine',{'gravity':gravity}),]
		else:
			O.engines=O.engines+[gravity]
	O.engines=O.engines+[DeusExMachina('NewtonsDampedLaw',{'damping':damping}),]
	
		

def ColorizedVelocityFilter(isFilterActivated=True,autoScale=True,minValue=0,maxValue=0,posX=0,posY=0.2,width=0.05,height=0.5,title='Velocity, m/s'):
    f = DeusExMachina('ColorizedVelocityFilter',{'isFilterActivated':isFilterActivated,'autoScale':autoScale,'minValue':minValue,'maxValue':maxValue,'posX':posX,'posY':posY,'width':width,'height':height,'title':title})
    O.engines+=[f]
    return f

def ColorizedTimeFilter(point=[0,0,0],normal=[0,1,0],isFilterActivated=True,autoScale=True,minValue=0,maxValue=0,posX=0,posY=0.2,width=0.05,height=0.5,title='Time, m/s'):
    f = DeusExMachina('ColorizedTimeFilter',{'point':point,'normal':normal,'isFilterActivated':isFilterActivated,'autoScale':autoScale,'minValue':minValue,'maxValue':maxValue,'posX':posX,'posY':posY,'width':width,'height':height,'title':title})
    O.engines+=[f]
    return f

def PythonRunnerFilter(command='pass',isFilterActivated=True):
    f = DeusExMachina('PythonRunnerFilter',{'command':command,'isFilterActivated':isFilterActivated})
    O.engines+=[f]
    return f
