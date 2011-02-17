#!/usr/bin/python
# encoding: utf-8
"""
Miscillaneous functions, which are can be useful for simulation.
"""

from yade.wrapper import *
from miniEigen import *
import utils,math,numpy

#spheresPackDimensions==================================================
def spheresPackDimensions(idSpheres):
	"""The function accepts the list of spheres idSpheres, and calculates max and min dimensions.

	:param list idSpheres: list of spheres
	
	:return: dictionary with keys ``min`` (minimal dimension, Vector3), ``max`` (maximal dimension, Vector3), ``minId`` (minimal dimension sphere Id, Vector3), ``miaxnId`` (maximal dimension sphere Id, Vector3), ``center`` (central point of bounding box, Vector3), ``extends`` (sizes of bounding box, Vector3)
	
	"""
	
	try:
		if (len(idSpheres)<2):
			raise RuntimeError("Only a list of particles with length > 1 can be analyzed")
	except TypeError:
			raise TypeError("There should be list if ints")
		
	min = Vector3.Zero
	max = Vector3.Zero
	
	minId = Vector3.Zero
	maxId = Vector3.Zero
	
	counter = 0
	
		
	for i in idSpheres:
		spherePosition=O.bodies[i].state.pos
		try:
			sphereRadius=O.bodies[i].shape.radius	#skip non-spheres
		except AttributeError: continue
		
		sphereRadiusVec3 = Vector3(sphereRadius,sphereRadius,sphereRadius)
		
		sphereMax = spherePosition + sphereRadiusVec3
		sphereMin = spherePosition - sphereRadiusVec3
		
		for dim in range(0,3):
			if ((sphereMax[dim]>max[dim]) or (counter==0)): 
				max[dim]=sphereMax[dim]
				maxId[dim] = i
			if ((sphereMin[dim]<min[dim]) or (counter==0)): 
				min[dim]=sphereMin[dim]
				minId[dim] = i
		counter += 1
	
	center = (max-min)/2.0+min
	extends = max-min
	
	dimensions = {'max':max,'min':min,'maxId':maxId,'minId':minId,'center':center, 'extends':extends}
	return dimensions
	
