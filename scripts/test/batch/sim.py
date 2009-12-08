from yade import *
from yade import utils
utils.readParamsFromTable(unknownOk=True,
	important=6,
	unimportant='foo',
	this=-1,
	notInTable='notInTable'
)
print O.tags['description']
print 'important',important
print 'unimportant',unimportant
#print O.tags['params']
#print O.tags['defaultParams']
import time
#time.sleep(5)
print 'finished'
import sys
sys.stdout.flush()
sys.exit(0)
