/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                 *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v3 or later. See file LICENSE for details. *
*                                                                        *
* Luc Scholtès luc.scholtes@univ-lorraine.fr                             *
* and Luc Sibille luc.sibille@3sr-grenoble.fr also contributed to this   *
* code.                                                                  *
*                                                                        *
* Lominé F., Scholtès L., Sibille L., Poullain P. (2013)                 *
* Modelling of fluid-solid interaction in granular media with coupled    *
* LB/DE methods: application to piping erosion. International Journal    *
* for Numerical and Analytical Methods in Geomechanics, 37(6):577-596    *
* doi: 10.1002/nag.1109                                                  *
*                                                                        *
* Sibille L., Lominé F., Marot D. (2012) Investigation In Modelling      *
* Piping Erosion With a Coupled «Lattice Boltzmann – Discrete Element»   *
* Numerical Method. in Proc. 6th Int. Conference on Scour and Erosion    *
* (ICSE-6), pp. 93-100.                                                  *
*                                                                        *
*************************************************************************/
#ifdef LBM_ENGINE

#pragma once
#include<yade/pkg/common/LBMnode.hpp>
#include<yade/pkg/common/LBMlink.hpp>
#include<yade/pkg/common/LBMbody.hpp>
#include<yade/core/GlobalEngine.hpp>


class HydrodynamicsLawLBM : public GlobalEngine
{
    private :
        std::ofstream ofile;

    public :
        bool    firstRun,                       /*!  = 1 if it is the first iteration during 1 YADE simulation*/
                use_ConvergenceCriterion,       /*! use stop condition based on the convergence criterion*/
                SAVE_VELOCITY,                  /*! Switch to save node velocities*/
                SAVE_VELOCITYCOMP,              /*! Switch to save node velocities in each directions*/
                SAVE_RHO,                       /*! Switch to save node densities*/
                SAVE_FORCES,                    /*! Switch to save node force and momentum on grid*/
                SAVE_BODIES,                    /*! Switch to save particle nodes*/
                SAVE_NODEBD,                    /*! Switch to save fluid or solid boundary nodes*/
                SAVE_NODEISNEW,                 /*! Switch to save new fluid/solid nodes*/
                SAVE_DEBUGFILES,                /*! Switch to save some debug data*/
                SAVE_OBSERVEDPTC,               /*! Switch to save properties of the observed particle*/
                SAVE_OBSERVEDNODE,              /*! Switch to save properties of the observed node*/
                SAVE_CONTACTINFO,               /*! Switch to save contact properties*/
		SAVE_SPHERES,			/*! Switch to save spheres properties*/
                COMPRESS_DATA,                  /*! Switch to enable file compression*/
                Xperiodicity,                   /*! Switch to activate lattice periodicity in x direction*/
                Yperiodicity,                   /*! Switch to activate lattice periodicity in y direction*/
                Zperiodicity;                   /*! Switch to activate lattice periodicity in z direction*/

        int     NB_BODIES,                      /*! Number of bodies*/
                NB_GRAINS,                      /*! number of grains*/
                NB_DYNGRAINS,                   /*! number of dynamic grains*/
                NB_DYNBODIES,                   /*! number of dynamic bodies*/
                NB_WALLS,                       /*! Number of walls*/
                DEM_ITER,                       /*! Number of iteration of the DEM loop*/
                LBM_ITER,                       /*! Number of iteration of the LBM loop*/
                MODE,                           /*! 1->only a LBM loop, 2->lbm subcycling, 3->lbm subcycling after lbm loop*/
                dim,                            /*! dimension*/
                NbDir,                          /*! number of directions of the lattice model*/
                NbNodes,                        /*! Total number of nodes*/
                NbFluidNodes,                   /*! Number of fluid nodes*/
                NbSolidNodes,                   /*! Number of solid nodes*/
                NbParticleNodes,                /*! Number of particle nodes*/
                NbContacts,                     /*! Number of Contact*/
                InitialNumberOfDynamicParticles,/*! Initial number of dynamic particles*/
                NumberOfDynamicParticles,       /*! Number of dynamic particles*/
                Ny,                             /*! Number of grid divisions in y direction */
                Nz,                             /*! Number of grid divisions in z direction */
                NumberPtcEroded,                /*! The bumber of eroded/removed particles*/
                iter,                           /*! LBM Iteration number in current DEM loop (=1 in mode=2)*/
                IdFirstSphere;                  /*! Id of the first sphere*/

