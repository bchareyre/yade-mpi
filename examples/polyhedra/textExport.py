from yade import polyhedra_utils,export
polyhedra_utils.fillBox((0,0,0), (0.3,0.3,0.3),defaultMaterial(),sizemin=(0.025,0.025,0.025),sizemax=(0.05,0.05,0.05),seed=4)
export.textPolyhedra('/tmp/textPolyhedra.txt')
