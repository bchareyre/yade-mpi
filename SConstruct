#!/usr/bin/scons
# coding: UTF-8
# vim:syntax=python:

#
# This is the master build file for scons (http://www.scons.org). It is experimental, though it build very well for me. Prequisities for running are having scons installed (debian & family: package scons)
#
# Type "scons -h" for yade-specific options and "scons -H" for scons' options. Note that yade options will be remembered (saved in scons.config) so that you need to specify them only for the first time. Like this, for example:
#
#	scons -j2 pretty=1 debug=0 optimize=1 profile=1 exclude=extra,fem,lattice,realtime-rigidbody,mass-spring CPPPATH=/usr/local/include/wm3
#
# Next time, you can simply run "scons" or "scons -j4" (for 4-parallel builds) to rebuild targets that need it. IF YOU NEED TO READ CODE IN THIS FILE, SOMETHING IS BROKEN AND YOU SHOULD REALLY TELL ME (the goal is to replace qmake eventually).
#
# Scons will do preparatory steps (generating SConscript files from .pro files, creating local include directory and symlinking all headers from there, ...), compile files and install them.
#
# To clean the build, run `scons -c'. Please note that it will also _uninstall_ yade from $PREFIX!
#
# TODO:
#  1. [REMOVED] [DONE] retrieve target list and append install targets dynamically;
#  2. [DONE] configuration and option handling.
#  3. Have build.log with commands and all output...
#
# And as usually, clean up the code, get rid of workarounds and hacks etc.
import os,os.path,string,re,sys,shutil
import SCons
# SCons version numbers are needed a few times
# rewritten for python2.4
ver=[str(x) for x in SCons.__version__.split('.')]
for i in range(0,len(ver)):
	def any2num(x):
		if x in ('0123456789'): return x
		else: return '0'
	ver[i]="".join([any2num(x) for x in ver[i]])
	if len(ver[i])>2: ver[i]=ver[i][0:2]+"."+ver[i][2:]
sconsVersion=10000*float(ver[0])
if len(ver)>1: sconsVersion+=100*float(ver[1])
if len(ver)>2: sconsVersion+=float(ver[2])
# print sconsVersion
#

##########################################################################################
########## PROXY TO NEWER SCONS (DOWNLOADED IF NEEDED) ###################################
##########################################################################################
#print sconsVersion
if sconsVersion<9700.20071212: ##9700.20071212:  ##<9700 ##<9693: 
	#tgzParams=("http://dfn.dl.sourceforge.net/sourceforge/scons/scons-local-0.97.tar.gz","scons-local-0.97")
	#tgzParams=("http://ovh.dl.sourceforge.net/sourceforge/scons/scons-local-0.97.0d20070918.tar.gz","/scons-local-0.97.0d20070918") ## sconsVersion<=9700
	tgzParams=("http://switch.dl.sourceforge.net/sourceforge/scons/scons-local-0.97.0d20071212.tar.gz","/scons-local-0.97.0d20071212")
	newPrefix="./scons-local";
	newUrl,newDir=tgzParams[0],newPrefix+"/"+tgzParams[1]
	if not os.path.exists(newDir):
		print "Scons version too old, downloading new version. All subsequent calls will be proxied to the new version transparently."
		import urllib,tarfile
		(filename,headers)=urllib.urlretrieve(newUrl)
		print filename,"\n",headers
		tar=tarfile.open(filename, "r:gz")
		for tarinfo in tar: tar.extract(tarinfo,newPrefix)
		print "Done extracting scons to",newDir
		assert(os.path.exists(newDir))
	if os.path.exists(newDir):
		Exit(os.execv(newPrefix+"/scons.py",[newPrefix+'/scons.py']+sys.argv[1:]))


##########################################################################################
############# OPTIONS ####################################################################
##########################################################################################

optsFile='scons.config'

