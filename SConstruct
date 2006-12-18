#!/usr/bin/scons
# coding: UTF-8
# vim:syntax=python:

#
# This is the master build file for scons (http://www.scons.org). It is experimental, though it build very well for me. Prequisities for running:
# 1. have scons installed (debian & family: package scons)
# 2. edit the few lines below that are marked `these lines should be adapted'. In particular, remove LOG4CXX macro and log4cxx library if you don't want to use it. (Autodetection and configuration is planned for later)
# 3. go to yade-scripts and run ./erskine3-apply.sh. It should generate SConscript files for subprojects.
#
# To run the build, just run `scons' from the top-level (this) directory. This step will do preparatory steps (creating local include directory and symlinking all headers from there, create install directories), compile files and install them. Scons has great support for parallel builds, you can try `scons -j4'.
#
# To clean the build, run `scons -c'. Please note that it will also _uninstall_ yade. This is only temporary inconvenience that will disappera, though...
#
# What it does: running the script always checks for existence of installation directories for both libs and headers (hack).
# Prebuild target symlinks all headers to the ./include/* directories to make compilation possible without installing includes to $PREFIX.
# The install target takes care of both compilation and installation of binary, libraries and headers.
#
#
# TODO:
#  1. [DONE] retrieve target list and append install targets dynamically;
#  2. [DONE] configuration and option handling.
#  3. Have build.log with commands and all output...
#
# And as usually, clean up the code, get rid of workarounds and hacks etc.

import os,os.path,string,re,sys

##########################################################################################
############# OPTIONS ####################################################################
##########################################################################################

opts=Options('yade.scons.conf')
opts.AddOptions(
	PathOption('PREFIX', 'Install path prefix', '/usr/local'),
	BoolOption('DEBUG', 'Enable debugging information and disable optimizations',1),
	('CPPPATH', 'Additional paths for the C preprocessor (whitespace separated)',None,None,Split),
	('LIBPATH','Additional paths for the linker (whitespace separated)',None,None,Split),
	('QTDIR','Directories where to look for qt3',['/usr/share/qt3','/usr/lib/qt'],None,Split),
	('CXX','The c++ compiler','ccache g++-4.0'),
	BoolOption('pretty',"Don't show compiler command line (like the Linux kernel)",1)
)

### create THE environment
env=Environment(tools=['default'],options=opts)
# do not propagate PATH from outside, to ensure identical builds on different machines
env.Append(ENV={'PATH':['/usr/local/bin','/bin','/usr/bin']})
# ccache needs $HOME to be set; colorgcc needs $TERM
propagatedEnvVars=['HOME','TERM','DISTCC_HOSTS']
for v in propagatedEnvVars:
	if os.environ.has_key(v): env.Append(ENV={v:os.environ[v]})
opts.Save('yade.scons.conf',env)
Help(opts.GenerateHelpText(env))


##########################################################################################
############# CONFIGURATION ##############################################################
##########################################################################################

def CheckQt(context, qtdirs):
	# make sure they exist and save them for restoring if a test fails
	context.env.Append(CPPPATH='',LIBPATH='',LIBS='')
	origs={'LIBS':context.env['LIBS'],'LIBPATH':context.env['LIBPATH'],'CPPPATH':context.env['CPPPATH']}
	for qtdir in qtdirs:
		context.Message( 'Checking for qt-mt in '+qtdir+'... ' )
		context.env['QTDIR']=qtdir
		context.env.Append(LIBS='qt-mt',LIBPATH=qtdir+'/lib',CPPPATH=qtdir+'/include' )
		ret=context.TryLink('#include<qapp.h>\nint main(int argc, char **argv){QApplication qapp(argc, argv);return 0;}\n','.cpp')
		context.Result(ret)
		if not ret:
			for k in origs.keys(): context.env[k]=origs[k]
		else:
			return ret
	return False

conf=Configure(env,custom_tests={'CheckQt':CheckQt})

