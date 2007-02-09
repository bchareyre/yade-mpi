// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#include"Clump.hpp"

#include<algorithm>

char* yadePluginClasses[]={
	"Clump",
	"ClumpLeapFrogPositionAndOrientationIntegrator",
	NULL /*sentinel*/
};

CREATE_LOGGER(Clump);
CREATE_LOGGER(ClumpLeapFrogPositionAndOrientationIntegrator);

/*! @pre Body must be dynamic.
 * @pre Body must not be part or this clump already.
 * @todo se3 calculation is not tested yet
 */

void Clump::add(Body::id_t subId){
	Body* subBody=Body::byId(subId);

	// preconditions
	assert(subBody->isDynamic);
	assert(find(SubBodies.begin(),subBodies.end(),subId)==subBodies.end());

	// begin actual setup
	subBody->clumpId=id;
	subBody->isDynamic=false;
	subBodies.push_back(subId);
	//FIXME: should be this way or the other way around? Will have to try!
	//FIXME: should assert validity of this->physicalParameters?! How to do that with shared_ptr?
	Se3r rel(subBody->physicalParameters->se3,physicalParameters->se3);
	subSe3s.push_back(rel);
	LOG_DEBUG("Added body #"<<subId<<" to clump #"<<id);
}

/*! @pre Body with given id must be in the clump.
 */
void Clump::del(Body::id_t subId){
	// find position of the body in vector
	std::vector<Body::id_t>::iterator subBodiesIter=find(subBodies.begin(),subBodies.end(),subId);
	// removing body that is not part of the clump is error
	assert(subBodiesIter!=subBodies.end());
	// se3 is at the same position
	size_t idx=distance(subBodies.being(),subBodiesIter);
	std::vector<Se3>::iterator subSe3sIter=subSe3s.begin()+idx;
	// erase these elements;
	subBodies.erase(subBodiesIter);
	subSe3s.erase(subSe3sIter);
	// restore body's internal parameters;
	Body* subBody=Body::byId(subId);
	subBody->clumpId=Body::ID_NONE;
	subBody->isDynamic=true;
	LOG_DEBUG("Removed body #"<<subId<<" from clump #"<<id);
}

/*! Clump's se3 will be updated (origin at centroid and axes coincident with principal inertia axes) and subSe3 modified in such a way that subBodies positions in world coordinates will not change.

	The algorithm is as follows:
	-# Clump::subBodiesSe3s and Clump::physicalParameters::se3 are invalid from this point
	-# M=0; S=vector3r(0,0,0); I=zero tensor; (ALL calculations are in world coordinates!)
	-# loop over Clump::subBodies (position x_i, mass m_i, inertia at subBody's centroid I_i)
		- M+=m_i
		- S+=m_i*x_i (local static moments are zero (centroid)
		- get inertia tensor of subBody in world coordinates, by rotating the principal (local) tensor against subBody->se3->orientation; then translate it to world origin (parallel axes theorem), then I+=I_i_world
	-# clumpPos=S/M
	-# translate aggregate's inertia tensor; parallel axes on I (R=clumpPos): I^c_jk=I'_jk-M*(delta_jk R.R - R_j*R_k) [http://en.wikipedia.org/wiki/Moments_of_inertia#Parallel_axes_theorem]
	-# eigen decomposition of I, get principal inertia and rotation matrix of the clump
	-# se3->orientation=quaternion(rotation_matrix); se3->position=clumpPos
	-#	update subBodiesSe3s

	@note User is responsible for calling this function when appropriate (after adding/removing bodies and before any subsequent simulation). This function can be rather slow by virtue of numerical integration.
	@todo Needs to be implemented and tested.
	@param intersecting if true, evaluate mass and inertia numerically; otherwise, use analytical methods (parallel axes theorem) which disregard any intersections, but are much faster. */
void Clump::update(bool intersecting){
	LOG_DEBUG("Updating clump #"<<id<<" parameters");
	assert(subBodies.size()>0);
	// maybe handle the case of only one clump subBody specially?!
}



