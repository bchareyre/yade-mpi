/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_OPENGL

#include <pkg/dem/deformablecohesive/Gl1_DeformableElement.hpp>
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <lib/opengl/OpenGLWrapper.hpp>

typedef DeformableElement::NodeMap NodeMap;
void Gl1_DeformableElement::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire,const GLViewInfo&)
{   
	    DeformableElement* element = static_cast<DeformableElement*>(cm.get());
	    std::vector<Vector3r> triangles = element->faces;


		if(!(cm->wire || wire)){
			glDisable(GL_CULL_FACE);
			//Vector3r normal=(facet->vertices[1]-facet->vertices[0]).cross(facet->vertices[2]-facet->vertices[1]); normal.normalize();
			glColor3v(cm->color);
			glBegin(GL_TRIANGLES);

			FOREACH(Vector3r vertices,triangles){
				   NodeMap::iterator i1(element->localmap.begin());
				   NodeMap::iterator i2(i1);
				   NodeMap::iterator i3(i1);
				   NodeMap::iterator i4(i1);

				   //Find the opposite vertex total indice sum equals to six therefore the last one is going to be 6-sum of indices
				   // I accept it is very uncommon
				   int oppositevertexindex=6-vertices.sum();

				   std::advance(i1,vertices[0]);
				   std::advance(i2,vertices[1]);
				   std::advance(i3,vertices[2]);
				   std::advance(i4,oppositevertexindex);

				   const shared_ptr<Body>& member1=i1->first;
				   const shared_ptr<Body>& member2=i2->first;
				   const shared_ptr<Body>& member3=i3->first;

				   Vector3r& vertex0=i1->first->state->pos;
				   Vector3r& vertex1=i2->first->state->pos;
				   Vector3r& vertex2=i3->first->state->pos;
				   Vector3r& vertex3=i4->first->state->pos;

   				   Vector3r normal=(vertex1-vertex0).cross(vertex2-vertex0);
				   normal.normalize();	

			   	   if((vertex3-vertex0).dot(normal)>0)
        				   normal=-normal;


				   glNormal3v(normal); // this makes every triangle different WRT the light   
				   glVertex3v(member1->state->pos);
      				   glVertex3v(member2->state->pos);
				   glVertex3v(member3->state->pos);

			}
		 }
			glEnd();

			glBegin(GL_LINE_LOOP);
			glColor3v(Vector3r(0,0,0));
			FOREACH(Vector3r vertices,triangles){
				   
                                 NodeMap::iterator i1(element->localmap.begin());
                                 NodeMap::iterator i2(i1);
                                 NodeMap::iterator i3(i1);

                                 std::advance(i1,vertices[0]);
                                 std::advance(i2,vertices[1]);
                                 std::advance(i3,vertices[2]);

                                 const shared_ptr<Body>& member1=i1->first;
                                 const shared_ptr<Body>& member2=i2->first;
                                 const shared_ptr<Body>& member3=i3->first;


                                 glVertex3v(member1->state->pos);
                                 glVertex3v(member2->state->pos);
                                 glVertex3v(member3->state->pos);



			}

			glEnd();

}

YADE_PLUGIN((Gl1_DeformableElement));

#endif /* YADE_OPENGL */
