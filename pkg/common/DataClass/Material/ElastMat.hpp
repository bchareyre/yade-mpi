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
	YADE_CLASS_BASE_DOC_ATTRS(ElastMat,Material,"Purely elastic material.",
		((young,"Young's modulus [Pa]"))
		((poisson,"Poisson's ratio [-]"))
	);
	REGISTER_CLASS_INDEX(ElastMat,Material);
};
REGISTER_SERIALIZABLE(ElastMat);

/*! Granular material */
class FrictMat: public ElastMat{
	public:
	Real frictionAngle;
	FrictMat(): frictionAngle(.5){ createIndex(); }
	virtual ~FrictMat();
	YADE_CLASS_BASE_DOC_ATTRS(FrictMat,ElastMat,"Material with internal friction.",
		((frictionAngle,"Internal friction angle (in radians) [-]"))
	);
	REGISTER_CLASS_INDEX(FrictMat,ElastMat);
};
REGISTER_SERIALIZABLE(FrictMat);
