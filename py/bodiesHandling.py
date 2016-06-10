# encoding: utf-8
"""
Miscellaneous functions, which are useful for handling bodies.
"""

from yade.wrapper import *
import utils,math,numpy

from minieigen import *

#spheresPackDimensions==================================================
def spheresPackDimensions(idSpheres=[],mask=-1):
	"""The function accepts the list of spheres id's or list of bodies and calculates max and min dimensions, geometrical center.

	:param list idSpheres: list of spheres
	:param int mask: :yref:`Body.mask` for the checked bodies
	
	:return: dictionary with keys ``min`` (minimal dimension, Vector3), ``max`` (maximal dimension, Vector3), ``minId`` (minimal dimension sphere Id, Vector3), ``maxId`` (maximal dimension sphere Id, Vector3), ``center`` (central point of bounding box, Vector3), ``extends`` (sizes of bounding box, Vector3), ``volume`` (volume of spheres, Real), ``mass`` (mass of spheres, Real), ``number`` (number of spheres, int), 
	
	"""
	idSpheresIter=[]
	
	if (len(idSpheres)<1):
		#check mask
		ifSpherMask=[]
		if (mask>-1):   #The case, when only the mask was given, without list of ids
			for i in O.bodies:
				if ((i.mask&mask)<>0):
					ifSpherMask.append(i.id)
			if (len(ifSpherMask)<2):
				raise RuntimeWarning("Not enough bodies to analyze with given mask")
			else:
				idSpheresIter=ifSpherMask
		else:
			raise RuntimeWarning("Only a list of particles with length > 1 can be analyzed")
	else:
		idSpheresIter=idSpheres
	
	
	minVal = Vector3.Zero
	maxVal = Vector3.Zero
	
	minId = Vector3.Zero
	maxId = Vector3.Zero
	
	counter = 0
	volume = 0.0
	mass = 0.0
	
		
	for i in idSpheresIter:
		if (type(i).__name__=='int'):
			b = O.bodies[i]			#We have received a list of ID's
		elif (type(i).__name__=='Body'):
			b = i								#We have recevied a list of bodies
		else:
			raise TypeError("Unknow type of data, should be list of int's or bodies's")
		
		if (b):
			spherePosition=b.state.pos	#skip non-existent spheres
			
			try:
				sphereRadius=b.shape.radius	#skip non-spheres
			except AttributeError: continue
			
			if (mask>-1) and ((mask&b.mask)==0): continue			#skip bodies with wrong mask
			
			
			sphereRadiusVec3 = Vector3(sphereRadius,sphereRadius,sphereRadius)
			
			sphereMax = spherePosition + sphereRadiusVec3
			sphereMin = spherePosition - sphereRadiusVec3
			
			for dim in range(0,3):
				if ((sphereMax[dim]>maxVal[dim]) or (counter==0)): 
					maxVal[dim]=sphereMax[dim]
					maxId[dim] = b.id
				if ((sphereMin[dim]<minVal[dim]) or (counter==0)): 
					minVal[dim]=sphereMin[dim]
					minId[dim] = b.id
			volume += 4.0/3.0*math.pi*sphereRadius*sphereRadius*sphereRadius
			mass += b.state.mass
			counter += 1
	
	center = (maxVal-minVal)/2.0+minVal
	extends = maxVal-minVal
	
	dimensions = {'max':maxVal,'min':minVal,'maxId':maxId,'minId':minId,'center':center,
		'extends':extends, 'volume':volume, 'mass':mass, 'number':counter}
	return dimensions

