# 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
# https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

"""
Auxiliary functions for polyhedra
"""


import math,random,doctest,geom,numpy
from yade import Vector3
from yade.wrapper import *
#from miniEigen import *
try: # use psyco if available
	import psyco
	psyco.full()
except ImportError: pass


# c++ implementations for performance reasons
from yade._polyhedra_utils import *

#**********************************************************************************
def randomColor(seed=None):
	random.seed(seed);
	#Return random Vector3 with each component in interval 0...1 (uniform distribution)
	return Vector3(random.random(),random.random(),random.random())

#**********************************************************************************
#create polyhedra, one can specify vertices directly, or leave it empty for random shape
def polyhedra(material,size=Vector3(1,1,1),seed=None,v=[],mask=1,fixed=False, color=[-1,-1,-1]):
	"""create polyhedra, one can specify vertices directly, or leave it empty for random shape.
	
	:param Material material: material of new body
	:param Vector3 size: size of new body (see Polyhedra docs)
	:param float seed: seed for random operations
	:param [Vector3] v: list of body vertices (see Polyhedra docs)
	"""
	b=Body()
	random.seed(seed);
	b.aspherical = True
	if len(v)>0:
		b.shape = Polyhedra(v=v)
	else:
		b.shape = Polyhedra(size = size, seed=random.randint(0,1E6))
	if color[0] == -1:
		b.shape.color = randomColor(seed=random.randint(0,1E6))
	else:
		b.shape.color = color
	b.mat = material
	b.state.mass = b.mat.density*b.shape.GetVolume()
	b.state.inertia = b.shape.GetInertia()*b.mat.density
	b.state.ori = b.shape.GetOri()
	b.state.pos = b.shape.GetCentroid()
	b.mask=mask
	if fixed:
		b.state.blockedDOFs = 'xyzXYZ'
	return b

#**********************************************************************************
#creates polyhedra having N vertices and resembling sphere 
def polyhedralBall(radius, N, material, center,mask=1):
	"""creates polyhedra having N vertices and resembling sphere 

	:param float radius: ball radius
	:param int N: number of vertices
	:param Material material: material of new body
	:param Vector3 center: center of the new body
	"""
	pts = []
 
	inc = math.pi * (3. - math.sqrt(5.))
	off = 2. / float(N)
	for k in range(0, N):
		y = k * off - 1. + (off / 2.)
		r = math.sqrt(1. - y*y)
		phi = k * inc
		pts.append([math.cos(phi)*r*radius, y*radius, math.sin(phi)*r*radius])
 	
	ball = polyhedra(material,v=pts)
	ball.state.pos = center
	return ball

#**********************************************************************************
#fill box [mincoord, maxcoord] by non-overlaping polyhedrons with random geometry and sizes within the range (uniformly distributed)
def fillBox(mincoord, maxcoord,material,sizemin=[1,1,1],sizemax=[1,1,1],ratio=[0,0,0],seed=None,mask=1):
	"""fill box [mincoord, maxcoord] by non-overlaping polyhedrons with random geometry and sizes within the range (uniformly distributed)
	:param Vector3 mincoord: first corner
	:param Vector3 maxcoord: second corner
	:param Vector3 sizemin: minimal size of bodies
	:param Vector3 sizemax: maximal size of bodies
	:param Vector3 ratio: scaling ratio
	:param float seed: random seed
	"""
	random.seed(seed);
	v = fillBox_cpp(mincoord, maxcoord, sizemin,sizemax,  ratio, random.randint(0,1E6), material)
	#lastnan = -1
	#for i in range(0,len(v)):
	#	if(math.isnan(v[i][0])):
	#		O.bodies.append(polyhedra(material,seed=random.randint(0,1E6),v=v[lastnan+1:i],mask=1,fixed=False))
	#		lastnan = i
		

