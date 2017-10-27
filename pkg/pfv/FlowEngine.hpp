#pragma once

/// Frequently used:
// typedef CGT::CVector CVector;
// typedef CGT::Point Point;
/*
/// Converters for Eigen and CGAL vectors
inline CVector makeCgVect ( const Vector3r& yv ) {return CVector ( yv[0],yv[1],yv[2] );}
inline Point makeCgPoint ( const Vector3r& yv ) {return Point ( yv[0],yv[1],yv[2] );}
inline Vector3r makeVector3r ( const Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
inline Vector3r makeVector3r ( const CVector& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}*/

/// The following macros can be used to expose CellInfo members.
/// The syntax is CELL_SCALAR_GETTER(double,.p(),pressure), note the "." before member name, data members would be without the "()" 
#define CELL_SCALAR_GETTER(type, param, getterName) \
type getterName(unsigned int id){\
			if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return 0;}\
			return solver->T[solver->currentTes].cellHandles[id]->info()param;}
			
#define CELL_SCALAR_SETTER_EXTRA(type, param, setterName, extra) \
void setterName(unsigned int id, type value){\
			if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return;}\
			solver->T[solver->currentTes].cellHandles[id]->info()param=value;\
			extra;}
			
#define CELL_SCALAR_SETTER(type, param, setterName) CELL_SCALAR_SETTER_EXTRA(type, param, setterName,return)
			
#define CELL_VECTOR_GETTER(param, getterName) \
Vector3r getterName(unsigned int id){\
			if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return Vector3r(0,0,0);}\
			return makeVector3r(solver->T[solver->currentTes].cellHandles[id]->info()param);}

#ifdef LINSOLV
#define DEFAULTSOLVER CGT::FlowBoundingSphereLinSolv<_Tesselation>
#else
#define DEFAULTSOLVER CGT::FlowBoundingSphere<_Tesselation>
#endif
