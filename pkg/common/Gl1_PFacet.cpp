#ifdef YADE_OPENGL
#include<pkg/common/Gl1_PFacet.hpp>

#include<lib/opengl/OpenGLWrapper.hpp>

// bool Gl1_PFacet::normals=false;
bool Gl1_PFacet::wire=true;
void Gl1_PFacet::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& st,bool wire2,const GLViewInfo&)
{   
	PFacet* Pfacet = static_cast<PFacet*>(cm.get());
	vector<Vector3r> vertices;
	vertices.push_back(Pfacet->node1->state->pos);
	vertices.push_back(Pfacet->node2->state->pos);
	vertices.push_back(Pfacet->node3->state->pos);
	
	Vector3r pos=Pfacet->node1->state->pos;
	
	st->ori = Quaternionr::Identity();// Otherwise clumped connexions get rotated by the clump motion and the view is messed up (note that orientation is never used in mechanical calculations in the case of connexions and pfacets).

	vertices[0]=vertices[0]-pos;
	vertices[1]=vertices[1]-pos;
	vertices[2]=vertices[2]-pos;
	
	vector<Vector3r> verticesF1 = vertices;
	Vector3r normal=(vertices[1]-vertices[0]).cross(vertices[2]-vertices[1]); normal.normalize();
	verticesF1[0]=vertices[0] + normal*Pfacet->radius;
	verticesF1[1]=vertices[1] + normal*Pfacet->radius;
	verticesF1[2]=vertices[2] + normal*Pfacet->radius;
	
	vector<Vector3r> verticesF2 = vertices;
	
	verticesF2[0] = vertices[0] - normal*Pfacet->radius;
	verticesF2[1] = vertices[1] - normal*Pfacet->radius;
	verticesF2[2] = vertices[2] - normal*Pfacet->radius;
	
	if(!wire2||!wire){

		glDisable(GL_CULL_FACE); 
		
		glColor3v(cm->color);
		glBegin(GL_TRIANGLES);
			glNormal3v(normal); // this makes every triangle different WRT the light direction; important!
			glVertex3v(verticesF1[0]);
			glVertex3v(verticesF1[1]);
			glVertex3v(verticesF1[2]);
		glEnd();
		glBegin(GL_TRIANGLES);
			glNormal3v(Pfacet->normal); // this makes every triangle different WRT the light direction; important!
			glVertex3v(verticesF2[2]);
			glVertex3v(verticesF2[1]);
			glVertex3v(verticesF2[0]);
		glEnd();

	}
}

YADE_PLUGIN((Gl1_PFacet));

#endif