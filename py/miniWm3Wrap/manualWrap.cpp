// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<boost/python.hpp>
#include<boost/lexical_cast.hpp>
#include<string>
#include<stdexcept>
#include<sstream>

#include<yade/lib-base/Math.hpp>

namespace bp=boost::python;

#define IDX_CHECK(i,MAX){ if(i<0 || i>=MAX) { PyErr_SetString(PyExc_IndexError, ("Index out of range 0.." + boost::lexical_cast<std::string>(MAX-1)).c_str()); bp::throw_error_already_set(); } }
#define IDX2_CHECKED_TUPLE_INTS(tuple,max2,arr2) {int l=bp::len(tuple); if(l!=2) { PyErr_SetString(PyExc_IndexError,"Index must be integer or a 2-tuple"); bp::throw_error_already_set(); } for(int _i=0; _i<2; _i++) { bp::extract<int> val(tuple[_i]); if(!val.check()) throw std::runtime_error("Unable to convert "+boost::lexical_cast<std::string>(_i)+"-th index to int."); int v=val(); IDX_CHECK(v,max2[_i]); arr2[_i]=v; }  }

void Vector2_set_item(Vector2r & self, int idx, Real value){ IDX_CHECK(idx,2); self[idx]=value; }
void Vector3_set_item(Vector3r & self, int idx, Real value){ IDX_CHECK(idx,3); self[idx]=value; }
void Quaternion_set_item(Quaternionr & self, int idx, Real value){ IDX_CHECK(idx,4);  if(idx==0) self.w()=value; else if(idx==1) self.x()=value; else if(idx==2) self.y()=value; else if(idx==3) self.z()=value; }
void Matrix3_set_item(Matrix3r & self, bp::tuple _idx, Real value){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); self(idx[0],idx[1])=value; }
void Matrix3_set_item_linear(Matrix3r & self, int idx, Real value){ IDX_CHECK(idx,9); self(idx/3,idx%3)=value; }

Real Vector2_get_item(const Vector2r & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }
Real Vector3_get_item(const Vector3r & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
Real Quaternion_get_item(const Quaternionr & self, int idx){ IDX_CHECK(idx,4); if(idx==0) return self.w(); if(idx==1) return self.x(); if(idx==2) return self.y(); return self.z(); }
Real Matrix3_get_item(Matrix3r & self, bp::tuple _idx){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); return self(idx[0],idx[1]); }
Real Matrix3_get_item_linear(Matrix3r & self, int idx){ IDX_CHECK(idx,9); return self(idx/3,idx%3); }

