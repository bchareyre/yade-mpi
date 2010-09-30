# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
"""
Module containing utility functions for plotting inside yade. See :ysrc:`scripts/simple-scene-plot.py` or :ysrc:`examples/concrete/uniax.py` for example of usage.

"""

## all exported names
__all__=['data','plots','labels','live','liveInterval','autozoom','plot','reset','resetData','splitData','reverse','addData','saveGnuplot','saveDataTxt']

# multi-threaded support for Tk
# safe to import even if Tk will not be used
import mtTkinter as Tkinter

import matplotlib,os,time,math

# running in batch
#
# If GtkAgg is the default, X must be working, which is not the case
# with batches (DISPLAY is unset in such case) and importing pylab fails then.
#
# Agg does not require the GUI part and works without any DISPLAY active
# just fine.
#
# see http://www.mail-archive.com/yade-dev@lists.launchpad.net/msg04320.html 
# and https://lists.launchpad.net/yade-users/msg03289.html
#
import yade.runtime
if not yade.runtime.hasDisplay: matplotlib.use('Agg')

#matplotlib.use('TkAgg')
#matplotlib.use('GTKAgg')
##matplotlib.use('QtAgg')
matplotlib.rc('axes',grid=True) # put grid in all figures
import pylab

data={}
"Global dictionary containing all data values, common for all plots, in the form {'name':[value,...],...}. Data should be added using plot.addData function. All [value,...] columns have the same length, they are padded with NaN if unspecified."
plots={} # dictionary x-name -> (yspec,...), where yspec is either y-name or (y-name,'line-specification')
"dictionary x-name -> (yspec,...), where yspec is either y-name or (y-name,'line-specification')"
labels={}
"Dictionary converting names in data to human-readable names (TeX names, for instance); if a variable is not specified, it is left untranslated."
xylabels={}
"Dictionary of 2-tuples specifying (xlabel,ylabel) for respective plots; if either of them is None, the default auto-generated title is used."


live=True if yade.runtime.hasDisplay else False
"Enable/disable live plot updating. Disabled by default for now, since it has a few rough edges."
liveInterval=1
"Interval for the live plot updating, in seconds."
autozoom=True
"Enable/disable automatic plot rezooming after data update."
scientific=True if hasattr(pylab,'ticklabel_format') else False  ## safe default for older matplotlib versions
"Use scientific notation for axes ticks."
axesWd=0
"Linewidth (in points) to make *x* and *y* axes better visible; not activated if non-positive."
current=-1
"Point that is being tracked with a scatter point. -1 is for the last point, set to *nan* to disable."

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

def addDataColumns(dd):
	'''Add new columns with NaN data, without adding anything to other columns. Does nothing for columns that already exist'''
	numSamples=len(data[data.keys()[0]]) if len(data)>0 else 0
	for d in dd:
		if d in data.keys(): continue
		d[d]=[nan for i in range(numSamples)]

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
	d=(d_in[0] if len(d_in)>0 else {})
	d.update(**kw)
	for name in d:
		if not name in data.keys():
			data[name]=[nan for i in range(numSamples)] #numpy.array([nan for i in range(numSamples)])
	for name in data:
		if name in d: data[name].append(d[name]) #numpy.append(data[name],[d[name]],1)
		else: data[name].append(nan)

# not public functions
def addPointTypeSpecifier(o):
	"""Add point type specifier to simple variable name"""
	if type(o) in [tuple,list]: return o
	else: return (o,'')
def tuplifyYAxis(pp):
	"""convert one variable to a 1-tuple"""
	if type(pp) in [tuple,list]: return pp
	else: return (pp,)
def xlateLabel(l):
	"Return translated label; return l itself if not in the labels dict."
	global labels
	if l in labels.keys(): return labels[l]
	else: return l

