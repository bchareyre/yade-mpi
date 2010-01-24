# encoding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>
"""
Module for 2d postprocessing, containing classes to project points from 3d to 2d in various ways,
providing basic but flexible framework for extracting arbitrary scalar values from bodies and plotting the
results. There are 2 basic components: flatteners and extractors.

Flatteners
==========
Instance of classes that convert 3d (model) coordinates to 2d (plot) coordinates. Their interface is
defined by the Flatten class (__call__, planar, normal).

Extractors
==========
Callable objects returning scalar or vector value, given a body object. If a 3d vector is returned,
Flattener.planar is called, which should return only in-plane components of the vector.

Example
=======
This example can be found in examples/concrete/uniax-post.py ::

 from yade import post2d
 import pylab # the matlab-like interface of matplotlib

 O.load('/tmp/uniax-tension.xml.bz2')

 # flattener that project to the xz plane
 flattener=post2d.AxisFlatten(useRef=False,axis=1)
 # return scalar given a Body instance
 extractDmg=lambda b: b.state.normDmg
 # will call flattener.planar implicitly
 # the same as: extractVelocity=lambda b: flattener.planar(b,b.state.vel)
 extractVelocity=lambda b: b.state.vel

 # create new figure
 pylab.figure()
 # plot raw damage
 post2d.plot(post2d.data(extractDmg,flattener))

 # plot smooth damage into new figure
 pylab.figure(); ax,map=post2d.plot(post2d.data(extractDmg,flattener,stDev=2e-3))
 # show color scale
 pylab.colorbar(map,orientation='horizontal')

 # raw velocity (vector field) plot
 pylab.figure(); post2d.plot(post2d.data(extractVelocity,flattener))

 # smooth velocity plot; data are sampled at regular grid
 pylab.figure(); ax,map=post2d.plot(post2d.data(extractVelocity,flattener,stDev=1e-3))
 # save last (current) figure to file
 pylab.gcf().savefig('/tmp/foo.png') 

 # show the figures
 pylab.show()

"""
from yade import *

class Flatten:
	"""Abstract class for converting 3d point into 2d. Used by post2d.data2d."""
	def __init__(self): pass
	def __call__(self,b):
		"""Given a Body instance, should return either 2d coordinates as a 2-tuple, or None if the Body should be discarded.""" 
		pass
	def planar(self,pos,vec):
		"Given position and vector value, project the vector value to the flat plane and return its 2 in-plane components."
	def normal(self,pos,vec):
		"Given position and vector value, return lenght of the vector normal to the flat plane."

class SpiralFlatten(Flatten):
	"""Class converting 3d point to 2d based on projection from spiral.
	The y-axis in the projection corresponds to the rotation axis"""
	def __init__(self,useRef,thetaRange,dH_dTheta,axis=2,periodStart=0):
		"""@param useRef: use reference positions rather than actual positions
		@param thetaRange: (thetaMin,thetaMax) tuple; bodies outside this range will be discarded
		@param dH_dTheta: inclination of the spiral (per radian)
		@param axis: axis of rotation of the spiral
		@param periodStart: height of the spiral for zero angle
		"""
		self.useRef,self.thetaRange,self.dH_dTheta,self.axis,self.periodStart=useRef,thetaRange,dH_dTheta,axis,periodStart
		self.ax1,self.ax2=(axis+1)%3,(axis+2)%3
	def _getPos(self,b):
		return b.state.refPos if self.useRef else b.state.pos
	def __call__(self,b):
		import yade.utils
		xy,theta=yade.utils.spiralProject(_getPos(b),self.dH_dTheta,self.axis,self.periodStart)
		if theta<thetaRange[0] or theta>thetaRange[1]: return None
		return xy
	def planar(self,b,vec):
		from math import sqrt
		pos=_getPos(b)
		pos[self.axis]=0; pos.Normalize()
		return pos.Dot(vec),vec[axis]
	def normal(self,pos,vec):
		ax=Vector3(0,0,0); ax[axis]=1; pos=_getPos(b)
		circum=ax.Cross(pos); circum.Normalize()
		return circum.Dot(vec)
		

