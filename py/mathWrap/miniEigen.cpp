// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<boost/python.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<string>
#include<stdexcept>
#include<sstream>
#include<iostream>

#include<yade/lib/base/Math.hpp>
#include<yade/lib/pyutil/doc_opts.hpp>


namespace bp=boost::python;

#define IDX_CHECK(i,MAX){ if(i<0 || i>=MAX) { PyErr_SetString(PyExc_IndexError, ("Index out of range 0.." + boost::lexical_cast<std::string>(MAX-1)).c_str()); bp::throw_error_already_set(); } }
#define IDX2_CHECKED_TUPLE_INTS(tuple,max2,arr2) {int l=bp::len(tuple); if(l!=2) { PyErr_SetString(PyExc_IndexError,"Index must be integer or a 2-tuple"); bp::throw_error_already_set(); } for(int _i=0; _i<2; _i++) { bp::extract<int> val(tuple[_i]); if(!val.check()) throw std::runtime_error("Unable to convert "+boost::lexical_cast<std::string>(_i)+"-th index to int."); int v=val(); IDX_CHECK(v,max2[_i]); arr2[_i]=v; }  }

void Vector6r_set_item(Vector6r & self, int idx, Real value){ IDX_CHECK(idx,6); self[idx]=value; }
void Vector6i_set_item(Vector6i & self, int idx, int  value){ IDX_CHECK(idx,6); self[idx]=value; }
void Vector3r_set_item(Vector3r & self, int idx, Real value){ IDX_CHECK(idx,3); self[idx]=value; }
void Vector3i_set_item(Vector3i & self, int idx, int  value){ IDX_CHECK(idx,3); self[idx]=value; }
void Vector2r_set_item(Vector2r & self, int idx, Real value){ IDX_CHECK(idx,2); self[idx]=value; }
void Vector2i_set_item(Vector2i & self, int idx, int  value){ IDX_CHECK(idx,2); self[idx]=value; }

void Quaternionr_set_item(Quaternionr & self, int idx, Real value){ IDX_CHECK(idx,4);  if(idx==0) self.x()=value; else if(idx==1) self.y()=value; else if(idx==2) self.z()=value; else if(idx==3) self.w()=value; }
void Matrix3r_set_item(Matrix3r & self, bp::tuple _idx, Real value){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); self(idx[0],idx[1])=value; }
void Matrix3r_set_item_linear(Matrix3r & self, int idx, Real value){ IDX_CHECK(idx,9); self(idx/3,idx%3)=value; }

void Matrix6r_set_item(Matrix6r & self, bp::tuple _idx, Real value){ int idx[2]; int mx[2]={6,6}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); self(idx[0],idx[1])=value; }
void Matrix6r_set_item_linear(Matrix6r & self, int idx, Real value){ IDX_CHECK(idx,36); self(idx/6,idx%6)=value; }

Real Vector6r_get_item(const Vector6r & self, int idx){ IDX_CHECK(idx,6); return self[idx]; }
int  Vector6i_get_item(const Vector6r & self, int idx){ IDX_CHECK(idx,6); return self[idx]; }
Real Vector3r_get_item(const Vector3r & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
int  Vector3i_get_item(const Vector3i & self, int idx){ IDX_CHECK(idx,3); return self[idx]; }
Real Vector2r_get_item(const Vector2r & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }
int  Vector2i_get_item(const Vector2i & self, int idx){ IDX_CHECK(idx,2); return self[idx]; }

Real Quaternionr_get_item(const Quaternionr & self, int idx){ IDX_CHECK(idx,4); if(idx==0) return self.x(); if(idx==1) return self.y(); if(idx==2) return self.z(); return self.w(); }
Real Matrix3r_get_item(Matrix3r & self, bp::tuple _idx){ int idx[2]; int mx[2]={3,3}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); return self(idx[0],idx[1]); }
Real Matrix3r_get_item_linear(Matrix3r & self, int idx){ IDX_CHECK(idx,9); return self(idx/3,idx%3); }
Real Matrix6r_get_item(Matrix6r & self, bp::tuple _idx){ int idx[2]; int mx[2]={6,6}; IDX2_CHECKED_TUPLE_INTS(_idx,mx,idx); return self(idx[0],idx[1]); }
Real Matrix6r_get_item_linear(Matrix6r & self, int idx){ IDX_CHECK(idx,36); return self(idx/6,idx%6); }