        Real    height,				/*! System height  */
                width,				/*! System width  */
                depth,				/*! System depth  */
                halfWallthickness,		/*! Half Wall thickness  */
                Wallthickness,			/*! Wall thickness  */
                cub,                            /*! A temporary variable to calculate equilibrium distribution function */
                c,                              /*! Lattice speed */
                c2,                             /*! The squared lattice speed*/
                dx,                             /*! The lattice size*/
                invdx,                          /*! 1 / lattice size*/
                dx2,                            /*! The squared lattice size*/
                uMax,                           /// TODO: PLEASE EXPLAIN uMax
                cs,                             /*! c/sqrt(3) */
                dt,                             /*! LBM timestep */
                invdt,                          /*! one over LBM timestep */
                nu,                             /*! LBM kinematic viscosity */
                feqb,                           /*! Equilibrium distribution function*/
                omega,				/*! 1/tau */
                Lx0,                            /*! LBM grid size in x direction*/
                Ly0,                            /*! LBM grid size in y direction*/
                Lz0,                            /*! LBM grid size in z direction*/
                outside_limit,                  /*! the x coordinate of a  point outside the system*/
                DEMdt,                          /*! timestep for the DEM iteration*/
                DEMdt0,                         /*! original timestep for the DEM iteration*/
                newDEMdt,                       /*! the new timestep for the DEM iteration*/
                Vr,                             /*! Volume of the removed particles*/
                Vo,                             /*! Initial volume of dynamic particles */
                VmeanFluidC,                    /*! Current mean fluid velocity */
                PrevVmeanFluidC,                /*! Previous mean fluid velocity */
                PrevPrevVmeanFluidC,            /*! Previous previous mean fluid velocity */
                VmaxC,                          /*! Maximum velocity during the current time step*/
                VminC,                          /*! Minimum velocity during the current time step*/
                RhomaxC,                        /*! Maximum density during the current time step*/
                RhominC,                        /*! Minimum density during the current time step*/
                LBM_TIME,			/*! The time ellapsed in the LB method*/
                DEM_TIME,			/*! The time ellapsed in the DE method*/
                RhoTot,                         /*! Cumulative density*/
                FmoyCur,                        /*! Mean force at the current LB iteration*/
                FmoyPrev,                       /*! Mean force at the previous LB iteration*/
                FmoyPrevPrev,                   /*! Mean force at 2 previous LB iteration*/
                UMaxtheo,                       /// TODO: PLEASE EXPLAIN UMaxtheo
                MaxBodyRadius,                  /*! Max radius of spheres*/
                MinBodyRadius,                  /*! Min radius of spheres*/
                MeanBodyRadius;                 /*! Mean radius of spheres*/

        std::string LBMlogFile,                 /*! Name of the logfile */
                    LBMmachFile,                /*! Name of the stat file */
                    LBMcontactsFile,            /*! Name of the contact file */
                    RemovedPtcFile,             /*! Name of the file to store removed particle informations*/
                    ObservedPtcFile,            /*! Name of the file to store observed particle informations*/
                    ObservedNodeFile,           /*! Name of the file to store observed particle informations*/
                    lbm_dir,                    /*! Directory name to save LBM files */
                    dem_dir,                    /*! Directory name to save DEM files */
                    cntct_dir;                  /*! Directory name to save contact properties */

