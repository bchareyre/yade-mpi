# encoding: utf-8
#
# utility functions for deformable cohesive elements
#
# burak er buraker88@yandex.com


import math,random,doctest,geom,numpy
from yade import *
from yade.wrapper import *
from yade.utils import *
try: # use psyco if available
	import psyco
	psyco.full()
except ImportError: pass


from minieigen import *

# c++ implementations for performance reasons
from yade._utils import *
from yade.utils import _commonBodySetup

#Deformable Element Node
def node(center,radius,dynamic=None,fixed=True,wire=False,color=None,highlight=False,material=-1,mask=1):
	"""Create sphere with given parameters; mass and inertia computed automatically.

	Last assigned material is used by default (*material* = -1), and utils.defaultMaterial() will be used if no material is defined at all.

	:param Vector3 center: center
	:param float radius: radius
	:param float dynamic: deprecated, see "fixed"
	:param float fixed: generate the body with all DOFs blocked?
	:param material:
		specify :yref:`Body.material`; different types are accepted:
			* int: O.materials[material] will be used; as a special case, if material==-1 and there is no shared materials defined, utils.defaultMaterial() will be assigned to O.materials[0]
			* string: label of an existing material that will be used
			* :yref:`Material` instance: this instance will be used
			* callable: will be called without arguments; returned Material value will be used (Material factory object, if you like)
	:param int mask: :yref:`Body.mask` for the body
	:param wire: display as wire sphere?
	:param highlight: highlight this body in the viewer?
	:param Vector3-or-None: body's color, as normalized RGB; random color will be assigned if ``None``.
	
	:return:
		A Body instance with desired characteristics.


	Creating default shared material if none exists neither is given::

		>>> O.reset()
		>>> from yade import utils
		>>> len(O.materials)
		0
		>>> s0=utils.sphere([2,0,0],1)
		>>> len(O.materials)
		1

	Instance of material can be given::

		>>> s1=utils.sphere([0,0,0],1,wire=False,color=(0,1,0),material=ElastMat(young=30e9,density=2e3))
		>>> s1.shape.wire
		False
		>>> s1.shape.color
		Vector3(0,1,0)
		>>> s1.mat.density
		2000.0

	Material can be given by label::

		>>> O.materials.append(FrictMat(young=10e9,poisson=.11,label='myMaterial'))
		1
		>>> s2=utils.sphere([0,0,2],1,material='myMaterial')
		>>> s2.mat.label
		'myMaterial'
		>>> s2.mat.poisson
		0.11

	Finally, material can be a callable object (taking no arguments), which returns a Material instance.
	Use this if you don't call this function directly (for instance, through yade.pack.randomDensePack), passing
	only 1 *material* parameter, but you don't want material to be shared.

	For instance, randomized material properties can be created like this:

		>>> import random
		>>> def matFactory(): return ElastMat(young=1e10*random.random(),density=1e3+1e3*random.random())
		...
		>>> s3=utils.sphere([0,2,0],1,material=matFactory)
		>>> s4=utils.sphere([1,2,0],1,material=matFactory)

	"""
	b=Body()
	b.shape=Node(radius=radius,color=color if color else randomColor(),wire=wire,highlight=highlight)
	V=(4./3)*math.pi*radius**3
	geomInert=(2./5.)*V*radius**2
	_commonBodySetup(b,V,Vector3(geomInert,geomInert,geomInert),material,pos=center,dynamic=dynamic,fixed=fixed,blockedDOFs='XYZ')
	b.aspherical=False
	b.mask=mask
	b.bounded=True
	return b

def randomColor():
	"""Return random Vector3 with each component in interval 0…1 (uniform distribution)"""
	return Vector3(random.random(),random.random(),random.random())

# Meshing
def clear_mesh(gmshmeshhandle_):
    sceneelements=gmshmeshhandle_[1];

    #first remove elements and nodes from the scene
    for elem in sceneelements:
	bdy=elem[0];
	nodes=elem[1];
	O.bodies.erase(bdy.id);
	for node in nodes:
	    O.bodies.erase(node.id);

def tetrahedronvolume(nodes):

	    
     V =0.166666666*numpy.linalg.det([[nodes[0][0], nodes[1][0] ,nodes[2][0] ,nodes[3][0]],[nodes[0][1], nodes[1][1], nodes[2][1] ,nodes[3][1]], [nodes[0][2] ,nodes[1][2], nodes[2][2], nodes[3][2]], [ 1,1,1,1]]);
	
     return V

