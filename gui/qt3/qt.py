# import module parts in c++ 
from _qt import *

if not isActive():
	raise ImportError("The Qt gui is not being used (run with -N QtGUI).")



def makeSimulationVideo(output,realPeriod=1,virtPeriod=0,iterPeriod=0,viewNo=0,fps=24):
	"""Create video by running simulation. SnapshotEngine is added (and removed once done), temporary
	files are deleted. The video is theora-encoded in the ogg container. Periodicity is controlled
	in the same way as for PeriodicEngine (SnapshotEngine is a PeriodicEngine and realPeriod, virtPeriod 
	and iterPeriod are passed to the new SnapshotEngine).

	viewNo is 0-based GL view number. 0 is the primary view and will be created if it doesn't exist.
	It is an error if viewNo>0 and the view doesn't exist.

	The simulation will run until it stops by itself. Either set Omega().stopAtIteration or have an engine
	that will call Omega().pause() at some point.

	See makePlayerVideo for more documentation.
	"""
	import os
	from yade import utils,wrapper
	o=wrapper.Omega()
	# create primary view if none
	if len(views())==0: View()
	# remove existing SnaphotEngines
	se=wrapper.StandAloneEngine('SnapshotEngine',{'iterPeriod':iterPeriod,'realPeriod':realPeriod,'virtPeriod':virtPeriod,'fileBase':os.tmpnam(),'ignoreErrors':False,'viewNo':viewNo})
	origEngines=o.engines; o.engines=[e for e in o.engines if e.name!='SnapshotEngine']+[se]
	o.run(); o.wait();
	wildcard=se['fileBase']+'%04d.png'
	print "Number of frames:",len(se['savedSnapshots'])
	utils.encodeVideoFromFrames(wildcard,output,renameNotOverwrite=True,fps=fps)
	for f in se['savedSnapshots']: os.remove(f)
	o.engines=origEngines


def makePlayerVideo(playerDb,out,viewerState=None,dispParamsNo=-1,stride=1,fps=24,postLoadHook=None):
	"""Create video by replaying a simulation. Snapshots are taken to temporary files,
	encoded to a .ogg stream (theora codec); temps are deleted at the end.

	If the output file exists already, a ~[number] is appended and the old file is renamed.

	playerDb is the database with saved simulation states,
	out is the output file (like a.ogg), fps is frames-per-second for the video that is created,
	dispParamsNo, stride and postLoadHook are passed to qt.runPlayer (docs in gui/qt3/QtGUI-Python.cpp).
	
	You need a display to run this (either virtual, like xvfb, or physical).

	Necessary packages: python-gst0.10 gstreamer0.10-plugins-good python-gobject
	"""
	import sys,os
	from yade import qt,utils
	# postLoadHook and viewerState have "" instead of None in the c++ interface
	wildcard,snaps=qt.runPlayerSession(
		playerDb,
		'', # snapBase
		savedQGLState=(viewerState if viewerState else ''),
		dispParamsNo=dispParamsNo,
		stride=stride,
		postLoadHook=(postLoadHook if postLoadHook else ''))
	utils.encodeVideoFromFrames(wildcard,out,renameNotOverwrite=True,fps=fps)
	print "Cleaning snapshot files."
	for f in snaps: os.remove(f)

