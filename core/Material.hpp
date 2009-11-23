// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<string>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

class MetaBody;
/*! Material properties associated with a body.

Historical note: this used to be part of the PhysicalParameters class.
The other data are now in the State class.
*/
class Material: public Serializable, public Indexable{
	public:
		Material(){ createIndex(); }
		~Material();
		//! global id of the material; if >= 0, the material is shared and can be found under this index in MetaBody::materials
		//! (necessary since yade::serialization doesn't track shared pointers)
		int id;
		//! textual name of material; if shared, can be looked up by that name
		std::string label;
		//! material density; used to compute mass from geometry of the body
		Real density;

		static const shared_ptr<Material> byId(int id, MetaBody* world=NULL);
		static const shared_ptr<Material> byId(int id, shared_ptr<MetaBody> world) {return byId(id,world.get());}
		static const shared_ptr<Material> byLabel(const std::string& label, MetaBody* world=NULL);
		static const shared_ptr<Material> byLabel(const std::string& label, shared_ptr<MetaBody> world) {return byLabel(label,world.get());}

	REGISTER_CLASS_AND_BASE(Material,Serializable);
	REGISTER_ATTRIBUTES(Serializable,(id)(label)(density));
	REGISTER_INDEX_COUNTER(Material);
};
REGISTER_SERIALIZABLE(Material);