std::string Vector6r_str(const Vector6r & self){ return std::string("Vector6(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+", "+boost::lexical_cast<std::string>(self[3])+","+boost::lexical_cast<std::string>(self[4])+","+boost::lexical_cast<std::string>(self[5])+")";}
std::string Vector6i_str(const Vector6i & self){ return std::string("Vector6i(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+", "+boost::lexical_cast<std::string>(self[3])+","+boost::lexical_cast<std::string>(self[4])+","+boost::lexical_cast<std::string>(self[5])+")";}
std::string Vector3r_str(const Vector3r & self){ return std::string("Vector3(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Vector3i_str(const Vector3i & self){ return std::string("Vector3i(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+","+boost::lexical_cast<std::string>(self[2])+")";}
std::string Vector2r_str(const Vector2r & self){ return std::string("Vector2(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Vector2i_str(const Vector2i & self){ return std::string("Vector2i(")+boost::lexical_cast<std::string>(self[0])+","+boost::lexical_cast<std::string>(self[1])+")";}
std::string Quaternionr_str(const Quaternionr & self){ AngleAxisr aa(self); return std::string("Quaternion((")+boost::lexical_cast<std::string>(aa.axis()[0])+","+boost::lexical_cast<std::string>(aa.axis()[1])+","+boost::lexical_cast<std::string>(aa.axis()[2])+"),"+boost::lexical_cast<std::string>(aa.angle())+")";}
std::string Matrix3r_str(const Matrix3r & self){ std::ostringstream oss; oss<<"Matrix3("; for(int i=0; i<3; i++) for(int j=0; j<3; j++) oss<<self(i,j)<<((i==2 && j==2)?")":",")<<((i<2 && j==2)?" ":""); return oss.str(); }
std::string Matrix6r_str(const Matrix6r & self){ std::ostringstream oss; oss<<"Matrix6("; for(int i=0; i<6; i++) for(int j=0; j<6; j++) oss<<self(i,j)<<((i==5 && j==5)?")":",")<<((i<5 && j==5)?" ":""); return oss.str(); }

int Vector6r_len(){return 6;}
int Vector6i_len(){return 6;}
int Vector3r_len(){return 3;}
int Vector3i_len(){return 3;}
int Vector2r_len(){return 2;}
int Vector2i_len(){return 2;}
int Quaternionr_len(){return 4;}
int Matrix3r_len(){return 9;}
int Matrix6r_len(){return 36;}

// pickling support
struct Matrix3r_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Matrix3r& x){ return bp::make_tuple(x(0,0),x(0,1),x(0,2),x(1,0),x(1,1),x(1,2),x(2,0),x(2,1),x(2,2));} };
struct Quaternionr_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Quaternionr& x){ return bp::make_tuple(x.w(),x.x(),x.y(),x.z());} };
struct Vector6r_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Vector6r& x){ return bp::make_tuple(x[0],x[1],x[2],x[3],x[4],x[5]);} };
struct Vector6i_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Vector6i& x){ return bp::make_tuple(x[0],x[1],x[2],x[3],x[4],x[5]);} };
struct Vector3r_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Vector3r& x){ return bp::make_tuple(x[0],x[1],x[2]);} };
struct Vector3i_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Vector3i& x){ return bp::make_tuple(x[0],x[1],x[2]);} };
struct Vector2r_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Vector2r& x){ return bp::make_tuple(x[0],x[1]);} };
struct Vector2i_pickle: bp::pickle_suite{static bp::tuple getinitargs(const Vector2i& x){ return bp::make_tuple(x[0],x[1]);} };


/* template to define custom converter from sequence/list or approriate length and type, to eigen's Vector
   - length is stored in VT::RowsAtCompileTime
	- type is VT::Scalar
*/
template<class VT>
struct custom_VectorAnyAny_from_sequence{
	custom_VectorAnyAny_from_sequence(){ bp::converter::registry::push_back(&convertible,&construct,bp::type_id<VT>()); }
	static void* convertible(PyObject* obj_ptr){ if(!PySequence_Check(obj_ptr) || PySequence_Size(obj_ptr)!=VT::RowsAtCompileTime) return 0; return obj_ptr; }
	static void construct(PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data){
		void* storage=((bp::converter::rvalue_from_python_storage<VT>*)(data))->storage.bytes;
		new (storage) VT;
		for(size_t i=0; i<VT::RowsAtCompileTime; i++) (*((VT*)storage))[i]=bp::extract<typename VT::Scalar>(PySequence_GetItem(obj_ptr,i));
		data->convertible=storage;
	}
};

static Matrix3r* Matrix3r_fromElements(Real m00, Real m01, Real m02, Real m10, Real m11, Real m12, Real m20, Real m21, Real m22){ Matrix3r* m(new Matrix3r); (*m)<<m00,m01,m02,m10,m11,m12,m20,m21,m22; return m; }
static Vector6r* Vector6r_fromElements(Real v0, Real v1, Real v2, Real v3, Real v4, Real v5){ Vector6r* v(new Vector6r); (*v)<<v0,v1,v2,v3,v4,v5; return v; }
static Vector6i* Vector6i_fromElements(int v0, int v1, int v2, int v3, int v4, int v5){ Vector6i* v(new Vector6i); (*v)<<v0,v1,v2,v3,v4,v5; return v; }
static Vector3r Matrix3r_diagonal(const Matrix3r& m){ return Vector3r(m.diagonal()); }
static Vector3r Matrix3r_row(const Matrix3r& m, int ix){ IDX_CHECK(ix,3); return Vector3r(m.row(ix)); }
static Vector3r Matrix3r_col(const Matrix3r& m, int ix){ IDX_CHECK(ix,3); return Vector3r(m.col(ix)); }
static Vector6r Matrix3r_toVoigt(const Matrix3r& m, bool strain=false){ return tensor_toVoigt(m,strain); }
static Matrix3r Vector6r_toSymmTensor(const Vector6r& v, bool strain=false){ return voigt_toSymmTensor(v,strain); }
static Vector6r Matrix6r_diagonal(const Matrix6r& m){ return Vector6r(m.diagonal()); }
static Vector6r Matrix6r_row(const Matrix6r& m, int ix){ IDX_CHECK(ix,6); return Vector6r(m.row(ix)); }
static Vector6r Matrix6r_col(const Matrix6r& m, int ix){ IDX_CHECK(ix,6); return Vector6r(m.col(ix)); }
static Quaternionr Quaternionr_setFromTwoVectors(Quaternionr& q, const Vector3r& u, const Vector3r& v){ return q.setFromTwoVectors(u,v); }
static Vector3r Quaternionr_Rotate(Quaternionr& q, const Vector3r& u){ return q*u; }
// supposed to return raw pointer (or auto_ptr), boost::python takes care of the lifetime management
static Quaternionr* Quaternionr_fromAxisAngle(const Vector3r& axis, const Real angle){ return new Quaternionr(AngleAxisr(angle,axis)); }
static Quaternionr* Quaternionr_fromAngleAxis(const Real angle, const Vector3r& axis){ return new Quaternionr(AngleAxisr(angle,axis)); }
static bp::tuple Quaternionr_toAxisAngle(const Quaternionr& self){ AngleAxisr aa(self); return bp::make_tuple(aa.axis(),aa.angle());}
static bp::tuple Quaternionr_toAngleAxis(const Quaternionr& self){ AngleAxisr aa(self); return bp::make_tuple(aa.angle(),aa.axis());}

