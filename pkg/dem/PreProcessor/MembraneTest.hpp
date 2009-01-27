/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/FileGenerator.hpp>


class MembraneTest : public FileGenerator
{
	private :
		unsigned int	nbX,nbZ;
		Real		XLength,ZLength;
		Vector3r	gravity;

		Real		bigBallRadius,
				membraneThickness,
				tc,en,es,
				mu;

                void createSphere(shared_ptr<Body>& body, Vector3r position, Real radius,bool dynamic);
		void createNode(shared_ptr<Body>& body, unsigned int i, unsigned int j);
                void connectNodes(shared_ptr<Body>& body, unsigned int id1, unsigned int id2);
		void createActors(shared_ptr<MetaBody>& rootBody);
		void positionRootBody(shared_ptr<MetaBody>& rootBody);

	public :
		MembraneTest ();
		~MembraneTest ();
		bool generate();

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(MembraneTest);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(MembraneTest);


