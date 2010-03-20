#include"SnapshotEngine.hpp"
#include<sstream>
#include<iomanip>
#include<boost/algorithm/string/case_conv.hpp>

CREATE_LOGGER(SnapshotEngine);
YADE_PLUGIN((SnapshotEngine));
void SnapshotEngine::action(){
	shared_ptr<GLViewer> glv;
	if(!YadeQtMainWindow::self || ((size_t)viewNo>=YadeQtMainWindow::self->glViews.size()) || !(glv=YadeQtMainWindow::self->glViews[viewNo])){
		if(!ignoreErrors) throw invalid_argument("View #"+lexical_cast<string>(viewNo)+" (SnapshotEngine::viewNo) doesn't exist.");
		return;
	}
	ostringstream fss; fss<<fileBase<<setw(4)<<setfill('0')<<counter++<<"."<<boost::algorithm::to_lower_copy(format);
	LOG_DEBUG("GL view #"<<viewNo<<" → "<<fss.str())
	glv->setSnapshotFormat(format);
	glv->nextFrameSnapshotFilename=fss.str();
	// wait for the renderer to save the frame (will happen at next postDraw)
	timespec t1,t2; t1.tv_sec=0; t1.tv_nsec=10000000; /* 10 ms */
	long waiting=0;
	while(!glv->nextFrameSnapshotFilename.empty()){
		nanosleep(&t1,&t2);
		if(((++waiting) % 1000)==0) LOG_WARN("Already waiting "<<waiting/100<<"s for snapshot to be saved. Something went wrong?");
	}
	savedSnapshots.push_back(fss.str());
	usleep((long)(msecSleep*1000));
}
