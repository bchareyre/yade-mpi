#include"SnapshotEngine.hpp"
#include<sstream>
#include<iomanip>
CREATE_LOGGER(SnapshotEngine);
void SnapshotEngine::action(MetaBody* rb){
	shared_ptr<GLViewer> glv;
	if(!YadeQtMainWindow::self || ((size_t)viewNo>=YadeQtMainWindow::self->glViews.size()) || !(glv=YadeQtMainWindow::self->glViews[viewNo])){
		if(!ignoreErrors) throw invalid_argument("View #"+lexical_cast<string>(viewNo)+" (SnapshotEngine::viewNo) doesn't exist.");
		return;
	}
	ostringstream fss; fss<<fileBase<<setw(4)<<setfill('0')<<counter++<<".png";
	LOG_DEBUG("GL view #"<<viewNo<<" → "<<fss.str())
	glv->setSnapshotFormat("PNG");
	glv->saveSnapshot(QString(fss.str()),/*overwrite*/ true);
	savedSnapshots.push_back(fss.str());
}

