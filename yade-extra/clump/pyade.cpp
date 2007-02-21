// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#include<Python.h>

#include<boost/shared_ptr.hpp>
#include<yade/yade-core/Body.hpp>
#include<yade/yade-core/MetaBody.hpp>
#include<yade/yade-package-common/PhysicalParametersEngineUnit.hpp>
#include<yade/yade-package-common/RigidBodyParameters.hpp>
#include<yade/yade-lib-base/Logging.hpp>
#include<yade/yade-lib-base/yadeWm3Extra.hpp>
#include<yade/yade-core/Omega.hpp>

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
		case bp_id: return Py_BuildValue("i",(int)(B->getId()));
		case bp_clumpId: return Py_BuildValue("i",(int)(B->clumpId));
		case bp_x: return Py_BuildValue("(ddd)",pp->se3.position[0],pp->se3.position[1],pp->se3.position[2]);
		case bp_r: return Py_BuildValue("(dddd)",pp->se3.orientation[0],pp->se3.orientation[1],pp->se3.orientation[2],pp->se3.orientation[3]);
		case bp_v: return Py_BuildValue("(ddd)",pap->velocity[0],pap->velocity[1],pap->velocity[2]);
		case bp_w: return Py_BuildValue("(ddd)",rbp->angularVelocity[0],rbp->angularVelocity[1],rbp->angularVelocity[2]);
		case bp_m: return Py_BuildValue("d",pap->mass);
		case bp_I: return Py_BuildValue("(ddd)",rbp->inertia[0],rbp->inertia[1],rbp->inertia[2]);
		case bp_isClump: return Py_BuildValue("i",(int)B->isClump());
		case bp_isClumpMember: return Py_BuildValue("i",(int)B->isClumpMember());
		case bp_isStandalone: return Py_BuildValue("i",(int)B->isStandalone());
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
	/*! simulation filename [float] */ sm_file};

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
		case sm_t: return Py_BuildValue("d",Omega::instance().getSimulationTime());
		case sm_dt: return Py_BuildValue("d",Omega::instance().getTimeStep());
		case sm_i: return Py_BuildValue("l",Omega::instance().getCurrentIteration());
		case sm_file: return Py_BuildValue("s",Omega::instance().getSimulationFileName().c_str());
	}
	return Py_BuildValue("");
}

static PyObject*
simulPropDict(PyObject *self, PyObject *args){
	#define SM(name) #name,sm_##name
		return Py_BuildValue("{sisisisi}",SM(t),SM(dt),SM(i),SM(file));
	#undef SM
}

/************************ module setup *************************/

static PyMethodDef _pyadeMethods[] = {
    {"bodyProp", bodyProp, METH_VARARGS,"Return body's (# first arg) property (second argument)."},
    {"bodyPropDict", bodyPropDict, METH_VARARGS,"Dictionary of available body properties, maps symbolic names to constants."},
    {"simulProp", simulProp, METH_VARARGS,"Return some simulation property (in argument)."},
    {"simulPropDict", simulPropDict, METH_VARARGS,"Dictionary of available simulation properties, maps symbolic names to constants."},
    {NULL, NULL, 0, NULL} /* sentinel */
};

PyMODINIT_FUNC init_pyade(void){ (void) Py_InitModule("_pyade", _pyadeMethods);}
