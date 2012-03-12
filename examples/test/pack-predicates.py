from yade.pack import *
s1=inSphere((0,0,0),1)
s2=inSphere((0,0,1),1)
# construct using explicit composition
s12=PredicateUnion(s1,s2)
print s12.aabb(), s12((0,0,.5),0)
# constuct the same predicate with operators
print (s1|s2).aabb(), (s1|s2)((0,0,.5),0)
# predicate difference: points in s1 and not in s2
print (s1-s2).aabb()
print (s1-s2)((0,0,1.5),0)
print (s1-s2)((0,0,-0.5),0)