opts=Options(optsFile)
#
# The convention now is, that
#  1. CAPITALIZED options are
#   (a) notorious shell variables (they correspons most the time to SCons environment options of the same name - like CPPPATH)
#   (b) c preprocessor macros available to the program source (like PREFIX and SUFFIX)
#  2. lowercase options influence the building process, compiler options and the like.
#
opts.AddOptions(
	### OLD: use PathOption with PathOption.PathIsDirCreate, but that doesn't exist in 0.96.1!
	('PREFIX','Install path prefix','/usr/local'),
	('runtimePREFIX','Runtime path prefix; DO NOT USE, inteded for packaging only.','$PREFIX'),
	('variant','Build variant, will be suffixed to all files, along with version (beware: if PREFIX is the same, headers of the older version will still be overwritten','',None,lambda x:x),
	BoolOption('debug', 'Enable debugging information and disable optimizations',1),
	BoolOption('profile','Enable profiling information',0),
	BoolOption('optimize','Turn on heavy optimizations (generates SSE2 instructions)',0),
	ListOption('exclude','Yade components that will not be built','none',names=['qt3','gui','extra','common','dem','fem','lattice','mass-spring','realtime-rigidbody']),
	# OK, dummy prevents bug in scons: if one selects all, it says all in scons.config, but without quotes, which generates error.
	ListOption('features','Optional features that are turned on','python,log4cxx',names=['python','log4cxx','dummy']),
	('jobs','Number of jobs to run at the same time (same as -j, but saved)',4,None,int),
	('extraModules', 'Extra directories with their own SConscript files (must be in-tree) (whitespace separated)',None,None,Split),
	('buildPrefix','Where to create build-[version][variant] directory for intermediary files','..'),
	('version','Yade version (if not specified, guess will be attempted)',None),
	('CPPPATH', 'Additional paths for the C preprocessor (whitespace separated)',None,None,Split),
	('LIBPATH','Additional paths for the linker (whitespace separated)',None,None,Split),
	('QTDIR','Directories where to look for qt3',['/usr/share/qt3','/usr/lib/qt','/usr/lib/qt3','/usr/qt/3','/usr/lib/qt-3.3'],None,Split),
	('CXX','The c++ compiler','g++'),
	('CXXFLAGS','Additional compiler flags; you can use them for tuning like -march=pentium4.',None,None,Split), # not tested if really propagates
	BoolOption('pretty',"Don't show compiler command line (like the Linux kernel)",1),
	BoolOption('useMiniWm3','use local miniWm3 library instead of Wm3Foundation',1),
	#BoolOption('useLocalQGLViewer','use in-tree QGLViewer library instead of the one installed in system',1),
)

### create THE environment
env=Environment(tools=['default'],options=opts)
# do not propagate PATH from outside, to ensure identical builds on different machines
#env.Append(ENV={'PATH':['/usr/local/bin','/bin','/usr/bin']})
# ccache needs $HOME to be set; colorgcc needs $TERM; distcc wants DISTCC_HOSTS
# fakeroot needs FAKEROOTKEY and LD_PRELOAD
propagatedEnvVars=['HOME','TERM','DISTCC_HOSTS','LD_PRELOAD','FAKEROOTKEY','LD_LIBRARY_PATH']
for v in propagatedEnvVars:
	if os.environ.has_key(v): env.Append(ENV={v:os.environ[v]})

opts.Save(optsFile,env)
if sconsVersion>9700: opts.FormatOptionHelpText=lambda env,opt,help,default,actual,alias: "%10s: %5s [%s] (%s)\n"%(opt,actual,default,help)
else: opts.FormatOptionHelpText=lambda env,opt,help,default,actual: "%10s: %5s [%s] (%s)\n"%(opt,actual,default,help)
Help(opts.GenerateHelpText(env))

###########################################
################# BUILD DIRECTORY #########
###########################################

##ALL generated stuff should go here - therefore we must determine it very early!!

if not env.has_key('version'):
	"Attempts to get yade version from RELEASE file if it exists or from svn."
	if os.path.exists('RELEASE'):
		env['version']=file('RELEASE').readline().strip()
	if not env.has_key('version'):
		for l in os.popen("LC_ALL=C bzr version-info").readlines():
			m=re.match(r'revno: ([0-9]+)',l)
			if m: env['version']='bzr'+m.group(1)
	if not env.has_key('version'):
		for l in os.popen("LC_ALL=C svn info").readlines():
			m=re.match(r'Revision: ([0-9]+)',l)
			if m: env['version']='svn'+m.group(1)
	if not env.has_key('version'):
		env['version']='unknown'

