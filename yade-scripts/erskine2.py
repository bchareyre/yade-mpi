#!/usr/bin/env python
# -*- Encoding: utf-8 -*-

"""
This program should really be run by the erskine2-apply.sh script

Erskine is Watt's colleague in the house of Mr. Nutting.

"""

import sys,pprint,string,os#,os.path
from logging import *
from os.path import *
from re import *
from copy import deepcopy

bgroup=('(','{','[')
egroup=(')','}',']')
# variables that do not propagate to sub-builds
localVars=['TEMPLATE','SUBDIRS','HEADERS','SOURCES','FORMS','LEXSOURCES','YACCSOURCES','TARGET','DESTDIR','PROJECT']
# variables / function that are ignored if undefined (replaced by ''); for all other, a warning is printed (and are ignored anyway)
undefOK=['CXXPATH','CXX','CXXFLAGS','YADE_QQMAKE_PATH','INCPATH']
discardedStatements=['^isEmpty\s*\(\s*YADE_QMAKE_PATH\s*\).*$']
#discardedStatements=[]

targetLangType={'yade-lib-serialization-qt':'qt3','QtGUI':'qt3'}
allSystemLibs=['glut','boost_date_time','boost_filesystem','boost_thread']


def splitgroup(s,delims=' '):
	"Split string at delims, but do not break parenthesized/bracketed/curly-bracketed groups"
	nest=0;ret=[];lastsplit=0
	for i in range(0,len(s)):
		if s[i] in bgroup: nest+=1
		elif s[i] in egroup: nest-=1
		elif (nest==0 and s[i] in delims):
			ret.append(s[lastsplit:i])
			lastsplit=i+1
	ret.append(s[lastsplit:len(s)])
	return ret

def parseProject(proFile,inheritedVariables={}):
	"""Returns dictionary of variables defined in this qmake project, replacing variables, that may propagate from parent project.
	No functions, only a few conditionals are expanded.
	
	An important 'parser' difference is that qmake obviously allows \\ continuation _after_ #.
	Since it is not nice at all, it doesn't work here. Also, it would be difficult to add."""

	vars=inheritedVariables

	# sanitize input: no comments, no \\ linebreaks (join lines), no empty lines, no leading/trailing whitespace
	qm=[]; l=''; nest=0
	for line in proFile:
		m=match(r'^\s*([^#]*)(#.*)?$',line[:-1])
		l+=m.group(1)
		l=l.rstrip()
		beginnest=nest
		if len(l)==0: continue
		for c in l:
			if c in bgroup:nest+=1
			elif c in egroup:nest-=1
		if l[-1]!='\\' and nest==0:
			qm.append(l)
			l=''
		else:
			if l[-1]=='\\': l=l[:-1]
			nest=beginnest

	# process statements sequentially, replace variables / functions
	for stat in qm:
		# discard what should be discarded
		discard=False
		for pat in discardedStatements:
			if match(pat,stat):
				discard=True; break
		if discard: continue
		#replace environment and qmake variables
		while True:
			m=search(r'\$\$?([a-zA-Z0-9_]+\b|\([a-zA-Z0-9_]+\)|\{[a-zA-Z0-9_]+\})',stat) # possible patters of variables
			if not m: break

			v=m.group(1)
			if v[0]=='{':
				v=v[1:-1]
				if os.environ.has_key(v):
					repl=os.environ[v]
					info("Substituting environment variable `%s` → `%s`"%(v,repl))
				else:
					if not v in undefOK: warning("Undefined environment variable `%s'."%(v))
					repl=''
			else:
				if v[0]=='(': v=v[1:-1]
				if vars.has_key(v): repl=vars[v]
				else:
					if not v in undefOK:
						if os.environ.has_key(v):
							repl=os.environ[v]
							warning("Undefined internal variable `%s', using environment variable instead."%(v))
						else:
							warning("Undefined internal variable `%s'."%(v))
							repl=''
					else: repl=''
			stat=stat[:m.start()]+repl+stat[m.end():]

		conditional=match(r'^\s*(!?[a-zA-Z0-9_]+)\s*{(.*)}$',stat) # this is a (possibly negated) conditional; nothing complicated, please; specifically, nested conditionals do not work
		if conditional:
			cond,then=conditional.group(1,2)
			if cond=='win32': continue # windows stuff is discarded completely
			elif cond=='!win32': stat=then # otherwise we pass the whole then-clause to further treatment
			else: warning("Unknown condition `%s'."%cond)
		assign=match(r'^\s*([a-zA-Z0-9_.]+)\s*(=|\+=)\s*(.*)$',stat) # this line is an assignment
		call=match(r'^\s*([a-z][a-zA-Z0-9_]+)\s*(\(.*)$',stat) # this line is a "function call"
		if assign:
			param,action,val=assign.group(1,2,3)
			val=sub('\s+$','',val)
			val=splitgroup(sub('\s+',' ',val))
			if not vars.has_key(param): vars[param]=[]
			if action =='=' or action=='+=':	vars[param]+=val
		elif call:
			func,rest=call.group(1,2)
			if not func in undefOK: warning("Function `%s' not implemented; arguments `%s'.\n"%(func,rest))
		else:
			warning("Line not parsed: `%s'.\n"%(stat))
	return vars