        std::stringstream spherefile_name;      /*! Name of the file where sphere data are saved*/

        vector<int> IdOfNextErodedPtc,          /*! List of particles which will be eroded*/
                    opp;                        /*! opposite nodes */

        vector<Real>    w;                      /*! Weighting factor */

        vector <LBMnode> nodes;                 /*! the LBM nodes*/
        vector <LBMlink> links;                 /*! the LBM links*/
       vector <LBMbody> LBbodies;                /*! the LBM bodies*/

        vector <Vector3r>   eib;                /*! node velocity directions*/

        Vector3r FhTotale;                      ///Total hydrodynamic force

        virtual ~HydrodynamicsLawLBM ();
        virtual bool isActivated();
        virtual void action();
        void save(int iter_number, Real timestep);
        void saveStats(int iter_number, Real timestep);
        void saveEroded(int iter_number, Real timestep);
        void saveContacts(int iter_number, Real timestep);
        void saveObservedNode(int iter_number, Real timestep);
        void saveObservedPtc(int iter_number, Real timestep);
        void createNewFiles();
	//void createDirectories(); // ModLuc: to create directories only if necessary
        void createDirectories(bool dirLBM, bool dirDem, bool dirCntct);
        void writelogfile();
        void modeTransition();
        void LbmEnd();
        void CalculateAndApplyForcesAndTorquesOnBodies(bool mean,bool apply);

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(HydrodynamicsLawLBM,GlobalEngine,"Engine to simulate fluid flow (with the lattice Boltzmann method) with a coupling with the discrete element method.\n If you use this Engine, please cite and refer to F. Lominé et al. International Journal For Numerical and Analytical Method in Geomechanics, 2012, doi: 10.1002/nag.1109",

				((int,WallYm_id,0,,"Identifier of the Y- wall"))
				((bool,useWallYm,true,,"Set true if you want that the LBM see the wall in Ym"))
				((int,YmBCType,2,,"Boundary condition for the wall in Ym (-1: unused, 1: pressure condition, 2: velocity condition)."))
				((Vector3r,YmBcVel,Vector3r::Zero(),,"(!!! not fully implemented !!) The velocity imposed at the boundary"))
				((Real,YmBcRho,-1,,"(!!! not fully implemented !!) The density imposed at the boundary"))
				((int,WallYp_id,1,,"Identifier of the Y+ wall"))
				((bool,useWallYp,true,,"Set true if you want that the LBM see the wall in Yp"))
				((int,YpBCType,2,,"Boundary condition for the wall in Yp (-1: unused, 1: pressure condition, 2: velocity condition)."))
				((Vector3r,YpBcVel,Vector3r::Zero(),,"(!!! not fully implemented !!) The velocity imposed at the boundary"))
				((Real,YpBcRho,-1,,"(!!! not fully implemented !!) The density imposed at the boundary"))
				((int,WallXm_id,2,,"Identifier of the X- wall"))
				((bool,useWallXm,false,,"Set true if you want that the LBM see the wall in Xm"))
				((int,XmBCType,1,,"Boundary condition for the wall in Xm (-1: unused, 1: pressure condition, 2: velocity condition)."))
				((Vector3r,XmBcVel,Vector3r::Zero(),,"(!!! not fully implemented !!) The velocity imposed at the boundary"))
				((Real,XmBcRho,-1,,"(!!! not fully implemented !!) The density imposed at the boundary"))
				((int,WallXp_id,3,,"Identifier of the X+ wall"))
				((bool,useWallXp,false,,"Set true if you want that the LBM see the wall in Xp"))
				((int,XpBCType,1,,"Boundary condition for the wall in Xp (-1: unused, 1: pressure condition, 2: velocity condition)."))
				((Vector3r,XpBcVel,Vector3r::Zero(),,"(!!! not fully implemented !!) The velocity imposed at the boundary"))
				((Real,XpBcRho,-1,,"(!!! not fully implemented !!) The density imposed at the boundary"))
				((int,WallZp_id,5,,"Identifier of the Z+ wall"))
				((bool,useWallZp,false,,"Set true if you want that the LBM see the wall in Zp"))
				((int,ZpBCType,-1,,"Boundary condition for the wall in Zp (-1: unused, 1: pressure condition, 2: velocity condition)."))
				((Vector3r,ZpBcVel,Vector3r::Zero(),,"(!!! not fully implemented !!) The velocity imposed at the boundary"))
				((Real,zpBcRho,-1,,"(!!! not fully implemented !!) The density imposed at the boundary"))
				((int,WallZm_id,4,,"Identifier of the Z- wall"))
				((bool,useWallZm,false,,"Set true if you want that the LBM see the wall in Zm"))
				((int,ZmBCType,-1,,"Boundary condition for the wall in Zm (-1: unused, 1: pressure condition, 2: velocity condition)."))
				((Vector3r,ZmBcVel,Vector3r::Zero(),,"(!!! not fully implemented !!) The velocity imposed at the boundary"))
				((Real,ZmBcRho,-1,,"(!!! not fully implemented !!) The density imposed at the boundary"))
 				((int,XmYmZpBCType,2,,"Boundary condition for the corner node XmYmZp (-1: unused, 1: pressure condition, 2: velocity condition)."))
 				((int,XmYpZpBCType,2,,"Boundary condition for the corner node XmYpZp (-1: unused, 1: pressure condition, 2: velocity condition)."))
 				((int,XpYmZpBCType,2,,"Boundary condition for the corner node XpYmZp (-1: unused, 1: pressure condition, 2: velocity condition)."))
 				((int,XpYpZpBCType,2,,"Boundary condition for the corner node XpYpZp (-1: unused, 1: pressure condition, 2: velocity condition)."))
 				((int,XmYmZmBCType,-1,,"Boundary condition for the corner node XmYmZm (not used with d2q9, -1: unused, 1: pressure condition, 2: velocity condition)."))
 				((int,XmYpZmBCType,-1,,"Boundary condition for the corner node XmYpZm (not used with d2q9, -1: unused, 1: pressure condition, 2: velocity condition)."))
  				((int,XpYmZmBCType,-1,,"Boundary condition for the corner node XpYmZm (not used with d2q9, -1: unused, 1: pressure condition, 2: velocity condition)."))
 				((int,XpYpZmBCType,-1,,"Boundary condition for the corner node XpYpZm (not used with d2q9, -1: unused, 1: pressure condition, 2: velocity condition)."))

				((int,defaultLbmInitMode,0,,"Switch between the two initialisation methods"))
				((Vector3r,dP,Vector3r(0.,0.,0.),,"Pressure difference between input and output"))
				((Real,Rho,1000.,,"Fluid density"))
				((Real,Nu,0.000001,,"Fluid kinematic viscosity"))
				((Real,tau,0.6,,"Relaxation time"))
				((int,Nx,1000,,"The number of grid division in x direction"))
				((int,IterMax,1,,"This variable can be used to do several LBM iterations during one DEM iteration. "))
				((int,IterPrint,1,,"Print info on screen every IterPrint iterations"))
				((int,SaveMode,1,,"Save Mode (1-> default, 2-> in time (not yet implemented)"))
				((int,IterSave,100,,"Data are saved every IterSave LBM iteration (or see TimeSave)"))
				((Real,TimeSave,-1,,"Data are saved at constant time interval  (or see IterSave)"))
				((int,SaveGridRatio,1,,"Grid data are saved every SaveGridRatio * IterSave LBM iteration (with SaveMode=1)"))
				((int,IterSubCyclingStart,-1,,"Iteration number when the subcycling process starts"))
				((int,DemIterLbmIterRatio,-1,,"Ratio between DEM and LBM iterations for subcycling"))
				((bool,EngineIsActivated,true,,"To activate (or not) the engine"))
				((bool,applyForcesAndTorques,true,,"Switch to apply forces and torques"))
				((int,ObservedNode,-1,,"The identifier of the node that will be observed (-1 means none)"))
				((int,ObservedPtc,-1,,"The identifier of the particle that will be observed (-1 means the first one)"))
				((Real,RadFactor,1.0,,"The radius of DEM particules seen by the LBM is the real radius of particules*RadFactor"))
				((Real,ConvergenceThreshold,0.000001,,""))
				((std::string,LBMSavedData," ",,"a list of data that will be saved. Can use velocity,velXY,forces,rho,bodies,nodeBD,newNode,observedptc,observednode,contacts,spheres,bz2"))
				((std::string,periodicity," ",,"periodicity"))
				((std::string,bc," ",,"Boundary condition"))
                ((std::string,model,"d2q9",,"The LB model. Until now only d2q9 is implemented"))
				((int,removingCriterion	,0,,"Criterion to remove a sphere (1->based on particle position, 2->based on particle velocity"))
				((Real,VelocityThreshold,-1.,,"Velocity threshold when removingCriterion=2"))
				((Real,EndTime,-1,,"the time to stop the simulation"))
                ((Vector3r,CstBodyForce,Vector3r::Zero(),,"A constant body force (=that does not vary in time or space, otherwise the implementation introduces errors)"))
				((Real,VbCutOff,-1,,"the minimum boundary velocity that is taken into account"))
                                ,
    			firstRun  = true;
    			omega = 1.0/tau;
    			DEM_TIME = 0.;
    			LBM_TIME = 0.;
    			iter = 0;
    			use_ConvergenceCriterion = true;
    			MODE=0;
    			dim=0;
    			NbDir=0;
    			NbNodes=0;
    			NbFluidNodes=0;
    			NbSolidNodes=0;
    			NbParticleNodes=0;
    			NbContacts=0;
    			InitialNumberOfDynamicParticles=0;
    			NumberOfDynamicParticles=0;
    			NumberPtcEroded=0;
    			Vr=0.;
    			Vo=0.;
    			MaxBodyRadius=-1000000.;
    			MinBodyRadius=1000000.;
    			MeanBodyRadius=0.;
    			lbm_dir="lbm-nodes";
    			dem_dir="dem-bodies";
    			cntct_dir="contacts";
    			LBMlogFile  ="LBM.log";
    			LBMmachFile ="LBM.mach";
    			LBMcontactsFile ="LBM.cntct";
    			RemovedPtcFile="eroded.dat";
    			ObservedPtcFile="observedPtc.dat";
    			ObservedNodeFile="observedNode.dat";
    			COMPRESS_DATA   = false;
    			SAVE_VELOCITY   = false;
    			SAVE_VELOCITYCOMP   = false;
    			SAVE_RHO        = false;
    			SAVE_FORCES     = false;
    			SAVE_BODIES     = false;
    			SAVE_NODEBD     = false;
    			SAVE_NODEISNEW  = false;
    			SAVE_DEBUGFILES = false;
    			SAVE_OBSERVEDPTC= false;
    			SAVE_OBSERVEDNODE=false;
    			SAVE_CONTACTINFO =false;
			SAVE_SPHERES    = false;  //to save spheres_* files only if it is required by the operator
    			Xperiodicity    = false;
    			Yperiodicity    = false;
    			Zperiodicity    = false;
    			Ny = 0;Nz = 0;
    			FmoyCur=0.;FmoyPrev=0.;
    			FmoyPrevPrev=0.;VmeanFluidC=0.;PrevVmeanFluidC=0.;PrevPrevVmeanFluidC=0.;
    			LBM_ITER=0;
    			DEM_ITER=0;
                IdFirstSphere=-1;
                timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);

				);
	DECLARE_LOGGER;
};


REGISTER_SERIALIZABLE(HydrodynamicsLawLBM);

#endif //LBM_ENGINE