class CylinderFlatten(Flatten):
	"""Class for converting 3d point to 2d based on projection from circle.
	The y-axis in the projection corresponds to the rotation axis; the x-axis is distance form the axis.
	"""
	def __init__(self,useRef,axis=2):
		"""@param useRef: use reference positions rather than actual positions.
		@param axis of the cylinder (0, 1 or 2)
		"""
		if axis not in (0,1,2): raise IndexError("axis must be one of 0,1,2 (not %d)"%axis)
		self.useRef,self.axis=useRef,axis
	def _getPos(self,b):
		return b.state.refPos if self.useRef else b.state.pos
	def __call__(self,b):
		p=_getPos(b)
		pp=(p[(self.axis+1)%3],p[(self.axis+2)%3])
		import math.sqrt
		return math.sqrt(pp[0]**2+pp[2]**2),p[self.axis]
	def planar(self,b,vec):
		pos=_getPos(b)
		from math import sqrt
		pos[self.axis]=0; pos.Normalize()
		return pos.Dot(vec),vec[axis]
	def normal(self,b,vec):
		pos=_getPos(b)
		ax=Vector3(0,0,0); ax[axis]=1
		circum=ax.Cross(pos); circum.Normalize()
		return circum.Dot(vec)
		

class AxisFlatten(Flatten):
	def __init__(self,useRef,axis=2):
		"""@param useRef: use reference positions rather than actual positions.
		@param axis: axis normal to the plane (0, 1 or 2); the return value will be simply position with this component dropped."""
		if axis not in (0,1,2): raise IndexError("axis must be one of 0,1,2 (not %d)"%axis)
		self.useRef,self.axis=useRef,axis
		self.ax1,self.ax2=(self.axis+1)%3,(self.axis+2)%3
	def __call__(self,b):
		p=b.state.refPos if self.useRef else b.state.pos
		return (p[self.ax1],p[self.ax2])
	def planar(self,pos,vec):
		return vec[self.ax1],vec[self.ax2]
	def normal(self,pos,vec):
		return vec[self.axis]

def data(extractor,flattener,onlyDynamic=True,stDev=None,relThreshold=3.,div=(50,50),margin=(0,0)):
	"""Filter all bodies (spheres only), project them to 2d and extract required scalar value;
	return either discrete array of positions and values, or smoothed data, depending on whether the stDev
	value is specified.

	@param extractor: callable object that receives Body instance; it should return scalar, a 2-tuple (vector fields) or None (to skip that body)
	@param flattener: callable object that receives Body instance and returns its 2d coordinates or None (to skip that body)
	@param onlyDynamic: skip all non-dynamic bodies
	@param stDev: standard deviation for averaging, enables smoothing; None (default) means raw mode.
	@param relThreshold: threshold for the gaussian weight function relative to stDev (smooth mode only)
	@param div: 2-tuple specifying number of cells for the gaussian grid (smooth mode only)
	@param margin: 2-tuple specifying margin around bounding box for data (smooth mode only)
	@return: Dictionary always containing keys 'type' (one of 'rawScalar','rawVector','smoothScalar','smoothVector', depending on value of smooth and on return value from extractor), 'x', 'y', 'bbox'.
	
		Raw data further contains 'radii'.

		Scalar fields contain 'val' (value from extractor), vector fields have 'valX' and 'valY' (2 components returned by the extractor).
	"""
	from miniWm3Wrap import Vector3
	xx,yy,dd1,dd2,rr=[],[],[],[],[]
	nDim=0
	for b in O.bodies:
		if onlyDynamic and not b.dynamic: continue
		if not isinstance(b.shape,Sphere): continue
		xy,d=flattener(b),extractor(b)
		if xy==None or d==None: continue
		if nDim==0: nDim=1 if isinstance(d,float) else 2
		if nDim==1: dd1.append(d);
		elif len(d)==2:
			dd1.append(d[0]); dd2.append(d[1])
		elif len(d)==3:
			d1,d2=flattener.planar(b,Vector3(d))
			dd1.append(d1); dd2.append(d2)
		else:
			raise RuntimeError("Extractor must return float or 2 or 3 (not %d) floats"%nDim)
		xx.append(xy[0]); yy.append(xy[1]); rr.append(b.shape['radius'])
	if stDev==None:
		bbox=(min(xx),min(yy)),(max(xx),max(yy))
		if nDim==1: return {'type':'rawScalar','x':xx,'y':yy,'val':dd1,'radii':rr,'bbox':bbox}
		else: return {'type':'rawVector','x':xx,'y':yy,'valX':dd1,'valY':dd2,'radii':rr,'bbox':bbox}
	
	from yade.WeightedAverage2d import GaussAverage
	import numpy
	lo,hi=(min(xx),min(yy)),(max(xx),max(yy))
	llo=lo[0]-margin[0],lo[1]-margin[1]; hhi=hi[0]+margin[0],hi[1]+margin[1]
	ga=GaussAverage(llo,hhi,div,stDev,relThreshold)
	ga2=GaussAverage(llo,hhi,div,stDev,relThreshold)
	for i in range(0,len(xx)):
		ga.add(dd1[i],(xx[i],yy[i]))
		if nDim>1: ga2.add(dd2[i],(xx[i],yy[i]))
	step=[(hhi[i]-llo[i])/float(div[i]) for i in [0,1]]
	xxx,yyy=[numpy.arange(llo[i]+.5*step[i],hhi[i],step[i]) for i in [0,1]]
	ddd=numpy.zeros((len(yyy),len(xxx)),float)
	ddd2=numpy.zeros((len(yyy),len(xxx)),float)
	for cx in range(0,div[0]):
		for cy in range(0,div[1]):
			ddd[cy,cx]=float(ga.avg((xxx[cx],yyy[cy])))
			if nDim>1: ddd2[cy,cx]=float(ga2.avg((xxx[cx],yyy[cy])))
	if nDim==1: return {'type':'smoothScalar','x':xxx,'y':yyy,'val':ddd,'bbox':(llo,hhi)} 
	else: return {'type':'smoothVector','x':xxx,'y':yyy,'valX':ddd,'valY':ddd2,'bbox':(llo,hhi)}
	
