#!/usr/bin/scons
# coding: UTF-8
# vim:syntax=python:

#
# This is the master build file for scons (http://www.scons.org). It is experimental, though it build very well for me. Prequisities for running are having scons installed (debian & family: package scons)
#
# Type "scons -h" for yade-specific options and "scons -H" for scons' options. Note that yade options will be remembered (saved in scons.config) so that you need to specify them only for the first time. Like this, for example:
#
#	scons -j2 pretty=1 debug=0 optimize=1 profile=1 exclude=extra,fem,lattice,realtime-rigidbody,mass-spring,snow
#
# Next time, you can simply run "scons" or "scons -j4" (for 4-parallel builds) to rebuild targets that need it. IF YOU NEED TO READ CODE IN THIS FILE, SOMETHING IS BROKEN AND YOU SHOULD REALLY TELL ME.
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
if sconsVersion<9806.0 and not os.environ.has_key('NO_SCONS_GET_RECENT'):
	# sconsVersion<10200.0 
	# tgzParams=("http://dfn.dl.sourceforge.net/sourceforge/scons/scons-local-1.2.0.d20090223.tar.gz","/scons-local-1.2.0.d20090223")
	tgzParams=("http://heanet.dl.sourceforge.net/sourceforge/scons/scons-local-1.0.0.tar.gz","/scons-local-1.0.0")
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

env=Environment(tools=['default'])
profileFile='scons.current-profile'

profOpts=Variables(profileFile)
profOpts.Add(('profile','Config profile to use (predefined: default or "", opt); append ! to use it but not save for next build (in scons.current-profile)','default'))
profOpts.Update(env)
# multiple profiles - run them all at the same time
# take care not to save current profile for those parallel builds
if env['profile']=='': env['profile']='default'
# save the profile only if the last char is not !
if env['profile'][-1]=='!': env['profile']=env['profile'][:-1]
else: profOpts.Save(profileFile,env)

if ',' in env['profile']:
	profiles=env['profile'].split(',')
	import threading,subprocess
	def runProfile(profile): subprocess.call([sys.argv[0],'-Q','profile='+profile+'!'])
	profileThreads=[]
	for arg in sys.argv[2:]:
		print "WARNING: parallel-building, extra argument `%s' ignored!"%arg
	for p in profiles:
		t=threading.Thread(target=runProfile,name='profile_'+p,args=(p,))
		t.start()
		profileThreads.append(t)
	import atexit
	def killAllProfileThreads():
		for t in profileThreads:
			t.join(0.1)
	atexit.register(killAllProfileThreads)
	for t in profileThreads:
		t.join()
	Exit()

if env['profile']=='': env['profile']='default'
optsFile='scons.profile-'+env['profile']
profile=env['profile']
print '@@@ Using profile',profile,'('+optsFile+') @@@'

# defaults for various profiles
if profile=='default': defOptions={'debug':1,'variant':'','optimize':0,'openmp':False}
elif profile=='opt': defOptions={'debug':0,'variant':'-opt','optimize':1,'openmp':False}
else: defOptions={'debug':0,'optimize':0,'variant':'-'+profile,'openmp':True}


