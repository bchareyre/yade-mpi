# this module is populated at initialization from the c++ part of PythonUI
"""Runtime variables, populated at yade startup."""
# default value
hasDisplay=False 


# find out about which ipython version we use -- 0.10* and 0.11 are supported, but they have different internals
import IPython
try: # attempt to get numerical version
	ipython_version=int(IPython.__version__.split('.',2)[1]) ## convert '0.10' to 10, '0.11.alpha1.bzr.r1223' to 11
except ValueError:
	print 'WARN: unable to extract IPython version from %s, defaulting to 10'%(IPython.__version__)
	ipython_version=10
if ipython_version not in (10,11): # versions that we are able to handle, round up or down correspondingly
	newipver=10 if ipython_version<10 else 11
	print 'WARN: unhandled IPython version 0.%d, assuming 0.%d instead.'%(ipython_version,newipver)
	ipython_version=newipver