std::string Vector2_str(const Vector2r & self){ return std::string("Vector2(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Vector3_str(const Vector3r & self){ return std::string("Vector3(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Quaternion_str(const Quaternionr & self){ AngleAxisr aa(angleAxisFromQuat(self)); return std::string("Quaternion((")+boost::lexical_cast<std::string>(aa.axis()[0])+","+boost::lexical_cast<std::string>(aa.axis()[1])+","+boost::lexical_cast<std::string>(aa.axis()[2])+"),"+boost::lexical_cast<std::string>(aa.angle())+")";}
std::string Matrix3_str(const Matrix3r & self){ std::ostringstream oss; oss<<"Matrix3("; for(int i=0; i<3; i++) for(int j=0; j<3; j++) oss<<self(i,j)<<((i==2 && j==2)?")":",")<<((i<2 && j==2)?" ":""); return oss.str(); }

int Vector2_len(){return 2;}
int Vector3_len(){return 3;}
int Quaternion_len(){return 4;}
int Matrix3_len(){return 9;}
#undef IDX_CHECK

#if 0
	// workarounds for static methods; disabled for now, see comment in miniWm3Wrap-generate.py
	#define _WORKAROUND(clss,member) clss clss##_##member(const clss& self){return clss::member;}
	_WORKAROUND(Vector2r,ONE); _WORKAROUND(Vector2r,UNIT_X); _WORKAROUND(Vector2r,UNIT_Y); _WORKAROUND(Vector2r,ZERO);
	_WORKAROUND(Vector3r,ONE); _WORKAROUND(Vector3r,UNIT_X); _WORKAROUND(Vector3r,UNIT_Y); _WORKAROUND(Vector3r,UNIT_Z); _WORKAROUND(Vector3r,ZERO);
	_WORKAROUND(Quaternionr,IDENTITY); _WORKAROUND(Quaternionr,ZERO);
	_WORKAROUND(Matrix3r,IDENTITY); _WORKAROUND(Matrix3r,ZERO);
	#undef _WORKAROUND
#endif

// automagic converter from tuple to Vector3r
struct custom_Vector3r_from_tuple{
	custom_Vector3r_from_tuple(){
		bp::converter::registry::push_back(&convertible,&construct,bp::type_id<Vector3r>());
	}
	static void* convertible(PyObject* obj_ptr){
		if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=3) return 0;
		return obj_ptr;
	}
	static void construct(PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data){
		void* storage=((bp::converter::rvalue_from_python_storage<Vector3r>*)(data))->storage.bytes;
		new (storage) Vector3r(bp::extract<Real>(PySequence_GetItem(obj_ptr,0)),bp::extract<Real>(PySequence_GetItem(obj_ptr,1)),bp::extract<Real>(PySequence_GetItem(obj_ptr,2)));
		data->convertible=storage;
	}
};


//static bp::tuple Quaternion_ToAxisAngle(const Quaternionr& q){
//	AngleAxisr aa(angleAxisFromQuat(q));
//    return bp::make_tuple(aa.axis(),aa.angle());
//}
static Quaternionr Quaternionr_setFromTwoVectors(Quaternionr& q, const Vector3r& u, const Vector3r& v){ return q.setFromTwoVectors(u,v); }
static Real Vector3r_dot(const Vector3r& self, const Vector3r& v){ return self.dot(v); }
static Real Vector2r_dot(const Vector2r& self, const Vector2r& v){ return self.dot(v); }
static Vector3r Vector3r_cross(const Vector3r& self, const Vector3r& v){ return self.cross(v); }
#define EIG_WRAP_METH1(klass,meth) static klass klass##_##meth(const klass& self){ return self.meth(); }
#define EIG_WRAP_METH0(klass,meth) static klass klass##_##meth(){ return klass().meth(); }
EIG_WRAP_METH1(Matrix3r,transpose);
EIG_WRAP_METH1(Matrix3r,inverse);
EIG_WRAP_METH0(Matrix3r,Zero);
EIG_WRAP_METH0(Matrix3r,Identity);
EIG_WRAP_METH0(Quaternionr,Identity);
EIG_WRAP_METH1(Quaternionr,inverse);
EIG_WRAP_METH1(Quaternionr,conjugate);
EIG_WRAP_METH0(Vector3r,Zero);
EIG_WRAP_METH0(Vector3r,UnitX);
EIG_WRAP_METH0(Vector3r,UnitY);
EIG_WRAP_METH0(Vector3r,UnitZ);
EIG_WRAP_METH0(Vector3r,Ones);
EIG_WRAP_METH0(Vector2r,Zero);
EIG_WRAP_METH0(Vector2r,UnitX);
EIG_WRAP_METH0(Vector2r,UnitY);
EIG_WRAP_METH0(Vector2r,Ones);

#define EIG_OP1(klass,op,sym) typeof((sym klass()).eval()) klass##op(const klass& self){ return (sym self).eval();}
#define EIG_OP2(klass,op,sym,klass2) typeof((klass() sym klass2()).eval()) klass##op##klass2(const klass& self, const klass2& other){ return (self sym other).eval(); }
#define EIG_OP2_INPLACE(klass,op,sym,klass2) void klass##op##klass2(klass& self, const klass2& other){ self sym other; }
EIG_OP2(Matrix3r,__add__,+,Matrix3r)
EIG_OP2(Matrix3r,__sub__,-,Matrix3r)
EIG_OP2(Matrix3r,__mul__,*,Real)
EIG_OP2(Matrix3r,__rmul__,*,Real)
//EIG_OP2(Matrix3r,__mul__,*,Matrix3r)
EIG_OP2(Matrix3r,__div__,/,Real)
EIG_OP2_INPLACE(Matrix3r,__iadd__,+=,Matrix3r)
EIG_OP2_INPLACE(Matrix3r,__isub__,-=,Matrix3r)
//EIG_OP2_INPLACE(Matrix3r,__imul__,*=,Matrix3r)
EIG_OP2_INPLACE(Matrix3r,__imul__,*=,Real)
EIG_OP2_INPLACE(Matrix3r,__idiv__,/=,Real)

//EIG_OP2(Matrix3r,__eq__,==,Matrix3r)
//EIG_OP2(Matrix3r,__ne__,!=,Matrix3r)
EIG_OP2(Vector3r,__add__,+,Vector3r)
EIG_OP2(Vector3r,__sub__,-,Vector3r)
EIG_OP2(Vector3r,__mul__,*,Real)
EIG_OP2(Vector3r,__rmul__,*,Real)
//EIG_OP2(Vector3r,__mul__,*,Real)
EIG_OP2(Vector3r,__div__,/,Real)
EIG_OP2_INPLACE(Vector3r,__iadd__,+,Vector3r)
EIG_OP2_INPLACE(Vector3r,__isub__,-,Vector3r)

Quaternionr Quaternionr__mul__Quaternionr(const Quaternionr& self, const Quaternionr& other){ return self*other; }
//EIG_OP2(Quaternionr,__mul__,*,Quaternionr)
EIG_OP2_INPLACE(Quaternionr,__imul__,*=,Quaternionr)

EIG_OP1(Matrix3r,__neg__,-)
EIG_OP1(Vector3r,__neg__,-)

BOOST_PYTHON_MODULE(miniWm3Wrap){
	bp::class_<Matrix3r >("Matrix3",bp::init<>())
		.def(bp::init<Matrix3r const &>((bp::arg("m"))))
		//.def(bp::init<Real,Real,Real,Real,Real,Real,Real,Real,Real>((bp::arg("n00"),bp::arg("n01"),bp::arg("n02"),bp::arg("n10"),bp::arg("n11"),bp::arg("n12"),bp::arg("n20"),bp::arg("n21"),bp::arg("n22"))))
		// .def(bp::init<const Vector3r&, const Vector3r&, const Vector3r&, bool>((bp::arg("v0"),bp::arg("v1"),bp::arg("v2"),bp::arg("columns"))))
		//.def(bp::init<Real,Real,Real>((bp::arg("n00"),bp::arg("n11"),bp::arg("n22"))))
		//.def("__init__",&matrixFromElements)
		//.def(bp::init<Vector3r>((bp::arg("diag"))))
		.def("determinant",&Matrix3r::determinant)
		.def("inverse",&Matrix3r_inverse)
		.def("transpose",&Matrix3r_transpose)
		//.def("__mul__", &Matrix3r__mul__Matrix3r)
		.def("__add__", &Matrix3r__add__Matrix3r)
		.def("__sub__", &Matrix3r__sub__Matrix3r)
		.def("__mul__", &Matrix3r__mul__Real)
		.def("__rmul__",&Matrix3r__rmul__Real)
		.def("__div__", &Matrix3r__div__Real)
		.def("__iadd__",&Matrix3r__iadd__Matrix3r)
		.def("__isub__",&Matrix3r__isub__Matrix3r)
		//.def("__imul__",&Matrix3r__imul__Matrix3r)
		.def("__imul__",&Matrix3r__imul__Real)
		.def("__idiv__",&Matrix3r__idiv__Real)
		.def("__neg__",&Matrix3r__neg__)
#if 0
		.def(bp::self * bp::self)
		//.def(bp::self *= bp::self)
		.def(bp::self + bp::self)
		.def(bp::self += bp::self)
		.def(bp::self - bp::self)
		.def(bp::self -= bp::self)
		.def(bp::self * bp::other<Real>())
		.def(bp::other<Real>() * bp::self)
		.def(bp::self *= bp::other<Real>())
		.def(bp::self / bp::other<Real>())
		.def(bp::self /= bp::other<Real>())
		//.def(bp::self == bp::self)
		//.def(bp::self != bp::self)
		.def(-bp::self)
#endif
 		.def("__len__",&::Matrix3_len).staticmethod("__len__")
		.def("__setitem__",&::Matrix3_set_item)
		.def("__getitem__",&::Matrix3_get_item)
		.def("__str__",&::Matrix3_str)
		.def("__repr__",&::Matrix3_str)
		/* extras for matrices */
		.def("__setitem__",&::Matrix3_set_item_linear)
		.def("__getitem__",&::Matrix3_get_item_linear)
		.def_readonly("Identity",Matrix3r_Identity())
		.def_readonly("Zero",Matrix3r_Zero())
		// wm3 compat
		.def_readonly("IDENTITY",Matrix3r_Identity())
		.def_readonly("ZERO",Matrix3r_Zero())
		.def("Determinant",&Matrix3r::determinant)
		.def("Inverse",&Matrix3r_inverse)
		.def("Transpose",&Matrix3r_transpose)
	;
	bp::class_<Quaternionr>("Quaternion",bp::init<>())
		//.def(bp::init<Vector3r,Real>((bp::arg("axis"),bp::arg("angle"))))
		//.def(bp::init<Matrix3r>((bp::arg("rotMatrix"))))
		.def(bp::init<Quaternionr>((bp::arg("other"))))
		.def("setFromTwoVectors",&Quaternionr_setFromTwoVectors,((bp::arg("v1"),bp::arg("v2"))))
		.def("conjugate",&Quaternionr_conjugate)
		//.def("FromAxisAngle",&Quaternionr::FromAxisAngle,((bp::arg("axis"),bp::arg("angle"))),bp::return_self<>())
		//.def("rotate",&Quaternionr::Rotate,((bp::arg("v"))))
		.def("inverse",&Quaternionr_inverse)
		.def("norm",&Quaternionr::norm)
		.def("normalize",&Quaternionr::normalize)
		.def("__mul__",&Quaternionr__mul__Quaternionr)
		.def("__imul__",&Quaternionr__imul__Quaternionr)
		//.def("ToRotationMatrix",&Quaternionr::ToRotationMatrix)
		//.def("ToAxisAngle",Quaternion_ToAxisAngle)
		//.def(bp::self != bp::self)
		//.def(bp::self == bp::self)
		//.def(bp::self * bp::self)
		//.def(bp::self * bp::other<Real>())
		//.def(bp::other<Real>() * bp::self)
		//.def(bp::self * bp::other<Vector3r>())
		//.def(bp::self *= bp::other<Real>())
		//.def(bp::self + bp::self)
		//.def(bp::self += bp::self)
		//.def(bp::self - bp::self)
		//.def(-bp::self)
		//.def(bp::self -= bp::self)
		//.def(bp::self / bp::other<Real>())
		//.def(bp::self /= bp::other<Real>())
		.def("__len__",&::Quaternion_len).staticmethod("__len__")
		.def("__setitem__",&::Quaternion_set_item)
		.def("__getitem__",&::Quaternion_get_item)
		.def("__str__",&::Quaternion_str)
		.def("__repr__",&::Quaternion_str)
		.def_readonly("Identity",Quaternionr_Identity())
		.def_readonly("IDENTITY",Quaternionr_Identity())
		// wm3 compat
		.def("Align",&Quaternionr_setFromTwoVectors,((bp::arg("v1"),bp::arg("v2"))))
		.def("Conjugate",&Quaternionr_conjugate)
		//.def("FromAxisAngle",&Quaternionr::FromAxisAngle,((bp::arg("axis"),bp::arg("angle"))),bp::return_self<>())
		//.def("Rotate",&Quaternionr::rotate,((bp::arg("v"))))
		.def("Inverse",&Quaternionr_inverse)
		.def("Length",&Quaternionr::norm)
		.def("Normalize",&Quaternionr::normalize)
	;
	bp::class_<Vector2r>("Vector2",bp::init<>())
		.def(bp::init<Vector2r>((bp::arg("other"))))
		.def(bp::init<Real,Real>((bp::arg("x"),bp::arg("y"))))
		.def("dot",&Vector2r_dot)
		.def("norm",&Vector2r::norm)
		.def("squaredNorm",&Vector2r::squaredNorm)
		.def("normalize",&Vector2r::normalize)
		//
	#if 0
		.def(bp::self != bp::self)
		.def(bp::self == bp::self)
		.def(bp::self * bp::other<Real>())
		.def(bp::other<Real>() * bp::self)
		.def(bp::self *= bp::other<Real>())
		.def(bp::self + bp::self)
		.def(bp::self += bp::self)
		.def(bp::self - bp::self)
		.def(bp::self -= bp::self)
		.def(-bp::self)
		.def(bp::self / bp::other<Real>())
		.def(bp::self /= bp::other<Real>())
	#endif
		.def("__len__",&::Vector2_len).staticmethod("__len__")
		.def("__setitem__",&::Vector2_set_item)
		.def("__getitem__",&::Vector2_get_item)
		.def("__str__",&::Vector2_str)
		.def("__repr__",&::Vector2_str)
		.def_readonly("Ones",Vector2r_Ones())
		.def_readonly("UnitX",Vector2r_UnitX())
		.def_readonly("UnitY",Vector2r_UnitY())
		.def_readonly("Zero",Vector2r_Zero())
		// wm3 compat
		.def("Dot",&Vector2r_dot)
		.def("Length",&Vector2r::norm)
		.def("SquaredLength",&Vector2r::squaredNorm)
		.def("Normalize",&Vector2r::normalize)
		.def_readonly("ONE",Vector2r_Ones())
		.def_readonly("UNIT_X",Vector2r_UnitX())
		.def_readonly("UNIT_Y",Vector2r_UnitY())
		.def_readonly("ZERO",Vector2r_Zero())
	;
	bp::class_<Vector3r>("Vector3",bp::init<>())
		.def(bp::init<Vector3r>((bp::arg("other"))))
		.def(bp::init<Real,Real,Real>((bp::arg("x"),bp::arg("y"),bp::arg("z"))))
		.def("dot",&Vector3r_dot)
		.def("cross",&Vector3r_cross)
		.def("norm",&Vector3r::norm)
		.def("squaredNorm",&Vector3r::squaredNorm)
		.def("normalize",&Vector3r::normalize)
		.def("__neg__",&Matrix3r__neg__)
		.def("__add__",&Vector3r__add__Vector3r)
		.def("__sub__",&Vector3r__sub__Vector3r)
		.def("__iadd__",&Vector3r__iadd__Vector3r)
		.def("__isub__",&Vector3r__isub__Vector3r)
		.def("__mul__",&Vector3r__mul__Real)
		.def("__div__",&Vector3r__div__Real)
		.def("__rmul__",&Vector3r__rmul__Real)
	#if 0
		.def(bp::self != bp::self)
		.def(bp::self == bp::self)
		.def(bp::self * bp::other<Real>())
		.def(bp::other<Real>() * bp::self)
		.def(bp::self *= bp::other<Real>())
		.def(bp::self + bp::self)
		.def(bp::self += bp::self)
		.def(bp::self - bp::self)
		.def(bp::self -= bp::self)
		.def(-bp::self)
		.def(bp::self / bp::other<Real>())
		.def(bp::self /= bp::other<Real>())
	#endif
		.def("__len__",&::Vector3_len).staticmethod("__len__")
		.def("__setitem__",&::Vector3_set_item)
		.def("__getitem__",&::Vector3_get_item)
		.def("__str__",&::Vector3_str)
		.def("__repr__",&::Vector3_str)
		.def_readonly("Ones",Vector3r_Ones())
		.def_readonly("UnitX",Vector3r_UnitX())
		.def_readonly("UnitY",Vector3r_UnitY())
		.def_readonly("UnitZ",Vector3r_UnitZ())
		.def_readonly("Zero",Vector3r_Zero())
		// wm3 compat
		.def("Dot",&Vector3r_dot)
		.def("Length",&Vector3r::norm)
		.def("SquaredLength",&Vector3r::squaredNorm)
		.def("Normalize",&Vector3r::normalize)
		.def_readonly("ONE",Vector3r_Ones())
		.def_readonly("UNIT_X",Vector3r_UnitX())
		.def_readonly("UNIT_Y",Vector3r_UnitY())
		.def_readonly("UNIT_Z",Vector3r_UnitZ())
		.def_readonly("ZERO",Vector3r_Zero())
	;	
	
};








