/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef POLYHEDRALSWEPTSPHERE_HPP
#define POLYHEDRALSWEPTSPHERE_HPP


#include <yade/yade-core/InteractingGeometry.hpp>


class PolyhedralSweptSphere : public InteractingGeometry
{

	private : 
		struct ltPair
		{
			bool operator()(const pair<int,int>& p1, const pair<int,int>& p2)
			{
				
				return ((p1.first<p2.first) || (p1.first==p2.first && p1.second<p2.second));
			}
		};

	public :
		vector<Vector3r>	vertices;
		vector<vector<int> >	faces;
		vector<pair<int,int> >	edges;
		float			radius;
		Real			minEdge;
	
		PolyhedralSweptSphere();
		virtual ~PolyhedralSweptSphere();
		void computeEdges();

/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(PolyhedralSweptSphere);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

	REGISTER_CLASS_INDEX(PolyhedralSweptSphere,InteractingGeometry);
};

REGISTER_SERIALIZABLE(PolyhedralSweptSphere,false);

#endif // POLYHEDRALSWEPTSPHERE_HPP