#facetsDimensions==================================================
def facetsDimensions(idFacets=[],mask=-1):
	"""The function accepts the list of facet id's or list of facets and calculates max and min dimensions, geometrical center.

	:param list idFacets: list of spheres
	:param int mask: :yref:`Body.mask` for the checked bodies
	
	:return: dictionary with keys ``min`` (minimal dimension, Vector3), ``max`` (maximal dimension, Vector3), ``minId`` (minimal dimension facet Id, Vector3), ``maxId`` (maximal dimension facet Id, Vector3), ``center`` (central point of bounding box, Vector3), ``extends`` (sizes of bounding box, Vector3), ``number`` (number of facets, int), 
	
	"""
	idFacetsIter=[]
	
	if (len(idFacets)<1):
		#check mask
		ifFacetMask=[]
		if (mask>-1):   #The case, when only the mask was given, without list of ids
			for i in O.bodies:
				if ((i.mask&mask)<>0):
					ifFacetMask.append(i.id)
			if (len(ifFacetMask)<2):
				raise RuntimeWarning("Not enough bodies to analyze with given mask")
			else:
				idFacetsIter=ifFacetMask
		else:
			raise RuntimeWarning("Only a list of particles with length > 1 can be analyzed")
	else:
		idFacetsIter=idFacets
	
	
	minVal = Vector3.Zero
	maxVal = Vector3.Zero
	
	minId = Vector3.Zero
	maxId = Vector3.Zero
	
	counter = 0
	
		
	for i in idFacetsIter:
		if (type(i).__name__=='int'):
			b = O.bodies[i]			#We have received a list of ID's
		elif (type(i).__name__=='Body'):
			b = i								#We have recevied a list of bodies
		else:
			raise TypeError("Unknow type of data, should be list of int's or bodies's")
		
		if (b):
			p = b.state.pos
			o = b.state.ori
			s = b.shape
			pt1 = p + o*s.vertices[0]
			pt2 = p + o*s.vertices[1]
			pt3 = p + o*s.vertices[2]
			
			if (mask>-1) and ((mask&b.mask)==0): continue			#skip bodies with wrong mask
			
			facetMax = Vector3(max(pt1[0], pt2[0], pt3[0]), max(pt1[1], pt2[1], pt3[1]), max(pt1[2], pt2[2], pt3[2]))
			facetMin = Vector3(min(pt1[0], pt2[0], pt3[0]), min(pt1[1], pt2[1], pt3[1]), min(pt1[2], pt2[2], pt3[2]))
			
			for dim in range(0,3):
				if ((facetMax[dim]>maxVal[dim]) or (counter==0)): 
					maxVal[dim]=facetMax[dim]
					maxId[dim] = b.id
				if ((facetMin[dim]<minVal[dim]) or (counter==0)): 
					minVal[dim]=facetMin[dim]
					minId[dim] = b.id
			counter += 1
	
	center = (maxVal-minVal)/2.0+minVal
	extends = maxVal-minVal
	
	dimensions = {'max':maxVal,'min':minVal,'maxId':maxId,'minId':minId,'center':center,
		'extends':extends, 'number':counter}
	return dimensions

#spheresPackDimensions==================================================
def spheresModify(idSpheres=[],mask=-1,shift=Vector3.Zero,scale=1.0,orientation=Quaternion((0,1,0),0.0),copy=False):
	"""The function accepts the list of spheres id's or list of bodies and modifies them: rotating, scaling, shifting.
	if copy=True copies bodies and modifies them.
	Also the mask can be given. If idSpheres not empty, the function affects only bodies, where the mask passes.
	If idSpheres is empty, the function search for bodies, where the mask passes.
	
	:param Vector3 shift: Vector3(X,Y,Z) parameter moves spheres.
	:param float scale: factor scales given spheres.
	:param Quaternion orientation: orientation of spheres
	:param int mask: :yref:`Body.mask` for the checked bodies
	:returns: list of bodies if copy=True, and Boolean value if copy=False
	"""
	
	idSpheresIter=[]
	
	if (len(idSpheres)==0):
		#check mask
		ifSpherMask=[]
		if (mask>-1):   #The case, when only the mask was given, without list of ids
			for i in O.bodies:
				if ((i.mask&mask)<>0):
					ifSpherMask.append(i.id)
			if (len(ifSpherMask)==0):
				raise RuntimeWarning("No bodies to modify with given mask")
			else:
				idSpheresIter=ifSpherMask
		else:
			raise RuntimeWarning("No bodies to modify")
	else:
		idSpheresIter=idSpheres
	
	dims = spheresPackDimensions(idSpheresIter)
	
	ret=[]
	for i in idSpheresIter:
		if (type(i).__name__=='int'):
			b = O.bodies[i]			#We have received a list of ID's
		elif (type(i).__name__=='Body'):
			b = i								#We have recevied a list of bodies
		else:
			raise TypeError("Unknown type of data, should be list of int's or bodies")
		
		try:
			sphereRadius=b.shape.radius	#skip non-spheres
		except AttributeError: continue
		
		if (mask>-1) and ((mask&b.mask)==0): continue			#skip bodies with wrong mask
		
		if (copy): b=sphereDuplicate(b)
		
		b.state.pos=orientation*(b.state.pos-dims['center'])+dims['center']
		b.shape.radius*=scale
		b.state.mass*=pow(scale, 3)
		b.state.inertia*=pow(scale, 5)
		b.state.pos=(b.state.pos-dims['center'])*scale + dims['center']
		
		b.state.pos+=shift
		
		if (copy): ret.append(b)
		
	if (copy): 
		return ret
	else:
		return True

#spheresDublicate=======================================================
def sphereDuplicate(idSphere):
	"""The functions makes a copy of sphere"""
	
	i=idSphere
	if (type(i).__name__=='int'):
		b = O.bodies[i]			#We have received a list of ID's
	elif (type(i).__name__=='Body'):
		b = i								#We have recevied a list of bodies
	else:
		raise TypeError("Unknown type of data, should be list of int's or bodies")
	
	try:
		sphereRadius=b.shape.radius	#skip non-spheres
	except AttributeError: 
		return False
	
	addedBody = utils.sphere(center=b.state.pos,radius=b.shape.radius,fixed=not(b.dynamic),wire=b.shape.wire,color=b.shape.color,highlight=b.shape.highlight,material=b.material,mask=b.mask)
	
	return addedBody
	

