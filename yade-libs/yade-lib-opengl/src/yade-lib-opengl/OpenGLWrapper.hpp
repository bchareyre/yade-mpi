/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef OPENGLWRAPPER_HPP
#define OPENGLWRAPPER_HPP

#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

#include <GL/gl.h>
#include <GL/glut.h>

template <bool> struct static_assert;
template <> struct static_assert<true> {};

struct OpenGLWrapper {}; // for ctags

///	Primary Templates

template< typename Type > inline void glRotate		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glScale		( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glScalev		( const Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTranslate	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTranslatev	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex2		( Type ,Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex3		( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex4		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex2v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glNormal3		( Type ,Type ,Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glNormal3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glIndex		( Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glIndexv		( Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor3		( Type ,Type ,Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor4		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor3v		( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor4v		( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord1	( Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord2	( Type ,Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord3	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord4	( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord1v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord2v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos2	( Type ,Type  )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos3	( Type ,Type , Type  )		{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos4	( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos2v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos3v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos4v	( const Type )			{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRect		( Type ,Type ,Type , Type  )	{	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glMaterial	( GLenum face, GLenum pname, Type param ){	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glMaterialv	( GLenum face, GLenum pname, Type param ){	static_assert<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };


///	Template Specializations


template< > inline void glRotate< double >			(double angle,double x,double y, double z )	{	glRotated(angle,x,y,z);	};
template< > inline void glRotate< float >			(float angle,float x,float y, float z )	{	glRotatef(angle,x,y,z);	};

template< > inline void glScale< double >			( double x,double y, double z )		{	glScaled(x,y,z);	};
template< > inline void glScale< float >			( float x,float y,float z )		{	glScalef(x,y,z);	};
template< > inline void glScalev< Vector3<double> >		( const Vector3<double> v )		{	glScaled(v[0],v[1],v[2]);};
template< > inline void glScalev< Vector3<float> >		( const Vector3<float> v )		{	glScalef(v[0],v[1],v[2]);};

template< > inline void glTranslate< double >			( double x,double y, double z )		{	glTranslated(x,y,z);	};
template< > inline void glTranslate< float >			( float x,float y,float z )		{	glTranslatef(x,y,z);	};
template< > inline void glTranslatev< Vector3<double> >		( const Vector3<double> v )		{	glTranslated(v[0],v[1],v[2]);};
template< > inline void glTranslatev< Vector3<float> >		( const Vector3<float> v )		{	glTranslatef(v[0],v[1],v[2]);};

template< > inline void glVertex2< double >			( double x,double y )			{	glVertex2d(x,y);	};
template< > inline void glVertex2< float >			( float x,float y )			{	glVertex2f(x,y);	};
template< > inline void glVertex2< int >			( int x,int y )				{	glVertex2i(x,y);	};
template< > inline void glVertex2< short >			( short x,short y )			{	glVertex2s(x,y);	};

template< > inline void glVertex3< double >			( double x,double y, double z )		{	glVertex3d(x,y,z);	};
template< > inline void glVertex3< float >			( float x,float y,float z )		{	glVertex3f(x,y,z);	};
template< > inline void glVertex3< int >			( int x, int y, int z )			{	glVertex3i(x,y,z);	};
template< > inline void glVertex3< short >			( short x, short y, short z )		{	glVertex3s(x,y,z);	};

template< > inline void glVertex4< double >			( double x,double y,double z, double w ){	glVertex4d(x,y,z,w);	};
template< > inline void glVertex4< float >			( float x,float y,float z, float w )	{	glVertex4f(x,y,z,w);	};
template< > inline void glVertex4< int >			( int x,int y,int z, int w )		{	glVertex4i(x,y,z,w);	};
template< > inline void glVertex4< short >			( short x,short y,short z,short w )	{	glVertex4s(x,y,z,w);	};

// :%s/\(void \)\(gl[A-Z,a-z,0-9]\+\)\(f\)( GLfloat \([a-z]\), GLfloat \([a-z]\), GLfloat \([a-z]\) );/template< > inline \1\2< float >			( float \4,float \5,float \6 )	{	\2\3(\4,\5,\6);	};/
template< > inline void glVertex2v< Vector3<double> >		( const Vector3<double> v )		{	glVertex2dv(v);		};
template< > inline void glVertex2v< Vector3<float> >		( const Vector3<float> v )		{	glVertex2fv(v);		};
template< > inline void glVertex2v< Vector3<int> >		( const Vector3<int> v )		{	glVertex2iv(v);		};
template< > inline void glVertex2v< Vector3<short> >		( const Vector3<short> v )		{	glVertex2sv(v);		};

template< > inline void glVertex3v< Vector3<double> >		( const Vector3<double> v )		{	glVertex3dv(v);		};
template< > inline void glVertex3v< Vector3<float> >		( const Vector3<float> v )		{	glVertex3fv(v);		};
template< > inline void glVertex3v< Vector3<int> >		( const Vector3<int> v )		{	glVertex3iv(v);		};
template< > inline void glVertex3v< Vector3<short> >		( const Vector3<short> v )		{	glVertex3sv(v);		};

template< > inline void glVertex4v< Vector3<double> >		( const Vector3<double> v )		{	glVertex4dv(v);		};
template< > inline void glVertex4v< Vector3<float> >		( const Vector3<float> v )		{	glVertex4fv(v);		};
template< > inline void glVertex4v< Vector3<int> >		( const Vector3<int> v )		{	glVertex4iv(v);		};
template< > inline void glVertex4v< Vector3<short> >		( const Vector3<short> v )		{	glVertex4sv(v);		};

template< > inline void glNormal3< signed char >		(signed char nx,signed char ny,signed char nz )	{	glNormal3b(nx,ny,nz);	};
template< > inline void glNormal3< double >			(double nx,double ny,double nz )	{	glNormal3d(nx,ny,nz);	};
template< > inline void glNormal3< float >			(float nx,float ny,float nz )		{	glNormal3f(nx,ny,nz);	};
template< > inline void glNormal3< int >			(int nx,int ny,int nz )			{	glNormal3i(nx,ny,nz);	};
template< > inline void glNormal3< short >			(short nx,short ny,short nz )		{	glNormal3s(nx,ny,nz);	};

template< > inline void glNormal3v< Vector3<signed char> >	( const Vector3<signed char> v )	{	glNormal3bv(v);		};
template< > inline void glNormal3v< Vector3<double> >		( const Vector3<double> v )		{	glNormal3dv(v);		};
template< > inline void glNormal3v< Vector3<float> >		( const Vector3<float> v )		{	glNormal3fv(v);		};
template< > inline void glNormal3v< Vector3<int> >		( const Vector3<int> v )		{	glNormal3iv(v);		};
template< > inline void glNormal3v< Vector3<short> >		( const Vector3<short> v )		{	glNormal3sv(v);		};

template< > inline void glIndex< double >			( double c )				{	glIndexd(c);	};
template< > inline void glIndex< float >			( float c )				{	glIndexf(c);	};
template< > inline void glIndex< int >				( int c )				{	glIndexi(c);	};
template< > inline void glIndex< short >			( short c )				{	glIndexs(c);	};
template< > inline void glIndex< unsigned char >		( unsigned char c )			{	glIndexub(c);	};

template< > inline void glIndexv<const Vector3<double> >	( const Vector3<double> c)		{	glIndexdv(c);	}
template< > inline void glIndexv<const Vector3<float> >	( const Vector3<float> c)		{	glIndexfv(c);	}
template< > inline void glIndexv<const Vector3<int> >		( const Vector3<int> c)			{	glIndexiv(c);	}
template< > inline void glIndexv<const Vector3<short> >	( const Vector3<short> c)		{	glIndexsv(c);	}
template< > inline void glIndexv<const Vector3<unsigned char> >( const Vector3<unsigned char> c)	{	glIndexubv(c);	}

template< > inline void glColor3< signed char >		(signed char red,signed char green,signed char blue )					{	glColor3b(red,green,blue);	};
template< > inline void glColor3< double >			(double red,double green,double blue )							{	glColor3d(red,green,blue);	};
template< > inline void glColor3< float >			(float red,float green,float blue )							{	glColor3f(red,green,blue);	};
template< > inline void glColor3< int >			(int red,int green,int blue )								{	glColor3i(red,green,blue);	};
template< > inline void glColor3< short >			(short red,short green,short blue )							{	glColor3s(red,green,blue);	};
template< > inline void glColor3< unsigned char >		(unsigned char red,unsigned char green,unsigned char blue )				{	glColor3ub(red,green,blue);	};
template< > inline void glColor3< unsigned int >		(unsigned int red,unsigned int green,unsigned int blue )				{	glColor3ui(red,green,blue);	};
template< > inline void glColor3< unsigned short >		(unsigned short red,unsigned short green,unsigned short blue )				{	glColor3us(red,green,blue);	};

template< > inline void glColor4< signed char >		(signed char red,signed char green,signed char blue, signed char alpha )		{	glColor4b(red,green,blue,alpha);	};
template< > inline void glColor4< double >			(double red,double green,double blue, double alpha )					{	glColor4d(red,green,blue,alpha);	};
template< > inline void glColor4< float >			(float red,float green,float blue, float alpha )					{	glColor4f(red,green,blue,alpha);	};
template< > inline void glColor4< int >			(int red,int green,int blue, int alpha )						{	glColor4i(red,green,blue,alpha);	};
template< > inline void glColor4< short >			(short red,short green,short blue, short alpha )					{	glColor4s(red,green,blue,alpha);	};
template< > inline void glColor4< unsigned char >		(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha )		{	glColor4ub(red,green,blue,alpha);	};
template< > inline void glColor4< unsigned int >		(unsigned int red,unsigned int green,unsigned int blue,unsigned int alpha )		{	glColor4ui(red,green,blue,alpha);	};
template< > inline void glColor4< unsigned short >		(unsigned short red,unsigned short green,unsigned short blue,unsigned short alpha )	{	glColor4us(red,green,blue,alpha);	};


template< > inline void glColor3v< Vector3<signed char> >	( const Vector3<signed char> v )	{	glColor3bv(v);		};
template< > inline void glColor3v< Vector3<double> >		( const Vector3<double> v )		{	glColor3dv(v);		};
template< > inline void glColor3v< Vector3<float> >		( const Vector3<float> v )		{	glColor3fv(v);		};
template< > inline void glColor3v< Vector3<int> >		( const Vector3<int> v )		{	glColor3iv(v);		};
template< > inline void glColor3v< Vector3<short> >		( const Vector3<short> v )		{	glColor3sv(v);		};
template< > inline void glColor3v< Vector3<unsigned char> >	( const Vector3<unsigned char> v )	{	glColor3ubv(v);		};
template< > inline void glColor3v< Vector3<unsigned int> >	( const Vector3<unsigned int> v )	{	glColor3uiv(v);		};
template< > inline void glColor3v< Vector3<unsigned short> >	( const Vector3<unsigned short> v )	{	glColor3usv(v);		};

template< > inline void glColor4v< Vector3<signed char> >	( const Vector3<signed char> v )	{	glColor4bv(v);		};
template< > inline void glColor4v< Vector3<double> >		( const Vector3<double> v )		{	glColor4dv(v);		};
template< > inline void glColor4v< Vector3<float> >		( const Vector3<float> v )		{	glColor4fv(v);		};
template< > inline void glColor4v< Vector3<int> >		( const Vector3<int> v )		{	glColor4iv(v);		};
template< > inline void glColor4v< Vector3<short> >		( const Vector3<short> v )		{	glColor4sv(v);		};
template< > inline void glColor4v< Vector3<unsigned char> >	( const Vector3<unsigned char> v )	{	glColor4ubv(v);		};
template< > inline void glColor4v< Vector3<unsigned int> >	( const Vector3<unsigned int> v )	{	glColor4uiv(v);		};
template< > inline void glColor4v< Vector3<unsigned short> >	( const Vector3<unsigned short> v )	{	glColor4usv(v);		};


template< > inline void glTexCoord1< double >			( double s )				{	glTexCoord1d(s);	};
template< > inline void glTexCoord1< float >			( float s )				{	glTexCoord1f(s);	};
template< > inline void glTexCoord1< int >			( int s )				{	glTexCoord1i(s);	};
template< > inline void glTexCoord1< short >			( short s )				{	glTexCoord1s(s);	};

template< > inline void glTexCoord2< double >			( double s,double t )			{	glTexCoord2d(s,t);	};
template< > inline void glTexCoord2< float >			( float s,float t )			{	glTexCoord2f(s,t);	};
template< > inline void glTexCoord2< int >			( int s,int t )				{	glTexCoord2i(s,t);	};
template< > inline void glTexCoord2< short >			( short s,short t )			{	glTexCoord2s(s,t);	};

template< > inline void glTexCoord3< double >			( double s,double t, double r )		{	glTexCoord3d(s,t,r);	};
template< > inline void glTexCoord3< float >			( float s,float t,float r )		{	glTexCoord3f(s,t,r);	};
template< > inline void glTexCoord3< int >			( int s, int t, int r )			{	glTexCoord3i(s,t,r);	};
template< > inline void glTexCoord3< short >			( short s, short t, short r )		{	glTexCoord3s(s,t,r);	};

template< > inline void glTexCoord4< double >			(double s,double t,double r, double q )	{	glTexCoord4d(s,t,r,q);	};
template< > inline void glTexCoord4< float >			(float s,float t,float r, float q )	{	glTexCoord4f(s,t,r,q);	};
template< > inline void glTexCoord4< int >			(int s,int t,int r, int q )		{	glTexCoord4i(s,t,r,q);	};
template< > inline void glTexCoord4< short >			(short s,short t,short r,short q )	{	glTexCoord4s(s,t,r,q);	};

template< > inline void glTexCoord1v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord1dv(v);	};
template< > inline void glTexCoord1v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord1fv(v);	};
template< > inline void glTexCoord1v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord1iv(v);	};
template< > inline void glTexCoord1v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord1sv(v);	};

template< > inline void glTexCoord2v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord2dv(v);	};
template< > inline void glTexCoord2v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord2fv(v);	};
template< > inline void glTexCoord2v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord2iv(v);	};
template< > inline void glTexCoord2v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord2sv(v);	};

template< > inline void glTexCoord3v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord3dv(v);	};
template< > inline void glTexCoord3v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord3fv(v);	};
template< > inline void glTexCoord3v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord3iv(v);	};
template< > inline void glTexCoord3v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord3sv(v);	};

template< > inline void glTexCoord4v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord4dv(v);	};
template< > inline void glTexCoord4v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord4fv(v);	};
template< > inline void glTexCoord4v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord4iv(v);	};
template< > inline void glTexCoord4v< Vector3<short> >		( const Vector3<short> v )		{	glTexCoord4sv(v);	};


template< > inline void glRasterPos2< double >			( double x,double y )			{	glRasterPos2d(x,y);	};
template< > inline void glRasterPos2< float >			( float x,float y )			{	glRasterPos2f(x,y);	};
template< > inline void glRasterPos2< int >			( int x,int y )				{	glRasterPos2i(x,y);	};
template< > inline void glRasterPos2< short >			( short x,short y )			{	glRasterPos2s(x,y);	};

template< > inline void glRasterPos3< double >			( double x,double y, double z )		{	glRasterPos3d(x,y,z);	};
template< > inline void glRasterPos3< float >			( float x,float y,float z )		{	glRasterPos3f(x,y,z);	};
template< > inline void glRasterPos3< int >			( int x, int y, int z )			{	glRasterPos3i(x,y,z);	};
template< > inline void glRasterPos3< short >			( short x, short y, short z )		{	glRasterPos3s(x,y,z);	};

template< > inline void glRasterPos4< double >			(double x,double y,double z, double w )	{	glRasterPos4d(x,y,z,w);	};
template< > inline void glRasterPos4< float >			(float x,float y,float z, float w )	{	glRasterPos4f(x,y,z,w);	};
template< > inline void glRasterPos4< int >			(int x,int y,int z, int w )		{	glRasterPos4i(x,y,z,w);	};
template< > inline void glRasterPos4< short >			(short x,short y,short z,short w )	{	glRasterPos4s(x,y,z,w);	};

template< > inline void glRasterPos2v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos2dv(v);	};
template< > inline void glRasterPos2v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos2fv(v);	};
template< > inline void glRasterPos2v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos2iv(v);	};
template< > inline void glRasterPos2v< Vector3<short> >	( const Vector3<short> v )		{	glRasterPos2sv(v);	};