opts=Variables(optsFile)
#
# The convention now is, that
#  1. CAPITALIZED options are
#   (a) notorious shell variables (they correspons most the time to SCons environment options of the same name - like CPPPATH)
#   (b) c preprocessor macros available to the program source (like PREFIX and SUFFIX)
#  2. lowercase options influence the building process, compiler options and the like.
#
opts.AddVariables(
	### OLD: use PathOption with PathOption.PathIsDirCreate, but that doesn't exist in 0.96.1!
	('PREFIX','Install path prefix','/usr/local'),
	('runtimePREFIX','Runtime path prefix; DO NOT USE, inteded for packaging only.','$PREFIX'),
	('variant','Build variant, will be suffixed to all files, along with version (beware: if PREFIX is the same, headers of the older version will still be overwritten',defOptions['variant'],None,lambda x:x),
	BoolVariable('debug', 'Enable debugging information and disable optimizations',defOptions['debug']),
	BoolVariable('gprof','Enable profiling information for gprof',0),
	BoolVariable('optimize','Turn on heavy optimizations',defOptions['optimize']),
	BoolVariable('openmp','Compile with openMP parallelization support',defOptions['openmp']),
	ListVariable('exclude','Yade components that will not be built','none',names=['qt3','gui','extra','common','dem','fem','lattice','mass-spring','realtime-rigidbody','snow']),
	EnumVariable('PGO','Whether to "gen"erate or "use" Profile-Guided Optimization','',['','gen','use'],{'no':'','0':'','false':''},1),
	# OK, dummy prevents bug in scons: if one selects all, it says all in scons.config, but without quotes, which generates error.
	ListVariable('features','Optional features that are turned on','python,log4cxx,openGL',names=['openGL','python','log4cxx','CGAL','dummy','GTS']),
	('jobs','Number of jobs to run at the same time (same as -j, but saved)',4,None,int),
	('extraModules', 'Extra directories with their own SConscript files (must be in-tree) (whitespace separated)',None,None,Split),
	('buildPrefix','Where to create build-[version][variant] directory for intermediary files','..'),
	EnumVariable('linkStrategy','How to link plugins together','per-class',['per-class','per-pkg[broken]','monolithic','static[broken]']),
	('version','Yade version (if not specified, guess will be attempted)',None),
	('CPPPATH', 'Additional paths for the C preprocessor (whitespace separated)',None,None,Split),
	('LIBPATH','Additional paths for the linker (whitespace separated)',None,None,Split),
	('QTDIR','Directories where to look for qt3',['/usr/share/qt3','/usr/lib/qt','/usr/lib/qt3','/usr/qt/3','/usr/lib/qt-3.3'],None,Split),
	('CXX','The c++ compiler','g++'),
	('CXXFLAGS','Additional compiler flags for compilation (like -march=core2).',None,None,Split),
	('march','Architecture to use with -march=... when optimizing','native',None,None),
	#('SHLINK','Linker for shared objects','g++'),
	('SHCCFLAGS','Additional compiler flags for linking (for plugins).',None,None,Split),
	BoolVariable('QUAD_PRECISION','typedef Real as long double (=quad)',0),
	BoolVariable('pretty',"Don't show compiler command line (like the Linux kernel)",1),
	BoolVariable('useMiniWm3','use local miniWm3 library instead of Wm3Foundation',1),
	#BoolVariable('useLocalQGLViewer','use in-tree QGLViewer library instead of the one installed in system',1),
)
opts.Update(env)
opts.Save(optsFile,env)


# do not propagate PATH from outside, to ensure identical builds on different machines
#env.Append(ENV={'PATH':['/usr/local/bin','/bin','/usr/bin']})
# ccache needs $HOME to be set; colorgcc needs $TERM; distcc wants DISTCC_HOSTS
# fakeroot needs FAKEROOTKEY and LD_PRELOAD
propagatedEnvVars=['HOME','TERM','DISTCC_HOSTS','LD_PRELOAD','FAKEROOTKEY','LD_LIBRARY_PATH']
for v in propagatedEnvVars:
	if os.environ.has_key(v): env.Append(ENV={v:os.environ[v]})

# get number of jobs from DEB_BUILD_OPTIONS if defined
# see http://www.de.debian.org/doc/debian-policy/ch-source.html#s-debianrules-options
if os.environ.has_key('DEB_BUILD_OPTIONS'):
	for opt in os.environ['DEB_BUILD_OPTIONS'].split():
		if opt.startswith('parallel='):
			j=opt.split('=')[1]
			print "Setting number of jobs (using DEB_BUILD_OPTIONS) to `%s'"%j
			env['jobs']=int(j)



if sconsVersion>9700: opts.FormatOptionHelpText=lambda env,opt,help,default,actual,alias: "%10s: %5s [%s] (%s)\n"%(opt,actual,default,help)
else: opts.FormatOptionHelpText=lambda env,opt,help,default,actual: "%10s: %5s [%s] (%s)\n"%(opt,actual,default,help)
Help(opts.GenerateHelpText(env))

###########################################
################# BUILD DIRECTORY #########
###########################################
import yadeSCons
## ALL generated stuff should go here - therefore we must determine it very early!!
env['realVersion']=yadeSCons.getRealVersion()
if not env.has_key('version'):
	env['version']=env['realVersion']
if not env['realVersion']: env['realVersion']=env['version']

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

if len(sys.argv)>1 and ('clean' in sys.argv) or ('tags' in sys.argv) or ('doc' in sys.argv):
	if 'clean' in sys.argv:
		if os.path.exists(buildDir):
			print "Cleaning: %s."%buildDir; shutil.rmtree(buildDir)
		else: print "Nothing to clean: %s."%buildDir
		sys.argv.remove('clean')
	if 'tags' in sys.argv:
		cmd="ctags -R --extra=+q --fields=+n --exclude='.*' --exclude=doc --exclude=scons-local --exclude=include --exclude=debian --exclude='*.so' --langmap=c++:+.inl,c++:+.tpp,c++:+.ipp"
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
env.Append(CPPPATH='',LIBPATH='',LIBS='',CXXFLAGS='',SHCCFLAGS='')

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
			context.env.Append(CPPPATH=ds.get_python_inc(),LIBS=ds.get_config_var('LIBS').split() if ds.get_config_var('LIBS') else None)
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

