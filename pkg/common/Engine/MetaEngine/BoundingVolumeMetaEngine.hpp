/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#ifndef BOUNDINGVOLUMEUPDATOR_HPP
#define BOUNDINGVOLUMEUPDATOR_HPP

#include "BoundingVolumeEngineUnit.hpp"

#include<yade/core/MetaEngine2D.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/BoundingVolume.hpp>
#include<yade/core/Body.hpp>

class BoundingVolumeMetaEngine :	public MetaEngine2D
					<	
						InteractingGeometry,						// base classe for dispatch
						BoundingVolume,							// second base classe for dispatch

						BoundingVolumeEngineUnit,					// class that provides multivirtual call

						void,								// return type
						TYPELIST_4(	  const shared_ptr<InteractingGeometry>&	// arguments
								, shared_ptr<BoundingVolume>&
								, const Se3r&
								, const Body* 					// with that - functors have all the data they may need
							  )
					>
{
	public :
		virtual void action(MetaBody*);

	REGISTER_CLASS_NAME(BoundingVolumeMetaEngine);
	REGISTER_BASE_CLASS_NAME(MetaEngine2D);
};

REGISTER_SERIALIZABLE(BoundingVolumeMetaEngine,false);

#endif // BOUNDINGVOLUMEUPDATOR_HPP

