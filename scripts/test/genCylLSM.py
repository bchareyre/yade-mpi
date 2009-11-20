"""
This example shows how LSMGenGeo generates Cylinder 
with given parameters.

Be sure LSMGenGeo library is installed.

Symply start this script:
  python genCylLSM.py

The result is 2 files:
  "cyl.geo" is the geometry file which can be imported into YADE with import_LSMGenGeo_geometry() function
  "cyl.vtk" is the VTK-filed which can be opened by any VTK-based software, for example Paraview

http://www.access.edu.au/lsmgengeo_python_doc/current/pythonapi/html/GenGeo-module.html
https://svn.esscc.uq.edu.au/svn/esys3/lsm/contrib/LSMGenGeo/
"""
from GenGeo import *
import sys

fileName="cyl"
radius=25
length=100

minRadius=0.5
maxRadius=2.0

origin=Vec3(0.0,0.0,0.0)
axis=Vec3(0.0,0.0,1.0)

minPoint=Vec3(-radius,-radius,-length)
maxPoint=Vec3(radius,radius,length)

box=CylinderVol (
  origin = origin,
  axis = axis,
  radius = radius,
  length = 2.0*length
)

mntable=MNTable3D (
  minPoint=minPoint,
  maxPoint=maxPoint,
  gridSize=2.5*maxRadius,
  numGroups=1
)

packer=InsertGenerator3D(
  minRadius=minRadius,
  maxRadius=maxRadius,
  insertFails=1000,
  maxIterations=1000,
  tolerance=1.0e-6,
  seed=0
)

packer.generatePacking (
  volume=box,
  ntable=mntable,
  groupID=0
)

mntable.write(fileName+".geo",1)
mntable.write(fileName+".vtu",2)
