#!/usr/bin/python
# syntax:python

# see file:///usr/share/doc/python2.6/html/library/sys.html#sys.setdlopenflags
# and various web posts on the topic, e.g.
# * http://gcc.gnu.org/faq.html#dso
# * http://www.code-muse.com/blog/?p=58
# * http://wiki.python.org/moin/boost.python/CrossExtensionModuleDependencies
import sys, DLFCN
sys.setdlopenflags(DLFCN.RTLD_NOW | DLFCN.RTLD_GLOBAL)


# find what is our version, based on argv[0]
import sys,re,os.path
m=re.match('(.*)/bin/yade(-.*)-py',sys.argv[0])
if not m: raise RuntimeError("Unable to find prefix and yade version from argv[0]=='%s'."%sys.argv[0])
prefix,suffix=m.group(1),m.group(2)
libDir=prefix+'/lib/yade'+suffix
pluginDirs=[libDir+dir for dir in ('','/plugins','/gui','/extra') if os.path.exists(libDir+dir)]
sys.path.append(libDir+'/py')

import yade.boot
yade.boot.initialize(pluginDirs,True)

from math import *
from yade.wrapper import *
from miniWm3Wrap import *
from yade._customConverters import *
from yade import runtime
from yade import utils



__builtins__.O=Omega()

runtime.prefix,runtime.suffix=prefix,suffix
runtime.argv=sys.argv
# todo
runtime.stopAfter=False
# todo
runtime.nonInteractive=False
# todo
runtime.features=['openmp','opengl','vtk','gts','log4cxx']

runtime.simulation=''
runtime.script=''
for arg in sys.argv:
	if arg.endswith('.xml') or arg.endswith('.xml.bz2'): runtime.simulation=arg
	elif arg.endswith('.py'): runtime.script=arg


execfile(prefix+'/lib/yade'+suffix+'/gui/PythonUI_rc.py')

#setupPythonRuntime()

#sys.argv[0]='<embedded python interpreter>'
if 0:
	if 1:
		from IPython.Shell import IPShellEmbed
		ipshell=IPShellEmbed(exit_msg='Bye.',rc_override={'execfile':[runtime.prefix+'/lib/yade'+runtime.suffix+'/py/yade/ipython.py']})
		ipshell()
	else:
		import bpython
		bpython.embed()

