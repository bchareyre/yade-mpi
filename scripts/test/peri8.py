a=1.; r=a/4.
O.bodies.append(
	[utils.sphere(c,r) for c in [(r,r,r),(3*r,r,r),(3*r,3*r,r),(r,3*r,r),(r,r,3*r),(3*r,r,3*r),(3*r,3*r,3*r),(r,3*r,3*r)]]
)
O.periodic=True
O.cell.setRefSize((a,a,a))
zRot=-pi/4.
O.cell.setTrsf(Matrix3(cos(zRot),-sin(zRot),0,sin(zRot),cos(zRot),0,0,0,1))
p7=O.bodies[7].state.pos
