#include "SimulationController.hpp"
#include "Omega.hpp"
#include "Math.hpp"
#include <qfiledialog.h>
#include <qlcdnumber.h>
#include <qlabel.h>
#include <boost/lexical_cast.hpp>
using namespace boost;


SimulationController::SimulationController(QWidget * parent) : QtGeneratedSimulationController(parent,"SimulationController")
{
	setMinimumSize(size());
	setMaximumSize(size());
	  
 	timer = new QTimer(this);
	connect( timer, SIGNAL(timeout()), SLOT(updateGUI()) );
}

SimulationController::~SimulationController()
{

}

void SimulationController::updateGUI()
{
	//static int turnId = Omega::instance().getNewTurnId();
 	
	//Omega::instance().waitMyTurn(turnId);
 
	lcdCurrentIteration->display(lexical_cast<string>(Omega::instance().getCurrentIteration()));
	double simulationTime = Omega::instance().getSimulationTime();
	
	
	/*long int nsec	= (long int)(simulationTime*1000000000);
	long int misec	= (long int)(simulationTime*1000000000);
	 
	int sec		= (int)((simulationTime));
	cout << sec << endl;
	int min		= (int)(((double)sec/60.0));	
	
	
	
	double infSec	= (simulationTime-sec)*1000;
	int msec	= infSec;
	cout << (simulationTime-sec)*1000 << endl;
	infSec		= (infSec-msec)*1000; 
	int misec	= (int)((infSec));
	cout << misec << endl;
	infSec		= (infSec-misec)*1000;
	int nsec	= (int)((infSec));
	sec		= sec-60*min;

cout << simulationTime << "  ||  " << min << " " << sec << " " << msec << " " << misec << " " << nsec << endl;
	lcdMinutev->display(lexical_cast<string>(min));
	lcdSecondv->display(lexical_cast<string>(sec));
	lcdMSecondv->display(lexical_cast<string>(msec));
	lcdMiSecondv->display(lexical_cast<string>(misec));
	lcdNSecondv->display(lexical_cast<string>(nsec));*/
	
	//Omega::instance().endMyTurn();

}

void SimulationController::pbLoadClicked()
{	
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", this,"Open File","Choose a file to open",&selectedFilter );

	if (!fileName.isEmpty() && selectedFilter == "XML Yade File (*.xml)")
	{
		Omega::instance().setSimulationFileName(fileName);
		Omega::instance().loadSimulation();

		tlCurrentSimulation->setText(fileName);
		
		if (glViews.size()==0)
		{
 			glViews.push_back(new GLViewer(this->parentWidget()->parentWidget()));
			glViews.back()->show();
		}
		timer->start(1);
		
	}
}

void SimulationController::pbNewViewClicked()
{
	glViews.push_back(new GLViewer( this->parentWidget()->parentWidget(), glViews.front() ) );
	glViews.back()->show();
}