env['SUFFIX']='-'+env['version']+env['variant']
print "Yade version is `%s', installed files will be suffixed with `%s'."%(env['version'],env['SUFFIX'])
# make buildDir absolute, saves hassles later
buildDir=os.path.abspath(env.subst('$buildPrefix/build$SUFFIX'))
print "All intermediary files will be in `%s'."%env.subst(buildDir)
env['buildDir']=buildDir
# these MUST be first so that builddir's headers are read before any locally installed ones
buildInc='$buildDir/include/yade-$version'
env.Append(CPPPATH=[buildInc])
if env['useMiniWm3']: env.Append(CPPPATH=[buildInc+'/yade/lib-miniWm3'])
#if env['useLocalQGLViewer']: env.Append(CPPPATH=[buildInc+'/yade/lib-QGLViewer'])

env.SConsignFile(buildDir+'/scons-signatures')

##########################################################################################
############# SHORTCUT TARGETS ###########################################################
##########################################################################################

if len(sys.argv)>1:
	if 'clean' in sys.argv:
		if os.path.exists(buildDir):
			print "Cleaning: %s."%buildDir; shutil.rmtree(buildDir)
		else: print "Nothing to clean: %s."%buildDir
		sys.argv.remove('clean')
	if 'tags' in sys.argv:
		cmd="ctags -R --extra=+q --fields=+n --exclude='.*' --exclude=scons-local --exclude=include --exclude='*.so' --langmap=c++:+.inl"
		print cmd; os.system(cmd)
		sys.argv.remove('tags')
	if 'doc' in sys.argv:
		cmd="cd doc; doxygen Doxyfile"
		print cmd; os.system(cmd)
		sys.argv.remove('doc')
	# still something on the command line? Ignore, but warn about that
	if len(sys.argv)>1: print "!!! WARNING: Shortcuts (clean,tags,doc) cannot be mixed with regular targets or options; ignoring:\n"+''.join(["!!!\t"+a+"\n" for a in sys.argv[1:]])
	# bail out
	Exit(0)


##########################################################################################
############# CONFIGURATION ##############################################################
##########################################################################################

# ensure non-None
env.Append(CPPPATH='',LIBPATH='',LIBS='',CXXFLAGS='')

def CheckQt(context, qtdirs):
	"Attempts to localize qt3 installation in given qtdirs. Sets necessary variables if found and returns True; otherwise returns False."
	# make sure they exist and save them for restoring if a test fails
	origs={'LIBS':context.env['LIBS'],'LIBPATH':context.env['LIBPATH'],'CPPPATH':context.env['CPPPATH']}
	for qtdir in qtdirs:
		context.Message( 'Checking for qt-mt in '+qtdir+'... ' )
		context.env['QTDIR']=qtdir
		context.env.Append(LIBS='qt-mt',LIBPATH=qtdir+'/lib',CPPPATH=qtdir+'/include' )
		ret=context.TryLink('#include<qapplication.h>\nint main(int argc, char **argv){QApplication qapp(argc, argv);return 0;}\n','.cpp')
		context.Result(ret)
		if not ret:
			for k in origs.keys(): context.env[k]=origs[k]
		else:
			return ret
	return False

def CheckPython(context):
	"Checks for functional python/c API. Sets variables if OK and returns true; otherwise returns false."
	origs={'LIBS':context.env['LIBS'],'LIBPATH':context.env['LIBPATH'],'CPPPATH':context.env['CPPPATH'],'LINKFLAGS':context.env['LINKFLAGS']}
	context.Message('Checking for Python development files... ')
	if 1:
		try:
			#FIXME: once caught, exception disappears along with the actual message of what happened...
			import distutils.sysconfig as ds
			context.env.Append(CPPPATH=ds.get_python_inc(),LIBS=ds.get_config_var('LIBS').split())
			context.env.Append(LINKFLAGS=ds.get_config_var('LINKFORSHARED').split()+ds.get_config_var('BLDLIBRARY').split())
			ret=context.TryLink('#include<Python.h>\nint main(int argc, char **argv){Py_Initialize(); Py_Finalize();}\n','.cpp')
			if not ret: raise RuntimeError
		except (ImportError,RuntimeError,ds.DistutilsPlatformError):
			for k in origs.keys(): context.env[k]=origs[k]
			context.Result(False)
			return False
	else:
		env.ParseConfig("python2.5-dbg-config --cflags")
		env.ParseConfig("python2.5-dbg-config --ldflags")
	context.Result(True)
	return True

