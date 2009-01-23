/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef ___SPHERESFACTORYENGINE___
#define ___SPHERESFACTORYENGINE___

#include <yade/core/StandAloneEngine.hpp>
#include <yade/core/BroadInteractor.hpp>
#include <yade/core/MetaBody.hpp>
#include <vector>
#include <string>

using namespace std;

class SpheresFactory : public StandAloneEngine {
public:

	SpheresFactory();
	virtual ~SpheresFactory();

	virtual void action(MetaBody*);

	vector<body_id_t> factoryFacets; 
	string labelBroadInteractor;

protected:
	BroadInteractor* bI;
	bool first_run;

	void createSphere(shared_ptr<Body>& body, const Vector3r& position, Real radius);

	DECLARE_LOGGER;

	REGISTER_ATTRIBUTES(StandAloneEngine,(factoryFacets)(labelBroadInteractor))
	REGISTER_CLASS_AND_BASE(SpheresFactory, StandAloneEngine);

};
REGISTER_SERIALIZABLE(SpheresFactory);

#endif //___SPHERESFACTORYENGINE___