def CheckPythonModule(context,modulename):
	context.Message("Checking for python module `%s' ..."%modulename)
	try:
		exec("import %s"%modulename)
		context.Result(True); return True
	except ImportError:
		context.Result(False); return False
def CheckScientificPython(context): return CheckPythonModule(context,"Scientific")
def CheckIPython(context): return CheckPythonModule(context,"IPython")
def CheckCXX(context):
	context.Message('Checking whether c++ compiler "%s" works...'%env['CXX'])
	ret=context.TryLink('#include<iostream>\nint main(int argc, char**argv){std::cerr<<std::endl;return 0;}\n','.cpp')
	context.Result(ret)
	return ret

if not env.GetOption('clean'):
	conf=env.Configure(custom_tests={'CheckQt':CheckQt,'CheckCXX':CheckCXX,'CheckPython':CheckPython,'CheckScientificPython':CheckScientificPython,'CheckIPython':CheckIPython},
		conf_dir='$buildDir/.sconf_temp',log_file='$buildDir/config.log')

	ok=True
	ok&=conf.CheckCXX()
	if not ok:
			print "\nYour compiler is broken, no point in continuing. See `%s' for what went wrong and use the CXX/CXXFLAGS parameters to change your compiler."%(buildDir+'/config.log')
			Exit(1)
	# check essential libs
	ok&=conf.CheckLibWithHeader('pthread','pthread.h','c','pthread_exit(NULL);',autoadd=1)

	# gentoo has threaded flavour named differently and it must have precedence over the non-threaded one
	def CheckLib_maybeMT(conf,lib,header,lang,func): return conf.CheckLibWithHeader(lib+'-mt',['limits.h',header],'c++',func,autoadd=1) or conf.CheckLibWithHeader(lib,['limits.h',header],lang,func,autoadd=1)
	ok&=CheckLib_maybeMT(conf,'boost_date_time','boost/date_time/posix_time/posix_time.hpp','c++','boost::posix_time::time_duration::time_duration();')
	ok&=CheckLib_maybeMT(conf,'boost_thread','boost/thread/thread.hpp','c++','boost::thread::thread();')
	ok&=CheckLib_maybeMT(conf,'boost_filesystem','boost/filesystem/path.hpp','c++','boost::filesystem::path();')
	ok&=CheckLib_maybeMT(conf,'boost_iostreams','boost/iostreams/device/file.hpp','c++','boost::iostreams::file_sink("");')
	ok&=CheckLib_maybeMT(conf,'boost_regex','boost/regex.hpp','c++','boost::regex("");')
	ok&=CheckLib_maybeMT(conf,'boost_serialization','boost/archive/archive_exception.hpp','c++','try{} catch (const boost::archive::archive_exception& e) {};')
	ok&=CheckLib_maybeMT(conf,'boost_program_options','boost/program_options.hpp','c++','boost::program_options::options_description o;')
	env['haveForeach']=conf.CheckCXXHeader('boost/foreach.hpp','<>')
	if not env['haveForeach']: print "(OK, local version will be used instead)"
	ok&=conf.CheckLibWithHeader('sqlite3','sqlite3.h','c++','sqlite3_close(0L);',autoadd=1)
	if not env['useMiniWm3']:
		conf.env.Append(CPPPATH='/usr/include/wm3') # packaged version
		ok&=conf.CheckLibWithHeader('Wm3Foundation','Wm3Math.h','c++','Wm3::Math<double>::PI;',autoadd=1)


	if not ok:
		print "\nOne of the essential libraries above was not found, unable to continue.\n\nCheck `%s' for possible causes, note that there are options that you may need to customize:\n\n"%(buildDir+'/config.log')+opts.GenerateHelpText(env)
		Exit(1)
	def featureNotOK(featureName):
		print "\nERROR: Unable to compile with optional feature `%s'.\n\nIf you are sure, remove it from features (scons features=featureOne,featureTwo for example) and build again."%featureName
		Exit(1)
	# check "optional" libs
	if 'openGL' in env['features']:
		ok=conf.CheckLibWithHeader('glut','GL/glut.h','c','glutGetModifiers();',autoadd=1)
		if not ok: featureNotOK('openGL')
		env.Append(CPPDEFINES=['YADE_OPENGL'])
	if 'GTS' in env['features']:
		env.ParseConfig('pkg-config glib-2.0 --cflags --libs');
		ok=conf.CheckLibWithHeader('gts','gts.h','c','gts_object_class();',autoadd=1)
		if not ok: featureNotOK('GTS')
		env.Append(CPPDEFINES=['YADE_GTS'])
	if 'qt3' not in env['exclude']:
		if 'openGL' not in env['features']:
			print "\nQt3 interface can only be used if openGL is enabled.\nEither add openGL to 'features' or add qt3 to 'exclude'."
			Exit(1)
		ok&=conf.CheckQt(env['QTDIR'])
		env.Tool('qt'); env.Replace(QT_LIB='qt-mt')
		env['QGLVIEWER_LIB']='yade-QGLViewer';

	if 'log4cxx' in env['features']:
		ok=conf.CheckLibWithHeader('log4cxx','log4cxx/logger.h','c++','log4cxx::Logger::getLogger("");',autoadd=1)
		if not ok: featureNotOK('log4cxx')
		env.Append(CPPDEFINES=['LOG4CXX'])
	if 'python' in env['features']:
		ok=(conf.CheckPython()
			and conf.CheckIPython() # not needed now: and conf.CheckScientificPython()
			and CheckLib_maybeMT(conf,'boost_python','boost/python.hpp','c++','boost::python::scope();')
			and conf.CheckCXXHeader(['Python.h','numpy/ndarrayobject.h'],'<>'))
		if not ok: featureNotOK('python')
		env.Append(CPPDEFINES=['YADE_PYTHON'])
	if 'CGAL' in env['features']:
		ok=cong.CheckLibWithHeader('CGAL','CGAL/Exact_predicates_inexact_constructions_kernel.h','c++','CGAL::Exact_predicates_inexact_constructions_kernel::Point_3();')
		if not ok: featureNotOK('CGAL')
	if env['useMiniWm3']: env.Append(LIBS='miniWm3',CPPDEFINES=['MINIWM3'])

	env=conf.Finish()

	if os.path.exists('../brefcom-mm.hh'):
		print "Will use full CPM model in ../brefcom-mm.hh"
		env.Append(CPPDEFINES='YADE_CPM_FULL_MODEL_AVAILABLE')

