# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
"""
Module containing utility functions for plotting inside yade. See :ysrc:`scripts/simple-scene-plot.py` or :ysrc:`examples/concrete/uniax.py` for example of usage.

"""
import matplotlib
#matplotlib.use('TkAgg')
#matplotlib.use('GTKCairo')
#matplotlib.use('QtAgg')
matplotlib.rc('axes',grid=True) # put grid in all figures
import pylab

"Global dictionary containing all data values, common for all plots, in the form {'name':[value,...],...}. Data should be added using plot.addData function. All [value,...] columns have the same length, they are padded with NaN if unspecified."
data={}
"dictionary x-name -> (yspec,...), where yspec is either y-name or (y-name,'line-specification')"
plots={} # dictionary x-name -> (yspec,...), where yspec is either y-name or (y-name,'line-specification')
"Dictionary converting names in data to human-readable names (TeX names, for instance); if a variable is not specified, it is left untranslated."
labels={}
#plotLines={} # dictionary x-name -> Line2d objects (that hopefully still correspond to yspec in plots)

def reset():
	"Reset all plot-related variables (data, plots, labels)"
	global data, plots, labels # plotLines
	data={}; plots={}; # plotLines={};
	pylab.close('all')

def resetData():
	"Reset all plot data; keep plots and labels intact."
	global data
	data={}

from yade.wrapper import *

def splitData():
	"Make all plots discontinuous at this point (adds nan's to all data fields)"
	addData({})

def reverseData():
	"""Reverse yade.plot.data order.
	
	Useful for tension-compression test, where the initial (zero) state is loaded and, to make data continuous, last part must *end* in the zero state.
	"""
	for k in data: data[k].reverse()

def addData(*d_in,**kw):
	"""Add data from arguments name1=value1,name2=value2 to yade.plot.data.
	(the old {'name1':value1,'name2':value2} is deprecated, but still supported)

	New data will be left-padded with nan's, unspecified data will be nan.
	This way, equal length of all data is assured so that they can be plotted one against any other.

	Nan's don't appear in graphs."""
	import numpy
	if len(data)>0: numSamples=len(data[data.keys()[0]])
	else: numSamples=0
	#reduceData(numSamples)
	nan=float('nan')
	d=(d_in[0] if len(d_in)>0 else {})
	d.update(**kw)
	for name in d:
		if not name in data.keys():
			data[name]=[nan for i in range(numSamples)] #numpy.array([nan for i in range(numSamples)])
	for name in data:
		if name in d: data[name].append(d[name]) #numpy.append(data[name],[d[name]],1)
		else: data[name].append(nan)

def _addPointTypeSpecifier(o):
	"""Add point type specifier to simple variable name"""
	if type(o) in [tuple,list]: return o
	else: return (o,'')
def _tuplifyYAxis(pp):
	"""convert one variable to a 1-tuple"""
	if type(pp) in [tuple,list]: return pp
	else: return (pp,)
def _xlateLabel(l):
	"Return translated label; return l itself if not in the labels dict."
	global labels
	if l in labels.keys(): return labels[l]
	else: return l

def plot(noShow=False):
	"""Do the actual plot, which is either shown on screen (and nothing is returned: if *noShow* is False) or returned as object (if *noShow* is True).
	
	You can use 
	
		>>> from yade import plot
		>>> plot.plot(noShow=True).saveFig('someFile.pdf')
		
	to save the figure to file automatically.
	"""
	if not noShow: pylab.ion() ## # no interactive mode (hmmm, I don't know why actually...)
	for p in plots:
		pylab.figure()
		plots_p=[_addPointTypeSpecifier(o) for o in _tuplifyYAxis(plots[p])]
		plots_p_y1,plots_p_y2=[],[]; y1=True
		for d in plots_p:
			if d[0]=='|||':
				y1=False; continue
			if y1: plots_p_y1.append(d)
			else: plots_p_y2.append(d)
		#plotLines[p]=
		pylab.plot(*sum([[data[p],data[d[0]],d[1]] for d in plots_p_y1],[]))
		pylab.legend([_xlateLabel(_p[0]) for _p in plots_p_y1],loc=('upper left' if len(plots_p_y2)>0 else 'best'))
		pylab.ylabel(','.join([_xlateLabel(_p[0]) for _p in plots_p_y1]))
		if len(plots_p_y2)>0:
			# try to move in the color palette a little further (magenta is 5th): r,g,b,c,m,y,k
			origLinesColor=pylab.rcParams['lines.color']; pylab.rcParams['lines.color']='m'
			# create the y2 axis
			pylab.twinx()
			#plotLines[p]+=
			[pylab.plot(*sum([[data[p],data[d[0]],d[1]] for d in plots_p_y2],[]))]
			pylab.legend([_xlateLabel(_p[0]) for _p in plots_p_y2],loc='upper right')
			pylab.rcParams['lines.color']=origLinesColor
			pylab.ylabel(','.join([_xlateLabel(_p[0]) for _p in plots_p_y2]))
		pylab.xlabel(_xlateLabel(p))
		if 'title' in O.tags.keys(): pylab.title(O.tags['title'])
	if not noShow: pylab.show()
	else: return pylab.gcf() # return current figure
