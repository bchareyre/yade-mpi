import os, os.path
from pyplusplus import module_builder
from pyplusplus.decl_wrappers import *
from pyplusplus.module_builder import call_policies


#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [os.path.abspath("miniWm3Wrap-toExpose.hpp")]
                                      , working_directory=r"."
                                      , include_paths=['miniWm3','.']
                                      , define_symbols=['USE_MINIWM3'] )
# we don't need system things from wm3
mb.decls(lambda decl: 'System' in decl.name).exclude() 
#mb.decls(lambda decl: 'noexpose' in decl.name).exclude() 
# exclude casting operators
mb.decls(lambda decl: 'operator double' in decl.name).exclude()
mb.member_functions(return_type='double &').exclude()
# we would have to do some magic here
mb.member_functions(lambda decl: decl.name in ['ToAxisAngle','ToRotationMatrix']).exclude()
mb.member_functions(lambda decl: decl.name in ['FromRotationMatrix','FromAxisAngle','Slerp','SlerpExtraSpins','Intermediate','Squad','Align']).exclude()
#mb.member_function(meth).call_policies=call_policies.return_value_policy(call_policies.return_internal_reference)

#mb.print_declarations()
mb.build_code_creator(module_name='miniWm3Wrap')
mb.write_module('miniWm3Wrap.cpp')

