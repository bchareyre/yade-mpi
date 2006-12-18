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
#  2. configuration and option handling.
#
# And as usually, clean up the code, get rid of workarounds and hacks etc.


import os, os.path, string, re

def prepareIncludes(prefix=None):
	"""symlink all in-tree headers into the ./include directory so that we can build before headers are installed"""
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
					try:
						os.symlink(join('..','..','..',root,f),join(subInc,f));
					except OSError: pass # if the file already exists, symlink fails
				else:
					env.Install(subInc,join(root,f))
					#assert(subInc in instIncludeDirs)


libDirs=['yade-libs','yade-packages/yade-package-common','yade-packages/yade-package-dem','yade-packages/yade-package-fem','yade-packages/yade-package-lattice','yade-packages/yade-package-mass-spring','yade-packages/yade-package-realtime-rigidbody','yade-extra','yade-guis']
instDirs=[os.path.join('$PREFIX','bin')]+map(lambda x: os.path.join('$PREFIX','lib','yade',string.split(x,os.path.sep)[-1]),libDirs)
instIncludeDirs=map(lambda x: os.path.join('$PREFIX','include','yade',string.split(x,os.path.sep)[-1]),libDirs+['yade-core'])


# this is to help the qt tool to know where it should look...
os.environ['QTDIR']='/usr/share/qt3'
env=Environment(tools=['default','qt'])
# do not propagate PATH from outside, to ensure identical builds on different machines
path=['/usr/local/bin', '/bin', '/usr/bin']
# ccache needs $HOME to be set; colorgcc needs $TERM
env.Append(ENV={'PATH':path,'HOME':os.environ['HOME'],'TERM':os.environ['TERM'],
	'DISTCC_HOSTS':'localhost'}) #'192.168.0.101/1 localhost'})

################################################
######## these setting should be adapted...
################################################

# also look at QTDIR above...
env.Replace(QT_LIB='qt-mt')
env.Replace(CXX='ccache g++-4.0')
env.Append(CPPDEFINES=['LOG4CXX'])
env.Append(LIBS=['glut','boost_date_time','boost_filesystem','boost_thread','pthread','log4cxx','Wm3Foundation'])
# The '#' expands to build root
env.Append(CPPPATH=['#/include','/usr/local/include/wm3'])
env.Append(LIBPATH='/usr/local/lib')

#################################################

env.Append(SHLINKFLAGS='-Wl,-soname=${TARGET.file} -rdynamic')
# if this is not present, vtables & typeinfos for classes in yade binary itself are not exported
# this breaks plugin loading
env.Append(LINKFLAGS='-rdynamic') 
env.Append(CXXFLAGS=['-pipe','-Wall','-ggdb3'])


env.Append(RPATH=map(lambda x: os.path.join('$PREFIX','lib','yade',string.split(x,os.path.sep)[-1]),libDirs))
env.Append(LIBPATH=map(lambda x: os.path.join('#',x),libDirs))

## workaround; keep CPPPATH for SConscripts until I find out how to append rather than replace CPPPATH for an individual target
#env.Append(CPPFLAGS='-I/usr/include/qt3')
#env.Append(CPPFLAGS='/usr/include/qt3')

#yadeQt=env.Copy(tools=['default','qt'],QT_LIB='qt-mt')
## consider dumping yadeQt altogether, it only complicates things...

opts=Options('env.conf')
opts.Add(PathOption('PREFIX', 'Install path prefix', '/usr/local'))
opts.Add(BoolOption('DEBUG', 'Enable debugging information and disable optimizations',1))
opts.Add(BoolOption('LOG4CXX','Use log4cxx logging framework',0))
opts.Update(env)
opts.Save('env.conf', env)


if False:
	## http://www.scons.org/wiki/HidingCommandLinesInOutput
	env.Replace(CXXCOMSTR='c++  $SOURCES → ${TARGET.file}')
	env.Replace(SHCXXCOMSTR='so++ $SOURCES → ${TARGET.file}')
	env.Replace(SHLINKCOMSTR='so $SOURCES → ${TARGET.file}')
	env.Replace(LINKCOMSTR='a $SOURCES → ${TARGET.file}')
	env.Replace(INSTALLSTR='⇒ $SOURCE → $TARGET')


## this is a workaround... env.Install method likes targets to be existing directories...
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
	

prepareLocalIncludesProxy=prepareIncludes()
prepareIncludesProxy=prepareIncludes(prefix=env['PREFIX'])
prebuildAlias=env.Alias('prebuild',prepareLocalIncludesProxy)
installAlias=env.Alias('install',[instDirs,prepareIncludesProxy,os.path.join('$PREFIX','include','yade')])
Depends(installAlias,prebuildAlias)
Default(installAlias)

Help(opts.GenerateHelpText(env))
Export('env');
SConscript(map(lambda x: os.path.join(x,'SConscript'),libDirs+['yade-core']),exports=['env'])

#################################################
########### Installation
# (this is UNIX specific!)

### cut&paste from http://www.scons.org/wiki/BuildDirGlob, then modified
def enumerateDotSoNodes(dirnode, level=0):
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