static Real Vector3r_dot(const Vector3r& self, const Vector3r& v){ return self.dot(v); }
static Matrix3r Vector3r_outer(const Vector3r& self, const Vector3r& v){ return self*v.transpose(); }
static Real Vector3i_dot(const Vector3i& self, const Vector3i& v){ return self.dot(v); }
static Real Vector2r_dot(const Vector2r& self, const Vector2r& v){ return self.dot(v); }
static Real Vector2i_dot(const Vector2i& self, const Vector2i& v){ return self.dot(v); }
static Vector3r Vector3r_cross(const Vector3r& self, const Vector3r& v){ return self.cross(v); }
static Vector3i Vector3i_cross(const Vector3i& self, const Vector3i& v){ return self.cross(v); }
static Vector3r Vector6r_head(const Vector6r& self){ return self.head<3>(); }
static Vector3r Vector6r_tail(const Vector6r& self){ return self.tail<3>(); }
static Vector3i Vector6i_head(const Vector6i& self){ return self.head<3>(); }
static Vector3i Vector6i_tail(const Vector6i& self){ return self.tail<3>(); }
static bool Quaternionr__eq__(const Quaternionr& q1, const Quaternionr& q2){ return q1==q2; }
static bool Quaternionr__neq__(const Quaternionr& q1, const Quaternionr& q2){ return q1!=q2; }
#include<Eigen/SVD>
static bp::tuple Matrix3r_polarDecomposition(const Matrix3r& self){ Matrix3r unitary,positive; Matrix_computeUnitaryPositive(self,&unitary,&positive); return bp::make_tuple(unitary,positive); }
static bp::tuple Matrix3r_eigenDecomposition(const Matrix3r& self){ Matrix3r rot,diag; matrixEigenDecomposition(self,rot,diag); return bp::make_tuple(rot,Vector3r(diag.diagonal())); }
static bp::tuple Matrix3r_svd(const Matrix3r& self){ Matrix3r u,s,v; Matrix_SVD(self,&u,&s,&v); return bp::make_tuple(u,s,v); }
static bp::tuple Matrix6r_polarDecomposition(const Matrix6r& self){ Matrix6r unitary,positive; Matrix_computeUnitaryPositive(self,&unitary,&positive); return bp::make_tuple(unitary,positive); }
static bp::tuple Matrix6r_eigenDecomposition(const Matrix6r& self){ Matrix6r rot,diag; matrixEigenDecomposition(self,rot,diag); return bp::make_tuple(rot,Vector6r(diag.diagonal())); }
static bp::tuple Matrix6r_svd(const Matrix6r& self){ Matrix6r u,s,v; Matrix_SVD(self,&u,&s,&v); return bp::make_tuple(u,s,v); }

#undef IDX_CHECK


#define EIG_WRAP_METH1(klass,meth) static klass klass##_##meth(const klass& self){ return self.meth(); }
#define EIG_WRAP_METH0(klass,meth) static klass klass##_##meth(){ return klass().meth(); }
EIG_WRAP_METH1(Matrix3r,transpose);
EIG_WRAP_METH1(Matrix3r,inverse);
EIG_WRAP_METH1(Matrix6r,transpose);
EIG_WRAP_METH1(Matrix6r,inverse);

EIG_WRAP_METH0(Matrix3r,Zero);
EIG_WRAP_METH0(Matrix3r,Identity);
EIG_WRAP_METH0(Matrix6r,Zero);
EIG_WRAP_METH0(Matrix6r,Identity);
EIG_WRAP_METH0(Vector6r,Zero); EIG_WRAP_METH0(Vector6r,Ones);
EIG_WRAP_METH0(Vector6i,Zero); EIG_WRAP_METH0(Vector6i,Ones);
EIG_WRAP_METH0(Vector3r,Zero); EIG_WRAP_METH0(Vector3r,UnitX); EIG_WRAP_METH0(Vector3r,UnitY); EIG_WRAP_METH0(Vector3r,UnitZ); EIG_WRAP_METH0(Vector3r,Ones);
EIG_WRAP_METH0(Vector3i,Zero); EIG_WRAP_METH0(Vector3i,UnitX); EIG_WRAP_METH0(Vector3i,UnitY); EIG_WRAP_METH0(Vector3i,UnitZ); EIG_WRAP_METH0(Vector3i,Ones);
EIG_WRAP_METH0(Vector2r,Zero); EIG_WRAP_METH0(Vector2r,UnitX); EIG_WRAP_METH0(Vector2r,UnitY); EIG_WRAP_METH0(Vector2r,Ones);
EIG_WRAP_METH0(Vector2i,Zero); EIG_WRAP_METH0(Vector2i,UnitX); EIG_WRAP_METH0(Vector2i,UnitY); EIG_WRAP_METH0(Vector2i,Ones);
EIG_WRAP_METH0(Quaternionr,Identity);

