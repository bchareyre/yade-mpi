// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a


#pragma once

#ifdef YADE_CGAL
#include "Polyhedra.hpp"
#include <pkg/common/PeriodicEngines.hpp>


//*********************************************************************************
/* Polyhedra Splitter */
class PolyhedraSplitter : public PeriodicEngine{
	public:
		virtual void action();
		double getStrength(const double & volume, const double & strength) const;
		void Symmetrize(Matrix3r & bStress);

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(
		PolyhedraSplitter,PeriodicEngine,"Engine that splits polyhedras."
		,
		,
		/*ctor*/
		,/*py*/
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PolyhedraSplitter);

class SplitPolyTauMax : public PolyhedraSplitter{
	public:
		virtual void action();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(
		SplitPolyTauMax,PolyhedraSplitter,"Split polyhedra along TauMax."
		,
		,
		/*ctor*/
		,/*py*/
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(SplitPolyTauMax);

class SplitPolyMohrCoulomb : public PolyhedraSplitter{
	public:
		virtual void action();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(
		SplitPolyMohrCoulomb,PolyhedraSplitter,"Split polyhedra according to Mohr-Coulomb criterion."
		,
		((string,fileName,"",,"Base."))
		,
		/*ctor*/
		,/*py*/
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(SplitPolyMohrCoulomb);

#endif // YADE_CGAL
