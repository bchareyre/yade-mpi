// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Material.hpp>
#include<limits>
/*! Elastic material */
class ElastMat: public Material{
	public:
	Real young;
	Real poisson;
	ElastMat(): young(1e9),poisson(.25) { createIndex(); }
	virtual ~ElastMat();
	//REGISTER_ATTRIBUTES(Material,(young)(poisson));
	//REGISTER_CLASS_AND_BASE(ElastMat,Material);
	YADE_CLASS_BASE_ATTRS(ElastMat,Material,(young)(poisson));
	REGISTER_CLASS_INDEX(ElastMat,Material);
};
REGISTER_SERIALIZABLE(ElastMat);

/*! Granular material */
class FrictMat: public ElastMat{
	public:
	Real frictionAngle;
	FrictMat(): frictionAngle(.5){ createIndex(); }
	virtual ~FrictMat();
	//REGISTER_ATTRIBUTES(ElastMat,(frictionAngle));
	//REGISTER_CLASS_AND_BASE(FrictMat,ElastMat);
	YADE_CLASS_BASE_ATTRS(FrictMat,ElastMat,(frictionAngle));
	REGISTER_CLASS_INDEX(FrictMat,ElastMat);
};
REGISTER_SERIALIZABLE(FrictMat);
