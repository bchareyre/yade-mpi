#!/usr/bin/env python

#
# Hacket is a character from Beckett's Watt. It is only briefly intorduced once at the beginning of Watt, 
# (IIRC) shortly before Watt hears choir singing about big fat bun for everyone.
#

#
# This script was used (once) to convert deprecated (bastardized) version of wm3 in yade to the official version
# Arguments to provide are explained below.
# Many things are hardcoded, don't ever try to use it without reading the source throughly.
# You have been warned.
#

#
# since the tags files must contain prototypes: I ran
#  ctags-exuberant --extra=+q --c++-kinds=+pf --language-force=c++ *
# in wm3's Foundation/Math directory as well as in yade-lib-wm3-math
#
# To perform the actual conversion, I did
#  ./hackett.py tags-ok tags-bastard ~/yade/trunk/yade-libs/yade-lib-wm3-math/src/yade-lib-wm3-math
# where tags-ok and tags-bastard are symlinked from their respective directories
#


import sys,os
from re import *
import string
import logging
from pprint import *
import copy

renamedTags={}
# renames that are not obvious to be guess by the first-letter-case-changing algorithm
# (they must include class name)
renamedTags[('Math::cosinus')]=('Math::Cos')
renamedTags[('Math::tangent')]=('Math::Tan')
renamedTags[('Math::sinus')]=('Math::Sin')
renamedTags[('Math::sqRoot')]=('Math::Sqrt')
renamedTags[('Math::power')]=('Math::Pow')
renamedTags[('Math::roundDown')]=('Math::Floor')
renamedTags[('Math::roundUp')]=('Math::Ceil')
renamedTags[('Math::invSqRoot')]=('Math::InvSqrt')
renamedTags[('Math::eExp')]=('Math::Exp')
renamedTags[('Math::logarithm')]=('Math::Log')

logging.basicConfig(level=logging.DEBUG)

def tagsParse(tagsFile):
	logging.info("Parsing tags file `%s'."%tagsFile)
	tags={}
	tagsFile=open(tagsFile)
	for l in tagsFile:
		l=l[:-1]
		# ctags header
		if l[0]=='!': continue

		if match('^\S*operator\s.*$',l): continue # discard operators
		if match('^.*\s[dntc]$',l): continue # discard #defines, namespaces, typedefs and classes
		#since the text field can contain \t which is otherwise field separator, pattern is clumsy but should work
		m=match('^(\S+|\S+operator \S+)\s+(\S+)\s+(/\^.*\$/;")\s+(\S+)\s+(\S+)?.*$',l)
		if not m:
			logging.warn("Line `%s' not matched"%l)
			continue
		name,file,text,type,extra=m.groups()[0:5]
		#print name,file,text,type,extra

		### this is probably not needed anymore
			# these are typedefs, like this:
			#	Wm3::Matrix2d   Wm3Matrix2.h    /^typedef Matrix2<double> Matrix2d;$/;" t       namespace:Wm3
			# we don't need these, since they weren't bastardized
		if extra[0:10]=='namespace:': continue
			# some two bizzare cases (namespace opening and something different...)
		if extra[0:5]=='file:': continue
			#if extra[0:6]!='class:':
			#	logging.warning('Skipping weird tag: %s, %s, %s, %s, %s',name,file,text,type,extra)
			#	continue
		
			## we want only prototypes and function definitions
			# perhaps they wouldn't pass through the regexp filter above anyway
		if not type in 'pf': continue
			# this would discard classes (have no '::'), but that should have been caught by the above as well...
		if name.find('::')<0: continue
			# end

		# prepend class name to the symbol name, unless it is already contained
		if extra[0:6]=='class':
			clss=extra[6:]
			if name.find(clss)!=0:
				name=clss+'::'+name
		if name[0:5]=='Wm3::': name=name[5:] # chop Wm3:: off the symbol name

		# ctags escape some metacharacters, we don't nee that
		text=text.replace('\\','')

		isAlready=False
		if tags.has_key(name):
			for t in tags[name]:
				if t[0]==type and t[1]==text and t[2]==file:
				# exact match found, skip
					isAlready=True
			if isAlready: continue

		if not tags.has_key(name): tags[name]=[]
		tags[name].append([type,text,file,False])
	return tags


good=tagsParse(sys.argv[1])
bad=tagsParse(sys.argv[2])
badDir=sys.argv[3]

#pprint(bad); pprint(good)
#for c,n in good.keys(): print c
#for c,n in bad.keys(): print c

#tags that are in bad but not in good
addedTags={}
# tags that are only renamed in bad and otherwise exist in good
# keys are (badClss,badName), values are keys in good: (goodClss,goodName)
# list of (class,name) tags, that have been kept intact
keptTags=[]


