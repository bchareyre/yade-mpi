#
# Run this file with yade to generate documentation for python modules & classes within yade:
#
#  yade-trunk yade-epydoc.py
#
#
from yade import *

import sys
# add the configuration file
sys.argv+=['--config=yade-epydoc.conf','-v'] # '-v'
from epydoc.cli import cli
cli()
quit()
