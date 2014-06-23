#pragma once
/// Frequently used:
typedef CGT::CVector CVector;
typedef CGT::Point Point;

/// Converters for Eigen and CGAL vectors
inline CVector makeCgVect ( const Vector3r& yv ) {return CVector ( yv[0],yv[1],yv[2] );}
inline Point makeCgPoint ( const Vector3r& yv ) {return Point ( yv[0],yv[1],yv[2] );}
inline Vector3r makeVector3r ( const Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
inline Vector3r makeVector3r ( const CVector& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}

#ifdef LINSOLV
#define DEFAULTSOLVER CGT::FlowBoundingSphereLinSolv<_Tesselation>
#else
#define DEFAULTSOLVER CGT::FlowBoundingSphere<_Tesselation>
#endif
