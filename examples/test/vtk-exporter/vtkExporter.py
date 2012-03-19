from yade import *
from yade import utils,export

O.bodies.append([
	utils.sphere((0,0,0),1),
	utils.sphere((0,2,0),1),
	utils.sphere((0,2,3),2),
	utils.facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(5,4,0)]),
	utils.facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(-5,4,0)])
])

vtkExporter = export.VTKExporter('vtkExporterTesting')
vtkExporter.exportSpheres(what=[('dist','b.state.pos.norm()')])
vtkExporter.exportFacets(what=[('pos','b.state.pos')])