for badTag in bad.keys():
	# probably not needed
	if match('::operator.*',badTag): continue
	bb=bad[badTag]
	# if the method is lowercase and uppercased appears in the good set, it was renamed
	cls,meth=badTag.split('::')
	goodTag=cls+'::'+meth[0].upper()+meth[1:]
	if renamedTags.has_key(badTag): continue
	elif good.has_key(badTag): keptTags.append(badTag)
	elif meth[0].lower()==meth[0] and good.has_key(goodTag):
		renamedTags[badTag]=goodTag
	# otherwise, it is an added method that has no corresponding good class
	else:
		addedTags[badTag]=bb[0]

######################################################################
### uncomment this to get symbols map for hackett-warn-replace.py 

if 0:
	r={}
	for t in renamedTags.keys(): r[t.split('::')[-1]]=renamedTags[t].split('::')[-1]
	for t in sorted(r.keys()): print "\t'%s':'%s',"%(t,r[t])
	sys.exit(0)

# now accumulate changes for particular files, so that we can iterate over files conveniently
fileReplace=[]
for t in renamedTags:
	for bb in bad[t]:
		f=bb[2]
		if not f in fileReplace: fileReplace.append(f)
#pprint(fileReplace)
#pprint(renamedTags)


##############################################
######## here begins the actual work
##############################################

import shutil
from os.path import join

for fName in fileReplace:
	origFile=join(badDir,fName)
	bcupFile=origFile+'.~bastardized.0~'
	logging.info("Processing `%s'."%origFile)
	if 1: # if 1, files will be copied and written over; the next branch writes to "backup" files
		shutil.move(origFile,bcupFile)
		fout=open(origFile,'w')
		fin=open(bcupFile)
	else:
		fin=open(origFile)
		fout=open(bcupFile,'w')
	# buffer for line that has not been syntactically terminated
	leftOver=''
	written=False
	for l in fin.xreadlines():
		if len(leftOver)>0: l=leftOver+l
		written=False
		for tt in renamedTags:
			for t in bad[tt]:
				#if t[0]!='p': continue # for now, only do prototypes...
				#print t
				pat=t[1][2:-4] # line hardcopy - discard leading /^ and trailing $;/" 
				f=t[2] # filename
				if f!=fName: continue
				if not l.find(pat)<0:
					if t[3]: # there are two different tags in the same file, but their text signature is the same - presumaly argument list that differs continues on the next line etc. Since it will cause erroneous output, shout to the user
						logging.info("The method `%s' had the same tag `%s' twice (overloaded?). Please fix by hand!!!"%(tt,pat))
						continue
					# classes may be discarded from names
					badMethName=tt.split('::')[-1]
					goodMethName=renamedTags[tt].split('::')[-1]
					# badMethName is the old method inline declarationand definition that is marked as deprecated and merely wraps the new method
					# goodMethName is the new method
					# badMeth and goodMeth are corresponding source lines that we will create

					# this pattern matches both declaration and definition header. It must return return type in \1, method argument list in \2.
					methodPattern=r'^(.*?)\b'+badMethName+r'\b\s?(\s?\(.*\)\s*(const)?)\s*(;|{).*$';
					badMeth,count=subn(methodPattern,r'\1'+badMethName+r'\2',l)
					goodMeth,count2=subn(methodPattern,r'\1'+goodMethName+r'\2 \4',l)
					# match the same times for both methods (always, since pattern is the same)
					# match at most once; otherwise output might be syntactically incorrect
					assert(count==count2); assert(count<=1)
					# we will append function body (wrapper) to badMeth, get rid of newline
					badMeth=badMeth.replace('\n',''); #goodMeth=goodMeth.replace('\n','')
					badMeth="\t__attribute__((deprecated)) inline "+badMeth

					if count==0: leftOver=l[:-1] # the declaration is not terminated at this lines, process with the next line
					elif count==1:
						leftOver=''
						t[3]=True # this particular tag has been processed by us - this allows us to list unprocessed tags at the end, if there are any, since taht would probably indicate some error
						if t[0]=='p': # we are dealing with prototype, hence we define wrapper for goodMeth
							args=search(r'\((.*)\)',goodMeth); # extract arguments
							assert(args)
							a=args.group(1);
							args=args.group(1).split(',') # process args one by one
							try: args.remove('') # in case of void methods, remove empty args
							except ValueError: pass

							# begin building the wrapper
							badMeth+='{return '+goodMethName+'('
							# pass it all arguments
							for i in range(0,len(args)):
								var=args[i].split()[-1] # for every arg, only the last word (variable name), not type
								var=sub('\[[0-9]+\]$','',var) # remove array brackets after variable
								badMeth+=var
								if i<len(args)-1: badMeth+=',' # append , except after the last arg
							badMeth+=');}\n'
							fout.write(badMeth) # wrapper is on the line preceeding the actual method
						fout.write(goodMeth) # write goodMeth
						written=True
		if not written and leftOver=='': fout.write(l) # write lines that were not interesting


# list tags that have not been processed. There shouldn't be any.
for tt in renamedTags:
	for t in bad[tt]:
		if t[3]==False: logging.warning("Tag `%s' was not processed (%s)!"%(tt,t))

