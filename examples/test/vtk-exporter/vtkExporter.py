from yade import *
from yade import export

O.bodies.append([
	sphere((0,0,0),1),
	sphere((0,3,0),1),
	sphere((0,2,4),2),
	sphere((0,5,2),1.5),
	facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(5,4,0)]),
	facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(-5,4,0)])
])

createInteraction(0,1)
createInteraction(0,2)
createInteraction(0,3)
createInteraction(1,2)
createInteraction(1,3)
createInteraction(2,3)

vtkExporter = export.VTKExporter('vtkExporterTesting')
vtkExporter.exportSpheres(what=[('dist','b.state.pos.norm()')])
vtkExporter.exportFacets(what=[('pos','b.state.pos')])
vtkExporter.exportInteractions(what=[('kn','i.phys.kn')])