##########################################################################################
############# BUILDING ###################################################################
##########################################################################################

### SCONS OPTIMIZATIONS
SCons.Defaults.DefaultEnvironment(tools = [])
env.Decider('MD5-timestamp')
env.SetOption('max_drift',5) # cache md5sums of files older than 5 seconds
SetOption('implicit_cache',0) # cache #include files etc.
env.SourceCode(".",None) # skip dotted directories
SetOption('num_jobs',env['jobs'])

### SHOWING OUTPUT
if env['pretty']:
	## http://www.scons.org/wiki/HidingCommandLinesInOutput
	env.Replace(CXXCOMSTR='C ${SOURCES}', # → ${TARGET.file}')
		CCOMSTR='C ${SOURCES}',
		SHCXXCOMSTR='C ${SOURCES}', 
		SHCCCOMSTR='C ${SOURCES}', 
		SHLINKCOMSTR='L ${TARGET.file}',
		LINKCOMSTR='L ${TARGET.file}',
		INSTALLSTR='⇒ $TARGET',
		QT_UICCOMSTR='U ${SOURCES}',
		QT_MOCCOMSTR='M ${SOURCES}')
else:
	env.Replace(INSTALLSTR='cp -f ${SOURCE} ${TARGET}')

### DIRECTORIES
## PREFIX must be absolute path. Why?!
env['PREFIX']=os.path.abspath(env['PREFIX'])

libDirs=('extra','gui','lib','py','plugins')
# where are we going to be installed... pkg/dem becomes pkg-dem
instLibDirs=[os.path.join('$PREFIX','lib','yade$SUFFIX',x) for x in libDirs]
# where are we going to be run - may be different (packaging)
runtimeLibDirs=[os.path.join('$runtimePREFIX','lib','yade$SUFFIX',x) for x in libDirs]


### PREPROCESSOR FLAGS
env.Append(CPPDEFINES=[('SUFFIX',r'\"$SUFFIX\"'),('PREFIX',r'\"$runtimePREFIX\"')])
if env['QUAD_PRECISION']: env.Append(CPPDEFINES='QUAD_PRECISION')

