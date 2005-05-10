#include "GLDrawLineSegment.hpp"
#include <yade-common/LineSegment.hpp>
#include <yade-lib-opengl/OpenGLWrapper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawLineSegment::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
	glColor3v(gm->diffuseColor);
	
	Real len = (static_cast<LineSegment*>(gm.get()))->length;
	
	glScalef(len,len/20,len/20); // it's a box, not a line. looks better :)

	if (gm->wire || wire)
	{
		glBegin(GL_LINES);
		glDisable(GL_LIGHTING);
		glVertex3(-0.5,0.0,0.0);
		glVertex3( 0.5,0.0,0.0);
		glEnd();
	}
	else
	{
		glEnable(GL_LIGHTING);
		glutSolidCube(1);
	}
}

