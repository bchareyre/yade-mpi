#include "GLDrawLineSegment.hpp"
#include "LineSegment.hpp"
#include "OpenGLWrapper.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawLineSegment::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<BodyPhysicalParameters>&)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
	glColor3v(gm->diffuseColor);
	
	Real len = (static_cast<LineSegment*>(gm.get()))->length;
	
	glScalef(len/20,len,len/20); // FIXME - how to draw a stupid line ?

	if (gm->wire)
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