def CheckScientificPython(context):
	context.Message('Checking for scientific python module (debian: python-scientific)... ')
	try:
		import Scientific
		context.Result(True); return True
	except ImportError:
		context.Result(False); return False


def CheckCXX(context):
	context.Message('Checking whether c++ compiler "%s" works...'%env['CXX'])
	ret=context.TryLink('#include<iostream>\nint main(int argc, char**argv){std::cerr<<std::endl;return 0;}\n','.cpp')
	context.Result(ret)
	return ret


if not env.GetOption('clean'):
	conf=env.Configure(custom_tests={'CheckQt':CheckQt,'CheckCXX':CheckCXX,'CheckPython':CheckPython,'CheckScientificPython':CheckScientificPython},
		conf_dir='$buildDir/.sconf_temp',log_file='$buildDir/config.log')

	ok=True
	ok&=conf.CheckCXX()
	if not ok:
			print "\nYour compiler is broken, no point in continuing. See `%s' for what went wrong and use the CXX/CXXFLAGS parameters to change your compiler."%(buildDir+'/config.log')
			Exit(1)
	# check essential libs
	ok&=conf.CheckLibWithHeader('pthread','pthread.h','c','pthread_exit(NULL);',autoadd=1)
	ok&=conf.CheckLibWithHeader('glut','GL/glut.h','c','glutGetModifiers();',autoadd=1)

	# gentoo has threaded flavour named differently and it must have precedence over the non-threaded one
	ok&=(conf.CheckLibWithHeader('boost_date_time-mt','boost/date_time/posix_time/posix_time.hpp','c++','boost::posix_time::time_duration::time_duration();',autoadd=1)
		or conf.CheckLibWithHeader('boost_date_time','boost/date_time/posix_time/posix_time.hpp','c++','boost::posix_time::time_duration::time_duration();',autoadd=1))
	ok&=(conf.CheckLibWithHeader('boost_thread-mt','boost/thread/thread.hpp','c++','boost::thread::thread();',autoadd=1)
		or conf.CheckLibWithHeader('boost_thread','boost/thread/thread.hpp','c++','boost::thread::thread();',autoadd=1))
	ok&=(conf.CheckLibWithHeader('boost_filesystem-mt','boost/filesystem/path.hpp','c++','boost::filesystem::path();',autoadd=1)
		or conf.CheckLibWithHeader('boost_filesystem','boost/filesystem/path.hpp','c++','boost::filesystem::path();',autoadd=1))

	if not env['useMiniWm3']: ok&=conf.CheckLibWithHeader('Wm3Foundation','Wm3Math.h','c++','Wm3::Math<double>::PI;',autoadd=1)

	if 'qt3' not in env['exclude']:
		ok&=conf.CheckQt(env['QTDIR'])
		env.Tool('qt'); env.Replace(QT_LIB='qt-mt')
		env['QGLVIEWER_LIB']='yade-QGLViewer';
		# one or another (QGLViewer is upstream name, 3dviewer is (teomporary) workaround for clashing name with obsolete package once in debian)
		# (this one has to be explicitly mentioned for each plugin that uses it, no autoadd)
		#if env['useLocalQGLViewer']:
		#	env['QGLVIEWER_LIB']='yade-QGLViewer';
		#else:
		#	if conf.CheckLibWithHeader('QGLViewer','QGLViewer/qglviewer.h','c++','QGLViewer(1);'): env['QGLVIEWER_LIB']='QGLViewer'
		#	elif conf.CheckLibWithHeader('qglviewer','QGLViewer/qglviewer.h','c++','QGLViewer(1);'): env['QGLVIEWER_LIB']='qglviewer'
		#	elif conf.CheckLibWithHeader('3dviewer','QGLViewer/qglviewer.h','c++','QGLViewer(1);'): env['QGLVIEWER_LIB']='3dviewer'
		#	else: ok=False
		#	if ok: print "!!! WARNING\n!!! Using system QGLViewer is not very well tested \n!!! and if there is version mismatch, wrong headers may be included.\n!!! If you get mysterious crashes, look here for cause. "

	if not ok:
		print "\nOne of the essential libraries above was not found, unable to continue.\n\nCheck `%s' for possible causes, note that there are options that you may need to customize:\n\n"%(buildDir+'/config.log')+opts.GenerateHelpText(env)
		Exit(1)

	# check optional libs
	if 'log4cxx' in env['features'] and conf.CheckLibWithHeader('log4cxx','log4cxx/logger.h','c++','log4cxx::Logger::getLogger("foo");',autoadd=1):
		env.Append(CPPDEFINES=['LOG4CXX'])
	if 'python' in env['features'] and conf.CheckPython() and ( ### not needed now: and conf.CheckScientificPython() and (
		conf.CheckLibWithHeader('boost_python-mt','boost/python.hpp','c++','boost::python::scope();',autoadd=1)
		or conf.CheckLibWithHeader('boost_python','boost/python.hpp','c++','boost::python::scope();',autoadd=1)):
		env.Append(CPPDEFINES=['EMBED_PYTHON'])

	if env['useMiniWm3']: env.Append(LIBS='miniWm3',CPPDEFINES=['MINIWM3'])

	env=conf.Finish()


