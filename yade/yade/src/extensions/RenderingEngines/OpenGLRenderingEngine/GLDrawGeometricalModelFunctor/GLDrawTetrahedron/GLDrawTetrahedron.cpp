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
	
	if (gm->wire || wire) // FIXME - draws only wire
	{
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
	}
	else
	{
		glBegin(GL_TRIANGLE_STRIP);
			glEnable(GL_LIGHTING); 
			
			glNormal3v (tet->v4);
			glVertex3fv(tet->v4);
			
			glNormal3v (tet->v1);
			glVertex3fv(tet->v1);
			
			glNormal3v (tet->v2);
			glVertex3fv(tet->v2);
			
			glNormal3v (tet->v3);
			glVertex3fv(tet->v3);
			
			glNormal3v (tet->v4);
			glVertex3fv(tet->v4);
			
			glNormal3v (tet->v1);
			glVertex3fv(tet->v1);
		glEnd();
	}
}

