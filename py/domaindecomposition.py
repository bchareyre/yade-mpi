# encoding: utf-8
'''
This module provides utilities for 3D domain decomposition. 
The modulue is istantiated with 

globalDomain = Globaldomain(mn,mx,numThreads) # automatically decompose the domain in all directions for desired number of threads

globalDomain = Globaldomain(mn,mx,xDecomp=1,yDecomp=1,zDecomp=numThreads) # manually decompose the domain in z direction for given number of threads

User can then find which subdomain a point = Point(x,y,z) is located within by running:
subdomain = var.findSubdomain(point)
'''

import sys
import numpy as np
import random as rand
from yade import *
from yade.wrapper import *

class SubdomainPoint(object):
	'''
	Class used by GlobaldomainCloud() to organize its subdomain points
	'''
	def __init__(self, point,worker):
		self.point = point
		self.worker= worker


class GlobaldomainCloud(object):
#		'''
#		Object creates a random cloud point based globaldomain with accessible subdomains
#		By default, the domain is automatically decomposed based on finding the furthest nearest neighbor in a group of random sample points
#		User can pass realiziations to increase the random point sample size
#		'''
	def __init__(self,lowerBound,upperBound,numThreads=1,realizations=int(1e3)):
		self.mx = upperBound
		self.mn = lowerBound	
		self.realizations=realizations
		self.numThreads = numThreads
		self.domainCornerPoints = []
		self.makeDomainCornerPoints()
		self.subdomainPoints = []
		self.generateRandomCloud()
				
	## generate random points and select the most remote one to keep. Do this for num threads
	def generateRandomCloud(self):
		for i in range(self.numThreads):
			for j in range(self.realizations):
				pointSample = []
				x = rand.uniform(self.mn[0],self.mx[0])
				y = rand.uniform(self.mn[1],self.mx[1])
				z = rand.uniform(self.mn[2],self.mx[2])
				pointSample.append(Point(x,y,z))
			point = self.findMostRemotePointNN(pointSample)
			self.subdomainPoints.append(SubdomainPoint(point,i+1))

	# most remot point by summed distance to existing domain points. Unactive
	def findMostRemotePoint(self,points):
		furthestDist=0
		point = None
		for i in points:
			dist = 0
			if self.subdomainPoints:
				for j in self.subdomainPoints:
					dist += np.linalg.norm(i.vec - j.point.vec)
			for k in self.domainCornerPoints:
				dist += np.linalg.norm(i.vec - k.vec)
			if dist > furthestDist: 
				furthestDist=dist
				point = i
		return point

	# most remot point by "furthest nearest neighbor". Active
	def findMostRemotePointNN(self,points):
		furthestNeighborDist = 0
		selectedPoint = None
		for i in points:
			nearestNeighborDist = 1000000
			nearestPoint = None
			if self.subdomainPoints:
				for j in self.subdomainPoints:
					dist = np.linalg.norm(i.vec - j.point.vec)
					if dist < nearestNeighborDist:
						nearestPoint = i
						nearestNeighborDist = dist
			for k in self.domainCornerPoints:
				dist = np.linalg.norm(i.vec - k.vec)
				if dist < nearestNeighborDist:
					nearestPoint = i
					nearestNeighborDist = dist
			
			if nearestNeighborDist > furthestNeighborDist:
				selectedPoint = i 
				furthestNeighborDist = nearestNeighborDist
		
		return selectedPoint


	def findSubdomain(self, point):
		'''
		User controlled. User provides a point = Point(x,y,z) and is returned with the subdomain it belongs to.
		'''
		minDist = 10000000
		homeDomain = 0
		for sd in self.subdomainPoints:
			totalDist = 0
			totalDist += np.linalg.norm(point.vec - sd.point.vec)
			if totalDist < minDist: 
				minDist = totalDist
				homeDomain = sd.worker
		return homeDomain

	def makeDomainCornerPoints(self):
		x = abs(self.mx[0]-self.mn[0])
		y = abs(self.mx[1]-self.mn[1])
		z = abs(self.mx[2]-self.mn[2])
		self.domainCornerPoints.append(Point(self.mn[0],self.mn[1],self.mn[2]))
		self.domainCornerPoints.append(Point(self.mn[0]+x,self.mn[1],self.mn[2]))
		self.domainCornerPoints.append(Point(self.mn[0],self.mn[1]+y,self.mn[2]))
		self.domainCornerPoints.append(Point(self.mn[0]+x,self.mn[1]+y,self.mn[2]))
		self.domainCornerPoints.append(Point(self.mn[0],self.mn[1],self.mn[2]+z))
		self.domainCornerPoints.append(Point(self.mn[0]+x,self.mn[1],self.mn[2]+z))
		self.domainCornerPoints.append(Point(self.mn[0],self.mn[1]+y,self.mn[2]+z))
		self.domainCornerPoints.append(Point(self.mn[0]+x,self.mn[1]+y,self.mn[2]+z))



