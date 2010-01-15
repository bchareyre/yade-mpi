#
# Run this file with yade to generate documentation for python modules & classes within yade:
#
#  yade-trunk yade-epydoc.py
#
#
from yade import *

import sys
# add the configuration file
# fix command-line args (epydoc would choke receiving "yade-something yade-epydoc.py --config=...", which is the case for python main; this changes to "yade-something --config=...")
sys.argv=[sys.argv[0]]+['--config=yade-epydoc.conf','-v'] # '-v'
from epydoc.cli import cli
cli()
quit()