// :%s/\(void \)\(gl[A-Z,a-z,0-9]\+\)\(us\)\(v\)( const GLushort \*\(v\) );/template< > inline \1\2\4< Vector3<unsigned short> >	( const Vector3<unsigned short> \5 )	{	\2\3\4(\5);		};/
template< > inline void glRasterPos3v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos3dv(v);		};
template< > inline void glRasterPos3v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos3fv(v);		};
template< > inline void glRasterPos3v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos3iv(v);		};
template< > inline void glRasterPos3v< Vector3<short> >	( const Vector3<short> v )		{	glRasterPos3sv(v);		};

template< > inline void glRasterPos4v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos4dv(v);		};
template< > inline void glRasterPos4v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos4fv(v);		};
template< > inline void glRasterPos4v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos4iv(v);		};
template< > inline void glRasterPos4v< Vector3<short> >	( const Vector3<short> v )		{	glRasterPos4sv(v);		};


template< > inline void glRect< double >			(double x1,double y1,double x2, double y2 )	{	glRectd(x1,y1,x2,y2);	};
template< > inline void glRect< float >			(float	x1,float  y1,float x2,float y2 )	{	glRectf(x1,y1,x2,y2);	};
template< > inline void glRect< int >				(int	x1,int	  y1,int x2, int y2 )		{	glRecti(x1,y1,x2,y2);	};
template< > inline void glRect< short >			(short	x1,short  y1,short x2,short y2 )	{	glRects(x1,y1,x2,y2);	};

