#include "GLDrawLineSegment.hpp"
#include "LineSegment.hpp"
#include "OpenGLWrapper.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawLineSegment::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<BodyPhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
	glColor3v(gm->diffuseColor);
	
	Real len = (static_cast<LineSegment*>(gm.get()))->length;
	
	glScalef(len,len/20,len/20); // it's a box, not a line. looks better :)

	if (gm->wire || wire)
	{
		glDisable(GL_LIGHTING);
		glutWireCube(1);
	}
	else
	{
		glEnable(GL_LIGHTING);
		glutSolidCube(1);
	}
}