def tetrahedral_mesh_generator(filename,tetrahedralelementshape,elementmat,interfaceelementshape,interfacemat):
	

	[nodeList,elementList,interactionList]=mshreader(filename)

	nodes=nodeList
  	
	meshelements=elementList

	interfacepairs=interactionList

	scenebodies=[]

	for elem in elementList:  
	#Create the corresponding mesh element
		nodesofelement=[nodes[elem[0]],nodes[elem[1]],nodes[elem[2]],nodes[elem[3]]]
		noderadius=0.1*(0.75*tetrahedronvolume(nodesofelement))**0.33333333;

		[elbody, nodebodies]=tetrahedral_element(elementmat,nodesofelement,tetrahedralelementshape,radius=noderadius)

		scenebodies.append([elbody,nodebodies])

	#Create the interfacing element pairs
	for interpair in interfacepairs: 
		
		#get element pair
		elementpair=interpair[0];
		#get node pairs
		pairofnodes=interpair[1]
		interfaceelementpairs=[]

		for pair in pairofnodes:

           		firstid=pair[0];
           		secondid=pair[1];
			el1nodebodies=scenebodies[elementpair[0]][1];
			el2nodebodies=scenebodies[elementpair[1]][1];
            		node1=el1nodebodies[firstid]
            		node2=el2nodebodies[secondid]
 	    		interfaceelementpairs.append([node1, node2])

        	[elbody, nodebodies]=interaction_element(interfacemat,interfaceelementpairs,interfaceelementshape);
		scenebodies.append([elbody,nodebodies])

        return scenebodies


#Read mesh file and get elements nodes and interactions
def mshreader(meshfile="file.mesh",shift=Vector3.Zero,scale=1.0,orientation=Quaternion.Identity,**kw):
	""" Imports volume mesh from gmsh2 file
	:Parameters:
		`shift`: [float,float,float]
			[X,Y,Z] parameter moves the specimen.
		`scale`: float
			factor scales the given data.
		`orientation`: quaternion
			orientation of the imported mesh
		`**kw`: (unused keyword arguments)
				is passed to :yref:`yade.utils.facet`
	:Returns: list of nodes and elements for the specimen.
	"""
	infile = open(meshfile,"r")
	lines = infile.readlines()
	infile.close()

	nodelistVector3=[]
	elementList=[]
	interactionList=[]	
	findVerticesString=0
	
	while (lines[findVerticesString].split()[0]<>'$Nodes'): #Find the string with the number of Vertices
		findVerticesString+=1
	findVerticesString+=1
	numNodes = int(lines[findVerticesString].split()[0])
	
	for line in lines[findVerticesString+1:numNodes+findVerticesString+1]:
		data = line.split()
		nodelistVector3.append(orientation*Vector3(float(data[1])*scale,float(data[2])*scale,float(data[3])*scale)+shift)
	
	findElementString=findVerticesString+numNodes
	while (lines[findElementString].split()[0]<>'$Elements'): #Find the string with the number of Elements
		findElementString+=1
	findElementString+=1
	numElements = int(lines[findElementString].split()[0])


	for line in lines[findElementString+1:findElementString+numElements+1]:
		data = line.split()
		numberofnodes=data[1]

		if(int(numberofnodes)==4):#tetrahedral element	

			id1 = int(data[5])-1
			id2 = int(data[6])-1
			id3 = int(data[7])-1
			id4 = int(data[8])-1
		        elementList.append([id1,id2,id3,id4])
	#Create the interaction list
 	i=0
	numVolElements=elementList.__len__()
	for elem in elementList:
		j=i+1

		while j<numVolElements :

			pair=check_coinciding_nodes(elem,elementList[j])
			if(pair.__len__()>0):
				interactionList.append([Vector2i(i,j),pair])
			j=j+1
		i=i+1
	return [nodelistVector3,elementList,interactionList]

def check_coinciding_nodes(element1,element2):

	pairstemp=[]
	#for each node of the element1
	i=0
	j=0
	for node1 in element1:

		for node2 in element2:
						
			if(node1==node2):
				pairstemp.append([i,j])
			j=j+1
		i=i+1
		j=0
	if(3<=pairstemp.__len__()):
		pairs=pairstemp
	else:
		pairs=[]			
	return pairs

def interaction_element(material,nodepairs,elementshape,radius=0.0015,dynamic=None,fixed=True,wire=False,color=Vector3(1,0,0),highlight=False,mask=1):
	
	# triangles for drawing(node indices)
	#faces=[Vector3(0,1,3),Vector3(1,2,3),Vector3(2,0,3),Vector3(0,1,2)]
	faces=[];
	shape=elementshape()
	shape.color=color if color else randomColor();
	[body_,nodes_]=finite_element(material,shape,nodepairs,faces,radius,interface=True)

	return [body_,nodes_]