class Globaldomain(object):
#		'''
#		Object creates a gridded globaldomain with accessible subdomains
#		By default, the domain is automatically decomposed based on balancing subdomain block edge lengths with least squares and the number of workers available
#		User can pass xDecomp,yDecomp,zDecomp arguments to manually control the decomposition
#		Subdomains are split into halves to accommodate random numbers of MPI threads
#		'''
	def __init__(self,lowerBound,upperBound,numThreads=1,xDecomp=0,yDecomp=0,zDecomp=0):
		self.subdomains = []
		self.mx = upperBound
		self.mn = lowerBound
		self.numThreads = numThreads
		self.workerDomain = 0
		self.unusedWorkers = 0
		# if no decomp arguments, automate side decomp:
		if xDecomp==yDecomp==zDecomp==0: self.sideDecomposition()
		else: # convert to grid for partitioning
			self.xDecomp = xDecomp + 1
			self.yDecomp = yDecomp + 1
			self.zDecomp = zDecomp + 1
		self.x_ = np.linspace(self.mn[0],self.mx[0],self.xDecomp)
		self.y_ = np.linspace(self.mn[1],self.mx[1],self.yDecomp)
		self.z_ = np.linspace(self.mn[2],self.mx[2],self.zDecomp)
		self.x,self.y,self.z = np.meshgrid(self.x_,self.y_,self.z_,indexing='ij')
		self.partitionSubdomains()


	def sideDecomposition(self):
		'''
		Not user controlled. This function takes the 3 edge lengths of a rectangular parallelpiped and the number of desired subdomains to return a balanced side decomposition 
		'''
		x = abs(self.mx[0]-self.mn[0])
		y = abs(self.mx[1]-self.mn[1])
		z = abs(self.mx[2]-self.mn[2])
	
		# least squares minimizes residuals between length/decomposition ratios for each of the 3 sides
		G = np.array([[x,y,0],[x,0,z],[0,y,z]])
		GT = np.transpose(G)
		invGTG=np.linalg.inv(np.dot(GT,G))
		sigma = 0.01  # set acceptable error
		b = sigma*np.array([1,1,1])
		m = np.dot(np.dot(invGTG,GT),b) # normal equation
		m = m**(-1)
		self.xDecomp = int(m[0])
		self.yDecomp = int(m[1])
		self.zDecomp = int(m[2])
		
		# hack for a fast simulated annealing constrained by numThreads and integer values: 
		multiplierDown = 0.99
		optimized = False
		while optimized==False:
			estimatedWorkers =  int(self.yDecomp)*int(self.xDecomp)*int(self.zDecomp)
			if estimatedWorkers > self.numThreads: # keep length/decomp ratio while decreasing number of decompositions
				self.xDecomp *= multiplierDown
				self.yDecomp *= multiplierDown
				self.zDecomp *= multiplierDown
			elif estimatedWorkers < self.numThreads: # increase length/decomp ratio for the direction that will bring us closest to numThreads
				remainingWorkers = self.numThreads - estimatedWorkers
				offAxis = np.array([int(self.yDecomp)*int(self.zDecomp),int(self.xDecomp)*int(self.zDecomp),int(self.yDecomp)*int(self.xDecomp)])
				workerDiff = offAxis - remainingWorkers
				if all(i>0 for i in workerDiff): 
					optimized=True
					break
				idx = self.idxOfSmallestPositiveNumber(workerDiff)
				if idx==0:
					self.xDecomp = int(self.xDecomp) + 1  
				if idx==1:
					self.yDecomp = int(self.yDecomp) + 1
				if idx==2:
					self.zDecomp = int(self.zDecomp) + 1
			elif estimatedWorkers == self.numThreads: optimized=True	
		print "workers used:",estimatedWorkers,". workers wanted:",self.numThreads 
		self.unusedWorkers=self.numThreads-estimatedWorkers
		self.numThreads=estimatedWorkers
		print 'xdecomp', int(self.xDecomp),'ydecomp', int(self.yDecomp),'zdecomp',int(self.zDecomp)

		# convert values to grid points for subdomain algorithm
		self.xDecomp = int(self.xDecomp)+1
		self.yDecomp = int(self.yDecomp)+1
		self.zDecomp = int(self.zDecomp)+1

	def idxOfSmallestPositiveNumber(self,array):
		smallestNumber=1000
		idx = 1000
		for i,j in enumerate(array):
			if i<smallestNumber and i>0:
				smallestNumber=j
				idx = i
		return idx

	def partitionSubdomains(self):
		'''
		Not user controlled. Steps through points of a grid to create subdomain objects associated with Globaldomain. 
		'''
		for i in range(self.xDecomp-1):
			for j in range(self.yDecomp-1):
				for k in range(self.zDecomp-1):
					if self.unusedWorkers>0:
						self.makeTwoSubdomains(i,j,k)
						self.unusedWorkers-=1		
					else: self.makeOneSubdomain(i,j,k)

		print "total subdomains", self.workerDomain


	def makeOneSubdomain(self,i,j,k):
		# 8 pts assigned to each subdomain
		point1 = Point(self.x[i,j,k],self.y[i,j,k],self.z[i,j,k])
		point2 = Point(self.x[i+1,j,k],self.y[i+1,j,k],self.z[i+1,j,k])
		point3 = Point(self.x[i,j+1,k],self.y[i,j+1,k],self.z[i,j+1,k])
		point4 = Point(self.x[i+1,j+1,k],self.y[i+1,j+1,k],self.z[i+1,j+1,k])
		point5 = Point(self.x[i,j,k+1],self.y[i,j,k+1],self.z[i,j,k+1])
		point6 = Point(self.x[i+1,j,k+1],self.y[i+1,j,k+1],self.z[i+1,j,k+1])
		point7 = Point(self.x[i,j+1,k+1],self.y[i,j+1,k+1],self.z[i,j+1,k+1])
		point8 = Point(self.x[i+1,j+1,k+1],self.y[i+1,j+1,k+1],self.z[i+1,j+1,k+1])
		self.workerDomain+=1
		subdomain = Subdomain(point1,point2,point3,point4,point5,point6,point7,point8,self.workerDomain)
		self.addSubdomain(subdomain)

	def makeTwoSubdomains(self,i,j,k):
		# create mid points 
		xHalfpt2 = self.x[i,j,k]+(self.x[i+1,j,k] - self.x[i,j,k])/2.
		xHalfpt4 = self.x[i,j+1,k]+(self.x[i+1,j+1,k] - self.x[i,j+1,k])/2.
		xHalfpt6 = self.x[i,j,k+1]+(self.x[i+1,j,k+1] - self.x[i,j,k+1])/2.
		xHalfpt8 = self.x[i,j+1,k+1]+(self.x[i+1,j+1,k+1] - self.x[i,j+1,k+1])/2.

		# first subdomain (half size)
		point1 = Point(self.x[i,j,k],self.y[i,j,k],self.z[i,j,k])
		point2 = Point(xHalfpt2,self.y[i+1,j,k],self.z[i+1,j,k])
		point3 = Point(self.x[i,j+1,k],self.y[i,j+1,k],self.z[i,j+1,k])
		point4 = Point(xHalfpt4,self.y[i+1,j+1,k],self.z[i+1,j+1,k])
		point5 = Point(self.x[i,j,k+1],self.y[i,j,k+1],self.z[i,j,k+1])
		point6 = Point(xHalfpt6,self.y[i+1,j,k+1],self.z[i+1,j,k+1])
		point7 = Point(self.x[i,j+1,k+1],self.y[i,j+1,k+1],self.z[i,j+1,k+1])
		point8 = Point(xHalfpt8,self.y[i+1,j+1,k+1],self.z[i+1,j+1,k+1])
		self.workerDomain+=1
		subdomain = Subdomain(point1,point2,point3,point4,point5,point6,point7,point8,self.workerDomain)
		self.addSubdomain(subdomain)

		# second subdomain (half size)
		point1 = Point(xHalfpt2,self.y[i+1,j,k],self.z[i+1,j,k])
		point2 = Point(self.x[i+1,j,k],self.y[i+1,j,k],self.z[i+1,j,k])
		point3 = Point(xHalfpt4,self.y[i+1,j+1,k],self.z[i+1,j+1,k])
		point4 = Point(self.x[i+1,j+1,k],self.y[i+1,j+1,k],self.z[i+1,j+1,k])
		point5 = Point(xHalfpt6,self.y[i+1,j,k+1],self.z[i+1,j,k+1])
		point6 = Point(self.x[i+1,j,k+1],self.y[i+1,j,k+1],self.z[i+1,j,k+1])
		point7 = Point(xHalfpt8,self.y[i+1,j+1,k+1],self.z[i+1,j+1,k+1])
		point8 = Point(self.x[i+1,j+1,k+1],self.y[i+1,j+1,k+1],self.z[i+1,j+1,k+1])
		self.workerDomain+=1
		subdomain = Subdomain(point1,point2,point3,point4,point5,point6,point7,point8,self.workerDomain)
		self.addSubdomain(subdomain)
		

	def findSubdomain(self, point):
		'''
		User controlled. User provides a point = Point(x,y,z) and is returned with the subdomain it belongs to.
		'''
		minDist = 10000000
		homeDomain = 0
		for sd in self.subdomains:
			totalDist = 0
			for sdpt in sd.points:
				vec = point.vec - sdpt.vec
				totalDist += np.linalg.norm(vec)
			if totalDist < minDist: 
				minDist = totalDist
				homeDomain = sd.worker
		return homeDomain

	def __getitem__(self,i):
		return self.subdomains[i]
	
	def addSubdomain(self,subdomain):
		self.subdomains.append(subdomain) 

class Subdomain(object):
	'''
	Class used by Globaldomain() to organize its subdomains
	'''
	def __init__(self, point1,point2,point3,point4,point5,point6,point7,point8,worker):
		self.points = [0] * 8
		self.points[0]=point1
		self.points[1]=point2
		self.points[2]=point3
		self.points[3]=point4
		self.points[4]=point5
		self.points[5]=point6
		self.points[6]=point7
		self.points[7]=point8
		self.worker = worker

	def __getitem__(self,i):
		return self.points[i]

class Point(object):
	'''
	Class used by Globaldomain() and user to create point = Point(x,y,z) for use within Globaldomain() and Globaldomain.findSubdomain() et al.
	'''
	def __init__(self, x, y, z):
		self.x=x
		self.y=y
		self.z=z
		self.vec = np.array([x,y,z])
