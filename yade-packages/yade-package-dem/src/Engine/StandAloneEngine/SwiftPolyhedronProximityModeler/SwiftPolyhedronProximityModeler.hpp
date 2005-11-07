/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SWIFTPOLYHEDRONPROXIMITYMODELER_HPP
#define SWIFTPOLYHEDRONPROXIMITYMODELER_HPP

#include "PolyhedralSweptSphere.hpp"
#include <yade/yade-core/Engine.hpp>
#include <yade/yade-lib-swiftpp/SWIFT.h>

class SwiftPolyhedronProximityModeler : public Engine
{
	private :
		SWIFT_Scene*	 scene;
		bool		 first;
		vector<int>	 ids;
		double *	 vs;
		int *		 fs;
		int *		 fv;
		int		 vn
				,fn;
		double		 R[9];
		double		 T[3];

		void getSwiftInfo(const PolyhedralSweptSphere* pss, double *& v, int *& f,  int*& fv,int& nbVertices, int& nbFaces);
	
	public :
		SwiftPolyhedronProximityModeler();
		virtual ~SwiftPolyhedronProximityModeler();
		virtual void action(Body* body);
	
	REGISTER_CLASS_NAME(SwiftPolyhedronProximityModeler);
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(SwiftPolyhedronProximityModeler,false);

#endif // SWIFTPOLYHEDRONPROXIMITYMODELER_HPP

