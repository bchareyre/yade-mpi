// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

/*! Material properties associated with a body.

Historical note: this used to be part of the PhysicalParameters class.
The other data are now in the State class.
*/
class Material: public Serializable, public Indexable{
	public:
		//! textual name of material; if shared, can be looked up by that name
		string label;
	REGISTER_CLASS_AND_BASE(Material,Serializable);
	REGISTER_ATTRIBUTES(Serializable,(label));
	REGISTER_INDEX_COUNTER(Material);
};
REGISTER_SERIALIZABLE(Material);
