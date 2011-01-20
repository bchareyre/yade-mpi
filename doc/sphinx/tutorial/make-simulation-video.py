# encoding: utf-8

# use another simulation script, create 3d output and graphs
#
# the simulation must not run by itself (no O.run() at the end of the script)
# when run, must stop by itself (without exiting)
#
# plotting should be set up as usual

import matplotlib
matplotlib.use('Agg') # headless backend, makes rendering plots after the simulation _much_ faster (and more reliable)

from yade import qt,plot

simulations=[
	('01-bouncing-sphere.py',u'Bouncing sphere',100000),
	('02-gravity-deposition.py',u'Gravity deposition',200),
	('03-oedometric-test.py',u'Oedometric test',500),
	('04-periodic-simple-shear.py',u'Simple shear\nwith\nperiodic boundary',100),
	('06-periodic-triaxial-test.py',u'Periodic triaxal test\nwith clumps',200)
]

#for script,title in simulations:
def do(n):
	global script,title
	script,title,freq=simulations[n]
	O.reset(); plot.reset()
	execfile(script,globals())
	# add output engines
	O.engines=O.engines+[qt.SnapshotEngine(fileBase=O.tmpFilename(),iterPeriod=freq,plot='snapshot')]
	# add the snapshot subplot
	plot.plots.update({'snapshot':None}) 
	# open the view so that it can be set up
	qt.View()

def done():
	utils.makeVideo(plot.savePlotSequence(O.tmpFilename(),title=title+u'\n\n\nVáclav Šmilauer\neu@doxos.eu\nhttp://www.yade-dem.org'),script+'.mpeg',fps=10,kbps=5000)
