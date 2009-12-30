#include"GLUtils.hpp"

void GLUtils::Parallelepiped(const Vector3r& a, const Vector3r& b, const Vector3r& c){
   glBegin(GL_LINE_STRIP);
	 	glVertex3v(b); glVertex3v(Vector3r::ZERO); glVertex3v(a); glVertex3v(a+b); glVertex3v(a+b+c); glVertex3v(b+c); glVertex3v(b); glVertex3v(a+b);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex3v(b+c); glVertex3v(c); glVertex3v(a+c); glVertex3v(a);
	glEnd();
	glBegin(GL_LINES);
		glVertex3v(a+c); glVertex3v(a+b+c);
	glEnd();
}
