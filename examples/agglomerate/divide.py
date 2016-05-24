######################################################################
# A script for subdivision of macro perticles into a dense packing of
# smaller particles
#
# Each aggragate is a dense packing, but macroscopically the packing
# is loose
######################################################################
from yade import export,ymport
import random
random.seed(1) # to make colors always the same

# load macroparticles
sp = ymport.text('/tmp/cloud.txt')
colors = [randomColor() for s in sp]
# each macroparticle is filled randomDensePack
for si,s in enumerate(sp):
	sphere = pack.inSphere(s.state.pos, s.shape.radius)
	sp1 = pack.randomDensePack(
		sphere,
		spheresInCell = 500,
		radius = .2,
		memoizeDb = '/tmp/agglomeratepackaux.db',
		returnSpherePack = True,
	)
	ids = sp1.toSimulation(color=colors[si]) # add the result to simulation with uniform color
	for i in ids:
		O.bodies[i].agglomerate = si # tell each particle who is its agglomerate

# save the result, including information of agglomerates which the particle belongs to
export.textExt('/tmp/divided.txt','x_y_z_r_attrs',attrs=['b.agglomerate'])

try:
	from yade import qt
	qt.View()
except:
	pass
