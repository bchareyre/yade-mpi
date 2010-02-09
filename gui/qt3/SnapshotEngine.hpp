#pragma once
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
	virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(SnapshotEngine,PeriodicEngine,"Periodically save snapshots of GLView(s) as .png files. Files are named *fileBase*+*counter*+'.png' (counter is left-padded by 0s, i.e. snap0004.png)",
		((string,fileBase,"","Basename for snapshots"))
		((int,counter,0,"Number appended to fileBase |yupdate|"))
		((int,viewNo,((void)"primary view",0),"The GLView number that we save."))
		((bool,ignoreErrors,true,"Silently return if selected view doesn't exist"))
		((vector<string>,savedSnapshots,,"Files that have been created so far"))
		((int,msecSleep,0,"number of msec to sleep after snapshot (to prevent 3d hw problems) [ms]"))
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(SnapshotEngine);
