from yade import ymport,export

f,n,e = ymport.unv('shell.unv',returnElementMap=True)
O.bodies.append(f)
vtk = export.VTKExporter('test')
vtk.exportFacetsAsMesh(elements=e)
