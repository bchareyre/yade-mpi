######################################################################
# A script for prepare loose packing of macro particles
######################################################################
from yade import export

dim    = (15,15,15) # dimensions for makeCloud
radius = 1
fuzz   = 0.2

# use of makeCloud function
sp = pack.SpherePack()
sp.makeCloud((0,0,0), dim, rMean=radius, rRelFuzz=fuzz, seed=1)
sp.toSimulation()

# save the result
export.text('/tmp/cloud.txt')

try:
	from yade import qt
	qt.View()
except:
	pass