#define EIG_OP1(klass,op,sym) typeof((sym klass()).eval()) klass##op(const klass& self){ return (sym self).eval();}
#define EIG_OP2(klass,op,sym,klass2) typeof((klass() sym klass2()).eval()) klass##op##klass2(const klass& self, const klass2& other){ return (self sym other).eval(); }
#define EIG_OP2_INPLACE(klass,op,sym,klass2) klass klass##op##klass2(klass& self, const klass2& other){ self sym other; return self; }


EIG_OP1(Matrix3r,__neg__,-)
EIG_OP2(Matrix3r,__add__,+,Matrix3r) EIG_OP2_INPLACE(Matrix3r,__iadd__,+=,Matrix3r)
EIG_OP2(Matrix3r,__sub__,-,Matrix3r) EIG_OP2_INPLACE(Matrix3r,__isub__,-=,Matrix3r)
EIG_OP2(Matrix3r,__mul__,*,Real) EIG_OP2(Matrix3r,__rmul__,*,Real) EIG_OP2_INPLACE(Matrix3r,__imul__,*=,Real)
EIG_OP2(Matrix3r,__mul__,*,int) EIG_OP2(Matrix3r,__rmul__,*,int) EIG_OP2_INPLACE(Matrix3r,__imul__,*=,int)
EIG_OP2(Matrix3r,__mul__,*,Vector3r) EIG_OP2(Matrix3r,__rmul__,*,Vector3r)
EIG_OP2(Matrix3r,__mul__,*,Matrix3r) EIG_OP2_INPLACE(Matrix3r,__imul__,*=,Matrix3r)
EIG_OP2(Matrix3r,__div__,/,Real) EIG_OP2_INPLACE(Matrix3r,__idiv__,/=,Real)
EIG_OP2(Matrix3r,__div__,/,int) EIG_OP2_INPLACE(Matrix3r,__idiv__,/=,int)

EIG_OP1(Matrix6r,__neg__,-)
EIG_OP2(Matrix6r,__add__,+,Matrix6r) EIG_OP2_INPLACE(Matrix6r,__iadd__,+=,Matrix6r)
EIG_OP2(Matrix6r,__sub__,-,Matrix6r) EIG_OP2_INPLACE(Matrix6r,__isub__,-=,Matrix6r)
EIG_OP2(Matrix6r,__mul__,*,Real) EIG_OP2(Matrix6r,__rmul__,*,Real) EIG_OP2_INPLACE(Matrix6r,__imul__,*=,Real)
EIG_OP2(Matrix6r,__mul__,*,int) EIG_OP2(Matrix6r,__rmul__,*,int) EIG_OP2_INPLACE(Matrix6r,__imul__,*=,int)
EIG_OP2(Matrix6r,__mul__,*,Vector6r) EIG_OP2(Matrix6r,__rmul__,*,Vector6r)
EIG_OP2(Matrix6r,__mul__,*,Matrix6r) EIG_OP2_INPLACE(Matrix6r,__imul__,*=,Matrix6r)
EIG_OP2(Matrix6r,__div__,/,Real) EIG_OP2_INPLACE(Matrix6r,__idiv__,/=,Real)
EIG_OP2(Matrix6r,__div__,/,int) EIG_OP2_INPLACE(Matrix6r,__idiv__,/=,int)

EIG_OP1(Vector6r,__neg__,-);
EIG_OP2(Vector6r,__add__,+,Vector6r); EIG_OP2_INPLACE(Vector6r,__iadd__,+=,Vector6r)
EIG_OP2(Vector6r,__sub__,-,Vector6r); EIG_OP2_INPLACE(Vector6r,__isub__,-=,Vector6r)
EIG_OP2(Vector6r,__mul__,*,Real) EIG_OP2(Vector6r,__rmul__,*,Real) EIG_OP2_INPLACE(Vector6r,__imul__,*=,Real) EIG_OP2(Vector6r,__div__,/,Real) EIG_OP2_INPLACE(Vector6r,__idiv__,/=,Real)
EIG_OP2(
Vector6r,__mul__,*,int) EIG_OP2(Vector6r,__rmul__,*,int) EIG_OP2_INPLACE(Vector6r,__imul__,*=,int) EIG_OP2(Vector6r,__div__,/,int) EIG_OP2_INPLACE(Vector6r,__idiv__,/=,int)

EIG_OP1(Vector6i,__neg__,-);
EIG_OP2(Vector6i,__add__,+,Vector6i); EIG_OP2_INPLACE(Vector6i,__iadd__,+=,Vector6i)
EIG_OP2(Vector6i,__sub__,-,Vector6i); EIG_OP2_INPLACE(Vector6i,__isub__,-=,Vector6i)
EIG_OP2(
Vector6i,__mul__,*,int) EIG_OP2(Vector6i,__rmul__,*,int) EIG_OP2_INPLACE(Vector6i,__imul__,*=,int) EIG_OP2(Vector6i,__div__,/,int) EIG_OP2_INPLACE(Vector6i,__idiv__,/=,int)

