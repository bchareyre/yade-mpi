#ifdef WIN32
#include <windows.h> // The Win32 versions of the GL header files require that you windows.h before gl.h/glu.h/glut.h, so that you get the #define types like WINGDIAPI and such
#endif

#include <GL/gl.h>
#include <GL/glut.h>

#include "SimpleBody.hpp"
#include "Math.hpp"

#include <iostream>

using namespace std;

// FIXME - delete this file

// SimpleBody::SimpleBody() : Body()
// {
// //	containSubBodies = false;
// }
