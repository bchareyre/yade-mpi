#ifdef WIN32
#include <windows.h> // The Win32 versions of the GL header files require that you windows.h before gl.h/glu.h/glut.h, so that you get the #define types like WINGDIAPI and such
#endif

#include <GL/gl.h>
#include <GL/glut.h>

#include "ConnexBody.hpp"
#include "Math.hpp"

#include <iostream>

using namespace std;

ConnexBody::ConnexBody() : Body()
{
}

ConnexBody::~ConnexBody() 
{

}


void ConnexBody::glDraw()
{	
	glPushMatrix();
	float angle;
	Vector3r axis;	
	se3.rotation.toAxisAngle(axis,angle);	
	glTranslatef(se3.translation[0],se3.translation[1],se3.translation[2]);
	glRotatef(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);	
	gm->glDraw();		
	glPopMatrix();
	
//	cout << se3.translation[0] << " " << se3.translation[1] << " " <<  se3.translation[2] << endl;
//	cout << angle << endl;

	//FIXME : hardcoded drawing of boundingvolume, sensor will fix that
	//glPushMatrix();
	//bv->glDraw();
	//glPopMatrix();
}


void ConnexBody::postProcessAttributes(bool deserializing)
{
	if(deserializing)
		invMass = 1.0/mass;
}

void ConnexBody::registerAttributes()
{
	Body::registerAttributes();
	REGISTER_ATTRIBUTE(mass);
	
	// FIXME : where to put gm,cm,bv
	//REGISTER_ATTRIBUTE(cm);
	//REGISTER_ATTRIBUTE(bv);
	//REGISTER_ATTRIBUTE(gm);
}

void ConnexBody::moveToNextTimeStep()
{
	vector<shared_ptr<Actor> >::iterator ai    = actors.begin();
	vector<shared_ptr<Actor> >::iterator aiEnd =  actors.end();
	for(;ai!=aiEnd;++ai)
		if ((*ai)->isActivated())
			(*ai)->action(this);
}
