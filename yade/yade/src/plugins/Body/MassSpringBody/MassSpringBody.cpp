#include "MassSpringBody.hpp"

#include "Mesh2D.hpp"
#include "AABB.hpp"
#include "Math.hpp"
#include "ParticleParameters.hpp"

MassSpringBody::MassSpringBody () : ComplexBody()
{
	//createIndex();
}


MassSpringBody::~MassSpringBody()
{

}

void MassSpringBody::postProcessAttributes(bool deserializing)
{
	ComplexBody::postProcessAttributes(deserializing);
}

void MassSpringBody::registerAttributes()
{
	ComplexBody::registerAttributes();
}


// void MassSpringBody::glDrawGeometricalModel()
// {
// 	Mesh2D * mesh2d = static_cast<Mesh2D*>(gm.get());
// 	unsigned int i=0;
// 	for(bodies->gotoFirst();bodies->notAtEnd();bodies->gotoNext(),i++)
// 		mesh2d->vertices[i]=bodies->getCurrent()->physicalParameters->se3.translation;
// 		
// //	gm->glDraw();
// }

