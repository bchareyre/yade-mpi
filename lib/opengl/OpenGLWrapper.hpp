/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#ifndef YADE_OPENGL
#error "This build doesn't support openGL. Therefore, this header must not be used."
#endif

#include<Wm3Vector3.h>
#include<yade/lib-base/Math.hpp>

#include<GL/gl.h>
#include<GL/glut.h>

template <bool> struct static_assert_;
template <> struct static_assert_<true> {};

struct OpenGLWrapper {}; // for ctags

///	Primary Templates

template< typename Type > inline void glRotate		( Type ,Type ,Type , Type  )	{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glScale		( Type ,Type , Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glScalev		( const Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTranslate	( Type ,Type , Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTranslatev	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex2		( Type ,Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex3		( Type ,Type , Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex4		( Type ,Type ,Type , Type  )	{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex2v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex3v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glVertex4v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glNormal3		( Type ,Type ,Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glNormal3v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glIndex		( Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glIndexv		( Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor3		( Type ,Type ,Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor4		( Type ,Type ,Type , Type  )	{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor3v		( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glColor4v		( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord1	( Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord2	( Type ,Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord3	( Type ,Type , Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord4	( Type ,Type ,Type , Type  )	{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord1v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord2v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord3v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glTexCoord4v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos2	( Type ,Type  )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos3	( Type ,Type , Type  )		{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos4	( Type ,Type ,Type , Type  )	{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos2v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos3v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRasterPos4v	( const Type )			{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glRect		( Type ,Type ,Type , Type  )	{	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glMaterial	( GLenum face, GLenum pname, Type param ){	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glMaterialv	( GLenum face, GLenum pname, Type param ){	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };
template< typename Type > inline void glMultMatrix	(const Type*){	static_assert_<false> GL_OpenGLWrapper_bad_type;(void) GL_OpenGLWrapper_bad_type; };


///	Template Specializations
template< > inline void glMultMatrix<double>(const double* m){glMultMatrixd(m);	};
template< > inline void glMultMatrix<long double>(const long double* m){double mm[16]; for(int i=0;i<16;i++)mm[i]=(double)m[i]; glMultMatrixd(mm);};

template< > inline void glRotate< double >			(double angle,double x,double y, double z )	{	glRotated(angle,x,y,z);	};
template< > inline void glRotate< float >			(float angle,float x,float y, float z )	{	glRotatef(angle,x,y,z);	};

template< > inline void glScale< double >			( double x,double y, double z )		{	glScaled(x,y,z);	};
template< > inline void glScale< long double >			( long double x,long double y, long double z )		{	glScaled(x,y,z);	};
template< > inline void glScale< float >			( float x,float y,float z )		{	glScalef(x,y,z);	};
template< > inline void glScalev< Vector3<long double> >		( const Vector3<long double> v )		{	glScaled(v[0],v[1],v[2]);};
template< > inline void glScalev< Vector3<double> >		( const Vector3<double> v )		{	glScaled(v[0],v[1],v[2]);};
template< > inline void glScalev< Vector3<float> >		( const Vector3<float> v )		{	glScalef(v[0],v[1],v[2]);};

template< > inline void glTranslate< double >			( double x,double y, double z )		{	glTranslated(x,y,z);	};
template< > inline void glTranslate< long double >			( long double x, long double y, long double z )		{	glTranslated(x,y,z);	};
template< > inline void glTranslate< float >			( float x,float y,float z )		{	glTranslatef(x,y,z);	};
template< > inline void glTranslatev< Vector3<long double> >		( const Vector3<long double> v )		{	glTranslated(v[0],v[1],v[2]);};
template< > inline void glTranslatev< Vector3<double> >		( const Vector3<double> v )		{	glTranslated(v[0],v[1],v[2]);};
template< > inline void glTranslatev< Vector3<float> >		( const Vector3<float> v )		{	glTranslatef(v[0],v[1],v[2]);};

template< > inline void glVertex2< double >			( double x,double y )			{	glVertex2d(x,y);	};
template< > inline void glVertex2< float >			( float x,float y )			{	glVertex2f(x,y);	};
template< > inline void glVertex2< int >			( int x,int y )				{	glVertex2i(x,y);	};

template< > inline void glVertex3< double >			( double x,double y, double z )		{	glVertex3d(x,y,z);	};
template< > inline void glVertex3< float >			( float x,float y,float z )		{	glVertex3f(x,y,z);	};
template< > inline void glVertex3< int >			( int x, int y, int z )			{	glVertex3i(x,y,z);	};

template< > inline void glVertex4< double >			( double x,double y,double z, double w ){	glVertex4d(x,y,z,w);	};
template< > inline void glVertex4< float >			( float x,float y,float z, float w )	{	glVertex4f(x,y,z,w);	};
template< > inline void glVertex4< int >			( int x,int y,int z, int w )		{	glVertex4i(x,y,z,w);	};

// :%s/\(void \)\(gl[A-Z,a-z,0-9]\+\)\(f\)( GLfloat \([a-z]\), GLfloat \([a-z]\), GLfloat \([a-z]\) );/template< > inline \1\2< float >			( float \4,float \5,float \6 )	{	\2\3(\4,\5,\6);	};/
template< > inline void glVertex2v< Vector3<double> >		( const Vector3<double> v )		{	glVertex2dv((double*)&v);		};
template< > inline void glVertex2v< Vector3<float> >		( const Vector3<float> v )		{	glVertex2fv((float*)&v);		};
template< > inline void glVertex2v< Vector3<int> >		( const Vector3<int> v )		{	glVertex2iv((int*)&v);		};

template< > inline void glVertex3v< Vector3<double> >		( const Vector3<double> v )		{	glVertex3dv((double*)&v);		};
template< > inline void glVertex3v< Vector3<float> >		( const Vector3<float> v )		{	glVertex3fv((float*)&v);		};
template< > inline void glVertex3v< Vector3<int> >		( const Vector3<int> v )		{	glVertex3iv((int*)&v);		};

template< > inline void glVertex4v< Vector3<double> >		( const Vector3<double> v )		{	glVertex4dv((double*)&v);		};
template< > inline void glVertex4v< Vector3<float> >		( const Vector3<float> v )		{	glVertex4fv((float*)&v);		};
template< > inline void glVertex4v< Vector3<int> >		( const Vector3<int> v )		{	glVertex4iv((int*)&v);		};

template< > inline void glNormal3< double >			(double nx,double ny,double nz )	{	glNormal3d(nx,ny,nz);	};
template< > inline void glNormal3< float >			(float nx,float ny,float nz )		{	glNormal3f(nx,ny,nz);	};
template< > inline void glNormal3< int >			(int nx,int ny,int nz )			{	glNormal3i(nx,ny,nz);	};

template< > inline void glNormal3v< Vector3<double> >		( const Vector3<double> v )		{	glNormal3dv((double*)&v);		};
template< > inline void glNormal3v< Vector3<float> >		( const Vector3<float> v )		{	glNormal3fv((float*)&v);		};
template< > inline void glNormal3v< Vector3<int> >		( const Vector3<int> v )		{	glNormal3iv((int*)&v);		};

template< > inline void glIndex< double >			( double c )				{	glIndexd(c);	};
template< > inline void glIndex< float >			( float c )				{	glIndexf(c);	};
template< > inline void glIndex< int >				( int c )				{	glIndexi(c);	};
template< > inline void glIndex< unsigned char >		( unsigned char c )			{	glIndexub(c);	};

template< > inline void glIndexv<const Vector3<double> >	( const Vector3<double> c)		{	glIndexdv((double*)&c);	}
template< > inline void glIndexv<const Vector3<float> >	( const Vector3<float> c)		{	glIndexfv((float*)&c);	}
template< > inline void glIndexv<const Vector3<int> >		( const Vector3<int> c)			{	glIndexiv((int*)&c);	}

template< > inline void glColor3< double >			(double red,double green,double blue )							{	glColor3d(red,green,blue);	};
template< > inline void glColor3< float >			(float red,float green,float blue )							{	glColor3f(red,green,blue);	};
template< > inline void glColor3< int >			(int red,int green,int blue )								{	glColor3i(red,green,blue);	};

template< > inline void glColor4< double >			(double red,double green,double blue, double alpha )					{	glColor4d(red,green,blue,alpha);	};
template< > inline void glColor4< float >			(float red,float green,float blue, float alpha )					{	glColor4f(red,green,blue,alpha);	};
template< > inline void glColor4< int >			(int red,int green,int blue, int alpha )						{	glColor4i(red,green,blue,alpha);	};


template< > inline void glColor3v< Vector3<double> >		( const Vector3<double> v )		{	glColor3dv((double*)&v);		};
template< > inline void glColor3v< Vector3<float> >		( const Vector3<float> v )		{	glColor3fv((float*)&v);		};
template< > inline void glColor3v< Vector3<int> >		( const Vector3<int> v )		{	glColor3iv((int*)&v);		};

template< > inline void glColor4v< Vector3<double> >		( const Vector3<double> v )		{	glColor4dv((double*)&v);		};
template< > inline void glColor4v< Vector3<float> >		( const Vector3<float> v )		{	glColor4fv((float*)&v);		};
template< > inline void glColor4v< Vector3<int> >		( const Vector3<int> v )		{	glColor4iv((int*)&v);		};


template< > inline void glTexCoord1< double >			( double s )				{	glTexCoord1d(s);	};
template< > inline void glTexCoord1< float >			( float s )				{	glTexCoord1f(s);	};
template< > inline void glTexCoord1< int >			( int s )				{	glTexCoord1i(s);	};

template< > inline void glTexCoord2< double >			( double s,double t )			{	glTexCoord2d(s,t);	};
template< > inline void glTexCoord2< float >			( float s,float t )			{	glTexCoord2f(s,t);	};
template< > inline void glTexCoord2< int >			( int s,int t )				{	glTexCoord2i(s,t);	};

template< > inline void glTexCoord3< double >			( double s,double t, double r )		{	glTexCoord3d(s,t,r);	};
template< > inline void glTexCoord3< float >			( float s,float t,float r )		{	glTexCoord3f(s,t,r);	};
template< > inline void glTexCoord3< int >			( int s, int t, int r )			{	glTexCoord3i(s,t,r);	};

template< > inline void glTexCoord4< double >			(double s,double t,double r, double q )	{	glTexCoord4d(s,t,r,q);	};
template< > inline void glTexCoord4< float >			(float s,float t,float r, float q )	{	glTexCoord4f(s,t,r,q);	};
template< > inline void glTexCoord4< int >			(int s,int t,int r, int q )		{	glTexCoord4i(s,t,r,q);	};

template< > inline void glTexCoord1v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord1dv((double*)&v);	};
template< > inline void glTexCoord1v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord1fv((float*)&v);	};
template< > inline void glTexCoord1v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord1iv((int*)&v);	};

template< > inline void glTexCoord2v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord2dv((double*)&v);	};
template< > inline void glTexCoord2v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord2fv((float*)&v);	};
template< > inline void glTexCoord2v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord2iv((int*)&v);	};

template< > inline void glTexCoord3v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord3dv((double*)&v);	};
template< > inline void glTexCoord3v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord3fv((float*)&v);	};
template< > inline void glTexCoord3v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord3iv((int*)&v);	};

template< > inline void glTexCoord4v< Vector3<double> >	( const Vector3<double> v )		{	glTexCoord4dv((double*)&v);	};
template< > inline void glTexCoord4v< Vector3<float> >		( const Vector3<float> v )		{	glTexCoord4fv((float*)&v);	};
template< > inline void glTexCoord4v< Vector3<int> >		( const Vector3<int> v )		{	glTexCoord4iv((int*)&v);	};


template< > inline void glRasterPos2< double >			( double x,double y )			{	glRasterPos2d(x,y);	};
template< > inline void glRasterPos2< float >			( float x,float y )			{	glRasterPos2f(x,y);	};
template< > inline void glRasterPos2< int >			( int x,int y )				{	glRasterPos2i(x,y);	};

template< > inline void glRasterPos3< double >			( double x,double y, double z )		{	glRasterPos3d(x,y,z);	};
template< > inline void glRasterPos3< float >			( float x,float y,float z )		{	glRasterPos3f(x,y,z);	};
template< > inline void glRasterPos3< int >			( int x, int y, int z )			{	glRasterPos3i(x,y,z);	};

template< > inline void glRasterPos4< double >			(double x,double y,double z, double w )	{	glRasterPos4d(x,y,z,w);	};
template< > inline void glRasterPos4< float >			(float x,float y,float z, float w )	{	glRasterPos4f(x,y,z,w);	};
template< > inline void glRasterPos4< int >			(int x,int y,int z, int w )		{	glRasterPos4i(x,y,z,w);	};

template< > inline void glRasterPos2v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos2dv((double*)&v);	};
template< > inline void glRasterPos2v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos2fv((float*)&v);	};
template< > inline void glRasterPos2v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos2iv((int*)&v);	};


// :%s/\(void \)\(gl[A-Z,a-z,0-9]\+\)\(us\)\(v\)( const GLushort \*\(v\) );/template< > inline \1\2\4< Vector3<unsigned short> >	( const Vector3<unsigned short> \5 )	{	\2\3\4(\5);		};/
template< > inline void glRasterPos3v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos3dv((double*)&v);		};
template< > inline void glRasterPos3v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos3fv((float*)&v);		};
template< > inline void glRasterPos3v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos3iv((int*)&v);		};

template< > inline void glRasterPos4v< Vector3<double> >	( const Vector3<double> v )		{	glRasterPos4dv((double*)&v);		};
template< > inline void glRasterPos4v< Vector3<float> >	( const Vector3<float> v )		{	glRasterPos4fv((float*)&v);		};
template< > inline void glRasterPos4v< Vector3<int> >		( const Vector3<int> v )		{	glRasterPos4iv((int*)&v);		};


template< > inline void glRect< double >			(double x1,double y1,double x2, double y2 )	{	glRectd(x1,y1,x2,y2);	};
template< > inline void glRect< float >			(float	x1,float  y1,float x2,float y2 )	{	glRectf(x1,y1,x2,y2);	};
template< > inline void glRect< int >				(int	x1,int	  y1,int x2, int y2 )		{	glRecti(x1,y1,x2,y2);	};

template< > inline void glMaterial< float >			( GLenum face, GLenum pname, float param )			{	glMaterialf(face,pname,param);		};
template< > inline void glMaterial< double >			( GLenum face, GLenum pname, double param )			{	glMaterialf(face,pname,param);		};
template< > inline void glMaterial< int >			( GLenum face, GLenum pname, int param )			{	glMateriali(face,pname,param);		};
template< > inline void glMaterialv< Vector3<double> >	( GLenum face, GLenum pname, const Vector3<double> params )	{	Vector3f _p(params[0],params[1],params[2]); glMaterialfv(face,pname,(float*)&_p);	};
template< > inline void glMaterialv< Vector3<float> >		( GLenum face, GLenum pname, const Vector3<float> params )	{	glMaterialfv(face,pname,(float*)&params);	};
template< > inline void glMaterialv< Vector3<int> >		( GLenum face, GLenum pname, const Vector3<int> params )	{	glMaterialiv(face,pname,(int*)&params);	};


