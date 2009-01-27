// © 2009 Janek Kozicki <cosurgi@mail.berlios.de>

#include"Ef1_IstSnowLayersContact_glDraw.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-snow/IstSnowLayersContact.hpp>

#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/lib-opengl/GLUtils.hpp>

YADE_PLUGIN("Ef1_IstSnowLayersContact_glDraw");

void Ef1_IstSnowLayersContact_glDraw::go(
		const shared_ptr<InteractionGeometry>& ig,
		const shared_ptr<Interaction>& ip,
		const shared_ptr<Body>& b1,
		const shared_ptr<Body>& b2,
		bool wireFrame)
{
	IstSnowLayersContact* sc = static_cast<IstSnowLayersContact*>(ig.get());

	const Se3r& se31=b1->physicalParameters->dispSe3,se32=b2->physicalParameters->dispSe3;
	const Vector3r& pos1=se31.position,pos2=se32.position;

	if(wireFrame)
	{
		glPushMatrix();
			glTranslatev(sc->contactPoint);
			glBegin(GL_LINES);
				glColor3(0.0,0.0,1.0);
				glVertex3(0.0,0.0,0.0);
				glVertex3v(-1.0*(sc->normal*sc->radius1));
				glVertex3(0.0,0.0,0.0);
				glVertex3v( 1.0*(sc->normal*sc->radius2));
			glEnd();
		glPopMatrix();
	}
	else
	{
	}
}

