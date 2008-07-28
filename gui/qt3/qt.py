from _qt import *
if not isActive():
	raise ImportError("The Qt gui is not being used (run with -N QtGUI).")

def createVideo(playerDb,out,viewerState=None,dispParamsNo=-1,stride=1,fps=24,postLoadHook=None):
	"""Create video by replaying a simulation. Snapshots are taken to temporary files,
	encoded to a .ogg stream (theora codec); temps are deleted at the end.

	If the output file exists already, a ~[number] is appended and the old file is renamed.

	playerDb is the database with saved simulation states,
	out is the output file (like a.ogg), fps is frames-per-second for the video that is created,
	dispParamsNo, stride and postLoadHook are passed to qt.runPlayer (docs in gui/qt3/QtGUI-Python.cpp).
	
	You need a display to run this (either virtual, like xvfb, or physical).

	Necessary packages: python-gst0.10 gstreamer0.10-plugins-good python-gobject
	"""
	import pygst,sys,gobject,os
	pygst.require("0.10")
	import gst
	from yade import qt
	# postLoadHook and viewerState have "" instead of None in the c++ interface
	wildcard,snaps=qt.runPlayerSession(
		playerDb,
		'', # snapBase
		savedQGLState=(viewerState if viewerState else ''),
		dispParamsNo=dispParamsNo,
		stride=stride,
		postLoadHook=(postLoadHook if postLoadHook else ''))
	if(os.path.exists(out)):
		i=0;
		while(os.path.exists(out+"~%d"%i)): i+=1
		os.rename(out,out+"~%d"%i); print "Output file `%s' already existed, old file renamed to `%s'"%(out,out+"~%d"%i)
	print "Encoding video from %s (%d files total) to %s"%(wildcard,len(snaps),out)
	pipeline=gst.parse_launch('multifilesrc location="%s" index=0 caps="image/png,framerate=\(fraction\)%d/1" ! pngdec ! ffmpegcolorspace ! theoraenc sharpness=2 quality=32 ! oggmux ! filesink location="%s"'%(wildcard,fps,out))
	bus=pipeline.get_bus()
	bus.add_signal_watch()
	mainloop=gobject.MainLoop();
	bus.connect("message::eos",lambda bus,msg: mainloop.quit())
	pipeline.set_state(gst.STATE_PLAYING)
	mainloop.run()
	pipeline.set_state(gst.STATE_NULL); pipeline.get_state()
	print "Cleaning snapshot files."
	for f in snaps: os.remove(f)

