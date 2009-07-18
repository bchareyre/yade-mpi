#
# Run this file with yade to generate documentation for python modules & classes within yade:
#
#  yade-trunk yade-epydoc.py
#
#

import sys
# add the configuration file
sys.argv+=['--config=yade-epydoc.conf'] # '-v'
from epydoc.cli import cli
cli()
quit()
