/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <map>
#include <stdexcept>
#include <core/Body.hpp>
#include <lib/base/Logging.hpp>
#include <lib/base/Math.hpp>
#include <core/PartialEngine.hpp>
#include <pkg/dem/deformablecohesive/DeformableCohesiveElement.hpp>



class Lin4NodeTetra_Lin4NodeTetra_InteractionElement: public DeformableCohesiveElement {
	public:

		friend class If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat;
		virtual ~Lin4NodeTetra_Lin4NodeTetra_InteractionElement();
		void initialize(void);
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(Lin4NodeTetra_Lin4NodeTetra_InteractionElement,DeformableCohesiveElement,"Tetrahedral Deformable Element Composed of Nodes",
		,
		,
		createIndex(); /*ctor*/
		initialize();
		,
		/*py*/

	);
		DECLARE_LOGGER;

		REGISTER_CLASS_INDEX(Lin4NodeTetra_Lin4NodeTetra_InteractionElement,DeformableCohesiveElement);

};
// necessary
using namespace yade;
REGISTER_SERIALIZABLE(Lin4NodeTetra_Lin4NodeTetra_InteractionElement);