### COMPILER
if env['debug']: env.Append(CXXFLAGS='-ggdb2',CPPDEFINES=['YADE_DEBUG'])
else: env.Append(CXXFLAGS='-O2')
if env['openmp']: env.Append(CXXFLAGS='-fopenmp',LIBS='gomp',CPPDEFINES='YADE_OPENMP')
if env['optimize']:
	env.Append(CXXFLAGS=Split('-O3 -march=%s'%env['march']),
		CPPDEFINES=[('YADE_CAST','static_cast'),('YADE_PTR_CAST','static_pointer_cast'),'NDEBUG'])
	# NDEBUG is used in /usr/include/assert.h: when defined, asserts() are no-ops
else:
	env.Append(CPPDEFINES=[('YADE_CAST','dynamic_cast'),('YADE_PTR_CAST','dynamic_pointer_cast')])

if env['gprof']: env.Append(CXXFLAGS=['-pg'],LINKFLAGS=['-pg'],SHLINKFLAGS=['-pg'])
env.Prepend(CXXFLAGS=['-pipe','-Wall']) # '-Wc++0x-compat' ## not know to gcc-3.*

if env['PGO']=='gen': env.Append(CXXFLAGS=['-fprofile-generate'],LINKFLAGS=['-fprofile-generate'])
if env['PGO']=='use': env.Append(CXXFLAGS=['-fprofile-use'],LINKFLAGS=['-fprofile-use'])

### LINKER
## libs for all plugins
# Investigate whether soname is useful for something. Probably not: SHLINKFLAGS=['-Wl,-soname=${TARGET.file},'-rdynamic']
env.Append(LIBS=[],SHLINKFLAGS=['-rdynamic'])

#env.Append(LIBS=['yade-core']); env.Append(SHLINKFLAGS=['-Wl,--no-undefined']);

# if this is not present, vtables & typeinfos for classes in yade binary itself are not exported; breaks plugin loading
env.Append(LINKFLAGS=['-rdynamic']) 
# makes dynamic library loading easier (no LD_LIBRARY_PATH) and perhaps faster
env.Append(RPATH=runtimeLibDirs)
# find already compiled but not yet installed libraries for linking
env.Append(LIBPATH=instLibDirs) # this is if we link to libs that are installed, which is the case now


def installHeaders(prefix=None):
	"""symlink all in-tree headers into  some include directory so that we can build before headers are installed.
	If prefix is given, headers will be copied there.
	If not, include tree will be created and syumlinked in buildDir, using relative symlinks."""
	global env
	import os,string,re
	from os.path import join,split,isabs,isdir,exists,lexists,islink,isfile,sep
	if not prefix: yadeRoot=buildDir
	else: yadeRoot=prefix
	yadeInc=join(yadeRoot,'include','yade-%s'%env['version'],'yade')

	paths = ['.']
	for root, dirs, files in os.walk('.'):
		for dir in dirs:
			if os.path.islink(os.path.join(root,dir)):
				paths.append(os.path.join(root,dir))
	for path in paths:
		for root, dirs, files in os.walk(path,topdown=True):
			for d in ('.svn'): ## skip all files that are not part of sources in the proper sense!
				try: dirs.remove(d)
				except ValueError: pass
			# exclude non-lib directories (like doc, scripts, ...)
			if not re.match(r'\.[/\\](core|lib|pkg|gui|extra)[/\\]?.*',root): continue
			if 'noSymlinkHeaders' in root: continue
			# exclude headers from excluded stuff
			if re.match(r'^.*/('+'|'.join(env['exclude'])+')/',root): continue
			for f in files:
				if f.split('.')[-1] in ('hpp','inl','ipp','tpp','h','mcr'):
					m=re.match('^.*?'+sep+'((extra|core)|((gui|lib|pkg)'+sep+'.*?))(|'+sep+'.*)$',root)
					if not m:
						print "WARNING: file %s skipped while scanning for headers (no module)"
						continue
					subInc=join(yadeInc,m.group(1).replace(sep,'-')) # replace pkg/lattice by pkg-lattice
					if not prefix: # local include directory: do symlinks
						if not isdir(subInc): os.makedirs(subInc)
						linkName=join(subInc,f); linkTarget=relpath(linkName,join(root,f))
						if not exists(linkName):
							if lexists(linkName): os.remove(linkName) # broken symlink: remove it
							os.symlink(linkTarget,linkName)
					else: # install directory: use scons' Install facility
						env.Install(subInc,join(root,f))

