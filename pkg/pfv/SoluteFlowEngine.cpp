/*************************************************************************
*  Copyright (C) 2013 by T. Sweijen (T.sweijen@uu.nl)                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef YADE_CGAL
#ifdef FLOW_ENGINE

// #define SOLUTE_FLOW
#ifdef SOLUTE_FLOW

#include "FlowEngine_SoluteFlowEngineT.hpp"

#include <Eigen/Sparse>

class SoluteCellInfo : public FlowCellInfo_SoluteFlowEngineT
{	
	public:
	Real solute_concentration;
	SoluteCellInfo (void) : FlowCellInfo_SoluteFlowEngineT() {solute_concentration=0;}
	inline Real& solute (void) {return solute_concentration;}
	inline const Real& solute (void) const {return solute_concentration;}
	inline void getInfo (const SoluteCellInfo& otherCellInfo) {FlowCellInfo_SoluteFlowEngineT::getInfo(otherCellInfo); solute()=otherCellInfo.solute();}
};

typedef TemplateFlowEngine_SoluteFlowEngineT<SoluteCellInfo,FlowVertexInfo_SoluteFlowEngineT> SoluteFlowEngineT;
REGISTER_SERIALIZABLE(SoluteFlowEngineT);
YADE_PLUGIN((SoluteFlowEngineT));

class SoluteFlowEngine : public SoluteFlowEngineT
{
	public :
		void initializeSoluteTransport();
		void soluteTransport ();
		double getConcentration(unsigned int id){return solver->T[solver->currentTes].cellHandles[id]->info().solute();	}
		double insertConcentration(unsigned int id,double conc){
			solver->T[solver->currentTes].cellHandles[id]->info().solute() = conc;
			return conc;}
		void soluteBC(unsigned int bc_id1, unsigned int bc_id2, double bc_concentration1, double bc_concentration2,unsigned int s);
		double getConcentrationPlane (double Yobs,double Yr, int xyz);
                double getAverageConcentration();
		///Elaborate the description as you wish
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(SoluteFlowEngine,SoluteFlowEngineT,"A variant of :yref:`FlowEngine` with solute transport).",
		///No additional variable yet, else input here
// 		((Vector3r, gradP, Vector3r::Zero(),,"Macroscopic pressure gradient"))
                ((double,DiffusionCoefficient,0,,"Diffusion coefficient for molecular diffusion"))
		,,,
		.def("soluteTransport",&SoluteFlowEngine::soluteTransport,"Solute transport (advection and diffusion) engine for diffusion use a diffusion coefficient (D) other than 0.")
		.def("getConcentration",&SoluteFlowEngine::getConcentration,(boost::python::arg("id")),"get concentration of pore with ID")
		.def("insertConcentration",&SoluteFlowEngine::insertConcentration,(boost::python::arg("id"),boost::python::arg("conc")),"Insert Concentration (ID, Concentration)")
		.def("solute_BC",&SoluteFlowEngine::soluteBC,(boost::python::arg("bc_id1"),boost::python::arg("bc_id2"),boost::python::arg("bc_concentration1"),boost::python::arg("bc_concentration2"),boost::python::arg("s")),"Enter X,Y,Z for concentration observation'.")
                .def("initializeSoluteTransport",&SoluteFlowEngine::initializeSoluteTransport,"Initialize Solute Transport")
                .def("getConcentrationPlane",&SoluteFlowEngine::getConcentrationPlane,(boost::python::arg("Yobs"),boost::python::arg("Yr"),boost::python::arg("xyz")),"get concentration of pore with ID")
                .def("getAverageConcentration",&SoluteFlowEngine::getAverageConcentration,"The the volume averaged concentration")

		)
};
REGISTER_SERIALIZABLE(SoluteFlowEngine);


// PeriodicFlowEngine::~PeriodicFlowEngine(){}

void SoluteFlowEngine::initializeSoluteTransport ()
{
    FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles)
    {
	 cell->info().solute() = 0.0;
    }
}


void SoluteFlowEngine::soluteTransport ()
{       
        double deltatime = scene->dt;
	//soluteTransport is a function to solve transport of solutes for advection (+diffusion).
	//Call this function with a pyRunner in the python script, implement a diffusion coefficient and a dt.
	//Optimalization has to be done such that the coefficient matrix is not solved for each time step,only after triangulation.
	//Extensive testing has to be done to check its ability to simulate a deforming porous media.
	double coeff = 0.00;    //Ratio of dt and void volume   
	double coeff1 = 0.00;	//Coefficient for off-diagonal element
	double coeff2 = 0.00;   //Coefficient for diagonal element
	double qin = 0.00;	//Flux into the pore per pore throat 
	double Qout=0.0;	//Total Flux out of the pore
        double dt = 1e9;
	double invdistance = 0.0;	//Fluid facet area divided by pore throat length for each pore throat
	double invdistancelocal = 0.0;  //Sum of invdistance
	
	
	//Set vectors & matrix
	int ncells=solver->T[solver->currentTes].cellHandles.size();
	Eigen::SparseMatrix<double, Eigen::ColMajor> Aconc(ncells,ncells);
	typedef Eigen::Triplet<double> ETriplet2;
	std::vector<ETriplet2> tripletList2;
	Eigen::SparseLU<Eigen::SparseMatrix<double,Eigen::ColMajor>,Eigen::COLAMDOrdering<int> > eSolver2;	
	// Prepare (copy) concentration vector
	Eigen::VectorXd eb2(ncells); Eigen::VectorXd ex2(ncells);
	FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles){
	  eb2[cell->info().id]=cell->info().solute();
	}
	
	
	
	// Fill coefficient matrix
	FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles){
                cell->info().invVoidVolume() = 1.0 / ( std::abs(cell->info().volume()) - std::abs(solver->volumeSolidPore(cell) ) );    
                for (unsigned int ngb=0;ngb<4;ngb++){
                    CGT::Point& p2 = cell ->neighbor(ngb)->info();
                    CGT::Point& p1 = cell->info();
                    CGT::CVector l = p1-p2;
                    CGT::Real fluidSurf = sqrt(cell->info().facetSurfaces[ngb].squared_length())*cell->info().facetFluidSurfacesRatio[ngb];
                    invdistancelocal = (fluidSurf/sqrt(l.squared_length()));
                    invdistance+=(fluidSurf/sqrt(l.squared_length()));
                    coeff = deltatime*cell->info().invVoidVolume();
                    
                    qin=std::abs(cell->info().kNorm() [ngb])* ( cell->neighbor ( ngb )->info().p()-cell->info().p());
                    dt = std::min(( std::abs(cell->info().volume()) - std::abs(solver->volumeSolidPore(cell) ) )/ std::abs(qin), dt);
                    Qout=Qout+qin; //max(qin,0.0);
                    
                    coeff1=-1*coeff*(qin+DiffusionCoefficient*invdistancelocal); //(std::abs(max(qin,0.0))-(DiffusionCoefficient*invdistancelocal));       //off-diagonal
                    
                    if (coeff1 != 0.0){
                          tripletList2.push_back(ETriplet2(cell->info().id, cell->neighbor(ngb)->info().id,coeff1));
                    }
                }
                coeff2=1.0+(coeff*Qout)+(coeff*DiffusionCoefficient*invdistance); //diagonal
                tripletList2.push_back(ETriplet2(cell->info().id,cell->info().id,coeff2));
                Qout=0.0;
                invdistancelocal = 0.0;
                invdistance = 0.0;
	}   
	
        //Solve Matrix
	Aconc.setFromTriplets(tripletList2.begin(), tripletList2.end());
	//if (eSolver2.signDeterminant() < 1){cerr << "determinant is negative!!!!!!! " << eSolver2.signDeterminant()<<endl;}
	//eSolver2.setPivotThreshold(10e-8);
        eSolver2.analyzePattern(Aconc); 						
        eSolver2.factorize(Aconc); 		
        eSolver2.compute(Aconc);
        ex2 = eSolver2.solve(eb2);

	    
        //Copy data to concentration array

        FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles){
		cell->info().solute()= ex2[cell->info().id];
        }
        tripletList2.clear();
        if(dt != 1e9){scene->dt = dt;}
	    
  }

void SoluteFlowEngine::soluteBC(unsigned int bcid1, unsigned int bcid2, double bcconcentration1, double bcconcentration2, unsigned int s)
{
	//Boundary conditions according to soluteTransport.
	//It simply assigns boundary concentrations to cells with a common vertices (e.g. infinite large sphere which makes up the boundary condition in flowEngine)
	//s is a switch, if 0 only bc_id1 is used (advection only). If >0 than both bc_id1 and bc_id2 are used. 
	//NOTE (bruno): cell cirulators can be use to get all cells having bcid2 has a vertex more efficiently (see e.g. FlowBoundingSphere.ipp:721)
    	FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles)
	{
		for (unsigned int ngb=0;ngb<4;ngb++){ 
			if (cell->vertex(ngb)->info().id() == bcid1){cell->info().solute() = bcconcentration1;}
			if (s > 0){if (cell->vertex(ngb)->info().id() == bcid2){cell->info().solute() = bcconcentration2;}}
		}
	}
}

double SoluteFlowEngine::getConcentrationPlane (double Yobs,double Yr, int xyz)
{
	//Get the concentration within a certain plane (Y_obs), whilst the cells are located in a small volume around this plane
	//The concentration in cells are weighed for their distance to the observation point
	//for a point on the x-axis (xyz=0), point on the y-axis (xyz=1), point on the z-axis (xyz=2)
	double sumConcentration = 0.0;
	double sumFraction=0.0;
	double concentration=0.0;
	//Find cells within designated volume

	FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles)
	{
            CGT::Point& p1 = cell->info();
            if (std::abs(p1[xyz]) < std::abs(std::abs(Yobs) + std::abs(Yr))){
                if(std::abs(p1[xyz]) > std::abs(std::abs(Yobs) - std::abs(Yr))){
                    sumConcentration += cell->info().solute()*(1-(std::abs(p1[xyz])-std::abs(Yobs))/std::abs(Yr));
                    sumFraction += (1-(std::abs(p1[xyz])-std::abs(Yobs))/std::abs(Yr));
                }
            }
	}
        concentration = sumConcentration / sumFraction;
        return concentration;
}


double SoluteFlowEngine::getAverageConcentration()
{
        //Get volume-averaged concentration
        double summConc = 0.0, summVol = 0.0;
	FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles)
	{
            if(!cell->info().isFictious){
                summConc += cell->info().solute() * (std::abs(cell->info().volume()) - std::abs(solver->volumeSolidPore(cell) ));
                summVol += (std::abs(cell->info().volume()) - std::abs(solver->volumeSolidPore(cell) ));
            }
	}
	return(summConc / summVol);
}




YADE_PLUGIN ( ( SoluteFlowEngine ) );

#endif //SOLUTE_FLOW
#endif //FLOW_ENGINE

#endif /* YADE_CGAL */
