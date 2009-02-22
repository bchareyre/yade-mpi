/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ef1_BshSnowGrain_glDraw.hpp"
#include<yade/pkg-snow/BshSnowGrain.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>

inline qglviewer::Vec toQGLViewierVec(Vector3r v){return qglviewer::Vec(v[0],v[1],v[2]);};

bool light_selection(int which)
{
	GLfloat matAmbient[4];
	int select = Omega::instance().isoSec;
	if(select == which)
	{
		matAmbient[0] = 0.2;
		matAmbient[1] = 0.2;
		matAmbient[2] = 0.2;
		matAmbient[3] = 0.0;
	} else
	{
		matAmbient[0] = 0.0;
		matAmbient[1] = 0.0;
		matAmbient[2] = 0.0;
		matAmbient[3] = 0.0;
	}
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,matAmbient);
	return (select == which);
};


void Ef1_BshSnowGrain_glDraw::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	bool surface = !wire;
	BshSnowGrain* gr = static_cast<BshSnowGrain*>(gm.get());
	Real LEN=(gr->start - gr->end).Length();

	glColor3f(0.5,0.5,1.0);
	glutWireCube(LEN*0.1);

  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(gm->diffuseColor[0],gm->diffuseColor[1],gm->diffuseColor[2]));
	glColor3v(gm->diffuseColor);

	int current(0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	const std::vector<boost::tuple<Vector3r,Vector3r,Vector3r,Vector3r> >& f(gr->get_faces_const_ref());
	glBegin(GL_TRIANGLES);
		for(size_t i = 0; i < f.size() ; ++i)
		{
			if(light_selection(current++) || surface)
			{
				glNormal3v(get<3>(f[i]));
				glVertex3v(get<0>(f[i]));
				glVertex3v(get<1>(f[i]));
				glVertex3v(get<2>(f[i]));
			}
		}
	glEnd();
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);

	if(!surface)
	{
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_STRIP);
			glColor3f(0,0,0);
			glVertex3v(gr->start);
			glVertex3v(gr->end);
		glEnd();
			glColor3v(gm->diffuseColor);
			for(size_t i=0;i < gr->slices.size();++i)
			{
				glBegin(GL_LINE_STRIP);
					for(size_t j=0 ; j < gr->slices[i].size() ; ++j)
						glVertex3v(gr->slices[i][j]);
					glVertex3v(gr->slices[i][0]);
				glEnd();
			}
		glEnable(GL_LIGHTING);
	}

/*
	// plot depth tetrahedron of selected surface

//	int me = (int)(Omega::instance().selectedBody);
//	if(me > 0 && me < Omega::instance().getRootBody()->bodies->size())
//	{
//		BshSnowGrain* m = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[me]->geometricalModel.get());
//		if(m && m==gr)
//		{
//			if(gr->slices[0][0] == m->slices[0][0])
//			{
	current = 0;
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		for(size_t i = 0; i < f.size() ; ++i)
		{
			if(light_selection(current++) || surface)
			{
				Vector3r a(get<0>(f[i]));
				Vector3r b(get<1>(f[i]));
				Vector3r c(get<2>(f[i]));
				Vector3r n(get<3>(f[i]));
				Real l = gr->depth(i);
				Vector3r C((a+b+c)/3.0);
				n = n * l;
				//QGLViewer::drawArrow(toQGLViewierVec(C), toQGLViewierVec(n), l*0.05, 8);
				if(light_selection(current-1)) glColor3f(1.0,0.0,0.0); else glColor3f(0.0,1.0,0.0);
				glVertex3v(a);
				glVertex3v(C+n);
				glVertex3v(b);
				glVertex3v(C+n);				
				glVertex3v(c);
				glVertex3v(C+n);
			}
		}
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

//			}
//		}
//	}
*/


