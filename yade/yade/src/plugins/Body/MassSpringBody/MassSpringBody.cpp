#include "MassSpringBody.hpp"

// FIXME : redisign must not use a particular GM,CM or BV into Body
#include "Mesh2D.hpp"
#include "AABB.hpp"
#include "Math.hpp"
#include "Particle.hpp"

MassSpringBody::MassSpringBody () : NonConnexBody()
{
	createIndex();
}


MassSpringBody::~MassSpringBody()
{

}

void MassSpringBody::postProcessAttributes(bool deserializing)
{
	NonConnexBody::postProcessAttributes(deserializing);
}

void MassSpringBody::registerAttributes()
{
	NonConnexBody::registerAttributes();
}


void MassSpringBody::glDrawGeometricalModel()
{
	Mesh2D * mesh2d = static_cast<Mesh2D*>(gm.get());
	for(unsigned int i=0;i<mesh2d->vertices.size();i++)
	{
		Particle * particle = static_cast<Particle*>((*bodies)[i].get());
		mesh2d->vertices[i]=particle->se3.translation;
	}
	gm->glDraw();
}