##########################################################################################
############# BUILDING ###################################################################
##########################################################################################

### SCONS OPTIMIZATIONS
SCons.Defaults.DefaultEnvironment(tools = [])
env.SourceSignatures('MD5') #can be  'MD5' or 'timestamp'; 'timestamp' but is less reliable and not so much faster
env.SetOption('max_drift',5) # cache md5sums of files older than 5 seconds
SetOption('implicit_cache',1) # cache #include files etc.
env.SourceCode(".",None) # skip dotted directories
SetOption('num_jobs',env['jobs'])

### SHOWING OUTPUT
if env['pretty']:
	## http://www.scons.org/wiki/HidingCommandLinesInOutput
	env.Replace(CXXCOMSTR='C ${SOURCES}', # → ${TARGET.file}')
		SHCXXCOMSTR='C ${SOURCES}',  #→ ${TARGET.file}')
		SHLINKCOMSTR='L ${SOURCES}', # → ${TARGET.file}')
		LINKCOMSTR='L ${SOURCES}', # → ${TARGET.file}')
		INSTALLSTR='⇒ $TARGET',
		QT_UICCOMSTR='U ${SOURCES}',
		QT_MOCCOMSTR='M ${SOURCES}')
else:
	env.Replace(INSTALLSTR='cp -f ${SOURCE} ${TARGET}')

### DIRECTORIES
## PREFIX must be absolute path. Why?!
env['PREFIX']=os.path.abspath(env['PREFIX'])

# paths to in-tree SConscript files
libDirs=['lib','pkg/common','pkg/dem','pkg/fem','pkg/lattice','pkg/mass-spring','pkg/realtime-rigidbody','extra','gui']
# BUT: exclude stuff that should be excluded
libDirs=[x for x in libDirs if not re.match('^.*/('+'|'.join(env['exclude'])+')$',x)]
# where are we going to be installed... pkg/dem becomes pkg-dem
instLibDirs=[os.path.join('$PREFIX','lib','yade$SUFFIX',x.replace('/','-')) for x in libDirs]
# directory for yade-$version.pc
pcDir=os.path.join('$PREFIX','lib','pkgconfig')
# will install in the following dirs (needed?)
instDirs=[os.path.join('$PREFIX','bin')]+instLibDirs+[pcDir]
# where are we going to be run - may be different (packaging)
runtimeLibDirs=[os.path.join('$runtimePREFIX','lib','yade$SUFFIX',x.replace('/','-')) for x in libDirs]

## not used for now...
#instIncludeDirs=['yade-core']+[os.path.join('$PREFIX','include','yade',string.split(x,os.path.sep)[-1]) for x in libDirs]


### PREPROCESSOR FLAGS
env.Append(CPPDEFINES=[('SUFFIX',r'\"$SUFFIX\"'),('PREFIX',r'\"$runtimePREFIX\"')])

