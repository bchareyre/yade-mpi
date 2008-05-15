# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
"""
Module containing utility functions for plotting inside yade.

Experimental, interface may change (even drastically).

"""
import matplotlib
# matplotlib.use('GtkCairo')
matplotlib.rc('axes',grid=True) # put grid in all figures
import pylab



data={} # global, common for all plots: {'name':[value,...],...}
plots={} # dictionary x-name -> (yspec,...), where yspec is either y-name or (y-name,'line-specification')
plotLines={} # dictionary x-name -> Line2d objects (that hopefully still correspond to yspec in plots)
needsFullReplot=True

# we could have a yplot class, that would hold: (yspec,...), (Line2d,Line2d,...) ?


plotDataGetterEngine=None
import yade.wrapper
o=yade.wrapper.Omega()

maxDataLen=1000

def reduceData(l):
	"""If we have too much data, take every second value and double the step for DateGetterEngine. This will keep the samples equidistant.
	"""
	if l>maxDataLen:
		print "Reducing: %d>%d"%(l,maxDataLen)
		for d in data: data[d]=data[d][::2]
		global plotDataGetterEngine
		if not plotDataGetterEngine:
			plotDataGetterEngine=[e for e in o.engines if e.name=='PlotDataGetter'][0]
		if plotDataGetterEngine:
			plotDataGetterEngine['iterInterval']=2*plotDataGetterEngine['iterInterval']
			plotDataGetterEngine['timeInterval']=2*plotDataGetterEngine['timeInterval']
		else: raise RuntimeError("Interval for getting data couldn't be adjusted. (yade.plot.plotDataGetterEngine should be proxy to DataGetterEngine in yade, but is None. ")


def addData(d):
	"""Add data from argument {'name':value,...} to yade.plot.data.

	New data will be left-padded with nan's, unspecified data will be nan.
	This way, equal length of all data is assured so that they can be plotted one against any other.

	Nan's don't appear in graphs and are skipped. """
	if len(data)>0: numSamples=len(data[data.keys()[0]])
	else: numSamples=0
	reduceData(numSamples)
	nan=float('nan')
	for name in d:
		if not name in data.keys():
			data[name]=[nan for i in range(numSamples)]
	for name in data:
		if name in d: data[name].append(d[name])
		else: data[name].append(nan)

def fillNonSequence(o):
	if o.__class__==tuple().__class__ or o.__class__==list().__class__: return o
	else: return (o,'')

#def updatePlots():
#	fignum=0
#	for p in plots:
#		pylab.figure(fignum)
#		plots_p=[fillNonSequence(o) for o in plots[p]]
#		for d in plots_p:
#			pylab.axi
#		fignum+=1

import threading,gtk

def killPlots():
	pylab.clf() # clear figures
	needsFullReplot=True
	for t in [t for t in threading.enumerate() if t.getName()=='Thread-plots']:
		print "GTK quit"
		gtk.main_quit()

def fullPlot(): makePlot(update=False)
def updatePlot():
	raise RuntimeError("Updating plot not supported in non-interactive mode!")
	if needsFullReplot:
		print "updatePlot called, doing FULL replot"
		makePlot(update=False)
	else:
		print "updatePlot called, updating plot"
		makePlot(update=True)

def minMax(l): return [min(l),max(l)]

def makePlot(update=False):
	fignum=0 # figure counter
	if not update:
		killPlots() # quit gkt main loop so that GUi is not blocked
		pylab.clf() # clear figures, if any
		global needsFullReplot
		needsFullReplot=False
		print "FULL plot"
	for p in plots:
		print p,fignum,plots[p]
		pylab.figure(fignum)
		pylab.ioff() # turn off interactive mode
		plots_p=[fillNonSequence(o) for o in plots[p]]
		if update:
			for i in range(len(plots_p)): plotLines[p][i].set_data(data[p],data[plots_p[i][0]])
			pylab.axis(minMax(data[p])+minMax(sum([data[d[0]] for d in plots_p],[])))
		else:
			plotLines[p]=pylab.plot(*sum([[data[p],data[d[0]],d[1]] for d in plots_p],[]))
		pylab.legend([_p[0] for _p in plots_p])
		pylab.xlabel(p)
		#pylab.draw()
		fignum+=1
	if not update:
		pylab.show()
		#threading.Thread(target=pylab.show,name='Thread-plots').start() # works with GTK, cool! (will it work inside yade, though?!)
def show():
	fullPlot()
	
import random
if __name__ == "__main__":
	for i in range(10):
		addData({'a':random.random(),'b':random.random(),'t':i*.001,'i':i})
	print data
	for i in range(15):
		addData({'a':random.random(),'c':random.random(),'d':random.random(),'one':1,'t':(i+10)*.001,'i':i+10})
	print data
	# all lists must have the same length
	l=set([len(data[n]) for n in data])
	print l
	assert(len(l)==1)
	plots={'t':('a',('b','g^'),'d'),'i':('a',('one','g^'))}
	fullPlot()
	print "PLOT DONE!"
	fullPlot()
	plots['t']=('a',('b','r^','d'))
	print "FULL PLOT DONE!"
	for i in range(20):
		addData({'d':.1,'a':.5,'c':.6,'c':random.random(),'t':(i+25)*0.001,'i':i+25})
	updatePlot()
	print "UPDATED!"
	print data['d']
	import time
	#time.sleep(60)
	killPlots()
	#pylab.clf()


