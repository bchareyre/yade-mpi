#include<stdexcept>
#include<yade/core/Material.hpp>
#include<boost/foreach.hpp>
#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

Material::~Material(){}

const shared_ptr<Material> Material::byId(int id, MetaBody* w_){
	MetaBody* w=w_?w_:Omega::instance().getRootBody().get();
	assert(id>=0 && (size_t)id<w->materials.size());
	assert(w->materials[id]->id == id);
	return w->materials[id];
}

const shared_ptr<Material> Material::byLabel(const std::string& label, MetaBody* w_){
	MetaBody* w=w_?w_:Omega::instance().getRootBody().get();
	FOREACH(const shared_ptr<Material>& m, w->materials){
		if(m->label == label) return m;
	}
	throw std::runtime_error(("No material labeled `"+label+"'.").c_str());
}


