/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko				 				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include <yade/pkg-common/PeriodicEngines.hpp>
#include <yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include <yade/core/Collider.hpp>
#include <yade/core/Scene.hpp>
#include <vector>
#include <string>

using namespace std;

/// @brief Produces spheres over the course of a simulation. 
class ResetRandomPosition : public PeriodicEngine {
public:

	ResetRandomPosition();
	virtual ~ResetRandomPosition();

	/// @brief Create one sphere per call.
	virtual void action(Scene*);

	/// @brief The geometry of the section on which spheres will be placed. 
	vector<body_id_t> factoryFacets; 

    /// @brief Factory section may be a surface or volume (convex). 
    /// By default it is a surface. To make its a volume set volumeSection=true
    bool volumeSection;

	/// @brief Max attemps to place sphere.
	/// If placing the sphere in certain random position would cause an overlap with any other physical body in the model, SpheresFactory will try to find another position. Default 20 attempts allow.
	int maxAttempts;

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

	Vector3r point;
	Vector3r normal;

	std::vector< int >   subscribedBodies;

private:
	/// @brief Pointer to Collider.
	/// It is necessary in order to probe the bounding volume for new sphere.
	Collider* bI;
	
	/// @brief Pointer to InteractionGeometryDispatcher.
	/// It is necessary in order to detect a real overlap with other bodies.
	InteractionGeometryDispatcher* iGME;

	std::vector<shared_ptr<Body> > shiftedBodies;

	bool first_run;

	//bool generateNewPosition(const shared_ptr<Body>& b, Vector3r& new_position);
    Vector3r generatePositionOnSurface();
    Vector3r generatePositionInVolume();

	typedef	boost::variate_generator<boost::minstd_rand,boost::uniform_int<> > RandomInt;
	shared_ptr<RandomInt> randomFacet;

	static boost::variate_generator<boost::mt19937,boost::uniform_real<> > 
		randomUnit;
	static boost::variate_generator<boost::mt19937,boost::uniform_real<> >
		randomSymmetricUnit;

	DECLARE_LOGGER;

	REGISTER_ATTRIBUTES(PeriodicEngine,
			(factoryFacets)
			(subscribedBodies)
			(point)
			(normal)
			(volumeSection)
			(maxAttempts)
			(velocity)
			(velocityRange)
			(angularVelocity)
			(angularVelocityRange))
	REGISTER_CLASS_AND_BASE(ResetRandomPosition, GlobalEngine);
};
REGISTER_SERIALIZABLE(ResetRandomPosition);


