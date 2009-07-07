#include<boost/lexical_cast.hpp>
#include<string>
#include<stdexcept>

#define IDX_CHECK(i,MAX){ if(i<0 || i>=MAX) { PyErr_SetString(PyExc_IndexError, "Index out of range 0.." #MAX); boost::python::throw_error_already_set(); } }
void Vector2_set_item(Vector2r & self, int idx, Real value){ IDX_CHECK(idx,2); self[idx]=value; }
void Vector3_set_item(Vector3r & self, int idx, Real value){ IDX_CHECK(idx,3); self[idx]=value; }
void Quaternion_set_item(Quaternionr & self, int idx, Real value){ IDX_CHECK(idx,4); self[idx]=value; }
Real Vector2_get_item(const Vector2r & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }
Real Vector3_get_item(const Vector3r & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
Real Quaternion_get_item(const Quaternionr & self, int idx){ IDX_CHECK(idx,4); return self[idx]; }
std::string Vector2_str(const Vector2r & self){ return std::string("Vector2(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Vector3_str(const Vector3r & self){ return std::string("Vector3(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Quaternion_str(const Quaternionr & self){ Vector3r axis; Real angle; self.ToAxisAngle(axis,angle); return std::string("Quaternion((")+boost::lexical_cast<std::string>(axis[0])+","+boost::lexical_cast<std::string>(axis[1])+","+boost::lexical_cast<std::string>(axis[2])+"),"+boost::lexical_cast<std::string>(angle)+")";}
int Vector2_len(){return 2;}
int Vector3_len(){return 3;}
int Quaternion_len(){return 4;}
#undef IDX_CHECK

// automagic converter from tuple to Vector3r
struct custom_Vector3r_from_tuple{
	custom_Vector3r_from_tuple(){
		boost::python::converter::registry::push_back(&convertible,&construct,boost::python::type_id<Vector3r>());
	}
	static void* convertible(PyObject* obj_ptr){
		if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=3) return 0;
		return obj_ptr;
	}
	static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data){
		void* storage=((boost::python::converter::rvalue_from_python_storage<Vector3r>*)(data))->storage.bytes;
		new (storage) Vector3r(boost::python::extract<double>(PySequence_GetItem(obj_ptr,0)),boost::python::extract<double>(PySequence_GetItem(obj_ptr,1)),boost::python::extract<double>(PySequence_GetItem(obj_ptr,2)));
		data->convertible=storage;
	}
};