### COMPILER
if env['debug']: env.Append(CXXFLAGS='-ggdb3',CPPDEFINES=['YADE_DEBUG'])
else: env.Append(CXXFLAGS='-O2')
if env['optimize']:
	env.Append(CXXFLAGS=Split('-O3 -ffast-math'),
		CPPDEFINES=[('YADE_CAST','static_cast'),('YADE_PTR_CAST','static_pointer_cast'),'NDEBUG'])
	# NDEBUG is used in /usr/include/assert.h: when defined, asserts() are no-ops

	# -floop-optimize2 is a gcc-4.x flag, doesn't exist on previous version
	# CRASH -ffloat-store
	# maybe not CRASH?: -fno-math-errno
	# CRASH?: -fmodulo-sched  
	#   it is probably --ffast-fath that crashes !!!
	# gcc-4.1 only: -funsafe-loop-optimizations -Wunsafe-loop-optimizations
	# sure CRASH: -ftree-vectorize
	# CRASH (one of them): -fivopts -fgcse-sm -fgcse-las (one of them - not sure which one exactly)
	# ?: -ftree-loop-linear -ftree-loop-ivcanon
	## this will fail on non-i386 archs (includeing AMD64)
	#archFlags=Split('-march=pentium4 -mfpmath=sse,387') #-malign-double')
	#env.Append(CXXFLAGS=archFlags,LINKFLAGS=archFlags,SHLINKFLAGS=archFlags)
else:
	env.Append(CPPDEFINES=[('YADE_CAST','dynamic_cast'),('YADE_PTR_CAST','dynamic_pointer_cast')])

if env['profile']: env.Append(CXXFLAGS=['-pg'],LINKFLAGS=['-pg'],SHLINKFLAGS=['-pg'])
env.Append(CXXFLAGS=['-pipe','-Wall'])

### LINKER
#env['NONPLUGIN_LIBS']=env['LIBS']
## libs for all plugins
# Investigate whether soname is useful for something. Probably not: SHLINKFLAGS=['-Wl,-soname=${TARGET.file},'-rdynamic']
env.Append(LIBS=[],SHLINKFLAGS=['-rdynamic'])

#env.Append(LIBS=['yade-core']); env.Append(SHLINKFLAGS=['-Wl,--no-undefined']);

# if this is not present, vtables & typeinfos for classes in yade binary itself are not exported; breaks plugin loading
env.Append(LINKFLAGS=['-rdynamic']) 
# makes dynamic library loading easied (no LD_LIBRARY_PATH) and perhaps faster
env.Append(RPATH=runtimeLibDirs)
# find already compiled but not yet installed libraries for linking
#env.Append(LIBPATH=[os.path.join('#',x) for x in libDirs]	# -floop-optimize2 is a gcc-4.x flag, doesn't exist on previous version
env.Append(LIBPATH=instLibDirs) # this is if we link to libs that are installed, which is the case now


### this workaround is only needed for scons<=0.96.92, will disappear soon
###  (env.Install method chokes on no-existing directories)
if sconsVersion<=9692:
	## should reside in prepareIncludes or sth similar
	def createDirs(dirList):
		for d in dirList:
			dd=env.subst(d) #d.replace('$PREFIX',env['PREFIX'])
			if not os.path.exists(dd):
				print dd
				os.makedirs(dd)
			elif not os.path.isdir(dd): raise OSError("Installation directory `%s' is a file?!"%dd)
	createDirs(instDirs)
	#createDirs(instIncludeDirs)
	
