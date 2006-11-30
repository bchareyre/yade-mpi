#!/usr/bin/scons
# coding: UTF-8
# vim: set syntax=python

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
#
# TODO: 1. retrieve target list and append install targets dynamically; 2. configuration and option handling.


import os.path, string

def prepareIncludes():
	"""symlink all in-tree headers into the ./include directory so that we can build before headers are installed"""
	import os,string,re
	from os.path import join,split,isabs,isdir,exists
	yaderoot='.' # MUST be relative!!
	assert(not isabs(yaderoot));
	yadeinc=join(yaderoot,'include','yade')
	for root, dirs, files in os.walk(yaderoot):
		for d in ('.svn','yade-flat','include'):
			try: dirs.remove(d)
			except ValueError:pass
		for f in files:
			if f.split('.')[-1] in ('hpp','inl','ipp','tpp','h'):
				subinc=None
				for part in root.split(os.sep):
					if re.match('yade-((extra|core)|(gui|lib|package)-.*)',part):
						subinc=join(yadeinc,part)
						break
				assert(subinc)
				if not isdir(subinc): os.makedirs(subinc)
				try:
					# subinc in additional directory level, ascend one more directory...
					os.symlink(join('..','..','..',root,f),join(subinc,f));
				except OSError: pass

import os, os.path

libDirs=['yade-libs','yade-packages/yade-package-common','yade-packages/yade-package-dem','yade-packages/yade-package-fem','yade-packages/yade-package-lattice','yade-packages/yade-package-mass-spring','yade-packages/yade-package-realtime-rigidbody','yade-extra','yade-guis']
instDirs=[os.path.join('$PREFIX','bin')]+map(lambda x: os.path.join('$PREFIX','lib','yade',string.split(x,os.path.sep)[-1]),libDirs)

# this is to help the qt tool to know where it should look...
os.environ['QTDIR']='/usr/share/qt3'
env=Environment(tools=['default','qt'])
# do not propagate PATH from outside, to ensure identical builds on different machines
path=['/usr/local/bin', '/bin', '/usr/bin']
# ccache needs $HOME to be set; colorgcc needs $TERM
env.Append(ENV={'PATH':path,'HOME':os.environ['HOME'],'TERM':os.environ['TERM']})

################################################
######## these setting should be adapted...
################################################

# also look at QTDIR above...
env.Replace(QT_LIB='qt-mt')
env.Replace(CXX='ccache g++-4.0')
env.Append(CPPDEFINES='LOG4CXX')
env.Append(LIBS=['glut','boost_date_time','boost_filesystem','boost_thread','pthread','log4cxx'])

#################################################

env.Append(SHLINKFLAGS='-Wl,-soname=${TARGET.file} -rdynamic')
# if this is not present, vtables & typeinfos for classes in yade binary itself are not exported
# this breaks plugin loading
env.Append(LINKFLAGS='-rdynamic') 


env.Append(RPATH=map(lambda x: os.path.join('$PREFIX','lib','yade',string.split(x,os.path.sep)[-1]),libDirs))
env.Append(LIBPATH=map(lambda x: os.path.join('#',x),libDirs))
env.Append(CPPPATH='#/include')

## workaround; keep CPPPATH for SConscripts until I find out how to append rather than replace CPPPATH for an individual target
#env.Append(CPPFLAGS='-I/usr/include/qt3')
#env.Append(CPPFLAGS='/usr/include/qt3')

#yadeQt=env.Copy(tools=['default','qt'],QT_LIB='qt-mt')
## consider dumping yadeQt altogether, it only complicates things...

opts=Options('env.conf')
opts.Add(PathOption('PREFIX', 'Install path prefix', '/usr/local'))
opts.Add(BoolOption('DEBUG', 'Enable debugging information and disable optimizations',1))
opts.Add(BoolOption('DOUBLE_PRECISION', 'Use double precision for real numbers',0))
opts.Add(BoolOption('LOG4CXX','Use log4cxx logging framework',0))
opts.Update(env)
opts.Save('env.conf', env)



#Depends('compile','prebuild')
#Default(env.Program(target='compile'))


## http://www.scons.org/wiki/HidingCommandLinesInOutput
env.Replace(CXXCOMSTR='c++  $SOURCES → ${TARGET.file}')
env.Replace(SHCXXCOMSTR='so++ $SOURCES → ${TARGET.file}')
env.Replace(SHLINKCOMSTR='so $SOURCES → ${TARGET.file}')
env.Replace(LINKCOMSTR='a $SOURCES → ${TARGET.file}')



## this is a workaround... should reside in prepareIncludes or sth similar
for d in instDirs:
	dd=d.replace('$PREFIX',env['PREFIX'])
	if not os.path.exists(dd):
		print dd
		os.makedirs(dd)
	elif not os.path.isdir(dd): raise OSError("Installation directory `%s' is a file?!"%dd)

prepareIncludesProxy=prepareIncludes()
prebuildAlias=env.Alias('prebuild',prepareIncludesProxy)
installAlias=env.Alias('install',instDirs)
Depends(installAlias,prebuildAlias)
Default(installAlias)


Help(opts.GenerateHelpText(env))
Export('env');
SConscript(map(lambda x: os.path.join(x,'SConscript'),libDirs+['yade-core']),exports=['env'])

#['yade-/SConscript',
#	'yade-core/SConscript',
#	'yade-guis/SConscript',
#	'yade-extra/SConscript',
#	'yade-packages/yade-package-common/SConscript',
#	'yade-packages/yade-package-dem/SConscript',
#	'yade-packages/yade-package-fem/SConscript',
#	'yade-packages/yade-package-lattice/SConscript',
#	'yade-packages/yade-package-mass-spring/SConscript',
#	'yade-packages/yade-package-realtime-rigidbody/SConscript'],
#
#	exports=['yade','yadeQt'])
