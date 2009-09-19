import os, os.path
from pyplusplus import module_builder
from pyplusplus.decl_wrappers import *
from pyplusplus.module_builder import call_policies
from pyplusplus import function_transformers as FT




#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [os.path.abspath("miniWm3Wrap-toExpose.hpp")]
                                      , working_directory=r"."
                                      , include_paths=['../../lib','../../lib/miniWm3','/usr/include']
                                      , define_symbols=['USE_MINIWM3'] )
# exclude exerything first
mb.decls().exclude()
# include what is in Wm3
mb.decls(lambda d: ('Wm3' in d.location.file_name) if d.location else False).include()


## things we want to exclude:
# we don't need system things from wm3
mb.decls(lambda decl: 'System' in decl.name).exclude() 
# do not expose instances we created just to instantiate templates
mb.decls(lambda decl: decl.name.startswith('noexpose')).exclude() 
# exclude casting operators
mb.casting_operators().exclude()
# exclude functions&operators returning double& (not representable)
mb.member_functions(return_type='double &').exclude()
mb.global_ns.operators(return_type='double &').exclude() # global_ns is a bug workaround: http://www.mail-archive.com/cplusplus-sig@python.org/msg00730.html
# exclude everything (member functions, operators, ...) taking or returning types we do not wrap
mb.decls(lambda d: 'Matrix3' in str(d) or 'Matrix2' in str(d) or 'Vector4' in str(d)).exclude()
# exclude operator[] since we implement __getitem__/__setitem__ by ourselves
mb.member_operators(lambda o: o.symbol=='[]').exclude()
##


# register manual wraps
v2,v3,q4=mb.class_(lambda d: d.name=='Vector2<double>'),mb.class_(lambda d: d.name=='Vector3<double>'),mb.class_(lambda d: d.name=='Quaternion<double>')
v2.add_registration_code('def("__len__",&::Vector2_len)   .staticmethod("__len__").def("__setitem__",&::Vector2_set_item)   .def("__getitem__",&::Vector2_get_item)   .def("__str__",&::Vector2_str)   .def("__repr__",&::Vector2_str)')
v3.add_registration_code('def("__len__",&::Vector3_len)   .staticmethod("__len__").def("__setitem__",&::Vector3_set_item)   .def("__getitem__",&::Vector3_get_item)   .def("__str__",&::Vector3_str)   .def("__repr__",&::Vector3_str)')
q4.add_registration_code('def("__len__",&::Quaternion_len).staticmethod("__len__").def("__setitem__",&::Quaternion_set_item).def("__getitem__",&::Quaternion_get_item).def("__str__",&::Quaternion_str).def("__repr__",&::Quaternion_str)')

## workarounds for def_readonly on static members
# disabled, doesn't work on types (e.g. Vector3.ZERO), must pass instance (e.g. Vector3().ZERO)
if 1:
	mb.decls(lambda d: 'UNIT_' in str(d) or 'ZERO' in str(d) or 'IDENTITY' in str(d) or 'ONE' in str(d)).exclude()
	v2.add_registration_code('.'.join('add_property("%s",::Vector2r_%s)'%(prop,prop) for prop in ('ZERO','UNIT_X','UNIT_Y','ONE')))
	v3.add_registration_code('.'.join('add_property("%s",::Vector3r_%s)'%(prop,prop) for prop in ('ZERO','UNIT_X','UNIT_Y','UNIT_Z','ONE')))
	q4.add_registration_code('.'.join('add_property("%s",::Quaternionr_%s)'%(prop,prop) for prop in ('ZERO','IDENTITY')))

# arg0 and arg1 will be returned as tuple, instead of the references ToAxisAngle takes
mb.member_functions('ToAxisAngle').add_transformation(FT.output(0),FT.output(1))
# quaternion operations returning itself
mb.member_functions(lambda mf: 'Quaternion<double> &' in str(mf.return_type)).call_policies=call_policies.return_self()

mb.add_registration_code("custom_Vector3r_from_tuple();")

if 0:
	#mb.member_functions(return_type='Wm3::Matrix3<double> &').call_policies=call_policies.return_self
	mb.member_functions('ToAngle').add_transformation(FT.output(0)) # Matrix2
	mb.member_functions(lambda mf: 'Matrix3<double> &' in str(mf.return_type)).call_policies=call_policies.return_self()
	mb.decls(lambda decl: decl.name.startswith('ToEulerAngles')).add_transformation(FT.output(0),FT.output(1),FT.output(2))

mb.build_code_creator(module_name='miniWm3Wrap')
mb.code_creator.add_include("miniWm3Wrap-funcs.ipp")
mb.write_module('miniWm3Wrap.cpp')

# cleaup unneeded generated files
toClean='exposed_decl.pypp.txt','miniWm3Wrap.cpp~','named_tuple.py'
for f in toClean:
	if os.path.exists(f):
		try:
			os.remove(f)
		except IOError: pass

