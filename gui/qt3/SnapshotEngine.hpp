#pragma once
#include<yade/core/StandAloneEngine.hpp>
#include<yade/gui-qt3/GLViewer.hpp>
#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
/*! Engine for saving snapshots of GLView(s) as .png files.
 *
 * Derives from PeriodicEngine, which has detailed information on controlling periodicity.
 *
 * The files are named fileBase+counter+".png" (counter is left-padded by 0s, i.e. snap0004.png)
 */
class SnapshotEngine: public PeriodicEngine{
	public:
		//! Basename for snapshots
		string fileBase;
		//! Number appended to fileBase
		int counter;
		//! The GLView number that we save (default: 0, i.e. primary view)
		int viewNo;
		//! Silently return if selected view doesn't exist (default: true)
		bool ignoreErrors;
		//! files that have been created so far
		vector<string> savedSnapshots;
		//! number of msec to sleep after snapshot (to prevent 3d hw problems)
		int msecSleep;
	SnapshotEngine():counter(0),viewNo(0),ignoreErrors(true),msecSleep(0){}
	virtual void action(MetaBody*);
	REGISTER_ATTRIBUTES(PeriodicEngine,(fileBase)(counter)(viewNo)(ignoreErrors)(savedSnapshots)(msecSleep));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(SnapshotEngine,PeriodicEngine);
};
REGISTER_SERIALIZABLE(SnapshotEngine);
