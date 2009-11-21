// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

/*! Material properties associated with a body.

Historical note: this used to be part of the PhysicalParameters class.
The other data are now in the State class.
*/
class Material: public Serializable, public Indexable{
	public:
		//! global id of the material; if >= 0, the material is shared and can be found under this index in MetaBody::materials
		//! (necessary since yade::serialization doesn't track shared pointers)
		int id;
		//! textual name of material; if shared, can be looked up by that name
		string label;
		//! material density; used to compute mass from geometry of the body
		Real density;
	REGISTER_CLASS_AND_BASE(Material,Serializable);
	REGISTER_ATTRIBUTES(Serializable,(id)(label)(density));
	REGISTER_INDEX_COUNTER(Material);
};
REGISTER_SERIALIZABLE(Material);
