#include"GLUtils.hpp"

void GLUtils::Parallelepiped(const Vector3r& a, const Vector3r& b, const Vector3r& c){
   glBegin(GL_LINE_STRIP);
	 	glVertex3v(b); glVertex3v(Vector3r(Vector3r::Zero())); glVertex3v(a); glVertex3v(Vector3r(a+b)); glVertex3v(Vector3r(a+b+c)); glVertex3v(Vector3r(b+c)); glVertex3v(b); glVertex3v(Vector3r(a+b));
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex3v(Vector3r(b+c)); glVertex3v(c); glVertex3v(Vector3r(a+c)); glVertex3v(a);
	glEnd();
	glBegin(GL_LINES);
		glVertex3v(Vector3r(a+c)); glVertex3v(Vector3r(a+b+c));
	glEnd();
}
