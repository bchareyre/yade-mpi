################################################################################
#
# TODO
#
################################################################################

O.engines = [
	InsertionSortCollider([Bo1_Tetra_Aabb(),Bo1_Facet_Aabb()]),
]

ug = utils.UnstructuredGrid()
v = {
	1: (0,0,0),
	3: (1,0,0),
	5: (0,1,0),
	6: (0,0,1),
	2: (-1,0,0),
}
ct = {
	1: (1,3,5),
	4: (1,3,6),
	8: (1,5,2,6),
}
ug.setup(v,ct,wire=False)
ug.toSimulation()
O.step()

from yade import qt
qt.View()

def setNewCoords():
	ug.setPositionsOfNodes({
		1: (-1,-1,-1),
		3: (2,0,.5),
		5: (0,3,0),
		6: (0,0,.5),
		2: (-1,0,1)
	})
