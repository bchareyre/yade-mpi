# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#
# I doubt there functions will be useful for anyone besides me.
#


def oofemTextExport():
	"""Export simulation data in text format 
	
	The format is line-oriented as follows:
		# 3 lines of material parameters:
		1. E G # elastic
		2. epsCrackOnset epsFr expBending xiShear #tension
		3. cohesionT tanPhi # shear
		4. [number of spheres] [number of links]
		5. id x y z r -1/0/1[on negative/no/positive boundary] # spheres
		…
		n. id1 id2 contact_point_x cp_y cp_z A # interactions """
	from yade.wrapper import Omega
	material,bodies,interactions=[],[],[]
	o=Omega()

	ph=o.interactions.nth(0).phys # some params are the same everywhere
	material.append("%g %g"%(ph['E'],ph['G']))
	material.append("%g %g %g %g"%(ph['epsCrackOnset'],ph['epsFracture'],ph['expBending'],ph['xiShear']))
	material.append("%g %g"%(ph['undamagedCohesion'],ph['tanFrictionAngle']))

	# need strainer for getting bodies in positive/negative boundary
	strainers=[e for e in o.engines if e.name=='UniaxialStrainer']
	if len(strainers)>0: strainer=strainers[0]
	else: strainer=None

	for b in o.bodies:
		if strainer and b.id in strainer['negIds']: boundary=-1
		elif strainer and b.id in strainer['posIds']: boundary=1
		else: boundary=0
		bodies.append("%d %g %g %g %g %d"%(b.id,b.phys['se3'][0],b.phys['se3'][1],b.phys['se3'][2],b.shape['radius'],boundary))

	for i in o.interactions:
		if not i.geom or not i.phys: continue
		cp=i.geom['contactPoint']
		interactions.append("%d %d %g %g %g %g"%(i.id1,i.id2,cp[0],cp[1],cp[2],i.phys['crossSection']))

	return material+["%d %d"%(len(bodies),len(interactions))]+bodies+interactions
