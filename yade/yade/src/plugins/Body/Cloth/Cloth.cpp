#include "Cloth.hpp"

Cloth::Cloth () : ConnexBody()
{
}


Cloth::~Cloth()
{

}

void Cloth::processAttributes()
{
	ConnexBody::processAttributes();
}

void Cloth::registerAttributes()
{
	ConnexBody::registerAttributes();
	REGISTER_ATTRIBUTE(stiffness);
	REGISTER_ATTRIBUTE(damping);
	REGISTER_ATTRIBUTE(properties);

}


void Cloth::updateBoundingVolume(Se3& se3)
{
	//bv->update(se3);
}

void Cloth::updateCollisionModel(Se3& )
{

}

void Cloth::moveToNextTimeStep(float )
{
	//cout << mass << " " << isDynamic << endl;
}