class LineRef:
	"""Holds reference to plot line and to original data arrays (which change during the simulation),
	and updates the actual line using those data upon request."""
	def __init__(self,line,scatter,xdata,ydata):
		self.line,self.scatter,self.xdata,self.ydata=line,scatter,xdata,ydata
	def update(self):
		import numpy
		self.line.set_xdata(self.xdata)
		self.line.set_ydata(self.ydata)
		try:
			x,y=[self.xdata[current]],[self.ydata[current]]
		except IndexError: x,y=0,0
		# this could be written in a nicer way, very likely
		pt=numpy.ndarray((2,),buffer=numpy.array([x,y]))
		self.scatter.set_offsets(pt)

currLineRefs=[]
liveTimeStamp=0 # timestamp when live update was started, so that the old thread knows to stop if that changes
nan=float('nan')

def createPlots(subPlots=False):
	global currLineRefs
	figs=set([l.line.get_axes().get_figure() for l in currLineRefs]) # get all current figures
	for f in figs: pylab.close(f) # close those
	currLineRefs=[] # remove older plots (breaks live updates of windows that are still open)
	if len(plots)==0: return # nothing to plot
	if subPlots:
		# compute number of rows and colums for plots we have
		subCols=int(round(math.sqrt(len(plots)))); subRows=int(math.ceil(len(plots)*1./subCols))
		#print 'subplot',subCols,subRows
	for nPlot,p in enumerate(plots.keys()):
		pStrip=p.strip()
		if not subPlots: pylab.figure()
		else: pylab.subplot(subRows,subCols,nPlot)
		plots_p=[addPointTypeSpecifier(o) for o in tuplifyYAxis(plots[p])]
		plots_p_y1,plots_p_y2=[],[]; y1=True
		missing=set() # missing data columns
		if pStrip not in data.keys(): missing.add(pStrip)
		for d in plots_p:
			if d[0]=='|||' or d[0]==None:
				y1=False; continue
			if y1: plots_p_y1.append(d)
			else: plots_p_y2.append(d)
			if d[0] not in data.keys(): missing.add(d[0])
		if missing:
			if len(data.keys())==0 or len(data[data.keys()[0]])==0: # no data at all yet, do not add garbage NaNs
				for m in missing: data[m]=[]
			else:
				print 'Missing columns in plot.data, adding NaN: ',','.join(list(missing))
				addDataColumns(missing)
		# create y1 lines
		for d in plots_p_y1:
			line,=pylab.plot(data[pStrip],data[d[0]],d[1])
			scatterPt=([0],[0]) if len(data[pStrip])==0 else (data[pStrip][current],data[d[0]][current])
			scatter=pylab.scatter(scatterPt[0],scatterPt[1],color=line.get_color())
			currLineRefs.append(LineRef(line,scatter,data[pStrip],data[d[0]]))
		# create the legend
		pylab.legend([xlateLabel(_p[0]) for _p in plots_p_y1],loc=('upper left' if len(plots_p_y2)>0 else 'best'))
		pylab.ylabel((', '.join([xlateLabel(_p[0]) for _p in plots_p_y1])) if p not in xylabels or not xylabels[p][1] else xylabels[p][1])
		pylab.xlabel(xlateLabel(pStrip) if (p not in xylabels or not xylabels[p][0]) else xylabels[p][0])
		if scientific: pylab.ticklabel_format(style='sci',scilimits=(0,0),axis='both')
		if axesWd>0:
			pylab.axhline(linewidth=axesWd,color='k')
			pylab.axvline(linewidth=axesWd,color='k')
		# create y2 lines, if any
		if len(plots_p_y2)>0:
			# try to move in the color palette a little further (magenta is 5th): r,g,b,c,m,y,k
			origLinesColor=pylab.rcParams['lines.color']; pylab.rcParams['lines.color']='m'
			# create the y2 axis
			pylab.twinx()
			for d in plots_p_y2:
				line,=pylab.plot(data[pStrip],data[d[0]],d[1])
				scatterPt=([0],[0]) if len(data[pStrip])==0 else (data[pStrip][current],data[d[0]][current])
				scatter=pylab.scatter(scatterPt[0],scatterPt[1],color=line.get_color())
				currLineRefs.append(LineRef(line,scatter,data[pStrip],data[d[0]]))
			# legend
			pylab.legend([xlateLabel(_p[0]) for _p in plots_p_y2],loc='upper right')
			pylab.rcParams['lines.color']=origLinesColor
			pylab.ylabel((', '.join([xlateLabel(_p[0]) for _p in plots_p_y2])) if p not in xylabels or len(xylabels[p])<3 or not xylabels[p][2] else xylabels[p][2])
			## should be repeated for y2 axis, but in that case the 10^.. label goes to y1 axis (bug in matplotlib, it seems)
			# if scientific: pylab.ticklabel_format(style='sci',scilimits=(0,0),axis='both')

		if 'title' in O.tags.keys(): pylab.title(O.tags['title'])



