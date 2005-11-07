/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QGLTHREAD_HPP
#define QGLTHREAD_HPP

#include <yade/yade-lib-threads/Threadable.hpp>
#include <yade/yade-core/RenderingEngine.hpp>

class GLViewer;

class QGLThread : public Threadable<QGLThread>
{	
	private :
		bool * needCentering;
		bool * needResizing; // need to be a pointer otherwise modification into resize do not modify value into oneLoop and vice-versa
		int * newWidth;
		int * newHeight;
		shared_ptr<RenderingEngine> renderer;
		GLViewer * glViewer;
		
	public :
		void initializeGL();
		void centerScene();
	
		QGLThread (GLViewer * glv,shared_ptr<RenderingEngine> r);
		virtual ~QGLThread ();
	
		void resize(int w,int h);
		bool notEnd();
		void oneLoop();
};

#endif //  QGLTHREAD_HPP

