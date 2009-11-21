// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Material.hpp>
/*! Elastic material */
class ElasticMat: public Material{
	public:
	Real young;
	ElasticMat(){ createIndex(); }
	virtual ~ElasticMat();
	REGISTER_ATTRIBUTES(Material,(young));
	REGISTER_CLASS_AND_BASE(ElasticMat,Material);
	REGISTER_CLASS_INDEX(ElasticMat,Material);
};
REGISTER_SERIALIZABLE(ElasticMat);

/*! Granular material */
class GranularMat: public ElasticMat{
	public:
	Real frictionAngle;
	GranularMat(){ createIndex(); }
	virtual ~GranularMat();
	REGISTER_ATTRIBUTES(ElasticMat,(frictionAngle));
	REGISTER_CLASS_AND_BASE(GranularMat,ElasticMat);
	REGISTER_CLASS_INDEX(GranularMat,ElasticMat);
};
REGISTER_SERIALIZABLE(GranularMat);
