// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/lib-pyutil/gil.hpp>

class PeriodicPythonRunner: public PeriodicEngine {
	public :
		/* virtual bool isActivated: not overridden, PeriodicEngine handles that */
		virtual void action(Scene*){ if(command.size()>0) pyRunString(command); }
	YADE_CLASS_BASE_DOC_ATTRS(PeriodicPythonRunner,PeriodicEngine,
		"Execute a python command periodically, with defined (and adjustable) periodicity. See :yref:`PeriodicEngine` documentation for details.",
		((string,command,"","Command to be run by python interpreter. Not run if empty."))
	);
};
REGISTER_SERIALIZABLE(PeriodicPythonRunner);

