/*************************************************************************
*  © 2004 Olivier Galizzi  <olivier.galizzi@imag.fr>                     *
*  © 2008 Václav Šmilauer <eudoxos@arcig.cz>                             *
*  © 2008 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/GLDrawFunctors.hpp>
#include<yade/pkg/common/Sphere.hpp>


class Gl1_Sphere : public GlShapeFunctor{
	private:
		// for stripes
		static vector<Vector3r> vertices, faces;
		static int glSphereList;
		static int glGlutSphereList;
		void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
// 		void drawSphere(const Vector3r& color);
		void initGlLists();
		//for regenerating glutSphere list if needed
		static Real prevQuality;
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_Sphere,GlShapeFunctor,"Renders :yref:`Sphere` object",
		((Real,quality,1.0,,"Change discretization level of spheres. quality>1  for better image quality, at the price of more cpu/gpu usage, 0<quality<1 for faster rendering. This unique factor mutiplies :yref:`Gl1_Sphere::glutSlices` and :yref:`Gl1_Sphere::glutStacks`"))
		((bool,wire,false,,"Only show wireframe (controlled by ``glutSlices`` and ``glutStacks``."))
		((bool,stripes,false,,"In non-wire rendering, show stripes clearly showing particle rotation."))
		((bool,localSpecView,true,,"Compute specular light in local eye coordinate system."))
		((int,glutSlices,12,(Attr::noSave | Attr::readonly),"Base number of sphere slices, multiplied by :yref:`Gl1_Sphere::quality` before use); not used with ``stripes`` (see `glut{Solid,Wire}Sphere reference <http://www.opengl.org/documentation/specs/glut/spec3/node81.html>`_)"))
		((int,glutStacks,6,(Attr::noSave | Attr::readonly),"Base number of sphere stacks, multiplied by :yref:`Gl1_Sphere::quality` before use; not used with ``stripes`` (see `glut{Solid,Wire}Sphere reference <http://www.opengl.org/documentation/specs/glut/spec3/node81.html>`_)"))
	);
	RENDERS(Sphere);
};
REGISTER_SERIALIZABLE(Gl1_Sphere);


