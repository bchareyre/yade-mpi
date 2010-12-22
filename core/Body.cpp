
#include<yade/core/Body.hpp>
#include<limits>
#include<yade/core/Scene.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/InteractionContainer.hpp>

//! This could be -1 if id_t is re-typedef'ed as `int'
const Body::id_t Body::ID_NONE=Body::id_t(-1);

const shared_ptr<Body>& Body::byId(Body::id_t _id, Scene* rb){return (*((rb?rb:Omega::instance().getScene().get())->bodies))[_id];}
const shared_ptr<Body>& Body::byId(Body::id_t _id, shared_ptr<Scene> rb){return (*(rb->bodies))[_id];}

// return list of interactions of this particle
python::list Body::py_intrs(){
	Scene* scene=Omega::instance().getScene().get(); // potentially unsafe with multiple simulations
	python::list ret;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		if(I->getId1()==id || I->getId2()==id) ret.append(I);
	}
	return ret;
}