template< > inline void glMaterial< float >			( GLenum face, GLenum pname, float param )			{	glMaterialf(face,pname,param);		};
template< > inline void glMaterial< int >			( GLenum face, GLenum pname, int param )			{	glMateriali(face,pname,param);		};
template< > inline void glMaterialv< Vector3<float> >		( GLenum face, GLenum pname, const Vector3<float> params )	{	glMaterialfv(face,pname,params);	};
template< > inline void glMaterialv< Vector3<int> >		( GLenum face, GLenum pname, const Vector3<int> params )	{	glMaterialiv(face,pname,params);	};


///	Functions Not Coverted

/*
void glRectdv( const GLdouble *v1, const GLdouble *v2 );
void glRectfv( const GLfloat *v1, const GLfloat *v2 );
void glRectiv( const GLint *v1, const GLint *v2 );
void glRectsv( const GLshort *v1, const GLshort *v2 );

void glLightf( GLenum light, GLenum pname, GLfloat param );
void glLighti( GLenum light, GLenum pname, GLint param );
void glLightfv( GLenum light, GLenum pname, const GLfloat *params );
void glLightiv( GLenum light, GLenum pname, const GLint *params );

void glGetLightfv( GLenum light, GLenum pname, GLfloat *params );
void glGetLightiv( GLenum light, GLenum pname, GLint *params );

void glLightModelf( GLenum pname, GLfloat param );
void glLightModeli( GLenum pname, GLint param );
void glLightModelfv( GLenum pname, const GLfloat *params );
void glLightModeliv( GLenum pname, const GLint *params );

void glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );
void glGetMaterialiv( GLenum face, GLenum pname, GLint *params );

void glPixelStoref( GLenum pname, GLfloat param );
void glPixelStorei( GLenum pname, GLint param );

void glPixelTransferf( GLenum pname, GLfloat param );
void glPixelTransferi( GLenum pname, GLint param );

void glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values );
void glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values );
void glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values );

void glGetPixelMapfv( GLenum map, GLfloat *values );
void glGetPixelMapuiv( GLenum map, GLuint *values );
void glGetPixelMapusv( GLenum map, GLushort *values );

void glTexGend( GLenum coord, GLenum pname, GLdouble param );
void glTexGenf( GLenum coord, GLenum pname, GLfloat param );
void glTexGeni( GLenum coord, GLenum pname, GLint param );

void glTexGendv( GLenum coord, GLenum pname, const GLdouble *params );
void glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
void glTexGeniv( GLenum coord, GLenum pname, const GLint *params );

void glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params );
void glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );
void glGetTexGeniv( GLenum coord, GLenum pname, GLint *params );

void glTexEnvf( GLenum target, GLenum pname, GLfloat param );
void glTexEnvi( GLenum target, GLenum pname, GLint param );

void glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
void glTexEnviv( GLenum target, GLenum pname, const GLint *params );

void glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
void glGetTexEnviv( GLenum target, GLenum pname, GLint *params );

void glTexParameterf( GLenum target, GLenum pname, GLfloat param );
void glTexParameteri( GLenum target, GLenum pname, GLint param );

void glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
void glTexParameteriv( GLenum target, GLenum pname, const GLint *params );

void glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params);
void glGetTexParameteriv( GLenum target, GLenum pname, GLint *params );

void glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
void glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );

void glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
void glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );

void glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
void glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );

void glGetMapdv( GLenum target, GLenum query, GLdouble *v );
void glGetMapfv( GLenum target, GLenum query, GLfloat *v );
void glGetMapiv( GLenum target, GLenum query, GLint *v );

template< > inline void glEvalCoord1< double >			( double u )	{	glEvalCoord1d(u);	};
template< > inline void glEvalCoord1< float >			( float u )	{	glEvalCoord1f(u);	};

void glEvalCoord1dv( const GLdouble *u );
void glEvalCoord1fv( const GLfloat *u );

// :%s/\(void \)\(gl[A-Z,a-z,0-9]\+\)\(f\)( GLfloat \([a-z]\), GLfloat \([a-z]\) );/template< > inline \1\2< float >			( float \4,float \5 )	{	\2\3(\4,\5);	};/

template< > inline void glEvalCoord2< double >			( double u,double v )	{	glEvalCoord2d(u,v);	};
template< > inline void glEvalCoord2< float >			( float u,float v )	{	glEvalCoord2f(u,v);	};

void glEvalCoord2dv( const GLdouble *u );
void glEvalCoord2fv( const GLfloat *u );

void glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 );
void glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 );

void glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
void glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );


void glFogf( GLenum pname, GLfloat param );
void glFogi( GLenum pname, GLint param );

void glFogfv( GLenum pname, const GLfloat *params );
void glFogiv( GLenum pname, const GLint *params );


void glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params);
void glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params);
void glGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params );
void glGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params );
void glGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params );
void glGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params );
void glGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params );
void glGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params );
void glConvolutionParameterf( GLenum target, GLenum pname, GLfloat params );
void glConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params );
void glConvolutionParameteri( GLenum target, GLenum pname, GLint params );
void glConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params );
void glGetConvolutionParameterfv( GLenum target, GLenum pname,	GLfloat *params );
void glGetConvolutionParameteriv( GLenum target, GLenum pname,	GLint *params );

void glMultiTexCoord1d( GLenum target, GLdouble s );
void glMultiTexCoord1dv( GLenum target, const GLdouble *v );
void glMultiTexCoord1f( GLenum target, GLfloat s );
void glMultiTexCoord1fv( GLenum target, const GLfloat *v );
void glMultiTexCoord1i( GLenum target, GLint s );
void glMultiTexCoord1iv( GLenum target, const GLint *v );
void glMultiTexCoord1s( GLenum target, GLshort s );
void glMultiTexCoord1sv( GLenum target, const GLshort *v );

void glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t );
void glMultiTexCoord2dv( GLenum target, const GLdouble *v );
void glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t );
void glMultiTexCoord2fv( GLenum target, const GLfloat *v );
void glMultiTexCoord2i( GLenum target, GLint s, GLint t );
void glMultiTexCoord2iv( GLenum target, const GLint *v );
void glMultiTexCoord2s( GLenum target, GLshort s, GLshort t );
void glMultiTexCoord2sv( GLenum target, const GLshort *v );

void glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r );
void glMultiTexCoord3dv( GLenum target, const GLdouble *v );
void glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r );
void glMultiTexCoord3fv( GLenum target, const GLfloat *v );
void glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r );
void glMultiTexCoord3iv( GLenum target, const GLint *v );
void glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r );
void glMultiTexCoord3sv( GLenum target, const GLshort *v );

void glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );
void glMultiTexCoord4dv( GLenum target, const GLdouble *v );
void glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
void glMultiTexCoord4fv( GLenum target, const GLfloat *v );
void glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q );
void glMultiTexCoord4iv( GLenum target, const GLint *v );
void glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );
void glMultiTexCoord4sv( GLenum target, const GLshort *v );


void glLoadTransposeMatrixd( const GLdouble m[16] );
void glLoadTransposeMatrixf( const GLfloat m[16] );
void glMultTransposeMatrixd( const GLdouble m[16] );
void glMultTransposeMatrixf( const GLfloat m[16] );

void glMultiTexCoord1dARB(GLenum target, GLdouble s);
void glMultiTexCoord1dvARB(GLenum target, const GLdouble *v);
void glMultiTexCoord1fARB(GLenum target, GLfloat s);
void glMultiTexCoord1fvARB(GLenum target, const GLfloat *v);
void glMultiTexCoord1iARB(GLenum target, GLint s);
void glMultiTexCoord1ivARB(GLenum target, const GLint *v);
void glMultiTexCoord1sARB(GLenum target, GLshort s);
void glMultiTexCoord1svARB(GLenum target, const GLshort *v);
void glMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t);
void glMultiTexCoord2dvARB(GLenum target, const GLdouble *v);
void glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);
void glMultiTexCoord2fvARB(GLenum target, const GLfloat *v);
void glMultiTexCoord2iARB(GLenum target, GLint s, GLint t);
void glMultiTexCoord2ivARB(GLenum target, const GLint *v);
void glMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t);
void glMultiTexCoord2svARB(GLenum target, const GLshort *v);
void glMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r);
void glMultiTexCoord3dvARB(GLenum target, const GLdouble *v);
void glMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r);
void glMultiTexCoord3fvARB(GLenum target, const GLfloat *v);
void glMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r);
void glMultiTexCoord3ivARB(GLenum target, const GLint *v);
void glMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r);
void glMultiTexCoord3svARB(GLenum target, const GLshort *v);
void glMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
void glMultiTexCoord4dvARB(GLenum target, const GLdouble *v);
void glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void glMultiTexCoord4fvARB(GLenum target, const GLfloat *v);
void glMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q);
void glMultiTexCoord4ivARB(GLenum target, const GLint *v);
void glMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
void glMultiTexCoord4svARB(GLenum target, const GLshort *v);

void glPointParameterfEXT( GLenum pname, GLfloat param );
void glPointParameterfvEXT( GLenum pname, const GLfloat *params );
void glPointParameterfSGIS(GLenum pname, GLfloat param);
void glPointParameterfvSGIS(GLenum pname, const GLfloat *params);

void glWindowPos2iMESA( GLint x, GLint y );
void glWindowPos2sMESA( GLshort x, GLshort y );
void glWindowPos2fMESA( GLfloat x, GLfloat y );
void glWindowPos2dMESA( GLdouble x, GLdouble y );
void glWindowPos2ivMESA( const GLint *p );
void glWindowPos2svMESA( const GLshort *p );
void glWindowPos2fvMESA( const GLfloat *p );
void glWindowPos2dvMESA( const GLdouble *p );
void glWindowPos3iMESA( GLint x, GLint y, GLint z );
void glWindowPos3sMESA( GLshort x, GLshort y, GLshort z );
void glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z );
void glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z );
void glWindowPos3ivMESA( const GLint *p );
void glWindowPos3svMESA( const GLshort *p );
void glWindowPos3fvMESA( const GLfloat *p );
void glWindowPos3dvMESA( const GLdouble *p );
void glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w );
void glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w );
void glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
void glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void glWindowPos4ivMESA( const GLint *p );
void glWindowPos4svMESA( const GLshort *p );
void glWindowPos4fvMESA( const GLfloat *p );
void glWindowPos4dvMESA( const GLdouble *p );
*/

#endif // OPENGLWRAPPER_HPP

