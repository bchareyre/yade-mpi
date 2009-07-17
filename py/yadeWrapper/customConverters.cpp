// 2009 © Václav Šmilauer <eudoxos@arcig.cz>

#include<boost/python.hpp>
#include<boost/python/suite/indexing/container_suite.hpp>
#include<boost/python/suite/indexing/vector.hpp>
#include<boost/python/class.hpp>
#include<boost/python/module.hpp>
#include<boost/foreach.hpp>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

#include<vector>
#include<string>
#include<stdexcept>
#include<iostream>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>



using namespace boost::python;

struct custom_Vector3r_from_seq{
	custom_Vector3r_from_seq(){
		 converter::registry::push_back(&convertible,&construct,type_id<Vector3r>());
	}
	static void* convertible(PyObject* obj_ptr){
		 if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=3) return 0;
		 return obj_ptr;
	}
	static void construct(PyObject* obj_ptr, converter::rvalue_from_python_stage1_data* data){
		 void* storage=((converter::rvalue_from_python_storage<Vector3r>*)(data))->storage.bytes;
		 new (storage) Vector3r(extract<Real>(PySequence_GetItem(obj_ptr,0)),extract<Real>(PySequence_GetItem(obj_ptr,1)),extract<Real>(PySequence_GetItem(obj_ptr,2)));
		 data->convertible=storage;
	}
};

/* two-way se3 handling */
struct custom_se3_to_tuple{
	static PyObject* convert(const Se3r& se3){
		tuple ret=make_tuple(se3.position,se3.orientation);
		return incref(ret.ptr());
	}
};
struct custom_Se3r_from_seq{
	custom_Se3r_from_seq(){
		 converter::registry::push_back(&convertible,&construct,type_id<Se3r>());
	}
	static void* convertible(PyObject* obj_ptr){
		 if(!PySequence_Check(obj_ptr)) return 0;
		 if(PySequence_Size(obj_ptr)!=2 && PySequence_Size(obj_ptr)!=7) return 0;
		 return obj_ptr;
	}
	static void construct(PyObject* obj_ptr, converter::rvalue_from_python_stage1_data* data){
		void* storage=((converter::rvalue_from_python_storage<Se3r>*)(data))->storage.bytes;
		new (storage) Se3r; Se3r* se3=(Se3r*)storage;
		if(PySequence_Size(obj_ptr)==2){ // from vector and quaternion
			se3->position=extract<Vector3r>(PySequence_GetItem(obj_ptr,0));
			se3->orientation=extract<Quaternionr>(PySequence_GetItem(obj_ptr,1));
		} else if(PySequence_Size(obj_ptr)==7){ // 3 vector components, 3 axis components, angle
			se3->position=Vector3r(extract<Real>(PySequence_GetItem(obj_ptr,0)),extract<Real>(PySequence_GetItem(obj_ptr,1)),extract<Real>(PySequence_GetItem(obj_ptr,2)));
			Vector3r axis=Vector3r(extract<Real>(PySequence_GetItem(obj_ptr,3)),extract<Real>(PySequence_GetItem(obj_ptr,4)),extract<Real>(PySequence_GetItem(obj_ptr,5)));
			Real angle=extract<Real>(PySequence_GetItem(obj_ptr,6));
			se3->orientation.FromAxisAngle(axis,angle);
		} else throw std::logic_error(__FILE__ ": First, the sequence size for Se3r object was 2 or 7, but now is not? (programming error, please report!");
		data->convertible=storage;
	}
};

/*** c++-vector to python-list and python-(list,sequence) to c++-vector converters ***/
template<typename containedType>
struct custom_vector_to_list{
	static PyObject* convert(const std::vector<containedType>& v){
		list ret; FOREACH(const containedType& e, v) ret.append(e);
		return incref(ret.ptr());
	}
};
template<typename containedType>
struct custom_vector_from_seq{
	custom_vector_from_seq(){ converter::registry::push_back(&convertible,&construct,type_id<std::vector<containedType> >()); }
	static void* convertible(PyObject* obj_ptr){  if(!PySequence_Check(obj_ptr)) return 0; return obj_ptr; }
	static void construct(PyObject* obj_ptr, converter::rvalue_from_python_stage1_data* data){
		 void* storage=((converter::rvalue_from_python_storage<std::vector<containedType> >*)(data))->storage.bytes;
		 new (storage) std::vector<containedType>();
		 std::vector<containedType>* v=(std::vector<containedType>*)(storage);
		 int l=PySequence_Size(obj_ptr); v->reserve(l); for(int i=0; i<l; i++) { v->push_back(extract<containedType>(PySequence_GetItem(obj_ptr,i))); }
		 data->convertible=storage;
	}
};




using namespace boost::python;

BOOST_PYTHON_MODULE(_customConverters){
  // class_<std::vector<int> >("vecInt").def(indexing::container_suite<std::vector<int> >());

  custom_Vector3r_from_seq(); // Vector3r is wrapped, it is returned as a Vector3 instance; no to-python converter needed
  custom_Se3r_from_seq(); to_python_converter<Se3r,custom_se3_to_tuple>();
  // register from-python converter and to-python converter
  custom_vector_from_seq<int>(); to_python_converter<std::vector<int>, custom_vector_to_list<int> >();
  custom_vector_from_seq<Real>(); to_python_converter<std::vector<Real>, custom_vector_to_list<Real> >();
  custom_vector_from_seq<Vector3r>(); to_python_converter<std::vector<Vector3r>, custom_vector_to_list<Vector3r> >();
  custom_vector_from_seq<std::string>(); to_python_converter<std::vector<std::string>, custom_vector_to_list<std::string> >();
}





