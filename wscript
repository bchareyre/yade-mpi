APPNAME='yade'
VERSION='svn1008'
srcdir='.'
blddir='_build_'

import os.path, string                                                                                                                                 
def filesInDir(dir,files): return string.join(map(lambda x: os.path.join(dir,x),files)) 

def build(bld):
	bld.add_subdirs('yade-libs')
	bld.add_subdirs('yade-core')
	bld.add_subdirs('yade-guis')
	bld.add_subdirs('yade-packages/yade-package-common')
	bld.add_subdirs('yade-packages/yade-package-dem')
	bld.add_subdirs('yade-packages/yade-package-fem')
	bld.add_subdirs('yade-packages/yade-package-lattice')
	bld.add_subdirs('yade-packages/yade-package-mass-spring')
	bld.add_subdirs('yade-packages/yade-package-realtime-rigidbody')

def configure(conf):
	conf.check_tool(['g++','Qt3'])

	def getlib(lib):
		lcfg=conf.create_library_configurator()
		lcfg.uselib=lib
		lcfg.name=lib
		lcfg.paths=['/usr/lib','/usr/local/lib']
		lcfg.run()

	getlib('glut')
	getlib('boost_date_time')
	getlib('boost_filesystem')
	getlib('boost_thread')


def set_options(opt): 
	opt.tool_options('Qt3')
	#opt.tool_options('KDE3')
	#opt.add_option('--prefix', type='string', help='installation prefix', dest='prefix')
	#for i in "execprefix datadir libdir kdedir kdeincludes kdelibs qtdir qtincludes qtlibs libsuffix".split():
	#	opt.add_option('--'+i, type='string', default='', dest=i)
