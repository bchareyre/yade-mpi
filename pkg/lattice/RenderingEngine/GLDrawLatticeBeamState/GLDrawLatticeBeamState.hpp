/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_LATTICE_BEAM_STATE_HPP
#define GLDRAW_LATTICE_BEAM_STATE_HPP

#include<yade/pkg-common/GLDrawStateFunctor.hpp>

class GLDrawLatticeBeamState : public GLDrawStateFunctor
{
	public :
		virtual void go(const shared_ptr<PhysicalParameters>&);

	RENDERS(LatticeBeamParameters);
	REGISTER_CLASS_NAME(GLDrawLatticeBeamState);
	REGISTER_BASE_CLASS_NAME(GLDrawStateFunctor);

};

REGISTER_SERIALIZABLE(GLDrawLatticeBeamState,false);

#endif // GLDRAW_LINE_SEGMENT_HPP

