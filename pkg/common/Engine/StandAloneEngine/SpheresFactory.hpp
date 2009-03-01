/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include <yade/pkg-common/PeriodicEngines.hpp>
#include <yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include <yade/core/Collider.hpp>
#include <yade/core/MetaBody.hpp>
#include <vector>
#include <string>

using namespace std;

/// @brief Produces spheres over the course of a simulation. 
class SpheresFactory : public PeriodicEngine {
public:

	SpheresFactory();
	virtual ~SpheresFactory();

	/// @brief Create one sphere per call.
	virtual void action(MetaBody*);

	/// @brief The geometry of the section on which spheres will be placed. 
	vector<body_id_t> factoryFacets; 

    /// @brief Factory section may be a surface or volume (convex). 
    /// By default it is a surface. To make its a volume set volumeSection=true
    bool volumeSection;

	/// @brief Max attemps to place sphere.
	/// If placing the sphere in certain random position would cause an overlap with any other physical body in the model, SpheresFactory will try to find another position. Default 20 attempts allow.
	int maxAttempts; 

	/// @brief Mean radius of spheres.
	Real radius; 

	/// @brief Half size of a radii distribution interval.
	/// New sphere will have random radius within the range radius±radiusRange.
	Real radiusRange;

	/// @brief Mean velocity of spheres.
	Vector3r velocity;

	/// @brief Half size of a velocities distribution interval.
	/// New sphere will have random velocity within the range velocity±velocityRange.
	Vector3r velocityRange;
	
	/// @brief Mean angularVelocity of spheres.
	Vector3r angularVelocity;

	/// @brief Half size of a angularVelocity distribution interval.
	/// New sphere will have random angularVelocity within the range angularVelocity±angularVelocityRange.
	Vector3r angularVelocityRange;

	/// @brief Young modulus.
	Real young;
	/// @brief Poisson ratio.
	Real poisson;
	/// @brief Density of material.
	Real density;
	/// @brief Friction angle (radians).
	Real frictionAngle;
	/// @brief Color.
	Vector3r color;

private:
	/// @brief Pointer to Collider.
	/// It is necessary in order to probe the bounding volume for new sphere.
	Collider* bI;
	
	/// @brief Pointer to InteractionGeometryMetaEngine.
	/// It is necessary in order to detect a real overlap with other bodies.
	InteractionGeometryMetaEngine* iGME;

	bool first_run;

    Vector3r generatePositionOnSurface();
    Vector3r generatePositionInVolume();

	void createSphere(shared_ptr<Body>& body, const Vector3r& position, Real r);

	typedef	boost::variate_generator<boost::minstd_rand,boost::uniform_int<> > RandomInt;
	shared_ptr<RandomInt> randomFacet;

	DECLARE_LOGGER;

	REGISTER_ATTRIBUTES(PeriodicEngine,
			(factoryFacets)
			(volumeSection)
			(maxAttempts)
			(radius)
			(radiusRange)
			(velocity)
			(velocityRange)
			(angularVelocity)
			(angularVelocityRange)
			(young)
			(poisson)
			(density)
			(frictionAngle)
			(color))
	REGISTER_CLASS_AND_BASE(SpheresFactory, StandAloneEngine);

};
REGISTER_SERIALIZABLE(SpheresFactory);


