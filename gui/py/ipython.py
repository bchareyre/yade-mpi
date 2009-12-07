# encoding: UTF-8
#
# This file cannot be imported as module, since eval(objName) must be in the same scope as the command line
#
# Therefore, it is execfile'd at ipython startup
#
"""Yade-specific command-line completion hooks for in ipython (experimental)"""

#try:
#	import yade.qt
#except ImportError: pass

def yade_completers(self, event):
	import sys
	# this should parse the incomplete line and return only the object that pertains to this ['
	# it will work fine in cases like b['clum<TAB> if b is a Body, but not in b['id']+b2['id
	objName=event.line.split('[')[0]
	# print objName
	obj=eval(objName)
	# print str(type(obj))
	if 'yade.wrapper.' in str(type(obj)):
		return obj.keys()+['hovno']
def empty_completer(self,event):
	print 'Empty completer!'
	if event.line=='': # and 'yade.qt' in dir():
		yade.qt.Controller()
		return '@@@'
	return ['foo','bar','baz']

import IPython.ipapi
ip=IPython.ipapi.get()

o=ip.options
# from http://www.cv.nrao.edu/~rreid/casa/tips/ipy_user_conf.py
o.separate_in,o.separate_out,o.separate_out2="0","0","0"
# from ipython manpage
o.prompt_in1="Yade [\#]: "
o.prompt_in2="     .\D.. "
o.prompt_out=" ->  [\#]: "
#ip.options.profile='yade'

#ip.set_hook('complete_command',empty_completer,re_key=' ')
ip.set_hook('complete_command',yade_completers,re_key='.*\\b[a-zA-Z0-9_]+\[["\'][^"\'\]]*$')
