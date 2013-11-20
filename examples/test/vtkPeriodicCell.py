######################################################################
# Simple script to test VTK export of periodic cell
######################################################################
# enable periodic cell
O.periodic=True
# insert some bodies
sp = randomPeriPack(radius=1,initSize=(10,20,30),memoizeDb='/tmp/vtkPeriodicCell.sqlite')
sp.toSimulation()
# transform the cell a bit
O.cell.hSize *= Matrix3(1,.1,.1, .1,1,0, .1,0,1)  # skew the cell in xy and xz plane
O.cell.hSize *= Matrix3(1,0,0, 0,.8,.6, 0,-.6,.8) # rotate it along x axis

O.step()

# test of export.VTKExporter
from yade import export
vtk1 = export.VTKExporter('/tmp/vtkPeriodicCell-VTKExporter')
vtk1.exportSpheres()
vtk1.exportPeriodicCell()

# test of VTKReorder
vtk2 = VTKRecorder(fileName='/tmp/vtkPeriodicCell-VTKRecorder-',recorders=['spheres','pericell'])
vtk2() # do the export
