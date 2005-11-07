/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MESH2D_HPP
#define MESH2D_HPP

#include <yade/yade-core/GeometricalModel.hpp>

#define offset(i,j) ((i)*height+(j))

class Edge : public Serializable
{
	public :
		int	 first
			,second;

		Edge() {};
		Edge(int f,int s) :first(f), second(s) {};

	protected :
		virtual void postProcessAttributes(bool) {};
		void registerAttributes()
		{
			REGISTER_ATTRIBUTE(first);
			REGISTER_ATTRIBUTE(second);
		};
	REGISTER_CLASS_NAME(Edge);
	REGISTER_BASE_CLASS_NAME(Serializable);

};

REGISTER_SERIALIZABLE(Edge,true);

class Mesh2D : public GeometricalModel
{
	public :
		vector<Edge>		edges;

		vector<Vector3r>	 vertices
					,fNormals
					,vNormals;
		
		vector<vector<int> >	 faces
					,triPerVertices;

		Mesh2D ();
		virtual ~Mesh2D ();

/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(Mesh2D);
	REGISTER_BASE_CLASS_NAME(Serializable);

/// Indexable
	REGISTER_CLASS_INDEX(Mesh2D,GeometricalModel);
};

REGISTER_SERIALIZABLE(Mesh2D,false);

#endif // MESH2D_HPP

