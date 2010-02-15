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
void Quaternion_set_item(Quaternionr & self, int idx, Real value){ IDX_CHECK(idx,4); self[idx]=value; }
void Matrix3_set_item(Matrix3r & self, bp::tuple _idx, Real value){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); self(idx[0],idx[1])=value; }
void Matrix3_set_item_linear(Matrix3r & self, int idx, Real value){ IDX_CHECK(idx,9); self(idx/3,idx%3)=value; }

Real Vector2_get_item(const Vector2r & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }
Real Vector3_get_item(const Vector3r & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
Real Quaternion_get_item(const Quaternionr & self, int idx){ IDX_CHECK(idx,4); return self[idx]; }
Real Matrix3_get_item(Matrix3r & self, bp::tuple _idx){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); return self(idx[0],idx[1]); }
Real Matrix3_get_item_linear(Matrix3r & self, int idx){ IDX_CHECK(idx,9); return self(idx/3,idx%3); }

std::string Vector2_str(const Vector2r & self){ return std::string("Vector2(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Vector3_str(const Vector3r & self){ return std::string("Vector3(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Quaternion_str(const Quaternionr & self){ Vector3r axis; Real angle; self.ToAxisAngle(axis,angle); return std::string("Quaternion((")+boost::lexical_cast<std::string>(axis[0])+","+boost::lexical_cast<std::string>(axis[1])+","+boost::lexical_cast<std::string>(axis[2])+"),"+boost::lexical_cast<std::string>(angle)+")";}
std::string Matrix3_str(const Matrix3r & self){ std::ostringstream oss; oss<<"Matrix3("; for(int i=0; i<3; i++) for(int j=0; j<3; j++) oss<<self(i,j)<<((i==2 && j==2)?")":",")<<((i<2 && j==2)?" ":""); return oss.str(); }

int Vector2_len(){return 2;}
int Vector3_len(){return 3;}
int Quaternion_len(){return 4;}
int Matrix3_len(){return 9;}
#undef IDX_CHECK

#if 1
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


static bp::tuple Quaternion_ToAxisAngle(const Quaternionr& q){
    Vector3r axis; Real angle;
    q.ToAxisAngle(axis,angle);
    return bp::make_tuple(axis,angle);
}

BOOST_PYTHON_MODULE(miniWm3Wrap){
	bp::class_<Matrix3r >("Matrix3",bp::init<bp::optional<bool> >((bp::arg("zero")=true)))
		.def(bp::init<Matrix3r const &>((bp::arg("m"))))
		.def(bp::init<Real,Real,Real,Real,Real,Real,Real,Real,Real>((bp::arg("n00"),bp::arg("n01"),bp::arg("n02"),bp::arg("n10"),bp::arg("n11"),bp::arg("n12"),bp::arg("n20"),bp::arg("n21"),bp::arg("n22"))))
		// .def(bp::init<const Vector3r&, const Vector3r&, const Vector3r&, bool>((bp::arg("v0"),bp::arg("v1"),bp::arg("v2"),bp::arg("columns"))))
		.def(bp::init<Real,Real,Real>((bp::arg("n00"),bp::arg("n11"),bp::arg("n22"))))
		//.def(bp::init<Vector3r>((bp::arg("diag"))))
		.def("Determinant",&Matrix3r::Determinant)
		.def("Inverse",&Matrix3r::Inverse)
		.def("Transpose",&Matrix3r::Transpose)
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
		.def(bp::self == bp::self)
		.def(bp::self != bp::self)
		.def(-bp::self)
 		.def("__len__",&::Matrix3_len).staticmethod("__len__")
		.def("__setitem__",&::Matrix3_set_item)
		.def("__getitem__",&::Matrix3_get_item)
		.def("__str__",&::Matrix3_str)
		.def("__repr__",&::Matrix3_str)
		/* extras for matrices */
		.def("__setitem__",&::Matrix3_set_item_linear)
		.def("__getitem__",&::Matrix3_get_item_linear)
		.def_readonly("IDENTITY",Matrix3r::IDENTITY)
		.def_readonly("ZERO",Matrix3r::ZERO)
	;
	bp::class_<Quaternionr>("Quaternion",bp::init<>())
		.def(bp::init<Vector3r,Real>((bp::arg("axis"),bp::arg("angle"))))
		//.def(bp::init<Matrix3r>((bp::arg("rotMatrix"))))
		.def(bp::init<Quaternionr>((bp::arg("other"))))
		.def("Align",&Quaternionr::Align,((bp::arg("v1"),bp::arg("v2"))))
		.def("Conjugate",&Quaternionr::Conjugate)
		.def("FromAxisAngle",&Quaternionr::FromAxisAngle,((bp::arg("axis"),bp::arg("angle"))))
		.def("Inverse",&Quaternionr::Inverse)
		.def("Length",&Quaternionr::Length)
		.def("Normalize",&Quaternionr::Normalize)
		.def("ToRotationMatrix",&Quaternionr::ToRotationMatrix)
		.def("ToAxisAngle",Quaternion_ToAxisAngle)
		.def(bp::self != bp::self)
		.def(bp::self == bp::self)
		.def(bp::self * bp::self)
		.def(bp::self * bp::other<Real>())
		.def(bp::other<Real>() * bp::self)
		.def(bp::self * bp::other<Vector3r>())
		//.def(bp::self *= bp::other<Real>())
		.def(bp::self + bp::self)
		.def(bp::self += bp::self)
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
		.def_readonly("IDENTITY",Matrix3r::IDENTITY)
		.def_readonly("ZERO",Matrix3r::ZERO)
	;
	bp::class_<Vector2r>("Vector2",bp::init<>())
		.def(bp::init<Vector2r>((bp::arg("other"))))
		.def(bp::init<Real,Real>((bp::arg("x"),bp::arg("y"))))
		.def("Dot",&Vector2r::Dot)
		.def("Length",&Vector2r::Length)
		.def("SquaredLength",&Vector2r::SquaredLength)
		.def("Normalize",&Vector2r::Normalize)
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
		.def("__len__",&::Vector2_len).staticmethod("__len__")
		.def("__setitem__",&::Vector2_set_item)
		.def("__getitem__",&::Vector2_get_item)
		.def("__str__",&::Vector2_str)
		.def("__repr__",&::Vector2_str)
		.def_readonly("ONE",Vector2r::ONE)
		.def_readonly("UNIT_X",Vector2r::UNIT_X)
		.def_readonly("UNIT_Y",Vector2r::UNIT_Y)
		.def_readonly("ZERO",Vector2r::ZERO)
	;
	bp::class_<Vector3r>("Vector3",bp::init<>())
		.def(bp::init<Vector3r>((bp::arg("other"))))
		.def(bp::init<Real,Real,Real>((bp::arg("x"),bp::arg("y"),bp::arg("z"))))
		.def("Dot",&Vector3r::Dot)
		.def("Cross",&Vector3r::Cross)
		.def("Length",&Vector3r::Length)
		.def("SquaredLength",&Vector3r::SquaredLength)
		.def("Normalize",&Vector3r::Normalize)
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
		.def("__len__",&::Vector3_len).staticmethod("__len__")
		.def("__setitem__",&::Vector3_set_item)
		.def("__getitem__",&::Vector3_get_item)
		.def("__str__",&::Vector3_str)
		.def("__repr__",&::Vector3_str)
		.def_readonly("ONE",Vector3r::ONE)
		.def_readonly("UNIT_X",Vector3r::UNIT_X)
		.def_readonly("UNIT_Y",Vector3r::UNIT_Y)
		.def_readonly("UNIT_Z",Vector3r::UNIT_Z)
		.def_readonly("ZERO",Vector3r::ZERO)
	;	
	
};








