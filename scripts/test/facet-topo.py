import yade.log
yade.log.setLevel('FacetTopologyAnalyzer',yade.log.TRACE)

# Note: FacetTopologyAnalyzer is normally run as an initializer;
# it is only for testing sake that it is in O.engines here.
O.engines=[FacetTopologyAnalyzer(projectionAxis=(0,0,1),label='topo'),]

# most simple case: no touch at all
if 1:
	O.bodies.append([
		utils.facet([(0,0,0),(1,0,0),(0,1,0)]),
		utils.facet([(0,0,1),(1,0,1),(0,1,1)]),
	])
	O.step();
	assert(topo['commonEdgesFound']==0)
if 1:
	O.bodies.clear()
	O.bodies.append([
		utils.facet([(0,0,0),(1,0,0),(0,1,0)]),
		utils.facet([(1,1,0),(1,0,0),(0,1,0)]),
	])
	O.step()
	#assert(O.bodies[0].phys['edgeAdjIds'][1]==1 and O.bodies[1].phys['edgeAdjIds'][0]==1)
	assert(topo['commonEdgesFound']==1)
