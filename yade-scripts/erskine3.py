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
import pprint

def warning2(msg): warning(currentPro+' '+msg)

bgroup=('(','{','[')
egroup=(')','}',']')
# variables that do not propagate to sub-builds
localVars=['TEMPLATE','SUBDIRS','HEADERS','SOURCES','FORMS','LEXSOURCES','YACCSOURCES','TARGET','DESTDIR','PROJECT']
# variables / function that are ignored if undefined (replaced by ''); for all other, a warning is printed (and are ignored anyway)
undefOK=['CXXPATH','CXX','CXXFLAGS','YADE_QQMAKE_PATH','INCPATH','IDL_COMPILER']
# internal qmake variables that may be replace with the environment ones without warning
replaceOK=['YADECOMPILATIONPATH'] 
discardedStatements=['^isEmpty\s*\(\s*YADE_QMAKE_PATH\s*\).*$']
#discardedStatements=[]

targetLangType={'yade-lib-serialization-qt':'qt3','QtGUI':'qt3'}
# used to be yadeQt, but it was dropped...
targetEnv={'yade-lib-serialization-qt':'env','QtGUI':'env'}
# was: yade
defaultEnv='env'

currentPro='[none]'

allEnvs=[defaultEnv]
for t in targetEnv.keys():
	if not targetEnv[t] in allEnvs: allEnvs.append(targetEnv[t])
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
	#pprint.pprint(qm)

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
					if not v in undefOK: warning2("Undefined environment variable `%s'."%(v))
					repl=''
			else:
				if v[0]=='(': v=v[1:-1]
				if vars.has_key(v): repl=vars[v]
				else:
					if not v in undefOK:
						if os.environ.has_key(v):
							repl=os.environ[v]
							if not v in replaceOK: warning2("Undefined internal variable `%s', using environment variable instead."%(v))
						else:
							warning2("Undefined internal variable `%s'."%(v))
							repl=''
					else: repl=''
			stat=stat[:m.start()]+repl+stat[m.end():]

		conditional=match(r'^\s*(!?[a-zA-Z0-9_]+)\s*{(.*)}$',stat) # this is a (possibly negated) conditional; nothing complicated, please; specifically, nested conditionals do not work
		if conditional:
			cond,then=conditional.group(1,2)
			if cond=='win32': continue # windows stuff is discarded completely
			elif cond=='!win32': stat=then # otherwise we pass the whole then-clause to further treatment
			else: warning2("Unknown condition `%s'."%cond)
		assign=match(r'^\s*([a-zA-Z0-9_.]+)\s*(=|\+=)\s*(.*)$',stat) # this line is an assignment
		call=match(r'^\s*(!?[a-z][a-zA-Z0-9_]+)\s*(\(.*)$',stat) # this line is a "function call"
		if assign:
			param,action,val=assign.group(1,2,3)
			val=sub('\s+$','',val)
			val=splitgroup(sub('\s+',' ',val))
			if not vars.has_key(param): vars[param]=[]
			if action =='=' or action=='+=':	vars[param]+=val
		elif call:
			func,rest=call.group(1,2)
			if not func in undefOK: warning2("Function `%s' not implemented; arguments `%s'.\n"%(func,rest))
		else:
			warning2("Line not parsed: `%s'.\n"%(stat))
	return vars

