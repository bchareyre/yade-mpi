#include "GLDrawTetrahedron.hpp"
#include "Tetrahedron.hpp"
#include "OpenGLWrapper.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawTetrahedron::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<BodyPhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
	glColor3v(gm->diffuseColor);
	
	Tetrahedron* tet = static_cast<Tetrahedron*>(gm.get());
	
//	glScalef(len,len/20,len/20); // it's a box, not a line. looks better :)

//	if (gm->wire || wire) // FIXME - draws only wire
//	{
		glBegin(GL_LINES);
		glDisable(GL_LIGHTING);
		
		// FIXME - stupid method to draw lines of tetrahedron
		glVertex3v(tet->v1);
		glVertex3v(tet->v2);
		
		glVertex3v(tet->v1);
		glVertex3v(tet->v3);
		
		glVertex3v(tet->v1);
		glVertex3v(tet->v4);
		
		glVertex3v(tet->v3);
		glVertex3v(tet->v4);
		
		glVertex3v(tet->v2);
		glVertex3v(tet->v4);
		
		glVertex3v(tet->v2);
		glVertex3v(tet->v3);
		
		glEnd();
//	}
//	else
//	{
//		glEnable(GL_LIGHTING);
//		glutSolidCube(1);
//	}
}