EIG_OP1(Vector3r,__neg__,-);
EIG_OP2(Vector3r,__add__,+,Vector3r); EIG_OP2_INPLACE(Vector3r,__iadd__,+=,Vector3r)
EIG_OP2(Vector3r,__sub__,-,Vector3r); EIG_OP2_INPLACE(Vector3r,__isub__,-=,Vector3r)
EIG_OP2(Vector3r,__mul__,*,Real) EIG_OP2(Vector3r,__rmul__,*,Real) EIG_OP2_INPLACE(Vector3r,__imul__,*=,Real) EIG_OP2(Vector3r,__div__,/,Real) EIG_OP2_INPLACE(Vector3r,__idiv__,/=,Real)
EIG_OP2(
Vector3r,__mul__,*,int) EIG_OP2(Vector3r,__rmul__,*,int) EIG_OP2_INPLACE(Vector3r,__imul__,*=,int) EIG_OP2(Vector3r,__div__,/,int) EIG_OP2_INPLACE(Vector3r,__idiv__,/=,int)

EIG_OP1(Vector3i,__neg__,-);
EIG_OP2(Vector3i,__add__,+,Vector3i); EIG_OP2_INPLACE(Vector3i,__iadd__,+=,Vector3i)
EIG_OP2(Vector3i,__sub__,-,Vector3i); EIG_OP2_INPLACE(Vector3i,__isub__,-=,Vector3i)
EIG_OP2(Vector3i,__mul__,*,int) EIG_OP2(Vector3i,__rmul__,*,int)  EIG_OP2_INPLACE(Vector3i,__imul__,*=,int)

EIG_OP1(Vector2r,__neg__,-);
EIG_OP2(Vector2r,__add__,+,Vector2r); EIG_OP2_INPLACE(Vector2r,__iadd__,+=,Vector2r)
EIG_OP2(Vector2r,__sub__,-,Vector2r); EIG_OP2_INPLACE(Vector2r,__isub__,-=,Vector2r)
EIG_OP2(Vector2r,__mul__,*,Real) EIG_OP2(Vector2r,__rmul__,*,Real) EIG_OP2_INPLACE(Vector2r,__imul__,*=,Real) EIG_OP2(Vector2r,__div__,/,Real) EIG_OP2_INPLACE(Vector2r,__idiv__,/=,Real)
EIG_OP2(Vector2r,__mul__,*,int) EIG_OP2(Vector2r,__rmul__,*,int) EIG_OP2_INPLACE(Vector2r,__imul__,*=,int) EIG_OP2(Vector2r,__div__,/,int) EIG_OP2_INPLACE(Vector2r,__idiv__,/=,int)

EIG_OP1(Vector2i,__neg__,-);
EIG_OP2(Vector2i,__add__,+,Vector2i); EIG_OP2_INPLACE(Vector2i,__iadd__,+=,Vector2i)
EIG_OP2(Vector2i,__sub__,-,Vector2i); EIG_OP2_INPLACE(Vector2i,__isub__,-=,Vector2i)
EIG_OP2(Vector2i,__mul__,*,int)  EIG_OP2_INPLACE(Vector2i,__imul__,*=,int) EIG_OP2(Vector2i,__rmul__,*,int)


