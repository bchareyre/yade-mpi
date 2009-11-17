
import sphinx,sys
sys.exit(sphinx.main(['sphinx-build','-b','html','-d','_build/doctrees','.','_build/html']))
