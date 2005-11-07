/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FEM_SET_TEXT_LOADER_HPP
#define FEM_SET_TEXT_LOADER_HPP

#include <yade/yade-package-common/PhysicalParametersEngineUnit.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/MetaBody.hpp>

class FEMSetTextLoader : public PhysicalParametersEngineUnit
{
	private :
		int	 nodeGroupMask
			,tetrahedronGroupMask;

	public :
		string fileName; 

	public :
		virtual void go(	  const shared_ptr<PhysicalParameters>&
					, Body*);
	
		void createNode(	  shared_ptr<Body>& body
					, Vector3r position
					, unsigned int id);
					
		void createTetrahedron(   const MetaBody* rootBody
					, shared_ptr<Body>& body
					, unsigned int id
					, unsigned int id1
					, unsigned int id2
					, unsigned int id3
					, unsigned int id4);

	protected :
		virtual void registerAttributes();	
	REGISTER_CLASS_NAME(FEMSetTextLoader);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);

};

REGISTER_SERIALIZABLE(FEMSetTextLoader,false);

#endif // FEM_SET_TEXT_LOADER_HPP 

