#include "Sphere.hpp"
#include "Rand.hpp"
int Sphere::glSphereList=-1;
int Sphere::glWiredSphereList=-1;

vector<Vector3> Sphere::vertices;
vector<Vector3> Sphere::faces;


Sphere::Sphere (float r) : CollisionGeometry()
{
	radius = r;
	Sphere::Sphere();
}

Sphere::Sphere () : CollisionGeometry()
{

	float X = 0.525731112119133606;
	float Z = 0.850650808352039932;
	vertices.push_back(Vector3(-X,0,Z));
	vertices.push_back(Vector3(X,0,Z));
	vertices.push_back(Vector3(-X,0,-Z));
	vertices.push_back(Vector3(X,0,-Z));
	vertices.push_back(Vector3(0,Z,X));
	vertices.push_back(Vector3(0,Z,-X));
	vertices.push_back(Vector3(0,-Z,X));
	vertices.push_back(Vector3(0,-Z,-X));
	vertices.push_back(Vector3(Z,X,0));
	vertices.push_back(Vector3(-Z,X,0));
	vertices.push_back(Vector3(Z,-X,0));
	vertices.push_back(Vector3(-Z,-X,0));

	faces.push_back(Vector3(0,4,1));
	faces.push_back(Vector3(0,9,4));
	faces.push_back(Vector3(9,5,4));
	faces.push_back(Vector3(4,5,8));
	faces.push_back(Vector3(4,8,1));
	faces.push_back(Vector3(8,10,1));
	faces.push_back(Vector3(8,3,10));
	faces.push_back(Vector3(5,3,8));
	faces.push_back(Vector3(5,2,3));
	faces.push_back(Vector3(2,7,3));
	faces.push_back(Vector3(7,10,3));
	faces.push_back(Vector3(7,6,10));
	faces.push_back(Vector3(7,11,6));
	faces.push_back(Vector3(11,0,6));
	faces.push_back(Vector3(0,1,6));
	faces.push_back(Vector3(6,1,10));
	faces.push_back(Vector3(9,0,11));
	faces.push_back(Vector3(9,11,2));
	faces.push_back(Vector3(9,2,5));
	faces.push_back(Vector3(7,2,11));
}

Sphere::~Sphere ()
{

}

void Sphere::processAttributes()
{
	CollisionGeometry::processAttributes();
}

void Sphere::registerAttributes()
{
	CollisionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
}

bool Sphere::collideWith(CollisionGeometry* )
{
	return true;
}

void Sphere::subdivideTriangle(Vector3& v1,Vector3& v2,Vector3& v3, int depth)
{
	Vector3 v12,v23,v31;

	if (depth==0)
	{
		float pi3 = 2*Constants::PI/3;
		Vector3 v = (v1+v2+v3)/3.0;
		float angle = atan(v[2]/v[0])/v.length();

		GLfloat matAmbient[4];

		if (angle>-Constants::PI/6.0 && angle<=Constants::PI/6.0)
		{
			matAmbient[0] = 0.2;
			matAmbient[1] = 0.2;
			matAmbient[2] = 0.2;
			matAmbient[3] = 0.0;
		}
		else
		{
			matAmbient[0] = 0.0;
			matAmbient[1] = 0.0;
			matAmbient[2] = 0.0;
			matAmbient[3] = 0.0;
		}

		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,matAmbient);

		glBegin(GL_TRIANGLES);
			glNormal3fv(v3);
			glVertex3fv(v3);
			glNormal3fv(v2);
			glVertex3fv(v2);
			glNormal3fv(v1);
			glVertex3fv(v1);
		glEnd();

		return;
	}
	v12 = v1+v2;
	v23 = v2+v3;
	v31 = v3+v1;
	v12.normalize();
	v23.normalize();
	v31.normalize();
	subdivideTriangle(v1,v12,v31,depth-1);
	subdivideTriangle(v2,v23,v12,depth-1);
	subdivideTriangle(v3,v31,v23,depth-1);
	subdivideTriangle(v12,v23,v31,depth-1);
}

void Sphere::drawSphere(int depth)
{
	/*glShadeModel(GL_SMOOTH);
	GLfloat matSpecular[] = { 1.0,1.0,1.0,1.0};
	GLfloat matShininess[] = { 50.0};
	glMaterialfv(GL_FRONT,GL_SPECULAR,matSpecular);
	glMaterialfv(GL_FRONT,GL_SHININESS,matShininess);*/

	number=0;
	for(int i=0;i<20;i++)
	{
		//glColor3f(Rand::unitRandom(),Rand::unitRandom(),Rand::unitRandom());
		subdivideTriangle(vertices[(unsigned int)faces[i][0]],vertices[(unsigned int)faces[i][1]],vertices[(unsigned int)faces[i][2]],depth);
	}
}

void Sphere::glDraw()
{

	if (glSphereList==-1)
	{
		glWiredSphereList = glGenLists(1);
		glNewList(glWiredSphereList,GL_COMPILE);
			glDisable(GL_LIGHTING);
			drawSphere(1);
		glEndList();
		glSphereList = glGenLists(1);
		glNewList(glSphereList,GL_COMPILE);
			glEnable(GL_LIGHTING);
			drawSphere(1);
		glEndList();
	}

	glColor3fv(diffuseColor);
	if (wire)
	{
		glScalef(radius,radius,radius);
		glCallList(glWiredSphereList);
	}
	else
	{
		glScalef(radius,radius,radius);
		glCallList(glSphereList);
	}
}
