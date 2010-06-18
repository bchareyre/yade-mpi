# coding: utf-8
#
# Thanks to logilab for this code.
#
# Copied from the logilab.common.optik_ext module (GPL-licensed) and modified for yade.
#

import optparse,time

class ManHelpFormatter(optparse.HelpFormatter):
	"""Format help using man pages ROFF format"""
	def __init__(self,indent_increment=0,max_help_position=24,width=79,short_first=0):
		optparse.HelpFormatter.__init__(self, indent_increment, max_help_position, width, short_first)
	def format_heading(self, heading):
		return '.SH %s\n' % heading.upper()
	def format_description(self, description):
		return description
	def format_option(self, option):
		try:
			optstring = option.option_strings
		except AttributeError:
			optstring = self.format_option_strings(option)
		if option.help:
			help = ' '.join([l.strip() for l in option.help.splitlines()])
		else:
			help = ''
		return '''.IP "%s"
%s
''' % (optstring, help)
	def format_head(self, optparser, metadata, section=1):
		pgm = optparser.get_prog_name()
		short_desc = self.format_short_description(pgm, metadata['short_desc'])
		long_desc = self.format_long_description(pgm, metadata['long_desc'])
		return '%s\n%s\n%s\n%s' % (self.format_title(pgm, section), short_desc, self.format_synopsis(optparser), long_desc)
	def format_title(self, pgm, section):
		date = '-'.join([str(num) for num in time.localtime()[:3]])
		return '.TH %s %s "%s" %s' % (pgm, section, date, pgm)
	def format_short_description(self, pgm, short_desc):
		return '''.SH NAME
.B %s 
\- %s
''' % (pgm, short_desc.strip())
	def _markup(self, txt):
		"""Prepares txt to be used in man pages."""
		return txt.replace('-', '\\-')
	def format_synopsis(self, optparser):
		synopsis = optparser.get_usage()
		pgm=optparser.get_prog_name()                                    
		if synopsis:                                                           
			synopsis = synopsis.replace('Usage: %s '%pgm, '')                   
			return '.SH SYNOPSIS\n.B %s\n%s\n' % (self._markup(pgm),synopsis)
	def format_long_description(self, pgm, long_desc):
		long_desc = '\n'.join([line.lstrip()
								for line in long_desc.splitlines()])
		long_desc = long_desc.replace('\n.\n', '\n\n')
		if long_desc.lower().startswith(pgm):
			long_desc = long_desc[len(pgm):]
		return '''.SH DESCRIPTION
.B %s 
%s
''' % (pgm, long_desc.strip())
		 
	def format_tail(self, metadata, seealso=None):
		
		return ('.SH SEE ALSO\n%s\n'%seealso if seealso else '')+'''.SH COPYRIGHT 
%s %s

%s

.SH BUGS 
Please report bugs to the project\'s bug tracker at
.br
%s

.SH AUTHOR
%s (%s)
''' % (metadata['copyright'],metadata['author'],metadata['license'],metadata['bugtracker'],metadata['author'],metadata['author_contact'])


def generate_manpage(optparser,metadata,fileOut,seealso=None,section=1):
	"""generate a man page from an optparse parser"""
	out=open(fileOut,'w')
	formatter=ManHelpFormatter()
	formatter.parser=optparser
	out.write(formatter.format_head(optparser,metadata,section))
	out.write(optparser.format_option_help(formatter))
	out.write(formatter.format_tail(metadata,seealso))

