// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#include<Python.h>
#include<list>

#include<boost/shared_ptr.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/PhysicalParametersEngineUnit.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/lib-base/Logging.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/core/Omega.hpp>


/******************* basic type conversions *********/
static PyObject* __main__; // assigned in initializer
PyObject* toPython(int arg){return Py_BuildValue("i",arg);}
PyObject* toPython(Body::id_t arg){return toPython((int)arg);}
PyObject* toPython(long arg){return Py_BuildValue("l",arg);}
PyObject* toPython(bool arg){return toPython((int)arg);}
PyObject* toPython(double arg){return Py_BuildValue("d",arg);}
PyObject* toPython(std::string arg){return Py_BuildValue("s",arg.c_str());}
PyObject* toPython(Wm3::Vector3r arg){	return PyObject_Call(PyObject_GetAttrString(__main__,"Vector"),Py_BuildValue("(ddd)",arg[0],arg[1],arg[2]),NULL);}
PyObject* toPython(Wm3::Quaternionr arg){ return PyObject_Call(PyObject_GetAttrString(__main__,"Quaternion"),Py_BuildValue("(dddd)",arg[0],arg[1],arg[2],arg[3]),NULL);}
PyObject* toPython(list<Body::id_t> arg){	PyObject* ret=PyList_New(0); for(list<Body::id_t>::iterator I=arg.begin(); I!=arg.end(); I++) PyList_Append(ret,toPython(*I)); return ret;}
 
/******************* body properties ****************/

/*! Body parameters that can be accessed through this code.
 *
 * @note Other attributes may still be defined in the python interface - see pyade.py for those.
 * @todo Add a (acceleration), aa (angular acceleration).
 * */
enum bodyPropCodes {/*! see Body::id [int] */ bp_id,
	/*! Body::clumpId [int] */ bp_clumpId,
	/*! centroid position [3-float-tuple] */ bp_x,
	/*! body's orientation [quaternion as 4-float-tuple] */ bp_r,
	/*! velocity [3-float-tuple] */ bp_v,
	/*! angular velocity [3-float-tuple] */ bp_w,
	/*! mass [float] */ bp_m,
	/*! inertia [3-float-tuple] */ bp_I,
	/*! Body::isClump [bool as int] */ bp_isClump,
	/*! Body::isClumpMember [bool as int] */ bp_isClumpMember,
	/*! Body::isStandalone [bool as int] */ bp_isStandalone};

/* Retrieves this body's property.
 *
 * @return python object of which type depends on the property requested; bodyPropCodes has details.
 */
static PyObject*
bodyProp(PyObject *self, PyObject *args){
	long id, prop;
	if(!PyArg_ParseTuple(args, "ll", &id, &prop)) return NULL;
	shared_ptr<Body> B=Body::byId((Body::id_t)id);
	shared_ptr<PhysicalParameters> pp=B->physicalParameters;
	// FIXME: we will crash if user requests rbp for a body that doesn't have it!
	// later, do assert(rbp), raise exception (in a pythonic way - need to see docs on that) and return None
	shared_ptr<ParticleParameters> pap=dynamic_pointer_cast<ParticleParameters>(pp);
	shared_ptr<RigidBodyParameters> rbp=dynamic_pointer_cast<RigidBodyParameters>(pp);
	switch(prop){
		/* warning: no implicit conversions in additional arguments (not even int->double), attention!! */
		case bp_id: return toPython(B->getId());
		case bp_clumpId: return toPython(B->clumpId);
		case bp_x: return toPython(pp->se3.position);
		case bp_r: return toPython(pp->se3.orientation);
		case bp_v: return toPython(pap->velocity);
		case bp_w: return toPython(rbp->angularVelocity);
		case bp_m: return toPython(pap->mass);
		case bp_I: return toPython(rbp->inertia);
		case bp_isClump: return toPython(B->isClump());
		case bp_isClumpMember: return toPython(B->isClumpMember());
		case bp_isStandalone: return toPython(B->isStandalone());
	}
	return Py_BuildValue("");
}

// if we knew way how to make module's constant (dictionary), this would not be function returning constant dictionary.
static PyObject*
bodyPropDict(PyObject *self, PyObject *args){
	#define BP(name) #name,bp_##name
		return Py_BuildValue("{sisisisisisisisisisisi}",BP(id),BP(clumpId),BP(x),BP(r),BP(v),BP(w),BP(m),BP(I),BP(isClump),BP(isClumpMember),BP(isStandalone));
	#undef BP
}

/************************ simulation properties *****************************/
/*! Simulation parameters that can be accessed through this code.
 *
 * @note Other attributes may still be defined in the python interface - see pyade.py for those.
 */
