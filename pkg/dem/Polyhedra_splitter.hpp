// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a


#pragma once

#ifdef YADE_CGAL

#include <pkg/common/PeriodicEngines.hpp>
#include "Polyhedra.hpp"

//*********************************************************************************
/* Polyhedra Splitter */
class PolyhedraSplitter : public PeriodicEngine{

	public:
		virtual void action();
		double getStrength(const double & volume, const double & strength) const;
		void Symmetrize(Matrix3r & bStress);

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(PolyhedraSplitter,PeriodicEngine,"Engine that splits polyhedras.",
		,
		/*ctor*/
		,/*py*/
	);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(PolyhedraSplitter);

#endif // YADE_CGAL
