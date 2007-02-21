#!/usr/bin/env python
# (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

import _pyade
# convenience for users
from math import *

class Body:
	"""Dummy class that returns a few body parameters as if they were its attributes.

	C++ code in pyade.cpp is called to get the actual values.

	Properties calculable from those already existing may be defined here - see Etrans, Erot, Epot for examples.
	"""
	def __init__(self,id):
		self._id=id
	def __getattr__(self,name):
		#TODO: perhaps try directly, exception will occur anyway
		if name in _pyade.bodyPropDict():
			#print "calling bodyProp with parameters:",self._id,_pyade.bodyPropDict()[name]
			return _pyade.bodyProp(self._id,_pyade.bodyPropDict()[name])
		elif name=='Etrans': return .5*self.m*sum([v**2 for v in self.v])
		elif name=='Erot': return sum([.5*self.I[i]*self.w[i]**2 for i in range(3)])
		#FIXME: gravity hardcoded...
		elif name=='Epot': return -sum([self.x[i]*(0,0,-10)[i] for i in range(3)])*self.m
		else: raise KeyError("Invalid Body property `%s'."%name);

class Bodies:
	"""This class accesses individual bodies. User code uses its instance conveninently named "B".

	@todo implement Bodies.__iter__ so that iteration over all bodies is possible"""
	def __getitem__(self,id): return Body(id)
	def __len__(self): return 1024
	#def __iter__(self): pass
	def __contains__(self,n): return n<self.__len__()

class Simul:
	"""Class representing (current) simulation. User code uses its instance convenintly named "S". """
	def __getattr__(self,name):
		if name in _pyade.simulPropDict():
			return _pyade.simulProp(_pyade.simulPropDict()[name])
		#elif name=='...': return ...
		else: raise KeyError("Invalid Simul property `%s'."%name);

# instances
B=Bodies()
S=Simul()