enum simulPropCodes {/*! simulation time [double] */ sm_t,
	/*! iteration number [float] */ sm_i,
	/*! timestep [float] */ sm_dt,
	/*! simulation filename [float] */ sm_file,
	/*! number of bodies in rootBody */ sm_nBodies,
	/*! list of selected bodies (newest first) */ sm_sel };

/* Retrieves simulation's property.
 *
 * @todo Should be able to retrieve id's of selected bodies, like S.sel[i]. This would require changes in the UI, though.
 * Multiple selection (stack) would make it possible to display e.g. interaction forces between two specific bodies etc.
 * Interactions should however become a new class (besided Bodies and Simul) for that.
 * @return Python object of which type depends on the property requested; simulPropCodes has details.
 */
static PyObject*
simulProp(PyObject *self, PyObject *args){
	long prop;
	if(!PyArg_ParseTuple(args, "l", &prop)) return NULL;
	//shared_ptr<MetaBody> rootBody=Omega::instance()->getRootBody();
	switch(prop){
		case sm_t: return toPython(Omega::instance().getSimulationTime());
		case sm_dt: return toPython(Omega::instance().getTimeStep());
		case sm_i: return toPython(Omega::instance().getCurrentIteration());
		case sm_file: return toPython(Omega::instance().getSimulationFileName());
		case sm_nBodies: return toPython(Omega::instance().getRootBody()->bodies->size());
		case sm_sel: return toPython(Omega::instance().selectedBodies);
	}
	return Py_BuildValue("");
}

static PyObject*
simulPropDict(PyObject *self, PyObject *args){
	#define SM(name) #name,sm_##name
		return Py_BuildValue("{sisisisisisi}",SM(t),SM(dt),SM(i),SM(file),SM(nBodies),SM(sel));
	#undef SM
}

#if 0
/************************ interaction properties *****************************/
/*! Interaction parameters that can be accessed through this code.
 *
 * @note Other attributes may still be defined in the python interface - see pyade.py for those.
 */
enum interPropCodes { /*! id of the first body [long] */ it_id1,
	/*! id of the second body [long] */ it_id2,
	/*! contact point [Vector] */ it_cp,
	/*! normal [Vector] */ it_normal,
	/*! radius1 [float] */ it_r1,
	/*! radius2 [float] */ it_r2,
	/*! peneration depth [float] */ it_penetration }

/* Retrieves interaction property
 *
 * @return Python object of which type depends on the property requested; interPropCodes has details.
 */
static PyObject*
interProp(PyObject *self, PyObject *args){
	long id1,id2,prop; int isTransient;
	if(!PyArg_ParseTuple(args, "llli", &id1, &id2, &prop, &isTransient)) return NULL;
	shared_ptr<InteractionContainer> inters=isTransient?Omega::instance()->getRootBody()->transientInteractions:Omega::instance()->getRootBody()->persistentInteractions;
	switch(prop){
		case it_id1: return toPython(id1); // hmmm, these two are not needed?!
		case it_id2: return toPython(id2);
		case it_cp:{
			const shared_ptr<SpheresContactGeometry>& scg=dynamic_cast<SpheresContactGeometry>(inters->find(id1,id2));
			return return toPython(scg->contactPoint);
		}
		case sm_file: return toPython(Omega::instance().getSimulationFileName());
		case sm_nBodies: return toPython(Omega::instance().getRootBody()->bodies->size());
		case sm_sel: return toPython(Omega::instance().selectedBodies);
	}
	return Py_BuildValue("");
}

static PyObject*
interPropDict(PyObject *self, PyObject *args){
	#define IT(name) #name,it_##name
		return Py_BuildValue("{sisisisisisisi}",IT(id1),IT(id2),IT(cp),IT(normal),IT(r1),IT(r2),IT(penetration));
	#undef IT
}
#endif
/************************ module setup *************************/

static PyMethodDef _pyadeMethods[] = {
    {"bodyProp", bodyProp, METH_VARARGS,"Return body's (# first arg) property (second argument)."},
    {"bodyPropDict", bodyPropDict, METH_VARARGS,"Dictionary of available body properties, maps symbolic names to constants."},
    {"simulProp", simulProp, METH_VARARGS,"Return some simulation property (in argument)."},
    {"simulPropDict", simulPropDict, METH_VARARGS,"Dictionary of available simulation properties, maps symbolic names to constants."},
    {NULL, NULL, 0, NULL} /* sentinel */
};


PyMODINIT_FUNC init_pyade(void){
	(void) Py_InitModule("_pyade", _pyadeMethods);
	__main__=PyImport_Import(PyString_FromString("__main__"));
	assert(__main__);
}
