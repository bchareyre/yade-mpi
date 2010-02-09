// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Material.hpp>
#include<limits>
/*! Elastic material */
class ElastMat: public Material{
	public:
	virtual ~ElastMat();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(ElastMat,Material,"Purely elastic material.",
		((Real,young,1e9,"Young's modulus [Pa]"))
		((Real,poisson,.25,"Poisson's ratio [-]")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(ElastMat,Material);
};
REGISTER_SERIALIZABLE(ElastMat);

/*! Granular material */
class FrictMat: public ElastMat{
	public:
	virtual ~FrictMat();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(FrictMat,ElastMat,"Material with internal friction.",
		((Real,frictionAngle,.5,"Internal friction angle (in radians) [-]")),
		createIndex();
	);
	REGISTER_CLASS_INDEX(FrictMat,ElastMat);
};
REGISTER_SERIALIZABLE(FrictMat);
