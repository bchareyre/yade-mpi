// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<boost/python.hpp>
#include<boost/lexical_cast.hpp>
#include<string>
#include<stdexcept>
#include<sstream>

#include<yade/lib-base/Math.hpp>
#include<yade/lib-pyutil/doc_opts.hpp>


namespace bp=boost::python;

#define IDX_CHECK(i,MAX){ if(i<0 || i>=MAX) { PyErr_SetString(PyExc_IndexError, ("Index out of range 0.." + boost::lexical_cast<std::string>(MAX-1)).c_str()); bp::throw_error_already_set(); } }
#define IDX2_CHECKED_TUPLE_INTS(tuple,max2,arr2) {int l=bp::len(tuple); if(l!=2) { PyErr_SetString(PyExc_IndexError,"Index must be integer or a 2-tuple"); bp::throw_error_already_set(); } for(int _i=0; _i<2; _i++) { bp::extract<int> val(tuple[_i]); if(!val.check()) throw std::runtime_error("Unable to convert "+boost::lexical_cast<std::string>(_i)+"-th index to int."); int v=val(); IDX_CHECK(v,max2[_i]); arr2[_i]=v; }  }

void Vector3r_set_item(Vector3r & self, int idx, Real value){ IDX_CHECK(idx,3); self[idx]=value; }
void Vector3i_set_item(Vector3i & self, int idx, int  value){ IDX_CHECK(idx,3); self[idx]=value; }
void Vector2r_set_item(Vector2r & self, int idx, Real value){ IDX_CHECK(idx,2); self[idx]=value; }
void Vector2i_set_item(Vector2i & self, int idx, int  value){ IDX_CHECK(idx,2); self[idx]=value; }

void Quaternionr_set_item(Quaternionr & self, int idx, Real value){ IDX_CHECK(idx,4);  if(idx==0) self.w()=value; else if(idx==1) self.x()=value; else if(idx==2) self.y()=value; else if(idx==3) self.z()=value; }
void Matrix3r_set_item(Matrix3r & self, bp::tuple _idx, Real value){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); self(idx[0],idx[1])=value; }
void Matrix3r_set_item_linear(Matrix3r & self, int idx, Real value){ IDX_CHECK(idx,9); self(idx/3,idx%3)=value; }

