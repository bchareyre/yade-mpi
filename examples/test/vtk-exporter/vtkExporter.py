from yade import export,polyhedra_utils
mat = PolyhedraMat()

O.bodies.append([
	sphere((0,0,0),1),
	sphere((0,3,0),1),
	sphere((0,2,4),2),
	sphere((0,5,2),1.5),
	facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(5,4,0)]),
	facet([Vector3(0,-3,-1),Vector3(0,-2,5),Vector3(-5,4,0)]),
	polyhedra_utils.polyhedra(mat,(1,2,3),0),
	polyhedra_utils.polyhedralBall(2,20,mat,(-2,-2,4)),
])
O.bodies[-1].state.pos = (-2,-2,-2)
O.bodies[-1].state.ori = Quaternion((1,1,2),1)
O.bodies[-2].state.pos = (-2,-2,3)
O.bodies[-2].state.ori = Quaternion((1,2,0),1)

createInteraction(0,1)
createInteraction(0,2)
createInteraction(0,3)
createInteraction(1,2)
createInteraction(1,3)
createInteraction(2,3)

O.step()

vtkExporter = export.VTKExporter('vtkExporterTesting')
vtkExporter.exportSpheres(what=[('dist','b.state.pos.norm()')])
vtkExporter.exportFacets(what=[('pos','b.state.pos')])
vtkExporter.exportInteractions(what=[('kn','i.phys.kn')])
vtkExporter.exportPolyhedra(what=[('n','b.id')])