def installHeaders(prefix=None):
	"""symlink all in-tree headers into  some include directory so that we can build before headers are installed.
	If prefix is given, headers will be copied there.
	If not, include tree will be created and syumlinked in buildDir, using relative symlinks."""
	global env
	import os,string,re
	from os.path import join,split,isabs,isdir,exists,islink,isfile,sep
	if not prefix: yadeRoot=buildDir
	else: yadeRoot=prefix
	yadeInc=join(yadeRoot,'include','yade-%s'%env['version'],'yade')
	#print "CALLED, prefix=%s, yadeInc=%s"%(prefix,yadeInc)

	for root, dirs, files in os.walk('.',topdown=True):
		for d in ('.svn'): ## skip all files that are not part of sources in the proper sense!
			try: dirs.remove(d)
			except ValueError: pass
		# exclude non-lib directories (like doc, scripts, ...)
		if not re.match(r'\.[/\\](core|lib|pkg|gui|extra)[/\\]?.*',root): continue
		# exclude headers from excluded stuff
		if re.match(r'^.*/('+'|'.join(env['exclude'])+')/',root): continue
		for f in files:
			if f.split('.')[-1] in ('hpp','inl','ipp','tpp','h','mcr'):
				#m=re.match('^\./([^/]*)/.*$',root)
				m=re.match('^.*?'+sep+'((extra|core)|((gui|lib|pkg)'+sep+'.*?))(|'+sep+'.*)$',root)
				subInc=join(yadeInc,m.group(1).replace(sep,'-')) # replace pkg/lattice by pkg-lattice
				if not prefix: # local include directory: do symlinks
					if not isdir(subInc): os.makedirs(subInc)
					def relpath(pf,pt):
						"""Returns relative path from pf (path from) to pt (path to) as string.
						Last component of pf MUST be filename, not directory name. It can be empty, though. Legal pf's: 'dir1/dir2/something.cc', 'dir1/dir2/'. Illegal: 'dir1/dir2'."""
						from os.path import sep,join,abspath,split
						apfl=abspath(split(pf)[0]).split(sep); aptl=abspath(pt).split(sep); i=0
						while apfl[i]==aptl[i] and i<min(len(apfl),len(aptl))-1: i+=1
						return sep.join(['..' for j in range(0,len(apfl)-i)]+aptl[i:])
					linkName=join(subInc,f); linkTarget=relpath(linkName,join(root,f))
					if not exists(linkName): os.symlink(linkTarget,linkName)
				else: # install directory: use scons' Install facility
					env.Install(subInc,join(root,f))

def makePkgConfig(fileName):
	cflags,libs='Cflags: ','Libs: '
	cflags+=' '+' '.join(env['CXXFLAGS'])
	for df in env['CPPDEFINES']:
		if type(df)==tuple: cflags+=' -D%s=%s'%(df[0],df[1])
		else: cflags+=' -D%s'%df
	for p in env['CPPPATH']:
		if p[0]=='#': cflags+=' -I$PREFIX/include/yade$SUFFIX'
		else: cflags+=' -I%s'%p
	for l in env['SHLINKFLAGS']:
		if not l.find('soname')>=0: libs+=' '+l # skip -Wl,soname=...
	for p in env['RPATH']: libs+=' -Wl,-rpath=%s'%p
	for p in env['LIBPATH']: libs+=' -L%s'%p
	pc='Name: Yade\nDescription: Platform for dynamic mechanics\nVersion: %s\n%s\n%s\n'%(env['version'],cflags,libs)
	pc+='yadeLibDir: $PREFIX/lib/yade$SUFFIX\n'
	pc=re.sub(r'\bPREFIX\b','runtimePREFIX',pc) # fix prefix paths
	pc=re.sub(r'\bbuildDir\b','runtimePREFIX',pc) # fix include paths: divert to the installed location
	pc=env.subst(pc)+'\n'
	f=file(env.subst(fileName),'w'); f.write(pc); f.close()
	return None

# 1. symlink all headers to buildDir/include before the actual build
# 2. (unused now) instruct scons to install (not symlink) all headers to the right place as well
# 3. set the "install" target as default (if scons is called without any arguments), which triggers build in turn
# Should be cleaned up.

if not env.GetOption('clean'):
	# how to make that executed automatically??! For now, run always.
	#env.AddPreAction(installAlias,installHeaders)
	installHeaders() # install to buildDir always
	installHeaders(env.subst('$PREFIX')) # install to $PREFIX if specifically requested: like "scons /usr/local/include"
	makePkgConfig('$buildDir/yade${SUFFIX}.pc')
	env.Install(pcDir,'$buildDir/yade${SUFFIX}.pc')
	installAlias=env.Alias('install',instDirs) # build and install everything that should go to instDirs, which are $PREFIX/{bin,lib} (uses scons' Install); include pkgconfig stuff
	env.Default([installAlias,'$PREFIX'])

env.Export('env');

if env.has_key('extraModules'):
	env['yadeModules']=env['extraModules']+libDirs+['core']
	env.Append(LIBPATH=[os.path.join('#',x) for x in env['extraModules']])
else: env['yadeModules']=libDirs+['core']

#env['yadeModules']=['lib','core','extra']

# read top-level SConscript file. It is used only so that build_dir is set. This file reads all SConscripts from in yadeModules
env.SConscript(dirs=['.'],build_dir=buildDir,duplicate=0)





