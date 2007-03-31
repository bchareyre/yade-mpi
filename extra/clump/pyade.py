#!/usr/bin/env python
# (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

from Scientific.Geometry import Vector
from Scientific.Geometry.Quaternion import Quaternion
Vector.__dict__['__str__']=lambda self: "%g %g %g"%(self[0],self[1],self[2])
Quaternion.__dict__['__str__']=lambda self: "%g %g %g %g"%(self[0],self[1],self[2],self[3])

try: 
	import psyco; psyco.full()
except ImportError: pass

if __name__=='__main__': import pyadeDummy as _pyade # testing, without yade running
else:	import _pyade
# convenience for users
from math import *

"""
In order to make vector arithmetics possible, this module imports
Scientific.Geometry.{Quaternion,Vector}. Debian package with these modules is
called `python-scientific'. We overrider Vector's and Quaternion's __str__
method so that they are printed as numbers with spaces (suitable for gnuplot,
for example).

Future things to do (maybe):
 * use serialization attributes to access values really; something like Body.attr(name), with  Body.__getitem__(name) as shorthand.
 * implement history:
 	attribute will have kw t (time) and i (iteration); for non-negative values, they are absolute; for negative, relative.
 	class History:
 * Find out how to make B[5].I equivalent to B[5].I(), so that we can also say B[5].I(t=-1)
		If not possible, use __call__ and mandate B[5].I(). Or use B[5].attr('I',dt=...) since it will be used quite rarely
"""


class Bodies:
	"""This class accesses individual bodies. User code uses its instance conveninently named "B".
	Iteration is supported, so you can say "for b in B: ...". """
	def __getitem__(self,id): return Bodies.Body(id)
	def __len__(self): return S.nBodies
	def __contains__(self,n): return n<self.__len__()
	def __iter__(self):
		return Bodies._iterator()
	class _iterator:
		def __init__(self): self.index=0
		def next(self):
			if self.index>=S.nBodies: raise StopIteration
			self.index+=1
			return B[self.index]

	class Body:
		"""Dummy class that returns a few body parameters as if they were its attributes.
		C++ code in pyade.cpp is called to get the actual values.
		Properties calculable from those already existing may be defined here - see Etrans, Erot, Epot, E for examples."""
		def __init__(self,id):
			self._id=id
		def __getattr__(self,name):
			if   name=='Etrans': return .5*self.m*sum([v**2 for v in self.v])
			elif name=='Erot': return sum([.5*self.I[i]*self.w[i]**2 for i in range(3)])
			#FIXME: gravity hardcoded...
			elif name=='Epot': return -sum([self.x[i]*(0,0,-10)[i] for i in range(3)])*self.m
			elif name=='E': return self.Etrans+self.Erot+self.Epot
			else: return _pyade.bodyProp(self._id,_pyade.bodyPropDict()[name])

class Simul:
	"""Class representing (current) simulation. User code uses its instance convenintly named "S". """
	def __getattr__(self,name):
		return _pyade.simulProp(_pyade.simulPropDict()[name])

class Interactions:
	"Class accessing individual interactions, either persistent or transient (based on constructor parameter)."
	def __len__(self): return [S.nPersistent,S.nTransient][isTransient]
	def __init__(self, _isTransient):
		self.isTransient=_isTransient
	class _iterator:
		def __init__(self,_isTransient): self.index=0
		def next(self):
			if self.index>=[S.nPersistent,S.nTransient][_isTransient]: raise StopIteration
			self.index+=1
			return self.interNo(self.index)
			# tricky; need to call that with parameters; like self.interactionNo(n)->(id1,id2)
			#return B[self.index].
	def interNo(n): return self[_pyade.interNo(n,self.isTransient)]
	def __getitem__(self,id):
		if type(id)==tuple:
			#if len(id)==2: return Interactions.Interaction(id[0],id[1],self.isTransient)
			if len(id)==2: return self.Interaction(id[0],id[1],self.isTransient)
			else: raise ValueError("Interaction must be determined by 2 ids")
		if type(id)==int:
			inter=[Ip,It][self.isTransient]
			return [x for x in inter if x.id1==id or x.id2==id] # th
	class Interaction:
		def __init__(self, id1, id2): self._id1,self._id2,self._isTransient=id1,id2,self.isTransient
		def __getattr__(self,name):
			if name=='foo': return Vector(1,2,3)
			#elif ...
			else: return _pyade.interProp(self._id1,self._id2,_pyade.interPropDict()[name])







# instances
B=Bodies()
S=Simul()

# testing stuff
if __name__=='__main__':
		print B[0].m,B[0].I
		print B[2].x, B[2].r
		print S.nBodies
		for a in [(bb.m,bb.id) for bb in B if bb.id>50 and bb.id%2==0]: print a[0],a[1]