updatePeriod=0

def saveGnuplot(baseName,term='wxt',extension=None,timestamp=False,comment=None,title=None,varData=False):
	"""Save data added with :yref:`yade.plot.addData` into (compressed) file and create .gnuplot file that attempts to mimick plots specified with :yref:`yade.plot.plots`.

:parameters:
	baseName:
		used for creating baseName.gnuplot (command file for gnuplot),
		associated baseName.data (data) and output files (if applicable) in the form baseName.[plot number].extension
	term:
		specify the gnuplot terminal;
		defaults to x11, in which case gnuplot will draw persistent windows to screen and terminate; other useful terminals are 'png', 'cairopdf' and so on
	extension:
		defaults to terminal name; fine for png for example; if you use 'cairopdf', you should also say extension='pdf' however
	timestamp:
		append numeric time to the basename
	varData:
		whether file to plot will be declared as variable or be in-place in the plot expression
	comment:
		a user comment (may be multiline) that will be embedded in the control file

Returns name fo the gnuplot file created.
	"""
	import time,bz2
	if len(data.keys())==0: raise RuntimeError("No data for plotting were saved.")
	vars=data.keys(); vars.sort()
	lData=len(data[vars[0]])
	if timestamp: baseName+=time.strftime('_%Y%m%d_%H:%M')
	baseNameNoPath=baseName.split('/')[-1]
	fData=bz2.BZ2File(baseName+".data.bz2",'w');
	fData.write("# "+"\t\t".join(vars)+"\n")
	for i in range(lData):
		fData.write("\t".join([str(data[var][i]) for var in vars])+"\n")
	fData.close()
	fPlot=file(baseName+".gnuplot",'w')
	fPlot.write('#!/usr/bin/env gnuplot\n#\n# created '+time.asctime()+' ('+time.strftime('%Y%m%d_%H:%M')+')\n#\n')
	if comment: fPlot.write('# '+comment.replace('\n','\n# ')+'#\n')
	dataFile='"< bzcat %s.data.bz2"'%(baseNameNoPath)
	if varData:
		fPlot.write('dataFile=%s'%dataFile); dataFile='dataFile'
	if not extension: extension=term
	i=0
	for p in plots:
		# print p
		plots_p=[_addPointTypeSpecifier(o) for o in _tuplifyYAxis(plots[p])]
		if term in ['wxt','x11']: fPlot.write("set term %s %d persist\n"%(term,i))
		else: fPlot.write("set term %s; set output '%s.%d.%s'\n"%(term,baseNameNoPath,i,extension))
		fPlot.write("set xlabel '%s'\n"%_xlateLabel(p))
		fPlot.write("set grid\n")
		fPlot.write("set datafile missing 'nan'\n")
		if title: fPlot.write("set title '%s'\n"%title)
		y1=True; plots_y1,plots_y2=[],[]
		for d in plots_p:
			if d[0]=='|||':
				y1=False; continue
			if y1: plots_y1.append(d)
			else: plots_y2.append(d)
		fPlot.write("set ylabel '%s'\n"%(','.join([_xlateLabel(_p[0]) for _p in plots_y1]))) 
		if len(plots_y2)>0:
			fPlot.write("set y2label '%s'\n"%(','.join([_xlateLabel(_p[0]) for _p in plots_y2])))
			fPlot.write("set y2tics\n")
		ppp=[]
		for pp in plots_y1: ppp.append(" %s using %d:%d title '← %s(%s)' with lines"%(dataFile,vars.index(p)+1,vars.index(pp[0])+1,_xlateLabel(pp[0]),_xlateLabel(p),))
		for pp in plots_y2: ppp.append(" %s using %d:%d title '%s(%s) →' with lines axes x1y2"%(dataFile,vars.index(p)+1,vars.index(pp[0])+1,_xlateLabel(pp[0]),_xlateLabel(p),))
		fPlot.write("plot "+",".join(ppp)+"\n")
		i+=1
	fPlot.close()
	return baseName+'.gnuplot'


	
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