/*

// check inside of selected grain, with grain 17

//if(!surface)
//{

//	std::vector<Vector3r> me_inside;me_inside.clear();
//	std::vector<Vector3r> oth_inside;oth_inside.clear();

	int me = (int)(Omega::instance().selectedBody);
	if(me > 0 && me < Omega::instance().getRootBody()->bodies->size())
	{
		BshSnowGrain* m = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[me]->geometricalModel.get());
		if(m && m==gr)
		{
			if(gr->slices[0][0] == m->slices[0][0])
			{
				std::cerr << "got body " << me << "\n";
				int other=17;
				if(other > 0 && other < Omega::instance().getRootBody()->bodies->size())
				{
				BshSnowGrain* oth = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[other]->geometricalModel.get());
				if(oth)
				{
				Vector3r    my_pos((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.position);
				Vector3r    oth_pos((*(Omega::instance().getRootBody()->bodies))[other]->physicalParameters->se3.position);
				Quaternionr my_q((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.orientation);
				Quaternionr oth_q((*(Omega::instance().getRootBody()->bodies))[other]->physicalParameters->se3.orientation);

				glColor3f(1,0,0);
				for(size_t i=0;i < gr->slices.size();++i)
				{
					for(size_t j=0 ; j < gr->slices[i].size() ; ++j)
					{
						Vector3r v(gr->slices[i][j]);
						if(oth->is_point_inside_polyhedron( oth_q.Conjugate()*(my_q * v+my_pos-oth_pos)))
						{
//							me_inside.push_back( my_q * v+my_pos );
							glTranslatev(v);
							glutSolidCube(LEN*0.01);
							glTranslatev(-v);
						}
					}
				}
				}
				}
			}
		}
	}



// check inside of grain 17 with the selected one
	me = 17;
	if(me > 0 && me < Omega::instance().getRootBody()->bodies->size())
	{
		BshSnowGrain* m = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[me]->geometricalModel.get());
		if(m && m==gr)
		{
			if(gr->slices[0][0] == m->slices[0][0])
			{
				std::cerr << "got body " << me << "\n";
				int other=(int)(Omega::instance().selectedBody);
				if(other > 0 && other < Omega::instance().getRootBody()->bodies->size())
				{
				BshSnowGrain* oth = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[other]->geometricalModel.get());
				if(oth)
				{
				Vector3r    my_pos((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.position);
				Vector3r    oth_pos((*(Omega::instance().getRootBody()->bodies))[other]->physicalParameters->se3.position);
				Quaternionr my_q((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.orientation);
				Quaternionr oth_q((*(Omega::instance().getRootBody()->bodies))[other]->physicalParameters->se3.orientation);

				glColor3f(0,1,1);
				for(size_t i=0;i < gr->slices.size();++i)
				{
					for(size_t j=0 ; j < gr->slices[i].size() ; ++j)
					{
						Vector3r v(gr->slices[i][j]);
						if(oth->is_point_inside_polyhedron( oth_q.Conjugate()*(my_q * v+my_pos-oth_pos)))
						{
//							oth_inside.push_back( my_q * v+my_pos );
							glTranslatev(v);
							glutSolidCube(LEN*0.01);
							glTranslatev(-v);
						}
					}
				}
				}
				}
			}
		}
	}
//}

*/



	// check current grain insides
//if(!surface)
//{
//	int me = (int)(Omega::instance().selectedBody);
//	if(me > 0 && me < Omega::instance().getRootBody()->bodies->size())
//	{
//		BshSnowGrain* m = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[me]->geometricalModel.get());
//		if(m && m==gr)
//		{
//			Vector3r    my_pos((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.position);
//
//			for(float x=-1 ; x<1 ; x+=0.15)
//			for(float y=-1 ; y<1 ; y+=0.15)
//			for(float z=-1 ; z<1 ; z+=0.15)
//			{
//				Vector3r v=Vector3r(x,y,z)*LEN*0.7+my_pos-my_pos;
//				if(gr->is_point_inside_polyhedron(v))
//				{
//					glTranslatev(v);
//					glutSolidCube(LEN*0.02);
//					glTranslatev(-v);
//				}
//			}
//		}
//	}
//}
/*
	// check inside with other grain (check 35000 points, very slow)
if(!surface)
{
//	glBegin(GL_POINTS);
	int me = (int)(Omega::instance().selectedBody);
	if(me > 0 && me < Omega::instance().getRootBody()->bodies->size())
	{
		BshSnowGrain* m = dynamic_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[me]->geometricalModel.get());
		if(m)
		{
			if(gr->slices[0][0] == m->slices[0][0])
			{
				std::cerr << "got body " << me << "\n";
				int other=17;
				BshSnowGrain* oth = static_cast<BshSnowGrain*>((*(Omega::instance().getRootBody()->bodies))[other]->geometricalModel.get());

				Vector3r    my_pos((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.position);
				Vector3r    oth_pos((*(Omega::instance().getRootBody()->bodies))[other]->physicalParameters->se3.position);
				Quaternionr my_q((*(Omega::instance().getRootBody()->bodies))[me]->physicalParameters->se3.orientation);
				Quaternionr oth_q((*(Omega::instance().getRootBody()->bodies))[other]->physicalParameters->se3.orientation);

				glColor3f(1,0,0);
//				for(size_t i=0;i < gr->slices.size();++i){
//					for(size_t j=0 ; j < gr->slices[i].size() ; ++j){
//						Vector3r v(gr->slices[i][j]);



			for(float x=-1 ; x<1 ; x+=0.06)
			for(float y=-1 ; y<1 ; y+=0.06)
			for(float z=-1 ; z<1 ; z+=0.06)
			{
				Vector3r v=Vector3r(x,y,z)*LEN*1.2;
						if(oth->is_point_inside_polyhedron( oth_q.Conjugate()*(my_q * v+my_pos-oth_pos)))
						{
						//	glVertex3v(v);
					glTranslatev(v);
					glutSolidCube(LEN*0.01);
					glTranslatev(-1.0*(v));
						}
			}
//					}
//				}
			}
		}
	}
//	glEnd();
}
*/

}

YADE_PLUGIN();

