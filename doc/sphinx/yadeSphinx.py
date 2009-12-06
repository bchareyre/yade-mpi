
import sphinx,sys
#import yade.qt
#yade.qt.Controller()
#yade.qt.close()
sys.exit(sphinx.main(['sphinx-build','-b','html','-d','_build/doctrees','.','_build/html']))