def liveUpdate(timestamp):
	global liveTimeStamp
	liveTimeStamp=timestamp
	while True:
		if not live or liveTimeStamp!=timestamp: return
		figs,axes=set(),set()
		for l in currLineRefs:
			l.update()
			figs.add(l.line.get_figure())
			axes.add(l.line.get_axes())
		if autozoom:
			for ax in axes:
				try:
					ax.relim() # recompute axes limits
					ax.autoscale_view()
				except RuntimeError: pass # happens if data are being updated and have not the same dimension at the very moment
		for fig in figs:
			try:
				fig.canvas.draw()
			except RuntimeError: pass # happens here too
		time.sleep(liveInterval)
	

def plot(noShow=False,subPlots=False):
	"""Do the actual plot, which is either shown on screen (and nothing is returned: if *noShow* is ``False``) or, if *noShow* is ``True``, returned as matplotlib's Figure object or list of them.
	
	You can use 
	
		>>> from yade import plot
		>>> plot.plots={'foo':('bar',)}
		>>> plot.plot(noShow=True).savefig('someFile.pdf')
		>>> import os
		>>> os.path.exists('someFile.pdf')
		True
		
	to save the figure to file automatically.

	.. note:: For backwards compatibility reasons, *noShow* option will return list of figures for multiple figures but a single figure (rather than list with 1 element) if there is only 1 figure.
	"""
	createPlots(subPlots=subPlots)
	global currLineRefs
	if not noShow:
		if not yade.runtime.hasDisplay: return # would error out with some backends, such as Agg used in batches
		if live:
			import thread
			thread.start_new_thread(liveUpdate,(time.time(),))
		# pylab.show() # this blocks for some reason; call show on figures directly
		figs=set([l.line.get_axes().get_figure() for l in currLineRefs])
		for f in figs:
			f.show()
			# should have fixed https://bugs.launchpad.net/yade/+bug/606220, but does not work apparently
			if 0:
				import matplotlib.backend_bases
				if 'CloseEvent' in dir(matplotlib.backend_bases):
					def closeFigureCallback(event):
						ff=event.canvas.figure
						# remove closed axes from our update list
						global currLineRefs
						currLineRefs=[l for l in currLineRefs if l.line.get_axes().get_figure()!=ff] 
					f.canvas.mpl_connect('close_event',closeFigureCallback)
	else:
		figs=list(set([l.line.get_axes().get_figure() for l in currLineRefs]))
		if len(figs)==1: return figs[0]
		else: return figs

def saveDataTxt(fileName,vars=None):
	import bz2,gzip
	if not vars:
		vars=data.keys(); vars.sort()
	if fileName.endswith('.bz2'): f=bz2.BZ2File(fileName,'w')
	elif fileName.endswith('.gz'): f=gzip.GzipFile(fileName,'w')
	else: f=open(fileName,'w')
	f.write("# "+"\t\t".join(vars)+"\n")
	for i in range(len(data[vars[0]])):
		f.write("\t".join([str(data[var][i]) for var in vars])+"\n")
	f.close()


def savePylab(baseName,timestamp=False,title=None):
	import time
	if len(data.keys())==0: raise RuntimeError("No data for plotting were saved.")
	if timestamp: baseName+=_mkTimestamp()
	baseNameNoPath=baseName.split('/')[-1]
	saveDataTxt(fileName=baseName+'.data.bz2')
	py=file(baseName+'.py','w')
	py.write('#!/usr/bin/env python\n#\n# created '+time.asctime()+' ('+time.strftime('%Y%m%d_%H:%M')+')\n#\n')

