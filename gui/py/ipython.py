# encoding: UTF-8
#
# This file cannot be imported as module, since eval(objName) must be in the same scope as the command line
#
# Therefore, it is execfile'd at ipython startup
#
def yade_completers(self, event):
	# this should parse the incomplete line and return only the object that pertains to this ['
	# it will work fine in cases like b['clum<TAB> if b is a Body, but not in b['id']+b2['id
	objName=event.line.split('[')[0]
	# print objName
	obj=eval(objName)
	# print str(type(obj))
	if 'yade.wrapper.' in str(type(obj)):
		return obj.keys()
import IPython.ipapi
ip=IPython.ipapi.get()
ip.set_hook('complete_command',yade_completers,re_key='.*\\b[a-zA-Z0-9_]+\[["\'][^"\'\]]*$')

