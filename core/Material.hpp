// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<string>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<yade/core/State.hpp>
#include<yade/core/Dispatcher.hpp>


class Scene;
/*! Material properties associated with a body.

Historical note: this used to be part of the PhysicalParameters class.
The other data are now in the State class.
*/
class Material: public Serializable, public Indexable{
	public:
		virtual ~Material();

		//! Function to return empty default-initialized instance of State that 
		// is supposed to go along with this Material. Don't override unless you need
		// something else than basic State.
		virtual shared_ptr<State> newAssocState() const { return shared_ptr<State>(new State); }
		/*! Function that returns true if given State instance is what this material expects.
			
			Base Material class has no requirements, but the check would normally look like this:

				return (bool)dynamic_cast<State*> state;
		*/
		virtual bool stateTypeOk(State*) const { return true; }

		static const shared_ptr<Material> byId(int id, Scene* scene=NULL);
		static const shared_ptr<Material> byId(int id, shared_ptr<Scene> scene) {return byId(id,scene.get());}
		static const shared_ptr<Material> byLabel(const std::string& label, Scene* scene=NULL);
		static const shared_ptr<Material> byLabel(const std::string& label, shared_ptr<Scene> scene) {return byLabel(label,scene.get());}

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Material,Serializable,"Material properties of a :yref:`body<Body>`.",
		((int,id,((void)"not shared",-1),"Numeric id of this material; is non-negative only if this Material is shared (i.e. in O.materials), -1 otherwise. (necessary since yade::serialization doesn't track shared pointers)"))
		((string,label,,"Textual identifier for this material; can be used for shared materials lookup in :yref:`MaterialContainer`."))
		((Real,density,1000,"Density of the material [kg/m³]")),
		/* ctor */,
		/*py*/
		.def("newAssocState",&Material::newAssocState,"Return new State instance, which is associated to this Material. Some Material have special requirement on Body::state type.")
		YADE_PY_TOPINDEXABLE(Material)
	);
	REGISTER_INDEX_COUNTER(Material);
};
REGISTER_SERIALIZABLE(Material);
