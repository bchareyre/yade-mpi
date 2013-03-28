from yade import *
from yade import export

O.bodies.append([
	sphere((0,0,0),1),
	sphere((0,2,0),1),
	sphere((0,2,3),2),
	facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(5,4,0)]),
	facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(-5,4,0)])
])

vtkExporter = export.VTKExporter('vtkExporterTesting')
vtkExporter.exportSpheres(what=[('dist','b.state.pos.norm()')])
vtkExporter.exportFacets(what=[('pos','b.state.pos')])