ok=True
ok&=conf.CheckLibWithHeader('pthread','pthread.h','c','pthread_exit(NULL);')
ok&=conf.CheckLibWithHeader('glut','GL/glut.h','c','glutGetModifiers();')
ok&=conf.CheckLibWithHeader('boost_date_time','boost/date_time/posix_time/posix_time.hpp','c++','boost::posix_time::time_duration::time_duration();')
ok&=conf.CheckLibWithHeader('boost_thread','boost/thread/thread.hpp','c++','boost::thread::thread();')
ok&=conf.CheckLibWithHeader('boost_filesystem','boost/filesystem/path.hpp','c++','boost::filesystem::path();')
ok&=conf.CheckLibWithHeader('Wm3Foundation','Wm3Math.h','c++','Wm3::Math<double>::PI;')
ok&=conf.CheckQt(env['QTDIR'])
if not ok:
	print "One of the essential libraries above was not found, unable to continue.\n\nCheck config.log for possible causes, note that there are options that you may need to customize:\n"+opts.GenerateHelpText(env)
	Exit(1)

env.Tool('qt')
env.Replace(QT_LIB='qt-mt')
env.Append(LIBS=['glut','boost_date_time','boost_filesystem','boost_thread','pthread','Wm3Foundation'])

if conf.CheckLibWithHeader('log4cxx','log4cxx/logger.h','c++','log4cxx::Logger::getLogger("foo");'):
	env.Append(LIBS='log4cxx',CPPDEFINES='LOG4CXX')

env=conf.Finish()


##########################################################################################
############# BUILDING ###################################################################
##########################################################################################

### DIRECTORIES
libDirs=['yade-libs','yade-packages/yade-package-common','yade-packages/yade-package-dem','yade-packages/yade-package-fem','yade-packages/yade-package-lattice','yade-packages/yade-package-mass-spring','yade-packages/yade-package-realtime-rigidbody','yade-extra','yade-guis']
instDirs=[os.path.join('$PREFIX','bin')]+[os.path.join('$PREFIX','lib','yade',string.split(x,os.path.sep)[-1]) for x in libDirs]
instIncludeDirs=['yade-core']+[os.path.join('$PREFIX','include','yade',string.split(x,os.path.sep)[-1]) for x in libDirs]

### PREPROCESSOR
env.Append(CPPPATH=['#/include'])

### COMPILER
if env['DEBUG']: env.Append(CXXFLAGS='-ggdb3')
else: env.Append(CXXFLAGS='-O2 -ffast-math')
env.Append(CXXFLAGS=['-pipe','-Wall'])

### LINKER
env.Append(SHLINKFLAGS='-Wl,-soname=${TARGET.file} -rdynamic')
# if this is not present, vtables & typeinfos for classes in yade binary itself are not exported; breaks plugin loading
env.Append(LINKFLAGS='-rdynamic') 
# makes dynamic library loading easied (no LD_LIBRARY_PATH) and perhaps faster
env.Append(RPATH=[os.path.join('$PREFIX','lib','yade',string.split(x,os.path.sep)[-1]) for x in libDirs])
# find already compiled but not yet installed libraries for linking
env.Append(LIBPATH=[os.path.join('#',x) for x in libDirs])


### this workaround is only needed for scons<=0.96.92, will disappear soon
###  (env.Install method chokes on no-existing directories)
import SCons
major,minor,micro=SCons.__version__.split('.')
if major=='0' and int(minor)<=96 and int(micro)<93:
	## should reside in prepareIncludes or sth similar
	def createDirs(dirList):
		for d in dirList:
			dd=d.replace('$PREFIX',env['PREFIX'])
			if not os.path.exists(dd):
				print dd
				os.makedirs(dd)
			elif not os.path.isdir(dd): raise OSError("Installation directory `%s' is a file?!"%dd)
	createDirs(instDirs)
	createDirs(instIncludeDirs)
	