def proProcess(filename,_vars={},dir='.',nest=0):
	info("(%s) Processing project file `%s'."%(nest,filename))
	global currentPro
	currentPro=filename
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
			#warning("Template is `subdirs' but SUBDIRS is empty.")
			return None
		for subdir in childVars['SUBDIRS']:
			#print "dir=%s,subdir=%s"%(dir,subdir)
			subPro=dir+'/'+subdir+'/'+subdir.split('/')[-1]+'.pro' # .pro file is named the same as its immediate parent directory + .pro
			posterityVars.append(proProcess(subPro,_vars=childVars,dir=dir+'/'+subdir,nest=nest+1)) # recurse
		return [posterityVars]
	elif template=='app' or 'lib':
		guessedTarget=os.path.split(filename)[-1].split('.')[0] # garbage/foo.pro -> foo
		explicitTarget=None
		if childVars.has_key('TARGET'): explicitTarget=os.path.split(childVars['TARGET'][0])[1].split('.')[0]
		if explicitTarget: childVars['TARGET']=explicitTarget
		else: childVars['TARGET']=guessedTarget
		#if oldTarget!=newTarget: warning("Old and new target differ: `%s' vs. `%s' (using new one)."%(oldTarget,newTarget))
		return [childVars]
	else:
		warning2("%s: Unknown TEMPLATE `%s'"%(currentPro,template))
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

def pythonicTarget(t):
	#return target name such that it is acceptable as python identifier
	return sub('(^[0-9]|^|[^a-zA-Z0-9_])','_',t)


def scriptGen(v,dir):
	project=v['PROJECT']
	global currentPro
	currentPro=project
	target=v['TARGET']
	commentOut=False # waf doesn't like targetless sources; scons is fine with that
	template=v['TEMPLATE'][0]

	###################
	### PATHS

	projAbsPath=realpath(normpath(dirname(project)))
	dirAbsPath=realpath(normpath(dir))
	prefix=commonprefix([projAbsPath,dirAbsPath])
	relPath=projAbsPath[len(prefix)+1:]
	#warning("PATHS:\n%s\n%s →\n%s"%(projAbsPath,dirAbsPath,relPath))
	assert(not isabs(relPath))
	#return 'PROJECT=%s\nTARGET=%s\nSOURCES=%s\nINCLUDES=%s\nrelPath=%s\n'%(v['PROJECT'],v['TARGET'],string.join(v['SOURCES']),string.join(v['INCLUDEPATH']),relPath)

	###################
	### SOURCES

	if v.has_key('SOURCES'): sources=v['SOURCES']
	else: sources=[];
	if v.has_key('FORMS'): sources+=v['FORMS']
	#if v.has_key('IDLS'): sources+=v['IDLS']
	sources=listUnique(sources)

	###################
	### LIBRARIES
	
	def prependDirFile(d,f): return map(lambda x: join(d,x),f)
	def listLibs(ll):
		ret=[]
		for l in ll:
			if l[0:2]=='-l': ret.append(l[2:])
			elif l=='-rdynamic': pass
			else:
				warning2("Unknown LIBS item `%s'."%l)
		return ret
	libs=[]
	if v.has_key('LIBS'): libs=listLibs(v['LIBS'])

	###################
	### INCLUDE PATHS

	# waf/scons don't compile from the current directory, make sure it is there; duplicata removed later
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
	# filter out non-existent paths; without warning, it could be a short lambda...
	def DelAndWarnNonexistentPath(x):
		if not exists(normpath(join(dirAbsPath,x))):
			warning2("Include path `%s' is invalid, removed!"%(normpath(join(dirAbsPath,x))))
			return False
		return True
	includePath=filter(DelAndWarnNonexistentPath,includePath)



	# begin building the actual script
	ret="## %s\n"%v['PROJECT']

	if template=='lib': 
		if v.has_key('CONFIG') and 'dll' in v['CONFIG']: targetType='shlib'
		else: targetType='staticlib'
	elif template=='app': targetType='program'

	if buildEngine=='waf':
		assert(targetType) # "TEMPLATE is neither `lib' nor `app'.

		if not len(sources)==0:
			warning2("Project `%s' has empty source list, build rule will be commented out."%project)
			commentOut=True

		langType='cpp'
		localLibs,systemLibs=[],[]
		if target in targetLangType: langType=targetLangType[target]
		if langType=='qt3': systemLibs.append('QT3')

		ret+="obj=bld.create_obj('%s','%s')\n"%(langType,targetType)

		ret+="obj.name='%s'\n"%target
		ret+="obj.target='%s'\n"%target
		
		ret+="obj.source='%s'\n"%string.join(prependDirFile(relPath,sources))

		# FIXME: install not yet there
		#ret+="obj.install_in='%s'\n"%
		
		#includePath=filter(lambda x: exists(normpath(join(dirAbsPath,x))),includePath)

		ret+="obj.includes='%s'\n"%string.join(includePath)
		# libs
		if len(libs)>0:
			for l in libs:
				if l in allSystemLibs: systemLibs.append(l)
				else: localLibs.append(ll)
		if len(localLibs)>0:	ret+="obj.uselib_local='%s'\n"%(string.join(localLibs))
		if len(systemLibs)>0: ret+="obj.uselib='%s'\n"%(string.join(systemLibs))
	elif buildEngine=='scons':
		env=defaultEnv
		if target in targetEnv: env=targetEnv[target]
		# assign variable to each target, so that it can be passed to the Install method later	
		# uncomment this and look for EXPLICIT_INSTALL below as well
		# ret+="%s="%pythonicTarget(target)
		ret+="%s.%s('%s',%s%s"%(env,{'program':'Program','shlib':'SharedLibrary','staticlib':'StaticLibrary'}[targetType],
			target,fieldSep,toStr(prependDirFile(relPath,sources)))
		if installable.has_key((env,targetType)): installable[(env,targetType)].append(pythonicTarget(target))
		else: installable[(env,targetType)]=[pythonicTarget(target),]
		if len(libs)>0:
			ret+=",%sLIBS=%s['LIBS']+%s"%(fieldSep,env,toStr(libs))
		if len(includePath)>0:
			# using CPPPATH would override top-level settings (question posted on scons ML how to avoid this)
			# for now, put all paths prefixed with -I to CPPFLAGS as workaround
			#ret+=",%sCPPFLAGS=%s"%(fieldSep,toStr(map(lambda x: '-I'+x,includePath)))
			### NO, that doesn't work since CPPPATH is relative to SConscript dir, whereas CPPFLAGS are not prepended (logically)
			### try the inverse:
			ret+=",%sCPPPATH=%s['CPPPATH']+%s"%(fieldSep,env,toStr(includePath))
		ret+=')'
		
	else: raise ValueError('buildEngine must be waf or scons (--waf or --scons)');


	if commentOut:
		ret='# This rule is commented out because it has no source'+sub('(^|\n)','\n#',ret)

	ret+='\n'
	return ret