def plot(data,axes=None,alpha=.5,clabel=True,**kw):
	"""Given output from post2d.data, plot the scalar as discrete or smooth plot.

	For raw discrete data, plot filled circles with radii of particles, colored by the scalar value.

	For smooth discrete data, plot image with optional contours and contour labels.

	For vector data (raw or smooth), plot quiver (vector field), with arrows colored by the magnitude.

	@param axes: matplotlib.axes instance to plot to; if None, will be created from scratch
	@param data: return value from post2d.data
	@param clabel: show contour labels (smooth mode only)
	@return: tuple of (axes,mappable); mappable can be used in further calls to pylab.colorbar
	"""
	import pylab,math
	if not axes: axes=pylab.gca()
	if data['type']=='rawScalar':
		from matplotlib.patches import Circle
		import matplotlib.collections,numpy
		patches=[]
		for x,y,d,r in zip(data['x'],data['y'],data['val'],data['radii']):
			patches.append(Circle(xy=(x,y),radius=r))
		coll=matplotlib.collections.PatchCollection(patches,linewidths=0.,**kw)
		coll.set_array(numpy.array(data['val']))
		bb=coll.get_datalim(coll.get_transform())
		axes.add_collection(coll)
		axes.set_xlim(bb.xmin,bb.xmax); axes.set_ylim(bb.ymin,bb.ymax)
		axes.grid(True); axes.set_aspect('equal')
		return axes,coll
	elif data['type']=='smoothScalar':
		loHi=data['bbox']
		img=axes.imshow(data['val'],extent=(loHi[0][0],loHi[1][0],loHi[0][1],loHi[1][1]),origin='lower',aspect='equal',**kw)
		ct=axes.contour(data['x'],data['y'],data['val'],colors='k',origin='lower',extend='both')
		axes.update_datalim(loHi)
		if clabel: axes.clabel(ct,inline=1,fontsize=10)
		axes.set_xlim(loHi[0][0],loHi[1][0]); axes.set_ylim(loHi[0][1],loHi[1][1])
		axes.grid(True); axes.set_aspect('equal')
		return axes,img
	elif data['type'] in ('rawVector','smoothVector'):
		import numpy
		loHi=data['bbox']
		valX,valY=numpy.array(data['valX']),numpy.array(data['valY']) # rawVector data are plain python lists
		scalars=numpy.sqrt(valX**2+valY**2)
		# numpy.sqrt computes element-wise sqrt
		quiv=axes.quiver(data['x'],data['y'],data['valX'],data['valY'],scalars,**kw)
		#axes.update_datalim(loHi)
		axes.set_xlim(loHi[0][0],loHi[1][0]); axes.set_ylim(loHi[0][1],loHi[1][1])
		axes.grid(True); axes.set_aspect('equal')
		return axes,quiv


