/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERICALDEMSIMULATOR_HPP
#define SPHERICALDEMSIMULATOR_HPP

#include "SphericalDEM.hpp"
#include "Contact.hpp"
#include "PersistentAloneSAPCollider.hpp"
#include<yade/core/StandAloneSimulator.hpp>
#include<yade/core/MetaBody.hpp>

class SphericalDEMSimulator : public StandAloneSimulator
{
	private :
		shared_ptr<MetaBody>	 rootBody;
		vector<SphericalDEM>	 spheres;
		ContactVecSet		 contacts;
		Vector3r		 gravity;

		Real			 alpha
					,beta
					,gamma
					,dt
					,newDt
					,forceDamping
					,momentumDamping;

		bool			 useTimeStepper
					,computedSomething;

		PersistentAloneSAPCollider	sap;

		void findRealCollision(const vector<SphericalDEM>& spheres, ContactVecSet& contacts);
		void computeResponse(vector<SphericalDEM>& spheres, ContactVecSet& contacts);
		void addDamping(vector<SphericalDEM>& spheres);
		void applyResponse(vector<SphericalDEM>& spheres);
		void timeIntegration(vector<SphericalDEM>& spheres);
		Real computeDt(const vector<SphericalDEM>& spheres, const ContactVecSet& contacts);

		void findTimeStepFromBody(const SphericalDEM& sphere);
		void findTimeStepFromInteraction(unsigned int id1, const Contact& contact, const vector<SphericalDEM>& spheres);

	public :
		SphericalDEMSimulator();
		virtual ~SphericalDEMSimulator();

		virtual void setTimeStep(Real dt);
		virtual void doOneIteration();
		virtual void run(int nbIterations);

		virtual void loadConfigurationFile(const string& fileName);

	REGISTER_CLASS_NAME(SphericalDEMSimulator);
	REGISTER_BASE_CLASS_NAME(StandAloneSimulator);
};

REGISTER_FACTORABLE(SphericalDEMSimulator);

#endif // SPHERICALDEMSIMULATOR_HPP