def masterScriptGen(V,dir):
	#
	s=''
	if buildEngine=='scons':
		s+="Import('*')\n"
		s+=processVars(V,dir)
		for env,ttype in installable.keys(): # one of shlib, staticlib, program
			if len(installable[(env,ttype)])==0: continue
			# commented temporarily
			# if you need this back, uncomment also the other lines marked by EXPLICIT_INSTALL in comment
			# s+="\n%s.Install('%s',source=[%s])\n"%(env,installDirs[ttype],string.join(installable[(env,ttype)],','))
	elif buildEngine=='waf':
		s+=processVars(V,dir)
	return s


assert(len(sys.argv)>=3)
buildEngine,projects,scriptDir=sys.argv[1][2:],sys.argv[2:-1],sys.argv[-1]
assert(buildEngine in ['waf','scons'])
allVars=[]
for project in projects:
	allVars.append(proProcess(project,dir=dirname(project)))

# HACK: useful stuff to pass down via globals...
pretty=True
if pretty:
	toStr=pprint.pformat; fieldSep='\n'
else:
	toStr=str; fieldSep='';
installable={} # hash indexed by (env,targetType)->pythonicTargetName
installDirs={'shlib':join('$PREFIX','lib','yade',string.split(scriptDir,os.sep)[-1]),'staticlib':join('$PREFIX','lib','yade'),'program':join('$PREFIX','bin')}

print masterScriptGen(allVars,scriptDir)