def proProcess(filename,_vars={},dir='.',nest=0):
	info("(%s) Processing project file `%s'."%(nest,filename))
	posterityVars=[]
	vars=deepcopy(_vars)
	for l in localVars:
		if vars.has_key(l): del vars[l] # these are NOT propagated to sub-builds
	childVars=parseProject(open(filename,'r'),vars)
	childVars['PROJECT']=filename
	assert(childVars.has_key('TEMPLATE')) # otherwise the file is broken
	assert(len(childVars['TEMPLATE'])==1) # dtto
	template=childVars['TEMPLATE'][0]
	if template=='subdirs': #
		if not childVars.has_key('SUBDIRS'):
			warning("Template is `subdirs' but SUBDIRS is empty.")
			return None
		for subdir in childVars['SUBDIRS']:
			#print "dir=%s,subdir=%s"%(dir,subdir)
			subPro=dir+'/'+subdir+'/'+subdir.split('/')[-1]+'.pro' # .pro file is named the same as its immediate parent directory + .pro
			posterityVars.append(proProcess(subPro,_vars=childVars,dir=dir+'/'+subdir,nest=nest+1)) # recurse
		return [posterityVars]
	elif template=='app' or 'lib':
		childVars['TARGET']=os.path.split(filename)[-1].split('.')[0] # garbage/foo.pro -> foo
		return [childVars]
	else:
		warning("Unknown TEMPLATE `%s'"%template)
		return None

def listUnique(List):
	if len(List)<=1: return List
	List.sort()
	last=List[-1]
	for i in range(len(List)-2, -1, -1):
		if last==List[i]: del List[i]
		else: last=List[i]
	return List

def processVars(V,dir):
	ret=""
	for v in V:
		if isinstance(v,list):
			ret+=processVars(v,dir)
		if isinstance(v,dict):
			ret+=scriptGen(v,dir)
	return ret

def scriptGen(v,dir):
	project=v['PROJECT']
	target=v['TARGET']
	commentOut=False
	if not v.has_key('SOURCES'):
		v['SOURCES']=[]
		warning("Project `%s' has empty source list, build rule will be commented out."%project)
		commentOut=True
	sources=v['SOURCES']
	template=v['TEMPLATE'][0]
	projAbsPath=realpath(normpath(dirname(project)))
	dirAbsPath=realpath(normpath(dir))
	prefix=commonprefix([projAbsPath,dirAbsPath])
	relPath=projAbsPath[len(prefix)+1:]
	#warning("PATHS:\n%s\n%s →\n%s"%(projAbsPath,dirAbsPath,relPath))
	assert(not isabs(relPath))
	#return 'PROJECT=%s\nTARGET=%s\nSOURCES=%s\nINCLUDES=%s\nrelPath=%s\n'%(v['PROJECT'],v['TARGET'],string.join(v['SOURCES']),string.join(v['INCLUDEPATH']),relPath)
	if template=='lib': 
		if v.has_key('CONFIG') and 'dll' in v['CONFIG']: targetType='shlib'
		else: targetType='staticlib'
	elif template=='app': targetType='program'
	assert(targetType) # "TEMPLATE is neither `lib' nor `app'.

	# begin building the actual script
	ret="##\n## originating project file: `%s'.\n##\n"%v['PROJECT']

	langType='cpp'
	localLibs,systemLibs=[],[]
	if target in targetLangType: langType=targetLangType[target]
	if langType=='qt3': systemLibs.append('QT3')

	ret+="obj=bld.create_obj('%s','%s')\n"%(langType,targetType)

	ret+="obj.name='%s'\n"%target
	ret+="obj.target='%s'\n"%target
	def prependDirFile(d,f): return map(lambda x: join(d,x),f)
	if v.has_key('FORMS'): sources+=v['FORMS']
	#if v.has_key('IDLS'): sources+=v['IDLS']
	sources=listUnique(sources)
	
	ret+="obj.source='%s'\n"%string.join(prependDirFile(relPath,sources))

	# FIXME: install not yet there
	#ret+="obj.install_in='%s'\n"%
	
	# waf doesn't compile from teh current directory, make sure it is there; duplicata removed later
	if not v.has_key('INCLUDEPATH'): includePath=['.']
	else:	includePath=v['INCLUDEPATH']+['.']
	includePath=listUnique(includePath)
	#print "relPath=%s,INCLUDEPATH=%s"%(relPath,includePath)
	def prependDirFileIfRelative(d,f):
		def prepIf(ff):
			if not isabs(ff): return join(d,ff)
			return ff
		return map(prepIf,f)
	includePath=prependDirFileIfRelative(relPath,includePath)
	# filter out non-existent paths
	# without warning, it could be a short lambda...
	def DelAndWarnNonexistentPath(x):
		if not exists(normpath(join(dirAbsPath,x))):
			warning("Include path `%s' is invalid, removed!"%(normpath(join(dirAbsPath,x))))
			return False
		return True
	includePath=filter(DelAndWarnNonexistentPath,includePath)
	#includePath=filter(lambda x: exists(normpath(join(dirAbsPath,x))),includePath)

	ret+="obj.includes='%s'\n"%string.join(includePath)
	# libs
	if v.has_key('LIBS'):
		libs=v['LIBS']
		for l in libs:
			if l[0:2]=='-l':
				ll=l[2:]
				if ll in allSystemLibs: systemLibs.append(ll)
				else: localLibs.append(ll)
			elif l=='-rdynamic': pass
			else: warning("Unknown LIBS item `%s'."%l)
	if len(localLibs)>0:	ret+="obj.uselib_local='%s'\n"%(string.join(localLibs))
	if len(systemLibs)>0: ret+="obj.uselib='%s'\n"%(string.join(systemLibs))

	if commentOut:
		ret='# This rule is commented out because it has no source'+sub('(^|\n)','\n#',ret)

	ret+='\n'
	return ret


def masterScriptGen(V,dir):
	#
	s=processVars(V,dir)
	return s


assert(len(sys.argv)>=2)
sys.argv,scriptDir=sys.argv[1:-1],sys.argv[-1]
allVars=[]
for project in sys.argv:
	allVars.append(proProcess(project,dir=dirname(project)))
print masterScriptGen(allVars,scriptDir)

