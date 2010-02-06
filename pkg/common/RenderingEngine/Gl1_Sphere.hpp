/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class Gl1_Sphere : public GlShapeFunctor{	
	private :
		static bool wire, stripes, glutNormalize;
		static int glutSlices, glutStacks;

		// for stripes
		static vector<Vector3r> vertices, faces;
		static int glSphereList;
		void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
		void drawSphere(void);
		void initGlLists(void);
	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	YADE_CLASS_BASE_DOC_ATTRS(Gl1_Sphere,GlShapeFunctor,"Renders :yref:`Sphere` object",
		((wire,"|ystatic| Only show wireframe (controlled by ``glutSlices`` and ``glutStacks``. :ydefault:`false`"))
		((stripes,"|ystatic| In non-wire rendering, show stripes clearly showing particle rotation. :ydefault:`false` "))
		((glutNormalize,"|ystatic| Fix normals for non-wire rendering; see `http://lists.apple.com/archives/Mac-opengl/2002/Jul/msg00085.html`_ :ydefault:`true`"))
		((glutSlices,"|ystatic| Number of sphere slices; not used with ``stripes`` (see `glut{Solid,Wire}Sphere reference<http://www.opengl.org/documentation/specs/glut/spec3/node81.html>`_) :ydefault:`12` "))
		((glutStacks,"|ystatic| Number of sphere stacks; not used with ``stripes`` (see `glut{Solid,Wire}Sphere reference<http://www.opengl.org/documentation/specs/glut/spec3/node81.html>`_) :ydefault:`6` "))
	);
	RENDERS(Sphere);
};

REGISTER_SERIALIZABLE(Gl1_Sphere);