def tetrahedral_element(material,nodes,elementshape,radius=0.0015,dynamic=None,fixed=True,wire=False,color=Vector3(1,0,0),highlight=False,mask=1):
	
	# triangles for drawing(node indices)
	#faces are in the format [face_vertex0,face_vertex1,face_vertex2,opposite_vertex]
	faces=[Vector3(1,2,0),Vector3(2,1,3),Vector3(2,0,3),Vector3(3,1,0)]

	shape=elementshape()
	shape.color=color if color else randomColor();
	[body_,nodes_]=finite_element(material,shape,nodes,faces,radius)

	return [body_,nodes_]

def finite_element(material,shape,nodes,faces,radius,dynamic=None,fixed=True,wire=False,Color=None,highlight=False,mask=1,interface=False):
	"""Create sphere with given parameters; mass and inertia computed automatically.

	Last assigned material is used by default (*material* = -1), and utils.defaultMaterial() will be used if no material is defined at all.

	:param Vector3 center: center
	:param float radius: radius
	:param float dynamic: deprecated, see "fixed"
	:param float fixed: generate the body with all DOFs blocked?
	:param material:
		specify :yref:`Body.material`; different types are accepted:
			* int: O.materials[material] will be used; as a special case, if material==-1 and there is no shared materials defined, utils.defaultMaterial() will be assigned to O.materials[0]
			* string: label of an existing material that will be used
			* :yref:`Material` instance: this instance will be used
			* callable: will be called without arguments; returned Material value will be used (Material factory object, if you like)
	:param int mask: :yref:`Body.mask` for the body
	:param wire: display as wire sphere?
	:param highlight: highlight this body in the viewer?
	:param Vector3-or-None: body's color, as normalized RGB; random color will be assigned if ``None``.
	
	:return:
		A Body instance with desired characteristics.


	Creating default shared material if none exists neither is given::

		>>> O.reset()
		>>> from yade import utils
		>>> len(O.materials)
		0
		>>> s0=utils.sphere([2,0,0],1)
		>>> len(O.materials)
		1

	Instance of material can be given::

		>>> s1=utils.sphere([0,0,0],1,wire=False,color=(0,1,0),material=ElastMat(young=30e9,density=2e3))
		>>> s1.shape.wire
		False
		>>> s1.shape.color
		Vector3(0,1,0)
		>>> s1.mat.density
		2000.0

	Material can be given by label::

		>>> O.materials.append(FrictMat(young=10e9,poisson=.11,label='myMaterial'))
		1
		>>> s2=utils.sphere([0,0,2],1,material='myMaterial')
		>>> s2.mat.label
		'myMaterial'
		>>> s2.mat.poisson
		0.11

	Finally, material can be a callable object (taking no arguments), which returns a Material instance.
	Use this if you don't call this function directly (for instance, through yade.pack.randomDensePack), passing
	only 1 *material* parameter, but you don't want material to be shared.

	For instance, randomized material properties can be created like this:

		>>> import random
		>>> def matFactory(): return ElastMat(young=1e10*random.random(),density=1e3+1e3*random.random())
		...
		>>> s3=utils.sphere([0,2,0],1,material=matFactory)
		>>> s4=utils.sphere([1,2,0],1,material=matFactory)

	"""
	b=Body();
	b.shape=shape;
	O.bodies.append(b)
	nodes_ret=[];
	#b.shape.color=Color;
	#add nodes to the scene and keep them in the nodes list
	if(interface==False):
	#Deformable Element
		for vec in nodes:        # Second Example
        	    nod=node(vec,radius)
		    O.bodies.append(nod)
		    b.shape.addNode(nod)
		    nodes_ret.append(nod)
		#add faces for drawing purposes
		for face in faces:
		    b.shape.addFace(face)
	else:
	#Deformable Cohesive Element
		for nodepair in nodes:        # Second Example
		    b.shape.addPair(nodepair[0],nodepair[1])
		#add faces for drawing purposes
		for face in faces:
		    b.shape.addFace(face)

	V=1;#For symbolical
	geomInert=Vector3(1,1,1)#For symbolical
	#Only add body to draw it not for integration, therefore block all dof
	_commonBodySetup(b,V,geomInert,material,pos=Vector3(0,0,0),dynamic=False,fixed=True,blockedDOFs='xyzXYZ')
	b.aspherical=False
	b.mask=mask
	b.bounded=False
	return [b,nodes_ret]





##**********************End of Deformable Elements*********************************

