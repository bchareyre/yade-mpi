/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OPENGLWRAPPER_HPP__
#define __OPENGLWRAPPER_HPP__

#include <GL/gl.h>

template <bool> struct static_assert;
template <> struct static_assert<true> {};

/*

typedef unsigned int	GLenum;
typedef unsigned char	GLboolean;
typedef unsigned int	GLbitfield;
typedef void		GLvoid;
typedef signed char	GLbyte;	
typedef short		GLshort;
typedef int		GLint;	
typedef unsigned char	GLubyte;
typedef unsigned short	GLushort;
typedef unsigned int	GLuint;	
typedef int		GLsizei;
typedef float		GLfloat;
typedef float		GLclampf;
typedef double		GLdouble;
typedef double		GLclampd;
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename GLType>	inline GLAPI void GLAPIENTRY glVertex3v( const GLType )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type;	};

template< >	inline GLAPI void GLAPIENTRY glVertex3v< Vector3<double> >( const  Vector3<double> v )	{	glVertex3dv(v);		};

template< >	inline GLAPI void GLAPIENTRY glVertex3v< Vector3<float> >( const Vector3<float> v )	{	glVertex3fv(v);		};

template< >	inline GLAPI void GLAPIENTRY glVertex3v< Vector3<int> >( const Vector3<int> v )		{	glVertex3iv(v);		};

template< >	inline GLAPI void GLAPIENTRY glVertex3v< Vector3<short> >( const Vector3<short> v )	{	glVertex3sv(v);		};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __OPENGLWRAPPER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