def relpath(pf,pt):
	"""Returns relative path from pf (path from) to pt (path to) as string.
	Last component of pf MUST be filename, not directory name. It can be empty, though. Legal pf's: 'dir1/dir2/something.cc', 'dir1/dir2/'. Illegal: 'dir1/dir2'."""
	from os.path import sep,join,abspath,split
	apfl=abspath(split(pf)[0]).split(sep); aptl=abspath(pt).split(sep); i=0
	while apfl[i]==aptl[i] and i<min(len(apfl),len(aptl))-1: i+=1
	return sep.join(['..' for j in range(0,len(apfl)-i)]+aptl[i:])

# 1. symlink all headers to buildDir/include before the actual build
# 2. (unused now) instruct scons to install (not symlink) all headers to the right place as well
# 3. set the "install" target as default (if scons is called without any arguments), which triggers build in turn
# Should be cleaned up.

if not env.GetOption('clean'):
	# how to make that executed automatically??! For now, run always.
	#env.AddPreAction(installAlias,installHeaders)
	from os.path import join,split,isabs,isdir,exists,lexists,islink,isfile,sep
	installHeaders() # install to buildDir always
	if 0: # do not install headers
		installHeaders(env.subst('$PREFIX')) # install to $PREFIX if specifically requested: like "scons /usr/local/include"
	boostDir=buildDir+'/include/yade-'+env['version']+'/boost'
	if not exists(boostDir): os.makedirs(boostDir)
	def mkSymlink(link,target):
		if not exists(link):
			if lexists(link): os.remove(link) # remove dangling symlink
			os.symlink(relpath(link,target),link)
	if not env['haveForeach']:
		mkSymlink(boostDir+'/foreach.hpp','extra/foreach.hpp_local')
	mkSymlink(boostDir+'/python','py/3rd-party/boost-python-indexing-suite-v2-noSymlinkHeaders')
	#env.InstallAs(env['PREFIX']+'/include/yade-'+env['version']+'/boost/foreach.hpp',foreachTarget)
	env.Default(env.Alias('install',['$PREFIX/bin','$PREFIX/lib'])) # build and install everything that should go to instDirs, which are $PREFIX/{bin,lib} (uses scons' Install)

env.Export('env');

######
### combining builder
#####
# http://www.scons.org/wiki/CombineBuilder
import SCons.Action
import SCons.Builder
def combiner_build(target, source, env):
	"""generate a file, that's including all sources"""
	out=""
	for src in source: out+="#include \"%s\"\n"%src.abspath
	open(str(target[0]),'w').write(out)
	return 0
env.Append(BUILDERS = {'Combine': env.Builder(action = SCons.Action.Action(combiner_build, "> $TARGET"),target_factory = env.fs.File,)})

import yadeSCons
allPlugs=yadeSCons.scanAllPlugins(None)
buildPlugs=yadeSCons.getWantedPlugins(allPlugs,env['exclude'],env['features'],env['linkStrategy'])
def linkPlugins(plugins):
	"""Given list of plugins we need to link to, return list of real libraries that we should link to."""
	ret=set()
	for p in plugins:
		ret.add(buildPlugs[p].obj)
	return list(ret)

env['linkPlugins']=linkPlugins
env['buildPlugs']=buildPlugs

# read top-level SConscript file. It is used only so that build_dir is set. This file reads all necessary SConscripts
env.SConscript(dirs=['.'],build_dir=buildDir,duplicate=0)

#################################################################################
## remove plugins that are in the target dir but will not be installed now
## only when installing without requesting special path (we would have no way
## to know what should be installed overall.
if not COMMAND_LINE_TARGETS:
	toInstall=[str(node) for node in env.FindInstalledFiles()]
	for root,dirs,files in os.walk(env.subst('$PREFIX/lib/yade${SUFFIX}')):
		for f in files:
			ff=os.path.join(root,f)
			# do not delete python-optimized files and symbolic links (lib_gts__python-module.so, for instance)
			if ff not in toInstall and not ff.endswith('.pyo') and not os.path.islink(ff):
				print "Deleting extra plugin", ff
				os.remove(ff)

#################################################################################
#### DOCUMENTATION
# must be explicitly requested to be installed, e.g.:
#    scons /usr/local/share/doc
env.Install('$PREFIX/share/doc/yade$SUFFIX-doc/',['examples','scripts','doc'])

#Progress('.', interval=100, file=sys.stderr)