def _mkTimestamp():
	import time
	return time.strftime('_%Y%m%d_%H:%M')

def saveGnuplot(baseName,term='wxt',extension=None,timestamp=False,comment=None,title=None,varData=False):
	"""Save data added with :yref:`yade.plot.addData` into (compressed) file and create .gnuplot file that attempts to mimick plots specified with :yref:`yade.plot.plots`.

:param baseName: used for creating baseName.gnuplot (command file for gnuplot), associated ``baseName.data.bz2`` (data) and output files (if applicable) in the form ``baseName.[plot number].extension``
:param term: specify the gnuplot terminal; defaults to ``x11``, in which case gnuplot will draw persistent windows to screen and terminate; other useful terminals are ``png``, ``cairopdf`` and so on
:param extension: extension for ``baseName`` defaults to terminal name; fine for png for example; if you use ``cairopdf``, you should also say ``extension='pdf'`` however
:param bool timestamp: append numeric time to the basename
:param bool varData: whether file to plot will be declared as variable or be in-place in the plot expression
:param comment: a user comment (may be multiline) that will be embedded in the control file

:return: name of the gnuplot file created.
	"""
	if len(data.keys())==0: raise RuntimeError("No data for plotting were saved.")
	if timestamp: baseName+=_mkTimestamp()
	baseNameNoPath=baseName.split('/')[-1]
	vars=data.keys(); vars.sort()
	saveDataTxt(fileName=baseName+'.data.bz2',vars=vars)
	fPlot=file(baseName+".gnuplot",'w')
	fPlot.write('#!/usr/bin/env gnuplot\n#\n# created '+time.asctime()+' ('+time.strftime('%Y%m%d_%H:%M')+')\n#\n')
	if comment: fPlot.write('# '+comment.replace('\n','\n# ')+'#\n')
	dataFile='"< bzcat %s.data.bz2"'%(baseNameNoPath)
	if varData:
		fPlot.write('dataFile=%s'%dataFile); dataFile='dataFile'
	if not extension: extension=term
	i=0
	for p in plots:
		pStrip=p.strip()
		plots_p=[addPointTypeSpecifier(o) for o in tuplifyYAxis(plots[p])]
		if term in ['wxt','x11']: fPlot.write("set term %s %d persist\n"%(term,i))
		else: fPlot.write("set term %s; set output '%s.%d.%s'\n"%(term,baseNameNoPath,i,extension))
		fPlot.write("set xlabel '%s'\n"%xlateLabel(p))
		fPlot.write("set grid\n")
		fPlot.write("set datafile missing 'nan'\n")
		if title: fPlot.write("set title '%s'\n"%title)
		y1=True; plots_y1,plots_y2=[],[]
		for d in plots_p:
			if d[0]=='|||' or d[0]==None:
				y1=False; continue
			if y1: plots_y1.append(d)
			else: plots_y2.append(d)
		fPlot.write("set ylabel '%s'\n"%(','.join([xlateLabel(_p[0]) for _p in plots_y1]))) 
		if len(plots_y2)>0:
			fPlot.write("set y2label '%s'\n"%(','.join([xlateLabel(_p[0]) for _p in plots_y2])))
			fPlot.write("set y2tics\n")
		ppp=[]
		for pp in plots_y1: ppp.append(" %s using %d:%d title '← %s(%s)' with lines"%(dataFile,vars.index(pStrip)+1,vars.index(pp[0])+1,xlateLabel(pp[0]),xlateLabel(pStrip),))
		for pp in plots_y2: ppp.append(" %s using %d:%d title '%s(%s) →' with lines axes x1y2"%(dataFile,vars.index(pStrip)+1,vars.index(pp[0])+1,xlateLabel(pp[0]),xlateLabel(pStrip),))
		fPlot.write("plot "+",".join(ppp)+"\n")
		i+=1
	fPlot.close()
	return baseName+'.gnuplot'
