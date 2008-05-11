#!/usr/bin/env python
# encoding: utf-8
#
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>

"""
Import commented python source, format it for yade.wikia.com by interspersing <source> and literal text in comments.
If given a second files (which is a c++ file), the comments will be taken from the python source,
but the code from the c++ file.

There are special marks for comments in python: "##" for empty line
or "## some text" for non-empty line (the first space is stripped)

Special marks for c++ comments are //@, but their contents is flushed;
they serve merely to synchronize with python comments.

Adjacent comment lines and adjacent code lines will be merged.

Leading tabs in the code are replaced by unicode non-breakable spaces
(<source> would otherwise eat leading spaces of the first line)

Usage 1: generate simple-scene.py formatted for wiki

   ./py2wiki simple-scene.py

Usasge 2: generate SimpleScene.cpp formatted for wiki with comments from simple-scene.py

   ./py2wiki simple-scene.py ../extra/SimpleScene.cpp 
"""

import sys,re
class Text:
	def __init__(self,txt):
		self.text=[txt]
class Code(Text): pass
class Comment(Text): pass
pg=[]
commentPatterns=(r'\s*##($|\s(.*)$)',r'\s*//@((.*))$')
codeLangs=('python','cpp')
assert(len(sys.argv)<=3)
for i in range(0,len(sys.argv)-1):
	pg.append([Comment('')])
	sys.stderr.write(str(i)+': '+sys.argv[i+1]+'\n')
	for l in open(sys.argv[i+1]):
		l=l[:-1]
		m=re.match(commentPatterns[i],l)
		if m: ll=Comment(m.group(1)[1:] if (len(m.group(1))>0 and m.group(1)[0]==' ') else m.group(1))
		else: ll=Code(l)
		if pg[i][-1].__class__==ll.__class__:
			pg[i][-1].text.append(ll.text[0])
		else: pg[i].append(ll)

# replace tabs by 8 _nonbreakable_ spaces (\xc2 in utf-8) in code
# strip leading/trailing blank lines
for p in pg:
	for l in p:
		if l.__class__==Code('').__class__:
			l.text=[ll.replace('\t',8*' ') for ll in l.text]
			while len(l.text)>0 and l.text[-1]=='': l.text=l.text[:-1]
			while len(l.text)>0 and l.text[0]=='': l.text=l.text[1:]

table=False
showCode=len(pg)-1
for i in range(len(pg[0])):
	ll=pg[0][i]
	if ll.__class__==Comment('').__class__:
		for l in ll.text: print l
	if ll.__class__==Code('').__class__:
		if table: print '<table><tr>'
		for j in (range(len(pg)) if table else [showCode]):
			print '%s<source lang="%s">'%('<td>' if table else '',codeLangs[j])
			#if pg[j][i].text[0][0]==' ': print ' ',
			for l in pg[j][i].text: print l
			print '</source>%s'%('</td>' if table else '')
		if table: print '</tr></table>'
		
	

