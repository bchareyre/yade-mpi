
import sys

for i in range(4):
	for j in range(2):
		O.bodies.append(sphere((i+j/3.,j,0),0.5))
for b in O.bodies: b.shape.color = (0.8,0.6,0.8)
		
sub1=[0,1,2,3]
for k in sub1:
	O.bodies[k].subdomain=1
	O.bodies[k].shape.color = (0.2,0.3,0.8)
domainBody1=Body(shape=Subdomain(ids=[b.id for b in O.bodies if b.subdomain==1 and isinstance(b.shape,Sphere)],subDomainIndex=1),subdomain=-1) #note: not clear yet how shape.subDomainIndex and body.subdomain should interact
s=domainBody1.shape

subIdx1=O.bodies.append(domainBody1)
#O.bodies[subIdx1].shape.subDomainIndex=subIdx1 #points back to position in O.bodies?

#tell the collider how to handle this new thing
collider.boundDispatcher.functors=collider.boundDispatcher.functors+[Bo1_Subdomain_Aabb()]
#collider.__call__()
collider.boundDispatcher.__call__()
#this one will print a warning since this thread (scene.subdomain=0) is not supposed to have knowledge of bodies in subdomain 1
s.setMinMax() #should be done in another thread
for id in sub1:
	O.bodies[id].bounded=False
	O.bodies[id].bound=None
collider.doSort=True
collider.boundDispatcher.__call__()
collider.__call__() #see [1]
print domainBody1.bound.min, domainBody1.bound.max

#Get the index of bodies outside domain1 interacting with domain1 
#(here it will also show bodies form inside, in practice they should have been deleted
s.intersections=[[i.id1 if i.id2==domainBody1.id else i.id2 for i in O.interactions.withBodyAll(domainBody1.id) ]]
print "Bodies in 0 interacting with 1:",s.intersections[0]

#print b.shape.intersections

#ISSUE1:
    #comm.send(yade.Scene(), dest=1, tag=12) 
  #File "MPI/Comm.pyx", line 1130, in mpi4py.MPI.Comm.send (src/mpi4py.MPI.c:94609)
  #File "MPI/msgpickle.pxi", line 187, in mpi4py.MPI.PyMPI_send (src/mpi4py.MPI.c:35845)
  #File "MPI/msgpickle.pxi", line 88, in mpi4py.MPI._p_Pickle.dump (src/mpi4py.MPI.c:34491)
#TypeError: No to_python (by-value) converter found for C++ type: std::__cxx11::list<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >


##ISSUE2 / OPTIMIZATION:
#We could keep all bodies present in all scenes as soon as Newton will skip other subdomains.
#To skip interactions outside the current subdomain it would be enough to make bodies from other domains unbounded.
#The problem is the collider still sort bounds for unbounded bodies (using positions), which will generate a massive excess of bounds



#OPTIMIZATIONS
'''
- [1] no real need to run collider action ('collider.__call__()') twice to update subdomains bounds. We could dispatch+setMinMax+sort.
Alternatively, setMinMax could be optimized by taking first and last bounds after sort. In such case sort+setMinMax+sort would be needed. Note that the second __call__() should be faster since it uses pre-sorted bounds.
'''