BOOST_PYTHON_MODULE(miniEigen){
	bp::scope().attr("__doc__")="Basic math functions for Yade: small matrix, vector and quaternion classes. This module internally wraps small parts of the `Eigen <http://eigen.tuxfamily.org>`_ library. Refer to its documentation for details. All classes in this module support pickling.";

	YADE_SET_DOCSTRING_OPTS;


	custom_VectorAnyAny_from_sequence<Vector6r>();
	custom_VectorAnyAny_from_sequence<Vector6i>();
	custom_VectorAnyAny_from_sequence<Vector3r>();
	custom_VectorAnyAny_from_sequence<Vector3i>();
	custom_VectorAnyAny_from_sequence<Vector2r>();
	custom_VectorAnyAny_from_sequence<Vector2i>();

	bp::class_<Matrix3r>("Matrix3","3x3 float matrix.\n\nSupported operations (``m`` is a Matrix3, ``f`` if a float/int, ``v`` is a Vector3): ``-m``, ``m+m``, ``m+=m``, ``m-m``, ``m-=m``, ``m*f``, ``f*m``, ``m*=f``, ``m/f``, ``m/=f``, ``m*m``, ``m*=m``, ``m*v``, ``v*m``, ``m==m``, ``m!=m``.",bp::init<>())
		.def(bp::init<Matrix3r const &>((bp::arg("m"))))
		.def(bp::init<Quaternionr const &>((bp::arg("q"))))
		.def("__init__",bp::make_constructor(&Matrix3r_fromElements,bp::default_call_policies(),(bp::arg("m00"),bp::arg("m01"),bp::arg("m02"),bp::arg("m10"),bp::arg("m11"),bp::arg("m12"),bp::arg("m20"),bp::arg("m21"),bp::arg("m22"))))
		.def_pickle(Matrix3r_pickle())
		//
		.def("determinant",&Matrix3r::determinant)
		.def("trace",&Matrix3r::trace)
		.def("inverse",&Matrix3r_inverse)
		.def("transpose",&Matrix3r_transpose)
		.def("polarDecomposition",&Matrix3r_polarDecomposition)
		.def("eigenDecomposition",&Matrix3r_eigenDecomposition)
		.def("svd",&Matrix3r_svd)
		.def("diagonal",&Matrix3r_diagonal)
		.def("row",&Matrix3r_row)
		.def("col",&Matrix3r_col)

		//
		.def("__neg__",&Matrix3r__neg__)
		.def("__add__",&Matrix3r__add__Matrix3r).def("__iadd__",&Matrix3r__iadd__Matrix3r)
		.def("__sub__",&Matrix3r__sub__Matrix3r).def("__isub__",&Matrix3r__isub__Matrix3r)
		.def("__mul__",&Matrix3r__mul__Real).def("__rmul__",&Matrix3r__rmul__Real).def("__imul__",&Matrix3r__imul__Real)
		.def("__mul__",&Matrix3r__mul__int).def("__rmul__",&Matrix3r__rmul__int).def("__imul__",&Matrix3r__imul__int)
		.def("__mul__",&Matrix3r__mul__Vector3r).def("__rmul__",&Matrix3r__rmul__Vector3r)
		.def("__mul__",&Matrix3r__mul__Matrix3r).def("__imul__",&Matrix3r__imul__Matrix3r)
		.def("__div__",&Matrix3r__div__Real).def("__idiv__",&Matrix3r__idiv__Real)
		.def("__div__",&Matrix3r__div__int).def("__idiv__",&Matrix3r__idiv__int)
		.def(bp::self == bp::self)
		.def(bp::self != bp::self)
		//
 		.def("__len__",&::Matrix3r_len).staticmethod("__len__").def("__setitem__",&::Matrix3r_set_item).def("__getitem__",&::Matrix3r_get_item).def("__str__",&::Matrix3r_str).def("__repr__",&::Matrix3r_str)
		/* extras for matrices */
		.def("__setitem__",&::Matrix3r_set_item_linear).def("__getitem__",&::Matrix3r_get_item_linear)
		.add_static_property("Identity",&Matrix3r_Identity)
		.add_static_property("Zero",&Matrix3r_Zero)
		// specials
		.def("toVoigt",&Matrix3r_toVoigt,(bp::arg("strain")=false),"Convert 2nd order tensor to 6-vector (Voigt notation), symmetrizing the tensor;	if *strain* is ``True``, multiply non-diagonal compoennts by 2.")

	;
	bp::class_<Matrix6r>("Matrix6","6x6 float matrix.\n\nSupported operations (``m`` is a Matrix6, ``f`` if a float/int, ``v`` is a Vector6): ``-m``, ``m+m``, ``m+=m``, ``m-m``, ``m-=m``, ``m*f``, ``f*m``, ``m*=f``, ``m/f``, ``m/=f``, ``m*m``, ``m*=m``, ``m*v``, ``v*m``, ``m==m``, ``m!=m``.",bp::init<>())
		.def(bp::init<Matrix6r const &>((bp::arg("m"))))
		//
		.def("determinant",&Matrix6r::determinant)
		.def("trace",&Matrix6r::trace)
		.def("inverse",&Matrix6r_inverse)
		.def("transpose",&Matrix6r_transpose)
		.def("diagonal",&Matrix6r_diagonal)
		.def("row",&Matrix6r_row)
		.def("col",&Matrix6r_col)
		.def("polarDecomposition",&Matrix6r_polarDecomposition)
		.def("eigenDecomposition",&Matrix6r_eigenDecomposition)
		.def("svd",&Matrix6r_svd)
		//
		.def("__neg__",&Matrix6r__neg__)
		.def("__add__",&Matrix6r__add__Matrix6r).def("__iadd__",&Matrix6r__iadd__Matrix6r)
		.def("__sub__",&Matrix6r__sub__Matrix6r).def("__isub__",&Matrix6r__isub__Matrix6r)
		.def("__mul__",&Matrix6r__mul__Real).def("__rmul__",&Matrix6r__rmul__Real).def("__imul__",&Matrix6r__imul__Real)
		.def("__mul__",&Matrix6r__mul__int).def("__rmul__",&Matrix6r__rmul__int).def("__imul__",&Matrix6r__imul__int)
		.def("__mul__",&Matrix6r__mul__Vector6r).def("__rmul__",&Matrix6r__rmul__Vector6r)
		.def("__mul__",&Matrix6r__mul__Matrix6r).def("__imul__",&Matrix6r__imul__Matrix6r)
		.def("__div__",&Matrix6r__div__Real).def("__idiv__",&Matrix6r__idiv__Real)
		.def("__div__",&Matrix6r__div__int).def("__idiv__",&Matrix6r__idiv__int)
		.def(bp::self == bp::self)
		.def(bp::self != bp::self)
		//
 		.def("__len__",&::Matrix6r_len).staticmethod("__len__").def("__setitem__",&::Matrix6r_set_item).def("__getitem__",&::Matrix6r_get_item).def("__str__",&::Matrix6r_str).def("__repr__",&::Matrix6r_str)
		/* extras for matrices */
		.def("__setitem__",&::Matrix6r_set_item_linear).def("__getitem__",&::Matrix6r_get_item_linear)
		.add_static_property("Identity",&Matrix6r_Identity)
		.add_static_property("Zero",&Matrix6r_Zero)
		// specials
	;
	bp::class_<Quaternionr>("Quaternion","Quaternion representing rotation.\n\nSupported operations (``q`` is a Quaternion, ``v`` is a Vector3): ``q*q`` (rotation composition), ``q*=q``, ``q*v`` (rotating ``v`` by ``q``), ``q==q``, ``q!=q``.",bp::init<>())
		.def("__init__",bp::make_constructor(&Quaternionr_fromAxisAngle,bp::default_call_policies(),(bp::arg("axis"),bp::arg("angle"))))
		.def("__init__",bp::make_constructor(&Quaternionr_fromAngleAxis,bp::default_call_policies(),(bp::arg("angle"),bp::arg("axis"))))
		.def(bp::init<Real,Real,Real,Real>((bp::arg("w"),bp::arg("x"),bp::arg("y"),bp::arg("z")),"Initialize from coefficients.\n\n.. note:: The order of coefficients is *w*, *x*, *y*, *z*. The [] operator numbers them differently, 0…4 for *x* *y* *z* *w*!"))
		.def(bp::init<Matrix3r>((bp::arg("rotMatrix")))) //,"Initialize from given rotation matrix.")
		.def(bp::init<Quaternionr>((bp::arg("other"))))
		.def_pickle(Quaternionr_pickle())
		// properties
		.add_static_property("Identity",&Quaternionr_Identity)
		// methods
		.def("setFromTwoVectors",&Quaternionr_setFromTwoVectors,((bp::arg("u"),bp::arg("v"))))
		.def("conjugate",&Quaternionr::conjugate)
		.def("toAxisAngle",&Quaternionr_toAxisAngle).def("toAngleAxis",&Quaternionr_toAngleAxis)
		.def("toRotationMatrix",&Quaternionr::toRotationMatrix)
		.def("Rotate",&Quaternionr_Rotate,((bp::arg("v"))))
		.def("inverse",&Quaternionr::inverse)
		.def("norm",&Quaternionr::norm)
		.def("normalize",&Quaternionr::normalize)
		// operators
		.def(bp::self * bp::self)
		.def(bp::self *= bp::self)
		.def(bp::self * bp::other<Vector3r>())
		//.def(bp::self != bp::self).def(bp::self == bp::self) // these don't work... (?)
		.def("__eq__",&Quaternionr__eq__).def("__neq__",&Quaternionr__neq__)
		// specials
		.def("__len__",&Quaternionr_len).staticmethod("__len__")
		.def("__setitem__",&Quaternionr_set_item).def("__getitem__",&Quaternionr_get_item)
		.def("__str__",&Quaternionr_str).def("__repr__",&Quaternionr_str)
	;
	bp::class_<Vector6r>("Vector6","6-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector6, ``m`` is Matrix6): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``, ``v*m``, ``m*v``.\n\nImplicit conversion from sequence (list,tuple, …) of 6 floats.",bp::init<>())
		.def(bp::init<Vector6r>((bp::arg("other"))))
		.def("__init__",bp::make_constructor(&Vector6r_fromElements,bp::default_call_policies(),(bp::arg("v0"),bp::arg("v1"),bp::arg("v2"),bp::arg("v3"),bp::arg("v4"),bp::arg("v5"))))
		.def_pickle(Vector6r_pickle())
		// properties
		.add_static_property("Ones",&Vector6r_Ones).add_static_property("Zero",&Vector6r_Zero)
		//.add_static_property("UnitX",&Vector6r_UnitX).add_static_property("UnitY",&Vector6r_UnitY).add_static_property("UnitZ",&Vector6r_UnitZ)
		// methods
		//.def("dot",&Vector6r_dot).def("cross",&Vector6r_cross)
		.def("norm",&Vector6r::norm).def("squaredNorm",&Vector6r::squaredNorm).def("normalize",&Vector6r::normalize).def("normalized",&Vector6r::normalized)
		.def("head",&Vector6r_head).def("tail",&Vector6r_tail)
		// operators
		.def("__neg__",&Vector6r__neg__) // -v
		.def("__add__",&Vector6r__add__Vector6r).def("__iadd__",&Vector6r__iadd__Vector6r) // +, +=
		.def("__sub__",&Vector6r__sub__Vector6r).def("__isub__",&Vector6r__isub__Vector6r) // -, -=
		.def("__mul__",&Vector6r__mul__Real).def("__rmul__",&Vector6r__rmul__Real) // f*v, v*f
		.def("__div__",&Vector6r__div__Real).def("__idiv__",&Vector6r__idiv__Real) // v/f, v/=f
		.def("__mul__",&Vector6r__mul__int).def("__rmul__",&Vector6r__rmul__int) // f*v, v*f
		.def("__div__",&Vector6r__div__int).def("__idiv__",&Vector6r__idiv__int) // v/f, v/=f
		.def(bp::self != bp::self).def(bp::self == bp::self)
		// specials
		.def("__len__",&::Vector6r_len).staticmethod("__len__")
		.def("__setitem__",&::Vector6r_set_item).def("__getitem__",&::Vector6r_get_item)
		.def("__str__",&::Vector6r_str).def("__repr__",&::Vector6r_str)
		// specials
		.def("toSymmTensor",&Vector6r_toSymmTensor,(bp::args("strain")=false),"Convert Vector6 in the Voigt notation to the corresponding 2nd order symmetric tensor (as Matrix3); if *strain* is ``True``, multiply non-diagonal components by .5")
	;

	bp::class_<Vector6i>("Vector6i","6-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector6): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list,tuple, …) of 6 floats.",bp::init<>())
		.def(bp::init<Vector6i>((bp::arg("other"))))
		.def("__init__",bp::make_constructor(&Vector6i_fromElements,bp::default_call_policies(),(bp::arg("v0"),bp::arg("v1"),bp::arg("v2"),bp::arg("v3"),bp::arg("v4"),bp::arg("v5"))))
		.def_pickle(Vector6i_pickle())
		// properties
		.add_static_property("Ones",&Vector6i_Ones).add_static_property("Zero",&Vector6i_Zero)
		//.add_static_property("UnitX",&Vector6i_UnitX).add_static_property("UnitY",&Vector6i_UnitY).add_static_property("UnitZ",&Vector6i_UnitZ)
		// methods
		//.def("dot",&Vector6i_dot).def("cross",&Vector6i_cross)
		.def("head",&Vector6i_head).def("tail",&Vector6i_tail)
		// operators
		.def("__neg__",&Vector6i__neg__) // -v
		.def("__add__",&Vector6i__add__Vector6i).def("__iadd__",&Vector6i__iadd__Vector6i) // +, +=
		.def("__sub__",&Vector6i__sub__Vector6i).def("__isub__",&Vector6i__isub__Vector6i) // -, -=
		.def("__mul__",&Vector6i__mul__int).def("__rmul__",&Vector6i__rmul__int) // f*v, v*f
		.def("__div__",&Vector6i__div__int).def("__idiv__",&Vector6i__idiv__int) // v/f, v/=f
		.def(bp::self != bp::self).def(bp::self == bp::self)
		// specials
		.def("__len__",&::Vector6i_len).staticmethod("__len__")
		.def("__setitem__",&::Vector6i_set_item).def("__getitem__",&::Vector6i_get_item)
		.def("__str__",&::Vector6i_str).def("__repr__",&::Vector6i_str)
	;

	bp::class_<Vector3r>("Vector3","3-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``, plus operations with ``Matrix3`` and ``Quaternion``.\n\nImplicit conversion from sequence (list,tuple, …) of 3 floats.",bp::init<>())
		.def(bp::init<Vector3r>((bp::arg("other"))))
		.def(bp::init<Real,Real,Real>((bp::arg("x"),bp::arg("y"),bp::arg("z"))))
		.def_pickle(Vector3r_pickle())
		// properties
		.add_static_property("Ones",&Vector3r_Ones).add_static_property("Zero",&Vector3r_Zero)
		.add_static_property("UnitX",&Vector3r_UnitX).add_static_property("UnitY",&Vector3r_UnitY).add_static_property("UnitZ",&Vector3r_UnitZ)
		// methods
		.def("dot",&Vector3r_dot).def("cross",&Vector3r_cross)
		.def("outer",&Vector3r_outer)
		.def("norm",&Vector3r::norm).def("squaredNorm",&Vector3r::squaredNorm).def("normalize",&Vector3r::normalize).def("normalized",&Vector3r::normalized)
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
	;	
	bp::class_<Vector3i>("Vector3i","3-dimensional integer vector.\n\nSupported operations (``i`` if an int, ``v`` is a Vector3i): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*i``, ``i*v``, ``v*=i``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence  (list,tuple, …) of 3 integers.",bp::init<>())
		.def(bp::init<Vector3i>((bp::arg("other"))))
		.def(bp::init<int,int,int>((bp::arg("x"),bp::arg("y"),bp::arg("z"))))
		.def_pickle(Vector3i_pickle())
		// properties
		.add_static_property("Ones",&Vector3i_Ones).add_static_property("Zero",&Vector3i_Zero)
		.add_static_property("UnitX",&Vector3i_UnitX).add_static_property("UnitY",&Vector3i_UnitY).add_static_property("UnitZ",&Vector3i_UnitZ)
		// methods
		.def("dot",&Vector3i_dot).def("cross",&Vector3i_cross)
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
	;	
	bp::class_<Vector2r>("Vector2","3-dimensional float vector.\n\nSupported operations (``f`` if a float/int, ``v`` is a Vector3): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*f``, ``f*v``, ``v*=f``, ``v/f``, ``v/=f``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list,tuple, …) of 2 floats.",bp::init<>())
		.def(bp::init<Vector2r>((bp::arg("other"))))
		.def(bp::init<Real,Real>((bp::arg("x"),bp::arg("y"))))
		.def_pickle(Vector2r_pickle())
		// properties
		.add_static_property("Ones",&Vector2r_Ones).add_static_property("Zero",&Vector2r_Zero)
		.add_static_property("UnitX",&Vector2r_UnitX).add_static_property("UnitY",&Vector2r_UnitY)
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
	;	
	bp::class_<Vector2i>("Vector2i","2-dimensional integer vector.\n\nSupported operations (``i`` if an int, ``v`` is a Vector2i): ``-v``, ``v+v``, ``v+=v``, ``v-v``, ``v-=v``, ``v*i``, ``i*v``, ``v*=i``, ``v==v``, ``v!=v``.\n\nImplicit conversion from sequence (list,tuple, …) of 2 integers.",bp::init<>())
		.def(bp::init<Vector2i>((bp::arg("other"))))
		.def(bp::init<int,int>((bp::arg("x"),bp::arg("y"))))
		.def_pickle(Vector2i_pickle())
		// properties
		.add_static_property("Ones",&Vector2i_Ones).add_static_property("Zero",&Vector2i_Zero)
		.add_static_property("UnitX",&Vector2i_UnitX).add_static_property("UnitY",&Vector2i_UnitY)
		// methods
		.def("dot",&Vector2i_dot)
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
	;	
	
};