def prepareIncludes(prefix=None):
	"""symlink all in-tree headers into the ./include directory so that we can build before headers are installed.
	
	If prefix is defined, instruct scons to Install every header file to an appropriate place instead."""
	#print "prepareIncludes(prefix='%s')"%prefix
	global env
	import os,string,re
	from os.path import join,split,isabs,isdir,exists
	if not prefix: yadeRoot='.' # MUST be relative, otherwise relative symlinks for the local includes will break badly
	else: yadeRoot=prefix
	yadeInc=join(yadeRoot,'include','yade')
	for root, dirs, files in os.walk('.'):
		for d in ('.svn','yade-flat','include'):
			try: dirs.remove(d)
			except ValueError: pass
		for f in files:
			if f.split('.')[-1] in ('hpp','inl','ipp','tpp','h'):
				m=re.match('^.*?'+os.path.sep+'(yade-((extra|core)|((gui|lib|package)-.*?)))'+os.path.sep+'.*$',root)
				subInc=join(yadeInc,m.group(1))
				if not prefix: # local include directory
					if not isdir(subInc): os.makedirs(subInc)
					try: os.symlink(join('..','..','..',root,f),join(subInc,f));
					except OSError: pass # if the file already exists, symlink fails
				else: env.Install(subInc,join(root,f))



# 1. re-generate SConscript files if some of them do not exist. This must be done in-place for
#		scons postpones all actions after having processed SConscruct and SConscript files.
# 2. symlink all headers to ./include/... so that we can build without installing headers to $PREFIX first (pre-build)
# 3. instruct scons to install (not symlink) all headers to the right place as well
# 4. set the "install" target as default (if scons is called without any arguments)
# Should be cleaned up.

## re-run erskine if needed
from os.path import exists
if len([1 for x in [os.path.join(x,'SConscript') for x in libDirs+['yade-core']] if not exists(x)]):
	print "Generating SConscript files (warnings can be ignored safely)..."
	from subprocess import Popen
	p = Popen("cd yade-scripts && sh erskine3-apply.sh",shell=True)
	if(os.waitpid(p.pid, 0)[1])!=0:
		print "Error running yade-scripts/erskine3-apply.sh, try doing it by hand."
		Exit(1)


prepareLocalIncludesProxy=prepareIncludes()
prepareIncludesProxy=prepareIncludes(prefix=env['PREFIX'])
prebuildAlias=env.Alias('prebuild',[prepareLocalIncludesProxy])
installAlias=env.Alias('install',[instDirs,prepareIncludesProxy,os.path.join('$PREFIX','include','yade')])
Depends(installAlias,prebuildAlias)
Default(installAlias)

# read all SConscript files
env.Export('env');
SConscript([os.path.join(x,'SConscript') for x in libDirs+['yade-core']])


##########################################################################################
############# INSTALLATION ###############################################################
##########################################################################################

##### (this is UNIX specific!) ######################

def enumerateDotSoNodes(dirnode, level=0):
	" cut&paste from http://www.scons.org/wiki/BuildDirGlob, then modified "
	if type(dirnode)==type(''): dirnode=Dir(dirnode) # convert string to node
	ret=[]
	for f in dirnode.all_children():
		if type(f)==type(Dir('.')): # print str(f)
			ret+=enumerateDotSoNodes(f,level)
		if str(f)[-3:]=='.so':
			ret.append(f) #print "%s%s (%s: %s)"%(level * ' ', str(f),type(f.get_builder()),f.get_builder())
	return ret
installableNodes=enumerateDotSoNodes(Dir('.'))

# iterate over .so nodes we got previously and call Install for each of them
for n in installableNodes:
	f=str(n)
	m=re.match(r'(^|.*/)(yade-(extra|guis|libs|package-[^/]+))/lib[^/]+\.so$',f)
	assert(m)
	instDir=m.group(2)
	env.Install('$PREFIX/lib/yade/'+instDir,n)
# for the one and only binary, we do it by hand
env.Install('$PREFIX/bin','yade-core/yade')

##########################################################################################
############# MISCILLANEA ################################################################
##########################################################################################

if env['pretty']:
	## http://www.scons.org/wiki/HidingCommandLinesInOutput
	env.Replace(CXXCOMSTR='C ${SOURCES}') # → ${TARGET.file}')
	env.Replace(SHCXXCOMSTR='C ${SOURCES}')  #→ ${TARGET.file}')
	env.Replace(SHLINKCOMSTR='L ${SOURCES}') # → ${TARGET.file}')
	env.Replace(LINKCOMSTR='L ${SOURCES}') # → ${TARGET.file}')
	env.Replace(INSTALLSTR='⇒ $TARGET')
	env.Replace(QT_UICCOMSTR='U ${SOURCES}')
	env.Replace(QT_MOCCOMSTR='M ${SOURCES}')