Real Vector3r_get_item(const Vector3r & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
int  Vector3i_get_item(const Vector3r & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
Real Vector2r_get_item(const Vector2r & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }
int  Vector2i_get_item(const Vector2r & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }

Real Quaternionr_get_item(const Quaternionr & self, int idx){ IDX_CHECK(idx,4); if(idx==0) return self.w(); if(idx==1) return self.x(); if(idx==2) return self.y(); return self.z(); }
Real Matrix3r_get_item(Matrix3r & self, bp::tuple _idx){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); return self(idx[0],idx[1]); }
Real Matrix3r_get_item_linear(Matrix3r & self, int idx){ IDX_CHECK(idx,9); return self(idx/3,idx%3); }

std::string Vector3r_str(const Vector3r & self){ return std::string("Vector3(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Vector3i_str(const Vector3i & self){ return std::string("Vector3i(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Vector2r_str(const Vector2r & self){ return std::string("Vector2(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Vector2i_str(const Vector2i & self){ return std::string("Vector2i(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Quaternionr_str(const Quaternionr & self){ AngleAxisr aa(angleAxisFromQuat(self)); return std::string("Quaternion((")+boost::lexical_cast<std::string>(aa.axis()[0])+","+boost::lexical_cast<std::string>(aa.axis()[1])+","+boost::lexical_cast<std::string>(aa.axis()[2])+"),"+boost::lexical_cast<std::string>(aa.angle())+")";}
std::string Matrix3r_str(const Matrix3r & self){ std::ostringstream oss; oss<<"Matrix3("; for(int i=0; i<3; i++) for(int j=0; j<3; j++) oss<<self(i,j)<<((i==2 && j==2)?")":",")<<((i<2 && j==2)?" ":""); return oss.str(); }

int Vector3r_len(){return 3;}
int Vector3i_len(){return 3;}
int Vector2r_len(){return 2;}
int Vector2i_len(){return 2;}
int Quaternionr_len(){return 4;}
int Matrix3r_len(){return 9;}
#undef IDX_CHECK

// automagic converters from sequence (list, tuple, …) to Vector{2,3}{r,i}
struct custom_Vector3r_from_sequence{
	custom_Vector3r_from_sequence(){	bp::converter::registry::push_back(&convertible,&construct,bp::type_id<Vector3r>()); }
	static void* convertible(PyObject* obj_ptr){ if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=3) return 0;	return obj_ptr; }
	static void construct(PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data){
		void* storage=((bp::converter::rvalue_from_python_storage<Vector3r>*)(data))->storage.bytes;
		new (storage) Vector3r(bp::extract<Real>(PySequence_GetItem(obj_ptr,0)),bp::extract<Real>(PySequence_GetItem(obj_ptr,1)),bp::extract<Real>(PySequence_GetItem(obj_ptr,2)));
		data->convertible=storage;
	}
};
struct custom_Vector3i_from_sequence{
	custom_Vector3i_from_sequence(){	bp::converter::registry::push_back(&convertible,&construct,bp::type_id<Vector3i>()); }
	static void* convertible(PyObject* obj_ptr){ if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=3) return 0;	return obj_ptr; }
	static void construct(PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data){
		void* storage=((bp::converter::rvalue_from_python_storage<Vector3i>*)(data))->storage.bytes;
		new (storage) Vector3i(bp::extract<int>(PySequence_GetItem(obj_ptr,0)),bp::extract<int>(PySequence_GetItem(obj_ptr,1)),bp::extract<int>(PySequence_GetItem(obj_ptr,2)));
		data->convertible=storage;
	}
};
struct custom_Vector2r_from_sequence{
	custom_Vector2r_from_sequence(){	bp::converter::registry::push_back(&convertible,&construct,bp::type_id<Vector2r>()); }
	static void* convertible(PyObject* obj_ptr){ if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=2) return 0;	return obj_ptr; }
	static void construct(PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data){
		void* storage=((bp::converter::rvalue_from_python_storage<Vector2r>*)(data))->storage.bytes;
		new (storage) Vector2r(bp::extract<Real>(PySequence_GetItem(obj_ptr,0)),bp::extract<Real>(PySequence_GetItem(obj_ptr,1)));
		data->convertible=storage;
	}
};
struct custom_Vector2i_from_sequence{
	custom_Vector2i_from_sequence(){	bp::converter::registry::push_back(&convertible,&construct,bp::type_id<Vector2i>()); }
	static void* convertible(PyObject* obj_ptr){ if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=2) return 0;	return obj_ptr; }
	static void construct(PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data){
		void* storage=((bp::converter::rvalue_from_python_storage<Vector2i>*)(data))->storage.bytes;
		new (storage) Vector2i(bp::extract<int>(PySequence_GetItem(obj_ptr,0)),bp::extract<int>(PySequence_GetItem(obj_ptr,1)));
		data->convertible=storage;
	}
};

static Matrix3r* Matrix3r_fromElements(Real m00, Real m01, Real m02, Real m10, Real m11, Real m12, Real m20, Real m21, Real m22){ Matrix3r* m(new Matrix3r); (*m)<<m00,m01,m02,m10,m11,m12,m20,m21,m22; return m; }
static Quaternionr Quaternionr_setFromTwoVectors(Quaternionr& q, const Vector3r& u, const Vector3r& v){ return q.setFromTwoVectors(u,v); }
static Vector3r Quaternionr_Rotate(Quaternionr& q, const Vector3r& u){ return q*u; }
static Quaternionr* Quaternionr_fromAxisAngle(const Vector3r& axis, const Real angle){ return new Quaternionr(AngleAxisr(angle,axis)); }
static Quaternionr* Quaternionr_fromAngleAxis(const Real angle, const Vector3r& axis){ return new Quaternionr(AngleAxisr(angle,axis)); }
static bp::tuple Quaternionr_toAxisAngle(const Quaternionr& self){ AngleAxisr aa(self); return bp::make_tuple(aa.axis(),aa.angle());}
static bp::tuple Quaternionr_toAngleAxis(const Quaternionr& self){ AngleAxisr aa(self); return bp::make_tuple(aa.angle(),aa.axis());}
static void Matrix3r_fromAxisAngle(Matrix3r& self, const Vector3r& axis, const Real angle){ self=AngleAxisr(angle,axis).toRotationMatrix(); }

static Real Vector3r_dot(const Vector3r& self, const Vector3r& v){ return self.dot(v); }
static Real Vector3i_dot(const Vector3i& self, const Vector3i& v){ return self.dot(v); }
static Real Vector2r_dot(const Vector2r& self, const Vector2r& v){ return self.dot(v); }
static Real Vector2i_dot(const Vector2i& self, const Vector2i& v){ return self.dot(v); }
static Vector3r Vector3r_cross(const Vector3r& self, const Vector3r& v){ return self.cross(v); }
static Vector3i Vector3i_cross(const Vector3i& self, const Vector3i& v){ return self.cross(v); }
static bool Quaternionr__eq__(const Quaternionr& q1, const Quaternionr& q2){ return q1==q2; }
static bool Quaternionr__neq__(const Quaternionr& q1, const Quaternionr& q2){ return q1!=q2; }

#define EIG_WRAP_METH1(klass,meth) static klass klass##_##meth(const klass& self){ return self.meth(); }
//#define EIG_WRAP_METH0(klass,meth) static klass klass##_##meth(){ return klass().meth(); }
#define EIG_WRAP_METH0(klass,meth) static const klass klass##_##meth=klass().meth();
EIG_WRAP_METH1(Matrix3r,transpose);
EIG_WRAP_METH1(Matrix3r,inverse);

EIG_WRAP_METH0(Matrix3r,Zero);
EIG_WRAP_METH0(Matrix3r,Identity);
EIG_WRAP_METH0(Vector3r,Zero); EIG_WRAP_METH0(Vector3r,UnitX); EIG_WRAP_METH0(Vector3r,UnitY); EIG_WRAP_METH0(Vector3r,UnitZ); EIG_WRAP_METH0(Vector3r,Ones);
EIG_WRAP_METH0(Vector3i,Zero); EIG_WRAP_METH0(Vector3i,UnitX); EIG_WRAP_METH0(Vector3i,UnitY); EIG_WRAP_METH0(Vector3i,UnitZ); EIG_WRAP_METH0(Vector3i,Ones);
EIG_WRAP_METH0(Vector2r,Zero); EIG_WRAP_METH0(Vector2r,UnitX); EIG_WRAP_METH0(Vector2r,UnitY); EIG_WRAP_METH0(Vector2r,Ones);
EIG_WRAP_METH0(Vector2i,Zero); EIG_WRAP_METH0(Vector2i,UnitX); EIG_WRAP_METH0(Vector2i,UnitY); EIG_WRAP_METH0(Vector2i,Ones);
EIG_WRAP_METH0(Quaternionr,Identity);

#define EIG_OP1(klass,op,sym) typeof((sym klass()).eval()) klass##op(const klass& self){ return (sym self).eval();}
#define EIG_OP2(klass,op,sym,klass2) typeof((klass() sym klass2()).eval()) klass##op##klass2(const klass& self, const klass2& other){ return (self sym other).eval(); }
#define EIG_OP2_INPLACE(klass,op,sym,klass2) void klass##op##klass2(klass& self, const klass2& other){ self sym other; }


EIG_OP1(Matrix3r,__neg__,-)
EIG_OP2(Matrix3r,__add__,+,Matrix3r) EIG_OP2_INPLACE(Matrix3r,__iadd__,+=,Matrix3r)
EIG_OP2(Matrix3r,__sub__,-,Matrix3r) EIG_OP2_INPLACE(Matrix3r,__isub__,-=,Matrix3r)
EIG_OP2(Matrix3r,__mul__,*,Real) EIG_OP2(Matrix3r,__rmul__,*,Real) EIG_OP2_INPLACE(Matrix3r,__imul__,*=,Real)
EIG_OP2(Matrix3r,__mul__,*,int) EIG_OP2(Matrix3r,__rmul__,*,int) EIG_OP2_INPLACE(Matrix3r,__imul__,*=,int)
EIG_OP2(Matrix3r,__mul__,*,Vector3r) EIG_OP2(Matrix3r,__rmul__,*,Vector3r)
EIG_OP2(Matrix3r,__mul__,*,Matrix3r)
EIG_OP2(Matrix3r,__div__,/,Real) EIG_OP2_INPLACE(Matrix3r,__idiv__,/=,Real)
EIG_OP2(Matrix3r,__div__,/,int) EIG_OP2_INPLACE(Matrix3r,__idiv__,/=,int)

EIG_OP1(Vector3r,__neg__,-);
EIG_OP2(Vector3r,__add__,+,Vector3r); EIG_OP2_INPLACE(Vector3r,__iadd__,+,Vector3r)
EIG_OP2(Vector3r,__sub__,-,Vector3r); EIG_OP2_INPLACE(Vector3r,__isub__,-,Vector3r)
EIG_OP2(Vector3r,__mul__,*,Real) EIG_OP2(Vector3r,__rmul__,*,Real) EIG_OP2_INPLACE(Vector3r,__imul__,*=,Real) EIG_OP2(Vector3r,__div__,/,Real) EIG_OP2_INPLACE(Vector3r,__idiv__,/=,Real)
EIG_OP2(Vector3r,__mul__,*,int) EIG_OP2(Vector3r,__rmul__,*,int) EIG_OP2_INPLACE(Vector3r,__imul__,*=,int) EIG_OP2(Vector3r,__div__,/,int) EIG_OP2_INPLACE(Vector3r,__idiv__,/=,int)

EIG_OP1(Vector3i,__neg__,-);
EIG_OP2(Vector3i,__add__,+,Vector3i); EIG_OP2_INPLACE(Vector3i,__iadd__,+,Vector3i)
EIG_OP2(Vector3i,__sub__,-,Vector3i); EIG_OP2_INPLACE(Vector3i,__isub__,-,Vector3i)
EIG_OP2(Vector3i,__mul__,*,int) EIG_OP2(Vector3i,__rmul__,*,int)  EIG_OP2_INPLACE(Vector3i,__imul__,*=,int)

EIG_OP1(Vector2r,__neg__,-);
EIG_OP2(Vector2r,__add__,+,Vector2r); EIG_OP2_INPLACE(Vector2r,__iadd__,+,Vector2r)
EIG_OP2(Vector2r,__sub__,-,Vector2r); EIG_OP2_INPLACE(Vector2r,__isub__,-,Vector2r)
EIG_OP2(Vector2r,__mul__,*,Real) EIG_OP2(Vector2r,__rmul__,*,Real) EIG_OP2_INPLACE(Vector2r,__imul__,*=,Real) EIG_OP2(Vector2r,__div__,/,Real) EIG_OP2_INPLACE(Vector2r,__idiv__,/=,Real)
EIG_OP2(Vector2r,__mul__,*,int) EIG_OP2(Vector2r,__rmul__,*,int) EIG_OP2_INPLACE(Vector2r,__imul__,*=,int) EIG_OP2(Vector2r,__div__,/,int) EIG_OP2_INPLACE(Vector2r,__idiv__,/=,int)

EIG_OP1(Vector2i,__neg__,-);
EIG_OP2(Vector2i,__add__,+,Vector2i); EIG_OP2_INPLACE(Vector2i,__iadd__,+,Vector2i)
EIG_OP2(Vector2i,__sub__,-,Vector2i); EIG_OP2_INPLACE(Vector2i,__isub__,-,Vector2i)
EIG_OP2(Vector2i,__mul__,*,int)  EIG_OP2_INPLACE(Vector2i,__imul__,*=,int) EIG_OP2(Vector2i,__rmul__,*,int)


BOOST_PYTHON_MODULE(miniEigen){
	bp::scope().attr("__doc__")="Basic math functions for Yade: small matrix, vector and quaternion classes. This module internally wraps small parts of the `Eigen <http://eigen.tuxfamily.org>`_ library. Refer to its documentation for details.";

	YADE_SET_DOCSTRING_OPTS;

	custom_Vector3r_from_sequence();
	custom_Vector3i_from_sequence();
	custom_Vector2r_from_sequence();
	custom_Vector2i_from_sequence();

	bp::class_<Matrix3r>("Matrix3","3x3 float matrix.\n\nSupported operations (``m`` is a Matrix3, ``f`` if a float/int, ``v`` is a Vector3): ``-m``, ``m+m``, ``m+=m``, ``m-m``, ``m-=m``, ``m*f``, ``f*m``, ``m*=f``, ``m/f``, ``m/=f``, ``m*m``, ``m*v``, ``v*m``, ``m==m``, ``m!=m``.",bp::init<>())
		.def(bp::init<Matrix3r const &>((bp::arg("m"))))
		.def("__init__",bp::make_constructor(&Matrix3r_fromElements,bp::default_call_policies(),(bp::arg("m00"),bp::arg("m01"),bp::arg("m02"),bp::arg("m10"),bp::arg("m11"),bp::arg("m12"),bp::arg("m20"),bp::arg("m21"),bp::arg("m22"))))
		//
		.def("determinant",&Matrix3r::determinant)
		.def("inverse",&Matrix3r_inverse)
		.def("transpose",&Matrix3r_transpose)
		//
		.def("__neg__",&Matrix3r__neg__)
		.def("__add__",&Matrix3r__add__Matrix3r).def("__iadd__",&Matrix3r__iadd__Matrix3r)
		.def("__sub__",&Matrix3r__sub__Matrix3r).def("__isub__",&Matrix3r__isub__Matrix3r)
		.def("__mul__",&Matrix3r__mul__Real).def("__rmul__",&Matrix3r__rmul__Real).def("__imul__",&Matrix3r__imul__Real)
		.def("__mul__",&Matrix3r__mul__int).def("__rmul__",&Matrix3r__rmul__int).def("__imul__",&Matrix3r__imul__int)
		.def("__mul__",&Matrix3r__mul__Vector3r).def("__rmul__",&Matrix3r__rmul__Vector3r)
		.def("__mul__",&Matrix3r__mul__Matrix3r)
		.def("__div__",&Matrix3r__div__Real).def("__idiv__",&Matrix3r__idiv__Real)
		.def("__div__",&Matrix3r__div__int).def("__idiv__",&Matrix3r__idiv__int)
		.def(bp::self == bp::self)
		.def(bp::self != bp::self)
		//
 		.def("__len__",&::Matrix3r_len).staticmethod("__len__").def("__setitem__",&::Matrix3r_set_item).def("__getitem__",&::Matrix3r_get_item).def("__str__",&::Matrix3r_str).def("__repr__",&::Matrix3r_str)
		/* extras for matrices */
		.def("__setitem__",&::Matrix3r_set_item_linear).def("__getitem__",&::Matrix3r_get_item_linear)
		.def_readonly("Identity",&Matrix3r_Identity).def_readonly("Zero",&Matrix3r_Zero)
		// wm3 compat
		.def_readonly("IDENTITY",&Matrix3r_Identity,"|ydeprecated|").def_readonly("ZERO",&Matrix3r_Zero,"|ydeprecated|").def("Determinant",&Matrix3r::determinant,"|ydeprecated|").def("Inverse",&Matrix3r_inverse,"|ydeprecated|").def("Transpose",&Matrix3r_transpose,"|ydeprecated|")
		.def("FromAxisAngle",&Matrix3r_fromAxisAngle,"|ydeprecated|")
	;
	bp::class_<Quaternionr>("Quaternion","Quaternion representing rotation.\n\nSupported operations (``q`` is a Quaternion, ``v`` is a Vector3): ``q*q`` (rotation composition), ``q*=q``, ``q*v`` (rotating ``v`` by ``q``), ``q==q``, ``q!=q``.",bp::init<>())
		.def("__init__",bp::make_constructor(&Quaternionr_fromAxisAngle,bp::default_call_policies(),(bp::arg("axis"),bp::arg("angle"))))
		.def("__init__",bp::make_constructor(&Quaternionr_fromAngleAxis,bp::default_call_policies(),(bp::arg("angle"),bp::arg("axis"))))
		.def(bp::init<Quaternionr>((bp::arg("other"))))
		.def("setFromTwoVectors",&Quaternionr_setFromTwoVectors,((bp::arg("v1"),bp::arg("v2"))))
		.def("conjugate",&Quaternionr::conjugate)
		.def("toAxisAngle",&Quaternionr_toAxisAngle).def("toAngleAxis",&Quaternionr_toAngleAxis)
		.def("Rotate",&Quaternionr_Rotate,((bp::arg("v"))))
		.def("inverse",&Quaternionr::inverse)
		.def("norm",&Quaternionr::norm)
		.def("normalize",&Quaternionr::normalize)
		.def(bp::self * bp::self)
		.def(bp::self *= bp::self)
		.def(bp::self * bp::other<Vector3r>())
		.def("__eq__",&Quaternionr__eq__).def("__neq__",&Quaternionr__neq__)
		//.def(bp::self != bp::self).def(bp::self == bp::self) // these don't work... (?)
		//
		.def("__len__",&::Quaternionr_len).staticmethod("__len__")
		.def("__setitem__",&::Quaternionr_set_item).def("__getitem__",&::Quaternionr_get_item)
		.def("__str__",&::Quaternionr_str).def("__repr__",&::Quaternionr_str)
		.def_readonly("Identity",&Quaternionr_Identity)
		.def("toAxisAngle",&Quaternionr_toAxisAngle).def("toAngleAxis",&Quaternionr_toAxisAngle)
		// wm3 compat
		.def("Align",&Quaternionr_setFromTwoVectors,((bp::arg("v1"),bp::arg("v2"))),"|ydeprecated|").def("Conjugate",&Quaternionr::conjugate,"|ydeprecated|").def("Inverse",&Quaternionr::inverse,"|ydeprecated|").def("Length",&Quaternionr::norm,"|ydeprecated|").def("Normalize",&Quaternionr::normalize,"|ydeprecated|").def_readonly("IDENTITY",&Quaternionr_Identity)
		.def("ToAxisAngle",&Quaternionr_toAxisAngle,"|ydeprecated|").def("ToAngleAxis",&Quaternionr_toAxisAngle)
		//.def("Rotate",&Quaternionr::rotate,((bp::arg("v"))))
	;
	bp::class_<Vector3r>("Vector3","3-dimensional float vector.\n\nSupported operatrions (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``, plus operations with ``Matrix3`` and ``Quaternion``.\n\nImplicit conversion from sequence (list,tuple, …) of 3 floats.",bp::init<>())
		.def(bp::init<Vector3r>((bp::arg("other"))))
		.def(bp::init<Real,Real,Real>((bp::arg("x"),bp::arg("y"),bp::arg("z"))))
		// properties
		.def_readonly("Ones",&Vector3r_Ones).def_readonly("Zero",&Vector3r_Zero)
		.def_readonly("UnitX",&Vector3r_UnitX).def_readonly("UnitY",&Vector3r_UnitY).def_readonly("UnitZ",&Vector3r_UnitZ)
		// methods
		.def("dot",&Vector3r_dot).def("cross",&Vector3r_cross)
		.def("norm",&Vector3r::norm).def("squaredNorm",&Vector3r::squaredNorm).def("normalize",&Vector3r::normalize)
		// operators
		.def("__neg__",&Vector3r__neg__) // -v
		.def("__add__",&Vector3r__add__Vector3r).def("__iadd__",&Vector3r__iadd__Vector3r) // +, +=
		.def("__sub__",&Vector3r__sub__Vector3r).def("__isub__",&Vector3r__isub__Vector3r) // -, -=
		.def("__mul__",&Vector3r__mul__Real).def("__rmul__",&Vector3r__rmul__Real) // f*v, v*f
		.def("__div__",&Vector3r__div__Real).def("__idiv__",&Vector3r__idiv__Real) // v/f, v/=f
		.def("__mul__",&Vector3r__mul__int).def("__rmul__",&Vector3r__rmul__int) // f*v, v*f
		.def("__div__",&Vector3r__div__int).def("__idiv__",&Vector3r__idiv__int) // v/f, v/=f
		.def(bp::self != bp::self).def(bp::self == bp::self)
		// specials
		.def("__len__",&::Vector3r_len).staticmethod("__len__")
		.def("__setitem__",&::Vector3r_set_item).def("__getitem__",&::Vector3r_get_item)
		.def("__str__",&::Vector3r_str).def("__repr__",&::Vector3r_str)
		// wm3 compat
		.def("Dot",&Vector3r_dot,"|ydeprecated|").def("Cross",&Vector3r_cross,"|ydeprecated|").def("Length",&Vector3r::norm,"|ydeprecated|").def("SquaredLength",&Vector3r::squaredNorm,"|ydeprecated|").def("Normalize",&Vector3r::normalize,"|ydeprecated|")
		.def_readonly("ONE",&Vector3r_Ones,"|ydeprecated|").def_readonly("ZERO",&Vector3r_Zero,"|ydeprecated|").def_readonly("UNIT_X",&Vector3r_UnitX,"|ydeprecated|").def_readonly("UNIT_Y",&Vector3r_UnitY,"|ydeprecated|").def_readonly("UNIT_Z",&Vector3r_UnitZ,"|ydeprecated|")
	;	
	bp::class_<Vector3i>("Vector3i","3-dimensional integer vector.\n\nSupported operatrions (``i`` if an int, ``v`` is a Vector3i): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*i``, ``i*v``, ``v*=i``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence  (list,tuple, …) of 3 integers.",bp::init<>())
		.def(bp::init<Vector3i>((bp::arg("other"))))
		.def(bp::init<int,int,int>((bp::arg("x"),bp::arg("y"),bp::arg("z"))))
		// properties
		.def_readonly("Ones",&Vector3i_Ones).def_readonly("Zero",&Vector3i_Zero)
		.def_readonly("UnitX",&Vector3i_UnitX).def_readonly("UnitY",&Vector3i_UnitY).def_readonly("UnitZ",&Vector3i_UnitZ)
		// methods
		.def("dot",&Vector3i_dot).def("cross",&Vector3i_cross)
		.def("norm",&Vector3i::norm).def("squaredNorm",&Vector3i::squaredNorm)
		// operators
		.def("__neg__",&Vector3i__neg__) // -v
		.def("__add__",&Vector3i__add__Vector3i).def("__iadd__",&Vector3i__iadd__Vector3i) // +, +=
		.def("__sub__",&Vector3i__sub__Vector3i).def("__isub__",&Vector3i__isub__Vector3i) // -, -=
		.def("__mul__",&Vector3i__mul__int).def("__rmul__",&Vector3i__rmul__int) // f*v, v*f
		.def(bp::self != bp::self).def(bp::self == bp::self)
		// specials
		.def("__len__",&::Vector3i_len).staticmethod("__len__")
		.def("__setitem__",&::Vector3i_set_item).def("__getitem__",&::Vector3i_get_item)
		.def("__str__",&::Vector3i_str).def("__repr__",&::Vector3i_str)
		// wm3 compat
		.def("Dot",&Vector3i_dot,"|ydeprecated|").def("Cross",&Vector3i_cross,"|ydeprecated|").def("Length",&Vector3i::norm,"|ydeprecated|").def("SquaredLength",&Vector3r::squaredNorm,"|ydeprecated|")
		.def_readonly("ONE",&Vector3i_Ones,"|ydeprecated|").def_readonly("ZERO",&Vector3i_Zero,"|ydeprecated|").def_readonly("UNIT_X",&Vector3i_UnitX,"|ydeprecated|").def_readonly("UNIT_Y",&Vector3i_UnitY,"|ydeprecated|").def_readonly("UNIT_Z",&Vector3i_UnitZ,"|ydeprecated|")
	;	
	bp::class_<Vector2r>("Vector2","3-dimensional float vector.\n\nSupported operatrions (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list,tuple, …) of 2 floats.",bp::init<>())
		.def(bp::init<Vector2r>((bp::arg("other"))))
		.def(bp::init<Real,Real>((bp::arg("x"),bp::arg("y"))))
		// properties
		.def_readonly("Ones",&Vector2r_Ones).def_readonly("Zero",&Vector2r_Zero)
		.def_readonly("UnitX",&Vector2r_UnitX).def_readonly("UnitY",&Vector2r_UnitY)
		// methods
		.def("dot",&Vector2r_dot)
		.def("norm",&Vector2r::norm).def("squaredNorm",&Vector2r::squaredNorm).def("normalize",&Vector2r::normalize)
		// operators
		.def("__neg__",&Vector2r__neg__) // -v
		.def("__add__",&Vector2r__add__Vector2r).def("__iadd__",&Vector2r__iadd__Vector2r) // +, +=
		.def("__sub__",&Vector2r__sub__Vector2r).def("__isub__",&Vector2r__isub__Vector2r) // -, -=
		.def("__mul__",&Vector2r__mul__Real).def("__rmul__",&Vector2r__rmul__Real) // f*v, v*f
		.def("__div__",&Vector2r__div__Real).def("__idiv__",&Vector2r__idiv__Real) // v/f, v/=f
		.def("__mul__",&Vector2r__mul__int).def("__rmul__",&Vector2r__rmul__int) // f*v, v*f
		.def("__div__",&Vector2r__div__int).def("__idiv__",&Vector2r__idiv__int) // v/f, v/=f
		.def(bp::self != bp::self).def(bp::self == bp::self)
		// specials
		.def("__len__",&::Vector2r_len).staticmethod("__len__")
		.def("__setitem__",&::Vector2r_set_item).def("__getitem__",&::Vector2r_get_item)
		.def("__str__",&::Vector2r_str).def("__repr__",&::Vector2r_str)
		// wm3 compat
		.def("Dot",&Vector2r_dot,"|ydeprecated|").def("Length",&Vector2r::norm,"|ydeprecated|").def("SquaredLength",&Vector2r::squaredNorm,"|ydeprecated|").def("Normalize",&Vector2r::normalize)
		.def_readonly("ONE",&Vector2r_Ones,"|ydeprecated|").def_readonly("ZERO",&Vector2r_Zero,"|ydeprecated|").def_readonly("UNIT_X",&Vector2r_UnitX,"|ydeprecated|").def_readonly("UNIT_Y",&Vector2r_UnitY)
	;	
	bp::class_<Vector2i>("Vector2i","2-dimensional integer vector.\n\nSupported operatrions (``i`` if an int, ``v`` is a Vector2i): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*i``, ``i*v``, ``v*=i``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list,tuple, …) of 2 integers.",bp::init<>())
		.def(bp::init<Vector2i>((bp::arg("other"))))
		.def(bp::init<int,int>((bp::arg("x"),bp::arg("y"))))
		// properties
		.def_readonly("Ones",&Vector2i_Ones).def_readonly("Zero",&Vector2i_Zero)
		.def_readonly("UnitX",&Vector2i_UnitX).def_readonly("UnitY",&Vector2i_UnitY)
		// methods
		.def("dot",&Vector2i_dot)
		.def("norm",&Vector2i::norm).def("squaredNorm",&Vector2i::squaredNorm).def("normalize",&Vector2i::normalize)
		// operators
		.def("__neg__",&Vector2i__neg__) // -v
		.def("__add__",&Vector2i__add__Vector2i).def("__iadd__",&Vector2i__iadd__Vector2i) // +, +=
		.def("__sub__",&Vector2i__sub__Vector2i).def("__isub__",&Vector2i__isub__Vector2i) // -, -=
		.def("__mul__",&Vector2i__mul__int).def("__rmul__",&Vector2i__rmul__int) // f*v, v*f
		.def(bp::self != bp::self).def(bp::self == bp::self)
		// specials
		.def("__len__",&::Vector2i_len).staticmethod("__len__")
		.def("__setitem__",&::Vector2i_set_item).def("__getitem__",&::Vector2i_get_item)
		.def("__str__",&::Vector2i_str).def("__repr__",&::Vector2i_str)
		// wm3 compat
		.def("Dot",&Vector2i_dot,"|ydeprecated|").def("Length",&Vector2i::norm,"|ydeprecated|").def("SquaredLength",&Vector2i::squaredNorm,"|ydeprecated|").def("Normalize",&Vector2i::normalize)
		.def_readonly("ONE",&Vector2i_Ones,"|ydeprecated|").def_readonly("ZERO",&Vector2i_Zero,"|ydeprecated|").def_readonly("UNIT_X",&Vector2i_UnitX,"|ydeprecated|").def_readonly("UNIT_Y",&Vector2i_UnitY)
	;	
	
};








