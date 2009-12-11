// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<string>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<yade/core/State.hpp>


class Scene;
/*! Material properties associated with a body.

Historical note: this used to be part of the PhysicalParameters class.
The other data are now in the State class.
*/
class Material: public Serializable, public Indexable{
	public:
		Material(): id(-1), density(-1){ }
		~Material();
		//! global id of the material; if >= 0, the material is shared and can be found under this index in Scene::materials
		//! (necessary since yade::serialization doesn't track shared pointers)
		int id;
		//! textual name of material; if shared, can be looked up by that name
		std::string label;
		//! material density; used to compute mass from geometry of the body
		Real density;

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

	REGISTER_CLASS_AND_BASE(Material,Serializable);
	REGISTER_ATTRIBUTES(Serializable,(id)(label)(density));
	REGISTER_INDEX_COUNTER(Material);
};
REGISTER_SERIALIZABLE(Material);
