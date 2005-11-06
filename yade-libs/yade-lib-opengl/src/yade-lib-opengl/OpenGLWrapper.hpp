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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-wm3-math/Vector3.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <GL/gl.h>
#include <GL/glut.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template <bool> struct static_assert;
template <> struct static_assert<true> {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///	Primary Templates									///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


template< typename Type > inline GLAPI void GLAPIENTRY glRotate		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glScale		( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTranslate	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glVertex2	( Type ,Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glVertex3	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glVertex4	( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glVertex2v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glVertex3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glVertex4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glNormal3	( Type ,Type ,Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glNormal3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glIndex		( Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glIndexv		( Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glColor3		( Type ,Type ,Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glColor4		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glColor3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glColor4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord1	( Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord2	( Type ,Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord3	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord4	( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord1v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord2v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glTexCoord4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRasterPos2	( Type ,Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRasterPos3	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRasterPos4	( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRasterPos2v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRasterPos3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRasterPos4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glRect		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glMaterial	( GLenum face, GLenum pname, Type param ){	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline GLAPI void GLAPIENTRY glMaterialv	( GLenum face, GLenum pname, Type param ){	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///	Template Specializations								///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


template< > inline GLAPI void GLAPIENTRY glRotate< double >			(double angle,double x,double y, double z )	{	glRotated(angle,x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glRotate< float >			(float angle,float x,float y, float z )	{	glRotatef(angle,x,y,z);	};

template< > inline GLAPI void GLAPIENTRY glScale< double >			( double x,double y, double z )		{	glScaled(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glScale< float >			( float x,float y,float z )		{	glScalef(x,y,z);	};

template< > inline GLAPI void GLAPIENTRY glTranslate< double >			( double x,double y, double z )		{	glTranslated(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glTranslate< float >			( float x,float y,float z )		{	glTranslatef(x,y,z);	};

template< > inline GLAPI void GLAPIENTRY glVertex2< double >			( double x,double y )			{	glVertex2d(x,y);	};
template< > inline GLAPI void GLAPIENTRY glVertex2< float >			( float x,float y )			{	glVertex2f(x,y);	};
template< > inline GLAPI void GLAPIENTRY glVertex2< int >			( int x,int y )				{	glVertex2i(x,y);	};
template< > inline GLAPI void GLAPIENTRY glVertex2< short >			( short x,short y )			{	glVertex2s(x,y);	};

template< > inline GLAPI void GLAPIENTRY glVertex3< double >			( double x,double y, double z )		{	glVertex3d(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glVertex3< float >			( float x,float y,float z )		{	glVertex3f(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glVertex3< int >			( int x, int y, int z )			{	glVertex3i(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glVertex3< short >			( short x, short y, short z )		{	glVertex3s(x,y,z);	};

template< > inline GLAPI void GLAPIENTRY glVertex4< double >			( double x,double y,double z, double w ){	glVertex4d(x,y,z,w);	};
template< > inline GLAPI void GLAPIENTRY glVertex4< float >			( float x,float y,float z, float w )	{	glVertex4f(x,y,z,w);	};
template< > inline GLAPI void GLAPIENTRY glVertex4< int >			( int x,int y,int z, int w )		{	glVertex4i(x,y,z,w);	};
template< > inline GLAPI void GLAPIENTRY glVertex4< short >			( short x,short y,short z,short w )	{	glVertex4s(x,y,z,w);	};

// :%s/\(GLAPI void GLAPIENTRY \)\(gl[A-Z,a-z,0-9]\+\)\(f\)( GLfloat \([a-z]\), GLfloat \([a-z]\), GLfloat \([a-z]\) );/template< > inline \1\2< float >			( float \4,float \5,float \6 )	{	\2\3(\4,\5,\6);	};/
template< > inline GLAPI void GLAPIENTRY glVertex2v< Vector3<double> >		( const Vector3<double> v )		{	glVertex2dv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex2v< Vector3<float> >		( const Vector3<float> v )		{	glVertex2fv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex2v< Vector3<int> >		( const Vector3<int> v )		{	glVertex2iv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex2v< Vector3<short> >		( const Vector3<short> v )		{	glVertex2sv(v);		};

template< > inline GLAPI void GLAPIENTRY glVertex3v< Vector3<double> >		( const Vector3<double> v )		{	glVertex3dv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex3v< Vector3<float> >		( const Vector3<float> v )		{	glVertex3fv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex3v< Vector3<int> >		( const Vector3<int> v )		{	glVertex3iv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex3v< Vector3<short> >		( const Vector3<short> v )		{	glVertex3sv(v);		};

template< > inline GLAPI void GLAPIENTRY glVertex4v< Vector3<double> >		( const Vector3<double> v )		{	glVertex4dv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex4v< Vector3<float> >		( const Vector3<float> v )		{	glVertex4fv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex4v< Vector3<int> >		( const Vector3<int> v )		{	glVertex4iv(v);		};
template< > inline GLAPI void GLAPIENTRY glVertex4v< Vector3<short> >		( const Vector3<short> v )		{	glVertex4sv(v);		};

template< > inline GLAPI void GLAPIENTRY glNormal3< signed char >		(signed char nx,signed char ny,signed char nz )	{	glNormal3b(nx,ny,nz);	};
template< > inline GLAPI void GLAPIENTRY glNormal3< double >			(double nx,double ny,double nz )	{	glNormal3d(nx,ny,nz);	};
template< > inline GLAPI void GLAPIENTRY glNormal3< float >			(float nx,float ny,float nz )		{	glNormal3f(nx,ny,nz);	};
template< > inline GLAPI void GLAPIENTRY glNormal3< int >			(int nx,int ny,int nz )			{	glNormal3i(nx,ny,nz);	};
template< > inline GLAPI void GLAPIENTRY glNormal3< short >			(short nx,short ny,short nz )		{	glNormal3s(nx,ny,nz);	};

template< > inline GLAPI void GLAPIENTRY glNormal3v< Vector3<signed char> >	( const Vector3<signed char> v )	{	glNormal3bv(v);		};
template< > inline GLAPI void GLAPIENTRY glNormal3v< Vector3<double> >		( const Vector3<double> v )		{	glNormal3dv(v);		};
template< > inline GLAPI void GLAPIENTRY glNormal3v< Vector3<float> >		( const Vector3<float> v )		{	glNormal3fv(v);		};
template< > inline GLAPI void GLAPIENTRY glNormal3v< Vector3<int> >		( const Vector3<int> v )		{	glNormal3iv(v);		};
template< > inline GLAPI void GLAPIENTRY glNormal3v< Vector3<short> >		( const Vector3<short> v )		{	glNormal3sv(v);		};

template< > inline GLAPI void GLAPIENTRY glIndex< double >			( double c )				{	glIndexd(c);	};
template< > inline GLAPI void GLAPIENTRY glIndex< float >			( float c )				{	glIndexf(c);	};
template< > inline GLAPI void GLAPIENTRY glIndex< int >				( int c )				{	glIndexi(c);	};
template< > inline GLAPI void GLAPIENTRY glIndex< short >			( short c )				{	glIndexs(c);	};
template< > inline GLAPI void GLAPIENTRY glIndex< unsigned char >		( unsigned char c )			{	glIndexub(c);	};

template< > inline GLAPI void GLAPIENTRY glIndexv<const Vector3<double> >	( const Vector3<double> c)		{	glIndexdv(c);	}
template< > inline GLAPI void GLAPIENTRY glIndexv<const Vector3<float> >	( const Vector3<float> c)		{	glIndexfv(c);	}
template< > inline GLAPI void GLAPIENTRY glIndexv<const Vector3<int> >		( const Vector3<int> c)			{	glIndexiv(c);	}
template< > inline GLAPI void GLAPIENTRY glIndexv<const Vector3<short> >	( const Vector3<short> c)		{	glIndexsv(c);	}
template< > inline GLAPI void GLAPIENTRY glIndexv<const Vector3<unsigned char> >( const Vector3<unsigned char> c)	{	glIndexubv(c);	}

template< > inline GLAPI void GLAPIENTRY glColor3< signed char >		(signed char red,signed char green,signed char blue )					{	glColor3b(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< double >			(double red,double green,double blue )							{	glColor3d(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< float >			(float red,float green,float blue )							{	glColor3f(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< int >			(int red,int green,int blue )								{	glColor3i(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< short >			(short red,short green,short blue )							{	glColor3s(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< unsigned char >		(unsigned char red,unsigned char green,unsigned char blue )				{	glColor3ub(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< unsigned int >		(unsigned int red,unsigned int green,unsigned int blue )				{	glColor3ui(red,green,blue);	};
template< > inline GLAPI void GLAPIENTRY glColor3< unsigned short >		(unsigned short red,unsigned short green,unsigned short blue )				{	glColor3us(red,green,blue);	};

template< > inline GLAPI void GLAPIENTRY glColor4< signed char >		(signed char red,signed char green,signed char blue, signed char alpha )		{	glColor4b(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< double >			(double red,double green,double blue, double alpha )					{	glColor4d(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< float >			(float red,float green,float blue, float alpha )					{	glColor4f(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< int >			(int red,int green,int blue, int alpha )						{	glColor4i(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< short >			(short red,short green,short blue, short alpha )					{	glColor4s(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< unsigned char >		(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha )		{	glColor4ub(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< unsigned int >		(unsigned int red,unsigned int green,unsigned int blue,unsigned int alpha )		{	glColor4ui(red,green,blue,alpha);	};
template< > inline GLAPI void GLAPIENTRY glColor4< unsigned short >		(unsigned short red,unsigned short green,unsigned short blue,unsigned short alpha )	{	glColor4us(red,green,blue,alpha);	};


template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<signed char> >	( const Vector3<signed char> v )	{	glColor3bv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<double> >		( const Vector3<double> v )		{	glColor3dv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<float> >		( const Vector3<float> v )		{	glColor3fv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<int> >		( const Vector3<int> v )		{	glColor3iv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<short> >		( const Vector3<short> v )		{	glColor3sv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<unsigned char> >	( const Vector3<unsigned char> v )	{	glColor3ubv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<unsigned int> >	( const Vector3<unsigned int> v )	{	glColor3uiv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor3v< Vector3<unsigned short> >	( const Vector3<unsigned short> v )	{	glColor3usv(v);		};

template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<signed char> >	( const Vector3<signed char> v )	{	glColor4bv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<double> >		( const Vector3<double> v )		{	glColor4dv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<float> >		( const Vector3<float> v )		{	glColor4fv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<int> >		( const Vector3<int> v )		{	glColor4iv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<short> >		( const Vector3<short> v )		{	glColor4sv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<unsigned char> >	( const Vector3<unsigned char> v )	{	glColor4ubv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<unsigned int> >	( const Vector3<unsigned int> v )	{	glColor4uiv(v);		};
template< > inline GLAPI void GLAPIENTRY glColor4v< Vector3<unsigned short> >	( const Vector3<unsigned short> v )	{	glColor4usv(v);		};


template< > inline GLAPI void GLAPIENTRY glTexCoord1< double >			( double s )				{	glTexCoord1d(s);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord1< float >			( float s )				{	glTexCoord1f(s);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord1< int >			( int s )				{	glTexCoord1i(s);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord1< short >			( short s )				{	glTexCoord1s(s);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord2< double >			( double s,double t )			{	glTexCoord2d(s,t);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord2< float >			( float s,float t )			{	glTexCoord2f(s,t);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord2< int >			( int s,int t )				{	glTexCoord2i(s,t);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord2< short >			( short s,short t )			{	glTexCoord2s(s,t);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord3< double >			( double s,double t, double r )		{	glTexCoord3d(s,t,r);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord3< float >			( float s,float t,float r )		{	glTexCoord3f(s,t,r);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord3< int >			( int s, int t, int r )			{	glTexCoord3i(s,t,r);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord3< short >			( short s, short t, short r )		{	glTexCoord3s(s,t,r);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord4< double >			(double s,double t,double r, double q )	{	glTexCoord4d(s,t,r,q);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord4< float >			(float s,float t,float r, float q )	{	glTexCoord4f(s,t,r,q);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord4< int >			(int s,int t,int r, int q )		{	glTexCoord4i(s,t,r,q);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord4< short >			(short s,short t,short r,short q )	{	glTexCoord4s(s,t,r,q);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord1v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord1dv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord1v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord1fv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord1v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord1iv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord1v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord1sv(v);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord2v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord2dv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord2v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord2fv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord2v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord2iv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord2v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord2sv(v);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord3v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord3dv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord3v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord3fv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord3v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord3iv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord3v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord3sv(v);	};

template< > inline GLAPI void GLAPIENTRY glTexCoord4v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord4dv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord4v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord4fv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord4v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord4iv(v);	};
template< > inline GLAPI void GLAPIENTRY glTexCoord4v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord4sv(v);	};


template< > inline GLAPI void GLAPIENTRY glRasterPos2< double >			( double x,double y )			{	glRasterPos2d(x,y);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos2< float >			( float x,float y )			{	glRasterPos2f(x,y);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos2< int >			( int x,int y )				{	glRasterPos2i(x,y);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos2< short >			( short x,short y )			{	glRasterPos2s(x,y);	};

template< > inline GLAPI void GLAPIENTRY glRasterPos3< double >			( double x,double y, double z )		{	glRasterPos3d(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos3< float >			( float x,float y,float z )		{	glRasterPos3f(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos3< int >			( int x, int y, int z )			{	glRasterPos3i(x,y,z);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos3< short >			( short x, short y, short z )		{	glRasterPos3s(x,y,z);	};

template< > inline GLAPI void GLAPIENTRY glRasterPos4< double >			(double x,double y,double z, double w )	{	glRasterPos4d(x,y,z,w);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos4< float >			(float x,float y,float z, float w )	{	glRasterPos4f(x,y,z,w);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos4< int >			(int x,int y,int z, int w )		{	glRasterPos4i(x,y,z,w);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos4< short >			(short x,short y,short z,short w )	{	glRasterPos4s(x,y,z,w);	};

template< > inline GLAPI void GLAPIENTRY glRasterPos2v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos2dv(v);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos2v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos2fv(v);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos2v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos2iv(v);	};
template< > inline GLAPI void GLAPIENTRY glRasterPos2v< Vector3<short> >	( const Vector3<short> v )		{	glRasterPos2sv(v);	};


// :%s/\(GLAPI void GLAPIENTRY \)\(gl[A-Z,a-z,0-9]\+\)\(us\)\(v\)( const GLushort \*\(v\) );/template< > inline \1\2\4< Vector3<unsigned short> >	( const Vector3<unsigned short> \5 )	{	\2\3\4(\5);		};/
template< > inline GLAPI void GLAPIENTRY glRasterPos3v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos3dv(v);		};
template< > inline GLAPI void GLAPIENTRY glRasterPos3v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos3fv(v);		};
template< > inline GLAPI void GLAPIENTRY glRasterPos3v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos3iv(v);		};
template< > inline GLAPI void GLAPIENTRY glRasterPos3v< Vector3<short> >	( const Vector3<short> v )		{	glRasterPos3sv(v);		};

template< > inline GLAPI void GLAPIENTRY glRasterPos4v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos4dv(v);		};
template< > inline GLAPI void GLAPIENTRY glRasterPos4v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos4fv(v);		};
template< > inline GLAPI void GLAPIENTRY glRasterPos4v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos4iv(v);		};
template< > inline GLAPI void GLAPIENTRY glRasterPos4v< Vector3<short> >	( const Vector3<short> v )		{	glRasterPos4sv(v);		};


template< > inline GLAPI void GLAPIENTRY glRect< double >			(double x1,double y1,double x2, double y2 )	{	glRectd(x1,y1,x2,y2);	};
template< > inline GLAPI void GLAPIENTRY glRect< float >			(float	x1,float  y1,float x2,float y2 )	{	glRectf(x1,y1,x2,y2);	};
template< > inline GLAPI void GLAPIENTRY glRect< int >				(int	x1,int	  y1,int x2, int y2 )		{	glRecti(x1,y1,x2,y2);	};
template< > inline GLAPI void GLAPIENTRY glRect< short >			(short	x1,short  y1,short x2,short y2 )	{	glRects(x1,y1,x2,y2);	};

template< > inline GLAPI void GLAPIENTRY glMaterial< float >			( GLenum face, GLenum pname, float param )			{	glMaterialf(face,pname,param);		};
template< > inline GLAPI void GLAPIENTRY glMaterial< int >			( GLenum face, GLenum pname, int param )			{	glMateriali(face,pname,param);		};
template< > inline GLAPI void GLAPIENTRY glMaterialv< Vector3<float> >		( GLenum face, GLenum pname, const Vector3<float> params )	{	glMaterialfv(face,pname,params);	};
template< > inline GLAPI void GLAPIENTRY glMaterialv< Vector3<int> >		( GLenum face, GLenum pname, const Vector3<int> params )	{	glMaterialiv(face,pname,params);	};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///	Functions Not Coverted									///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
GLAPI void GLAPIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 );
GLAPI void GLAPIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 );
GLAPI void GLAPIENTRY glRectiv( const GLint *v1, const GLint *v2 );
GLAPI void GLAPIENTRY glRectsv( const GLshort *v1, const GLshort *v2 );

GLAPI void GLAPIENTRY glLightf( GLenum light, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glLighti( GLenum light, GLenum pname, GLint param );
GLAPI void GLAPIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params );

GLAPI void GLAPIENTRY glLightModelf( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glLightModeli( GLenum pname, GLint param );
GLAPI void GLAPIENTRY glLightModelfv( GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glLightModeliv( GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params );

GLAPI void GLAPIENTRY glPixelStoref( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glPixelStorei( GLenum pname, GLint param );

GLAPI void GLAPIENTRY glPixelTransferf( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glPixelTransferi( GLenum pname, GLint param );

GLAPI void GLAPIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values );
GLAPI void GLAPIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values );
GLAPI void GLAPIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values );

GLAPI void GLAPIENTRY glGetPixelMapfv( GLenum map, GLfloat *values );
GLAPI void GLAPIENTRY glGetPixelMapuiv( GLenum map, GLuint *values );
GLAPI void GLAPIENTRY glGetPixelMapusv( GLenum map, GLushort *values );

GLAPI void GLAPIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param );
GLAPI void GLAPIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param );

GLAPI void GLAPIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params );
GLAPI void GLAPIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params );
GLAPI void GLAPIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params );

GLAPI void GLAPIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param );

GLAPI void GLAPIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params );

GLAPI void GLAPIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param );

GLAPI void GLAPIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params);
GLAPI void GLAPIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params );

GLAPI void GLAPIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );

GLAPI void GLAPIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
GLAPI void GLAPIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );

GLAPI void GLAPIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
GLAPI void GLAPIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );

GLAPI void GLAPIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v );
GLAPI void GLAPIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v );
GLAPI void GLAPIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v );

template< > inline GLAPI void GLAPIENTRY glEvalCoord1< double >			( double u )	{	glEvalCoord1d(u);	};
template< > inline GLAPI void GLAPIENTRY glEvalCoord1< float >			( float u )	{	glEvalCoord1f(u);	};

GLAPI void GLAPIENTRY glEvalCoord1dv( const GLdouble *u );
GLAPI void GLAPIENTRY glEvalCoord1fv( const GLfloat *u );

// :%s/\(GLAPI void GLAPIENTRY \)\(gl[A-Z,a-z,0-9]\+\)\(f\)( GLfloat \([a-z]\), GLfloat \([a-z]\) );/template< > inline \1\2< float >			( float \4,float \5 )	{	\2\3(\4,\5);	};/

template< > inline GLAPI void GLAPIENTRY glEvalCoord2< double >			( double u,double v )	{	glEvalCoord2d(u,v);	};
template< > inline GLAPI void GLAPIENTRY glEvalCoord2< float >			( float u,float v )	{	glEvalCoord2f(u,v);	};

GLAPI void GLAPIENTRY glEvalCoord2dv( const GLdouble *u );
GLAPI void GLAPIENTRY glEvalCoord2fv( const GLfloat *u );

GLAPI void GLAPIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 );
GLAPI void GLAPIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 );

GLAPI void GLAPIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
GLAPI void GLAPIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );


GLAPI void GLAPIENTRY glFogf( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glFogi( GLenum pname, GLint param );

GLAPI void GLAPIENTRY glFogfv( GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glFogiv( GLenum pname, const GLint *params );


GLAPI void GLAPIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params);
GLAPI void GLAPIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params);
GLAPI void GLAPIENTRY glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params );
GLAPI void GLAPIENTRY glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params );
GLAPI void GLAPIENTRY glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params );
GLAPI void GLAPIENTRY glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params );
GLAPI void GLAPIENTRY glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glConvolutionParameteri( GLenum target, GLenum pname, GLint params );
GLAPI void GLAPIENTRY glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params );
GLAPI void GLAPIENTRY glGetConvolutionParameterfv( GLenum target, GLenum pname,	GLfloat *params );
GLAPI void GLAPIENTRY glGetConvolutionParameteriv( GLenum target, GLenum pname,	GLint *params );

GLAPI void GLAPIENTRY glMultiTexCoord1d( GLenum target, GLdouble s );
GLAPI void GLAPIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord1f( GLenum target, GLfloat s );
GLAPI void GLAPIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord1i( GLenum target, GLint s );
GLAPI void GLAPIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord1s( GLenum target, GLshort s );
GLAPI void GLAPIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t );
GLAPI void GLAPIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t );
GLAPI void GLAPIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t );
GLAPI void GLAPIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t );
GLAPI void GLAPIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r );
GLAPI void GLAPIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r );
GLAPI void GLAPIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r );
GLAPI void GLAPIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r );
GLAPI void GLAPIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );
GLAPI void GLAPIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
GLAPI void GLAPIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q );
GLAPI void GLAPIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );
GLAPI void GLAPIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v );


GLAPI void GLAPIENTRY glLoadTransposeMatrixd( const GLdouble m[16] );
GLAPI void GLAPIENTRY glLoadTransposeMatrixf( const GLfloat m[16] );
GLAPI void GLAPIENTRY glMultTransposeMatrixd( const GLdouble m[16] );
GLAPI void GLAPIENTRY glMultTransposeMatrixf( const GLfloat m[16] );

GLAPI void GLAPIENTRY glMultiTexCoord1dARB(GLenum target, GLdouble s);
GLAPI void GLAPIENTRY glMultiTexCoord1dvARB(GLenum target, const GLdouble *v);
GLAPI void GLAPIENTRY glMultiTexCoord1fARB(GLenum target, GLfloat s);
GLAPI void GLAPIENTRY glMultiTexCoord1fvARB(GLenum target, const GLfloat *v);
GLAPI void GLAPIENTRY glMultiTexCoord1iARB(GLenum target, GLint s);
GLAPI void GLAPIENTRY glMultiTexCoord1ivARB(GLenum target, const GLint *v);
GLAPI void GLAPIENTRY glMultiTexCoord1sARB(GLenum target, GLshort s);
GLAPI void GLAPIENTRY glMultiTexCoord1svARB(GLenum target, const GLshort *v);
GLAPI void GLAPIENTRY glMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t);
GLAPI void GLAPIENTRY glMultiTexCoord2dvARB(GLenum target, const GLdouble *v);
GLAPI void GLAPIENTRY glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);
GLAPI void GLAPIENTRY glMultiTexCoord2fvARB(GLenum target, const GLfloat *v);
GLAPI void GLAPIENTRY glMultiTexCoord2iARB(GLenum target, GLint s, GLint t);
GLAPI void GLAPIENTRY glMultiTexCoord2ivARB(GLenum target, const GLint *v);
GLAPI void GLAPIENTRY glMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t);
GLAPI void GLAPIENTRY glMultiTexCoord2svARB(GLenum target, const GLshort *v);
GLAPI void GLAPIENTRY glMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r);
GLAPI void GLAPIENTRY glMultiTexCoord3dvARB(GLenum target, const GLdouble *v);
GLAPI void GLAPIENTRY glMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r);
GLAPI void GLAPIENTRY glMultiTexCoord3fvARB(GLenum target, const GLfloat *v);
GLAPI void GLAPIENTRY glMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r);
GLAPI void GLAPIENTRY glMultiTexCoord3ivARB(GLenum target, const GLint *v);
GLAPI void GLAPIENTRY glMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r);
GLAPI void GLAPIENTRY glMultiTexCoord3svARB(GLenum target, const GLshort *v);
GLAPI void GLAPIENTRY glMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLAPI void GLAPIENTRY glMultiTexCoord4dvARB(GLenum target, const GLdouble *v);
GLAPI void GLAPIENTRY glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLAPI void GLAPIENTRY glMultiTexCoord4fvARB(GLenum target, const GLfloat *v);
GLAPI void GLAPIENTRY glMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q);
GLAPI void GLAPIENTRY glMultiTexCoord4ivARB(GLenum target, const GLint *v);
GLAPI void GLAPIENTRY glMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
GLAPI void GLAPIENTRY glMultiTexCoord4svARB(GLenum target, const GLshort *v);

GLAPI void GLAPIENTRY glPointParameterfEXT( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glPointParameterfvEXT( GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glPointParameterfSGIS(GLenum pname, GLfloat param);
GLAPI void GLAPIENTRY glPointParameterfvSGIS(GLenum pname, const GLfloat *params);

GLAPI void GLAPIENTRY glWindowPos2iMESA( GLint x, GLint y );
GLAPI void GLAPIENTRY glWindowPos2sMESA( GLshort x, GLshort y );
GLAPI void GLAPIENTRY glWindowPos2fMESA( GLfloat x, GLfloat y );
GLAPI void GLAPIENTRY glWindowPos2dMESA( GLdouble x, GLdouble y );
GLAPI void GLAPIENTRY glWindowPos2ivMESA( const GLint *p );
GLAPI void GLAPIENTRY glWindowPos2svMESA( const GLshort *p );
GLAPI void GLAPIENTRY glWindowPos2fvMESA( const GLfloat *p );
GLAPI void GLAPIENTRY glWindowPos2dvMESA( const GLdouble *p );
GLAPI void GLAPIENTRY glWindowPos3iMESA( GLint x, GLint y, GLint z );
GLAPI void GLAPIENTRY glWindowPos3sMESA( GLshort x, GLshort y, GLshort z );
GLAPI void GLAPIENTRY glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z );
GLAPI void GLAPIENTRY glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glWindowPos3ivMESA( const GLint *p );
GLAPI void GLAPIENTRY glWindowPos3svMESA( const GLshort *p );
GLAPI void GLAPIENTRY glWindowPos3fvMESA( const GLfloat *p );
GLAPI void GLAPIENTRY glWindowPos3dvMESA( const GLdouble *p );
GLAPI void GLAPIENTRY glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w );
GLAPI void GLAPIENTRY glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w );
GLAPI void GLAPIENTRY glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
GLAPI void GLAPIENTRY glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI void GLAPIENTRY glWindowPos4ivMESA( const GLint *p );
GLAPI void GLAPIENTRY glWindowPos4svMESA( const GLshort *p );
GLAPI void GLAPIENTRY glWindowPos4fvMESA( const GLfloat *p );
GLAPI void GLAPIENTRY glWindowPos4dvMESA( const GLdouble *p );
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __OPENGLWRAPPER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
