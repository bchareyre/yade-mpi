# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
"""
Module containing utility functions for plotting inside yade.

Experimental, interface may change (even drastically).

"""
import matplotlib
matplotlib.use('TkAgg')
#matplotlib.use('GTKCairo')
#matplotlib.use('QtAgg')
matplotlib.rc('axes',grid=True) # put grid in all figures
import pylab



data={} # global, common for all plots: {'name':[value,...],...}
plots={} # dictionary x-name -> (yspec,...), where yspec is either y-name or (y-name,'line-specification')
plotLines={} # dictionary x-name -> Line2d objects (that hopefully still correspond to yspec in plots)
needsFullReplot=True

# we could have a yplot class, that would hold: (yspec,...), (Line2d,Line2d,...) ?


plotDataCollector=None
import yade.wrapper
o=yade.wrapper.Omega()

maxDataLen=1000

def reduceData(l):
	"""If we have too much data, take every second value and double the step for DateGetterEngine. This will keep the samples equidistant.
	"""
	if l>maxDataLen:
		global plotDataCollector
		if not plotDataCollector: plotDataCollector=o.labeledEngine('plotDataCollector') # will raise RuntimeError if not found
		if plotDataCollector['mayDouble']: # may we double the period without getting over limits?
			print "Reducing data: %d > %d"%(l,maxDataLen)
			for d in data: data[d]=data[d][::2]
			for attr in ['virtTimeLim','realTimeLim','iterLim']:
				val=plotDataCollector[attr]
				plotDataCollector[attr]=[val[0],val[1]*2,val[2]]


def addData(d):
	"""Add data from argument {'name':value,...} to yade.plot.data.

	New data will be left-padded with nan's, unspecified data will be nan.
	This way, equal length of all data is assured so that they can be plotted one against any other.

	Nan's don't appear in graphs."""
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

def show(): plot()
def plot():
	pylab.ion() ## # no interactive mode (hmmm, I don't know why actually...)
	for p in plots:
		pylab.figure()
		plots_p=[fillNonSequence(o) for o in plots[p]]
		pylab.plot(*sum([[data[p],data[d[0]],d[1]] for d in plots_p],[]))
		pylab.legend([_p[0] for _p in plots_p])
		pylab.xlabel(p)
	pylab.show()

	
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


