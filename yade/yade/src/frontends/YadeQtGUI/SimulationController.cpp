#include "SimulationController.hpp"
#include "Omega.hpp"

#include <qfiledialog.h>
 


SimulationController::SimulationController(QWidget * parent) : QtGeneratedSimulationController(parent,"name")
{
	setMinimumSize(size());
	setMaximumSize(size());
}

SimulationController::~SimulationController()
{

}

void SimulationController::pbLoadClicked()
{

// FIXME - resetting the rootBody shouldn't be here. It shoul dbe inside Omega. As well as all Omega variables SHOULD be private!
// FIXME - and this doesn't work - spheres disappear!
//	Omega::instance().rootBody = shared_ptr<NonConnexBody>(new NonConnexBody);
	

 	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", this,"Open File","Choose a file to open",&selectedFilter );

	if (!fileName.isEmpty() && selectedFilter == "XML Yade File (*.xml)")
	{
		Omega::instance().setFileName(fileName);
		Omega::instance().loadTheFile();

		glViewer = shared_ptr<GLViewer>(new GLViewer(this->parentWidget()->parentWidget()));
		glViewer->show();
		//simulationLoop = shared_ptr<SimulationLoop>(new SimulationLoop());	

	}
}

