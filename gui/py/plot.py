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
plotsFilled={} # same as plots but with standalone plot specs filled to tuples (used internally only)
plotLines={} # dictionary x-name -> Line2d objects (that hopefully still correspond to yspec in plots)
needsFullReplot=True

def reset():
	global data, plots, plotsFilled, plotLines, needsFullReplot
	data={}; plots={}; plotsFilled={}; plotLines={}; needsFullReplot=True; 
	pylab.close('all')

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
		if plotDataCollector['mayStretch']: # may we double the period without getting over limits?
			plotDataCollector['stretchFactor']=2. # just to make sure
			print "Reducing data: %d > %d"%(l,maxDataLen)
			for d in data: data[d]=data[d][::2]
			for attr in ['virtPeriod','realPeriod','iterPeriod']:
				if(plotDataCollector[attr]>0): plotDataCollector[attr]=2*plotDataCollector[attr]

def reverseData():
	for k in data: data[k].reverse()

def addData(d):
	"""Add data from argument {'name':value,...} to yade.plot.data.

	New data will be left-padded with nan's, unspecified data will be nan.
	This way, equal length of all data is assured so that they can be plotted one against any other.

	Nan's don't appear in graphs."""
	import numpy
	if len(data)>0: numSamples=len(data[data.keys()[0]])
	else: numSamples=0
	reduceData(numSamples)
	nan=float('nan')
	for name in d:
		if not name in data.keys():
			data[name]=[nan for i in range(numSamples)] #numpy.array([nan for i in range(numSamples)])
	for name in data:
		if name in d: data[name].append(d[name]) #numpy.append(data[name],[d[name]],1)
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
		plotsFilled[p]=plots_p
		plotLines[p]=pylab.plot(*sum([[data[p],data[d[0]],d[1]] for d in plots_p],[]))
		pylab.legend([_p[0] for _p in plots_p])
		pylab.xlabel(p)
	pylab.show()
updatePeriod=0
def periodicUpdate(period):
	import time
	global updatePeriod
	while updatePeriod>0:
		doUpdate()
		time.sleep(updatePeriod)
def startUpdate(period=10):
	global updatePeriod
	updatePeriod=period
	import threading
	threading.Thread(target=periodicUpdate,args=(period,),name='Thread-update').start()
def stopUpdate():
	global updatePeriod
	updatePeriod=0
def doUpdate():
	pylab.close('all')
	plot()


def saveGnuplot(baseName,term='wxt',extension=None,timestamp=False,comment=None,title=None):
	"""baseName: used for creating baseName.gnuplot (command file for gnuplot),
			associated baseName.data (data) and output files (if applicable) in the form baseName.[plot number].extension
		term: specify the gnuplot terminal;
			defaults to x11, in which case gnuplot will draw persistent windows to screen and terminate
			other useful terminals are 'png', 'cairopdf' and so on
		extension: defaults to terminal name
			fine for png for example; if you use 'cairopdf', you should also say extension='pdf' however
		timestamp: append numeric time to the basename
		comment: a user comment (may be multiline) that will be embedded in the control file
	"""
	import time,bz2
	vars=data.keys()
	lData=len(data[vars[0]])
	if timestamp: baseName+=time.strftime('_%Y%m%d_%H:%M')
	baseNameNoPath=baseName.split('/')[-1]
	fData=bz2.BZ2File(baseName+".data.bz2",'w');
	fData.write("# "+"\t\t".join(vars)+"\n")
	for i in range(lData):
		fData.write("\t".join([str(data[key][i]) for key in data.keys()])+"\n")
	fData.close()
	fPlot=file(baseName+".gnuplot",'w')
	fPlot.write('#!/usr/bin/env gnuplot\n#\n# created '+time.asctime()+' ('+time.strftime('%Y%m%d_%H:%M')+')\n#\n')
	if comment: fPlot.write('# '+comment.replace('\n','\n# ')+'#\n')
	fPlot.write('dataFile="< bzcat %s.data.bz2"\n'%(baseNameNoPath))
	if not extension: extension=term
	i=0
	for p in plots:
		# print p
		plots_p=[fillNonSequence(o) for o in plots[p]]
		if term in ['wxt','x11']: fPlot.write("set term %s %d persist\n"%(term,i))
		else: fPlot.write("set term %s; set output '%s.%d.%s'\n"%(term,baseNameNoPath,i,extension))
		fPlot.write("set xlabel '%s'\n"%p)
		if title: fPlot.write("set title '%s'\n"%title)
		fPlot.write("plot "+",".join([" dataFile using %d:%d title '%s(%s)' with lines"%(vars.index(p)+1,vars.index(pp[0])+1,pp[0],p) for pp in plots_p])+"\n")
		i+=1
	fPlot.close()


	
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


