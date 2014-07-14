/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                 *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
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

#include"HydrodynamicsLawLBM.hpp"
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<boost/filesystem/operations.hpp>
#include<yade/pkg/common/Box.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/dem/CohesiveFrictionalContactLaw.hpp>


namespace bfs=boost::filesystem;

template<class Scalar> VECTOR3_TEMPLATE(Scalar) operator*(Scalar s, const VECTOR3_TEMPLATE(Scalar)& v) {return v*s;}
inline Vector3i vect3rToVect3i(Vector3r vect){Vector3i newvect((int)vect[0],(int)vect[1],(int)vect[2]);return(newvect);}


HydrodynamicsLawLBM::~HydrodynamicsLawLBM() {};

bool HydrodynamicsLawLBM::isActivated(){
    DEM_ITER=scene->iter;//+1;
    if(EngineIsActivated){
        if((DEM_ITER % DemIterLbmIterRatio==0)&&(DEM_ITER!=DemIterLbmIterRatio)) {
            if(DEM_ITER==0){DEMdt0 = scene->dt;scene->dt=1.e-50;}
            return(true);}
        else{
            if(applyForcesAndTorques) CalculateAndApplyForcesAndTorquesOnBodies(false,true);
            return(false);
        }
    }else return(false);
}

void HydrodynamicsLawLBM::action()
{
    timingDeltas->start();
    NB_BODIES=  scene->bodies->size();
    int I, J, step=0;
    NbFluidNodes=0;
    NbSolidNodes=0;
    Real CurMinVelOfPtc=1000000.;
    Real CurMaxVelOfPtc=-1000000.;
    int ErrorCriterion=2;
    /*------------------------------------------------------------------*/
    /*                       AT FIRST ITERATION                         */
    /*------------------------------------------------------------------*/
    if(firstRun){
        // createNewFiles();  //this line is move further to create files only when the recording configuration chosen by the operator is known
        bool initVbCutOff=false;
        if(VbCutOff==-1) initVbCutOff=true;
        halfWallthickness=1000000.;
        NB_WALLS=0;
        //NB_DYNWALLS=0;
        NB_DYNGRAINS=0;
        LBMbody tmpbody;
        FOREACH(const shared_ptr<Body>& b, *scene->bodies){
            if(!b) continue; // deleted bodies
            if (b->shape->getClassName()=="Box"){
                Vector3r ext(YADE_PTR_CAST<Box> ( b->shape )->extents);
                if (ext[0]<halfWallthickness) halfWallthickness=ext[0];
                if (ext[1]<halfWallthickness) halfWallthickness=ext[1];
                if (ext[2]<halfWallthickness) halfWallthickness=ext[2];
                NB_WALLS++;
                tmpbody.setAsBox();
                tmpbody.saveProperties=false;
                //if(b->isDynamic()) NB_DYNWALLS++;
            }
            if (b->shape->getClassName()=="Sphere"){
	            const shared_ptr<Sphere>& sph = YADE_PTR_CAST<Sphere> ( b->shape );
                if(IdFirstSphere==-1) IdFirstSphere=b->getId();
                tmpbody.setAsPtc();
                Real r=sph->radius;
                if(b->isDynamic()){
                    NB_DYNGRAINS++;
                    /*--- computation of the initial volume ---*/
                    if(!strcmp(model.c_str(), "d2q9" )) Vo += Mathr::PI*(r*r);
                    else Vo += 4./3.*Mathr::PI*(r*r*r);
                    if(initVbCutOff) VbCutOff=max(VbCutOff,b->state->vel.norm()+r*b->state->angVel.norm());
                    tmpbody.saveProperties=true;
                }else{
                    if(b->state->pos[1]>0.) tmpbody.saveProperties=true;
                    else tmpbody.saveProperties=false;
                }
                /*--- computation of Rmean Rmax Rmin --*/
                MaxBodyRadius=max(r,MaxBodyRadius);
                MinBodyRadius=min(r,MinBodyRadius);
                MeanBodyRadius+=r;
            }

        LBbodies.push_back(tmpbody);
        }
        Wallthickness=2.0*halfWallthickness;
        NB_GRAINS=NB_BODIES-NB_WALLS;
        //Luc: is it right to count the walls as dynamic bodies?
        //Franck why not ? Enhancement (see next line is coming ;-))   
        NB_DYNBODIES=NB_WALLS+NB_DYNGRAINS;   
        //NB_DYNBODIES=NB_DYNWALLS+NB_DYNGRAINS;
    	MeanBodyRadius=MeanBodyRadius/NB_GRAINS;
        InitialNumberOfDynamicParticles=NB_DYNGRAINS;

        /*-------------------------------------------------------------------------*/
        /*                  D2Q9 model configuration                               */
        /*-------------------------------------------------------------------------*/
        if(!strcmp(model.c_str(), "d2q9" )){
            dim=2;
            /*--------------------------------------*/
            /* D2Q9 model:	  6 2 5		            */
            /*		        3 _\|/__1	            */
            /*		           /|\		            */
            /*		          7 4 8		            */
            /*--------------------------------------*/
            /*----------- D2Q9 constants  ---------*/
            w.push_back(4.0/9.0);
            for(int aa=1;aa<=4;aa++) w.push_back(1.0/9.0);
            for(int aa=5;aa<=8;aa++) w.push_back(1.0/36.0);

            /*--------- node position vectors ----*/
            eib.push_back(Vector3r( 0., 0., 0.));   //0
            eib.push_back(Vector3r( 1., 0., 0.));   //1
            eib.push_back(Vector3r( 0., 1., 0.));   //2
            eib.push_back(Vector3r(-1., 0., 0.));   //3
            eib.push_back(Vector3r( 0.,-1., 0.));   //4
            eib.push_back(Vector3r( 1., 1., 0.));   //5
            eib.push_back(Vector3r(-1., 1., 0.));   //6
            eib.push_back(Vector3r(-1.,-1., 0.));   //7
            eib.push_back(Vector3r( 1.,-1., 0.));   //8
            NbDir=(int) eib.size();

            /*-------------- opposite nodes --------*/
            opp.push_back(0);   opp.push_back(3);   opp.push_back(4);
            opp.push_back(1);   opp.push_back(2);   opp.push_back(7);
            opp.push_back(8);   opp.push_back(5);   opp.push_back(6);
        }else {cerr<<"This model is not implemented yet: "<<model<<endl;exit(-1);}


        int res=0;
        int ll=0;
	bool CreateLbmDir = false;  //Flag to create directories if required by the recording configuration
	bool CreateDemDir = false;  //Flag to create directories if required by the recording configuration
	bool CreateCntctDir = false;  //Flag to create directories if required by the recording configuration
        /*-------------------------------------------------------------------------*/
        /*                       Recording configuration                           */
        /*-------------------------------------------------------------------------*/
        ll=LBMSavedData.size();
        res=LBMSavedData.find("Velocity");      if(res>=0&&res<ll) {SAVE_VELOCITY   =true; SAVE_VELOCITYCOMP=true; CreateLbmDir=true;}
        res=LBMSavedData.find("velocity");      if(res>=0&&res<ll) {SAVE_VELOCITY   =true; SAVE_VELOCITYCOMP=true; CreateLbmDir=true;}
        res=LBMSavedData.find("VelXY");         if(res>=0&&res<ll) {SAVE_VELOCITYCOMP=true; CreateLbmDir=true;}
        res=LBMSavedData.find("velXY");         if(res>=0&&res<ll) {SAVE_VELOCITYCOMP=true; CreateLbmDir=true;}
        res=LBMSavedData.find("Rho");           if(res>=0&&res<ll) {SAVE_RHO         =true; CreateLbmDir=true;}
        res=LBMSavedData.find("rho");           if(res>=0&&res<ll) {SAVE_RHO         =true; CreateLbmDir=true;}
        res=LBMSavedData.find("Forces");        if(res>=0&&res<ll) {SAVE_FORCES      =true; CreateLbmDir=true;}
        res=LBMSavedData.find("forces");        if(res>=0&&res<ll) {SAVE_FORCES      =true; CreateLbmDir=true;}
        res=LBMSavedData.find("Bodies");        if(res>=0&&res<ll) {SAVE_BODIES      =true; CreateLbmDir=true;}
        res=LBMSavedData.find("bodies");        if(res>=0&&res<ll) {SAVE_BODIES      =true; CreateLbmDir=true;}
        res=LBMSavedData.find("NodeBD");        if(res>=0&&res<ll) {SAVE_NODEBD      =true; CreateLbmDir=true;}
        res=LBMSavedData.find("nodeBD");        if(res>=0&&res<ll) {SAVE_NODEBD      =true; CreateLbmDir=true;}
        res=LBMSavedData.find("NewNode");       if(res>=0&&res<ll) {SAVE_NODEISNEW   =true; CreateLbmDir=true;}
        res=LBMSavedData.find("newNode");       if(res>=0&&res<ll) {SAVE_NODEISNEW   =true; CreateLbmDir=true;}
        res=LBMSavedData.find("bz2");           if(res>=0&&res<ll) COMPRESS_DATA    =true;
        res=LBMSavedData.find("ObservedPtc");   if(res>=0&&res<ll) SAVE_OBSERVEDPTC =true;
        res=LBMSavedData.find("observedptc");   if(res>=0&&res<ll) SAVE_OBSERVEDPTC =true;
	res=LBMSavedData.find("observedPtc");   if(res>=0&&res<ll) SAVE_OBSERVEDPTC =true;
        res=LBMSavedData.find("ObservedNode");  if(res>=0&&res<ll) SAVE_OBSERVEDNODE=true; // does not activate any recording in a a file currently
        res=LBMSavedData.find("observednode");  if(res>=0&&res<ll) SAVE_OBSERVEDNODE=true;
	res=LBMSavedData.find("observedNode");  if(res>=0&&res<ll) SAVE_OBSERVEDNODE=true;
        res=LBMSavedData.find("contacts");      if(res>=0&&res<ll) {SAVE_CONTACTINFO =true; CreateCntctDir=true;}  // SAVE_CONTACTINFO does not activate any (anymore) recording excepted the creation of a directory
        res=LBMSavedData.find("Contacts");      if(res>=0&&res<ll) {SAVE_CONTACTINFO =true; CreateCntctDir=true;}
	res=LBMSavedData.find("spheres");      if(res>=0&&res<ll) {SAVE_SPHERES =true; CreateDemDir=true;}  // To save spheres_* file only if it is required by the operator
        res=LBMSavedData.find("Spheres");      if(res>=0&&res<ll) {SAVE_SPHERES =true; CreateDemDir=true;}  // To save spheres_* file only if it is required by the operator

        // if(NB_DYNGRAINS==1) SAVE_OBSERVEDPTC =true; //Commented to avoid recording of observedPtc if not chosen by the operator
        if ((ObservedPtc==-1)&&(NB_GRAINS>0)&&(SAVE_OBSERVEDPTC)) ObservedPtc=IdFirstSphere;  //Condition If(SAVE_OBSERVEDPTC) is added to save observedPtc only if it is required by the operator
        if ((SAVE_OBSERVEDPTC)&&((unsigned)ObservedPtc>=LBbodies.size())){cerr<<"Error: ObservedPtc>bodies.size()"<<endl;exit(-1);}
	if ((SAVE_SPHERES)&&(NB_GRAINS<1)){cerr<<"Error: saving of sphere properties is switched on whereas NB_GRAINS<1"<<endl;exit(-1);}

        createDirectories(CreateLbmDir,CreateDemDir,CreateCntctDir);
	createNewFiles(); // Files are created now, since we know what the operator wants to record (ObservedPtc ... etc)


        /*--------------------------------------------------------------*/
        /*                  Periodicity configuration                   */
        /*--------------------------------------------------------------*/
        ll=periodicity.size();
        res=periodicity.find("x");     if(res>=0&&res<ll) Xperiodicity=true;
        res=periodicity.find("y");     if(res>=0&&res<ll) Yperiodicity=true;
        res=periodicity.find("z");     if(res>=0&&res<ll) Zperiodicity=true;
        cerr <<"Periodicity (XYZ): "<<Xperiodicity<<" "<<Yperiodicity<<" "<<Zperiodicity<<endl;



        /*--------------------------------------------------------------*/
        /*                     Lattice definition                       */
        /*--------------------------------------------------------------*/
        cerr << "---- Lattice setup -----" << endl;
        State* sWallYm=Body::byId(WallYm_id,scene)->state.get();
        State* sWallYp=Body::byId(WallYp_id,scene)->state.get();
        State* sWallXm=Body::byId(WallXm_id,scene)->state.get();
        State* sWallXp=Body::byId(WallXp_id,scene)->state.get();
        State* sWallZp=Body::byId(WallZp_id,scene)->state.get();
        State* sWallZm=Body::byId(WallZm_id,scene)->state.get();

        height = sWallYp->se3.position.y() - sWallYm->se3.position.y();
        width  = sWallXp->se3.position.x() - sWallXm->se3.position.x();
        depth  = sWallZp->se3.position.z() - sWallZm->se3.position.z();


        Lx0 = width-Wallthickness;
        Ly0 = height-Wallthickness;
        Lz0 = depth-Wallthickness;
        Real Lx1 = width+Wallthickness;
        Real Ly1 = height+Wallthickness;
        Real Lz1 = depth+Wallthickness;
        /*--------------------------------------------------------------*/
        /*   Computation of number of lattice nodes in each direction   */
        /*--------------------------------------------------------------*/
        dx =  Lx0 / (Real) (Nx-1);
        invdx=1./dx;

        //Number of nodes (after-correction)
        Ny = ceil(invdx*Ly1)+1;
        dx =  Ly1 / (Real) (Ny-1);
        invdx=1./dx;
        dx2=dx*dx;
        Nx = ceil(invdx*Lx1)+1;
        Ny = ceil(invdx*Ly1)+1;
        Nz=1;

        cerr <<"LXYZ0= "<<Lx0<<" "<<Ly0<<" "<<Lz0<<endl;
        cerr <<"LXYZ1= "<<Lx1<<" "<<Ly1<<" "<<Lz1<<endl;
        cerr <<"Ny= "<<Ny<<" "<<" Nx*Ly0/Lx0="<<Nx*Ly0/Lx0<<endl;
        cerr <<"Wallthickness= "<<Wallthickness<<" "<<" dx="<<dx<<endl;

        /*-------------------------------------------------------*/
        /* Verification of wall positionning in positive regions */
        /*-------------------------------------------------------*/
        if (sWallYm->se3.position.y()- halfWallthickness<0.){cerr <<"Wall position error: please avoid negatives region (Y)"<<endl;exit(-1);}
        if (sWallXm->se3.position.x()- halfWallthickness<0.){cerr <<"Wall position error: please avoid negatives region (X)"<<endl;exit(-1);}
        if ((dim==3)&&(sWallZm->se3.position.z()- halfWallthickness<0.)){cerr <<"Wall position error: please avoid negatives region (Z)"<<endl;exit(-1);}


        /*--------------------------------------------------------------*/
        /*         Some tests about the validity of parameters          */
        /*--------------------------------------------------------------*/
        if( (NB_DYNGRAINS==0)&&(use_ConvergenceCriterion)&&(ErrorCriterion==1)){
            cerr <<"ERROR: can't use ErrorCriterion=1 when (NB_DYNGRAINS=0"<<endl;
            exit(-1);}
        if((ObservedNode!=-1)&&(ObservedNode>=Nx*Ny)){
            cerr <<"Warning ObservedNode is >= Nx*Ny ... exit"<<endl;
            exit(-1);}
        if((SaveMode!=1)&&(SaveMode!=2)) {cerr <<"Warning unknown SaveMode."<<SaveMode<<endl; exit(-1);}
        if((SaveMode==1)&&(IterSave<=0)) {cerr <<"Warning SaveMode==1 and IterSave<=0."<<endl; exit(-1);}
        if((SaveMode==2)&&(TimeSave<=0)) {cerr <<"Warning SaveMode==2 and TimeSave<=0."<<endl; exit(-1);}
        /*---------------------------------------------------------------*/
        /*------------------ general node initialization ----------------*/
        /*---------------------------------------------------------------*/
        LBMnode aa;
        for(int nidx=0; nidx<Nx*Ny; nidx++) {nodes.push_back(aa);}
        bool j_update=false;
        int j=0;
        for (int nidx=0; nidx<Nx*Ny; nidx++){
            int i=nidx-j*Nx;
            if((nidx+1)%Nx==0) j_update=true;
            int k=0;

            nodes[nidx].SetCellIndexesAndPosition(i,j,k);
            nodes[nidx].DispatchBoundaryConditions(Nx,Ny,Nz);
            NbNodes++;
            for (int dndx=0; dndx<NbDir; dndx++){
                nodes[nidx].links_id.push_back(-1);
                I=nodes[nidx].i+eib[dndx].x();
                J=nodes[nidx].j+eib[dndx].y();
                if(((I==i)&&(J==j)) || (I==-1) || (J==-1) || (I==Nx) || (J==Ny)  ){
                    nodes[nidx].neighbour_id.push_back(-1);
                }
                else {nodes[nidx].neighbour_id.push_back(I+J*Nx);}
            }
        if(j_update) {j++;j_update=false;}
        }

        ////////////////////////////////////////////////////////////////////////////////////
        ///FIXME(flomine#1): periodicity should be implemented from links to facilitate the streaming step
        ///FIXME(flomine#1): to be optimise and bug should be corrected (cf test version)
        LBMlink bb;
        int link_id=-1;
         for (int nidx=0; nidx<Nx*Ny; nidx++){
            int I=nodes[nidx].i;
            int J=nodes[nidx].j;
            for (int dndx=0; dndx<NbDir; dndx++){
                if(dndx==0) continue;
                bb.PointingOutside=false;
                if((!strcmp(model.c_str(), "d2q9" )) && ((dndx==1)||(dndx==2)||(dndx==5)||(dndx==6))){
                    link_id++;bb.i=dndx;bb.nid1=nidx;
                    bb.nid2=nodes[nidx].neighbour_id[dndx];
                    if(bb.nid2==-1) bb.PointingOutside=true;
                    links.push_back(bb);
                    nodes[bb.nid1].links_id[dndx]=link_id;
                    if(bb.nid2!=-1) nodes[bb.nid2].links_id[opp[dndx]]=link_id;
                }else if(!strcmp(model.c_str(), "d2q9" )){
                    if((I==0)&&(J!=0)&&((dndx==3)||(dndx==7))){
                        link_id++;bb.i=dndx; bb.nid1=nidx;
                        bb.nid2=nodes[nidx].neighbour_id[dndx];
                        bb.PointingOutside=true;
                        if(bb.nid2!=-1) {cerr<<"ERROR: bb.id2!=-1"<<endl;exit(-1);}
                        links.push_back(bb);
                        nodes[bb.nid1].links_id[dndx]=link_id;
                        if(bb.nid2!=-1) nodes[bb.nid2].links_id[opp[dndx]]=link_id;
                    } else if((J==0)&&(I!=0)&&((dndx==4)||(dndx==7)||(dndx==8))){
                        link_id++;bb.i=dndx; bb.nid1=nidx;
                        bb.nid2=nodes[nidx].neighbour_id[dndx];
                        bb.PointingOutside=true;
                        if(bb.nid2!=-1) {cerr<<"ERROR: bb.id2!=-1"<<endl;exit(-1);}
                        links.push_back(bb);
                        nodes[bb.nid1].links_id[dndx]=link_id;
                        if(bb.nid2!=-1) nodes[bb.nid2].links_id[opp[dndx]]=link_id;
                    } else if((I==0)&&(J==0)&&((dndx==3)||(dndx==4)||(dndx==7)||(dndx==8))){
                        link_id++;bb.i=dndx; bb.nid1=nidx;
                        bb.nid2=nodes[nidx].neighbour_id[dndx];
                        bb.PointingOutside=true;
                        if(bb.nid2!=-1) {cerr<<"ERROR: bb.id2!=-1"<<endl;exit(-1);}
                        links.push_back(bb);
                        nodes[bb.nid1].links_id[dndx]=link_id;
                        if(bb.nid2!=-1) nodes[bb.nid2].links_id[opp[dndx]]=link_id;
                    }
                }else {cerr<<"ERROR: Unknow model type: "<<model<<endl;exit(-1);}
            }
         }
        ////////////////////////////////////////////////////////////////////////////////////

        if((ConvergenceThreshold==-1)||(ConvergenceThreshold==0)) use_ConvergenceCriterion=false;
        else {use_ConvergenceCriterion=true;ErrorCriterion=ConvergenceThreshold;}
        /*------------------------------------------------------------------*/
        /*------------------------ LBM PARAMETERS --------------------------*/
        /*------------------------------------------------------------------*/
        UMaxtheo = dP.norm()/(8.*Rho*Nu)*(height*height/width);
        if(defaultLbmInitMode){
            uMax = 0.1;             c    = UMaxtheo/uMax;
            cs   = c/sqrt(3.);      dt   = dx/c;
            nu   = dt/(dx*dx/Nu);   tau  = 3.*nu + 0.5;
        }else{
            nu   = (1.0/3.0)*(tau - 0.5);
            dt   =  nu*(dx*dx/Nu);  c    = dx/dt;
            cs   = c/sqrt(3.);      uMax = UMaxtheo / c;
        }
        omega = 1.0/tau;
        c2=c*c;
        invdt=1./dt;
        MaxBodyRadius=invdx*MaxBodyRadius;
        MinBodyRadius=invdx*MinBodyRadius;
        MeanBodyRadius=invdx*MeanBodyRadius;
        outside_limit=1.5*width;
    }//end if FirstRun

    /*************************************************************************/
    /*                     SOLID OBSTACLES SET-UP                            */
    /*************************************************************************/
    Real Rmin = 1000.;
    int newFluidCells_couter=0;
    int newObstacleCells_couter=0;

    State* sWallYm=Body::byId(WallYm_id,scene)->state.get();
    State* sWallYp=Body::byId(WallYp_id,scene)->state.get();
    State* sWallXm=Body::byId(WallXm_id,scene)->state.get();
    State* sWallXp=Body::byId(WallXp_id,scene)->state.get();
    State* sWallZp=Body::byId(WallZp_id,scene)->state.get();
    State* sWallZm=Body::byId(WallZm_id,scene)->state.get();

    timingDeltas->checkpoint("Reinit:Nodes0");
    #pragma omp parallel for
    for (int nidx=0; nidx<Nx*Ny; nidx++){
        /*------------------------------------------*/
        /* Reinitialization:                        */
        /*------------------------------------------*/
        nodes[nidx].body_id=-1;
        nodes[nidx].setAsFluid();
        nodes[nidx].isObstacleBoundary=false;
        nodes[nidx].isFluidBoundary=false;
        nodes[nidx].isNewObstacle=false;
        nodes[nidx].isNewFluid=false;

        /*--- according to X+ ---*/
        if (useWallXp&&(nodes[nidx].i>=invdx*(sWallXp->se3.position.x() - halfWallthickness))){
            nodes[nidx].setAsObstacle();
            nodes[nidx].isObstacleBoundary=true;
            nodes[nidx].body_id=WallXp_id;
            NbSolidNodes++;}
        /*--- according to X- ---*/
        else if (useWallXm&&(nodes[nidx].i<=invdx*(sWallXm->se3.position.x() + halfWallthickness))){
            nodes[nidx].setAsObstacle();
            nodes[nidx].isObstacleBoundary=true;
            nodes[nidx].body_id=WallXm_id;
            NbSolidNodes++;}
        /*--- according to Y+ ---*/
        else if (useWallYp&&(nodes[nidx].j>=invdx*(sWallYp->se3.position.y() - halfWallthickness))){
            nodes[nidx].setAsObstacle();
            nodes[nidx].isObstacleBoundary=true;
            nodes[nidx].body_id=WallYp_id;
            NbSolidNodes++;}
        /*--- according to Y- ---*/
        else if (useWallYm&&(nodes[nidx].j<=invdx*(sWallYm->se3.position.y() + halfWallthickness))){
            nodes[nidx].setAsObstacle();
            nodes[nidx].isObstacleBoundary=true;
            nodes[nidx].body_id=WallYm_id;
            NbSolidNodes++;}
         /*--- according to Z+ ---*/
        else if (useWallZp&&(nodes[nidx].k>=invdx*(sWallZp->se3.position.z() - halfWallthickness))){
            nodes[nidx].setAsObstacle();
            nodes[nidx].isObstacleBoundary=true;
            nodes[nidx].body_id=WallZp_id;
            NbSolidNodes++;}
        /*--- according to Z- ---*/
        else if (useWallZm&&(nodes[nidx].k<=invdx*(sWallZm->se3.position.z() + halfWallthickness))){
            nodes[nidx].setAsObstacle();
            nodes[nidx].isObstacleBoundary=true;
            nodes[nidx].body_id=WallZm_id;
            NbSolidNodes++;}

        if(firstRun){nodes[nidx].wasObstacle=nodes[nidx].isObstacle;}
    }

    /*---------------------------------------------------------------*/
    /*- Solid particle detection and recording of their properties --*/
    /*---------------------------------------------------------------*/
    NumberOfDynamicParticles=0;
    if(removingCriterion!=0) IdOfNextErodedPtc.clear();
    FOREACH(const shared_ptr<Body>& b, *scene->bodies){
        if(!b) continue; // deleted bodies
        State* state=b->state.get();
        const int id=b->getId();
        //if ((b->shape->getClassName()=="Sphere")&&(b->isDynamic())){  //ModLuc: removing of b->isDynamic() in order that non dynamic particle can be seen by the LBM
	if (b->shape->getClassName()=="Sphere"){
            const shared_ptr<Sphere>& sphere = YADE_PTR_CAST<Sphere> ( b->shape );

            LBbodies[id].pos=invdx*state->pos;
            LBbodies[id].vel=(state->vel)/c;
            LBbodies[id].AVel= (state->angVel)*dt;
	    LBbodies[id].radius=invdx*RadFactor*(sphere->radius);

            CurMinVelOfPtc=min(CurMinVelOfPtc,state->vel.norm());
            CurMaxVelOfPtc=max(CurMaxVelOfPtc,state->vel.norm());

            Vector3r posMax=LBbodies[id].pos+ Vector3r(LBbodies[id].radius,LBbodies[id].radius,LBbodies[id].radius);
            Vector3r posMin=LBbodies[id].pos- Vector3r(LBbodies[id].radius,LBbodies[id].radius,LBbodies[id].radius);

            Vector3r dist=Vector3r::Zero();
            for(int ii=posMin[0]-1;ii<=posMax[0]+1;ii++)
                for(int jj=posMin[1]-1;jj<=posMax[1]+1;jj++){
                    if((ii==-1)||(ii==Nx)||(jj==-1)||(jj==Ny)) continue;
                    if((ii<-1)||(ii>Nx)||(jj<-1)||(jj>Ny)) continue;
                    if (LBbodies[id].radius < Rmin) Rmin = LBbodies[id].radius;
                    int nidx=ii+jj*Nx;
                    dist=nodes[nidx].posb-LBbodies[id].pos;
                    if(dist.norm()<LBbodies[id].radius){
                        nodes[nidx].body_id = id;
                        nodes[nidx].setAsObstacle();
                        NbSolidNodes++;
                        NbParticleNodes++;}
                    if(firstRun){nodes[nidx].wasObstacle=nodes[nidx].isObstacle;}
                }
            /*-------------------------------------------------------------------*/
            /* ///NOTE : this should be removed since it can be done with python */
			 ///Fck: pas en MODE 1
            /*-------------------------------------------------------------------*/
            //cerr <<"Check removing"<<endl;
            //if(removingCriterion!=0){  //ModLuc adding of b->isDynamic() to remove only dynamic particles and not already removed particles changed into non dynamic
	    if((removingCriterion!=0)&&(b->isDynamic())){
                switch(removingCriterion){
                    case 1:
                        /* criterion with respect to the particle postion in x direction */
			if(LBbodies[id].pos.x()>(invdx*(sWallXp->se3.position.x())-1.05*MaxBodyRadius/RadFactor)){
                            IdOfNextErodedPtc.push_back(id);
                        }
                        break;
                    case 2:
                        /* criterion on particle velocity */
                        if((LBbodies[id].vel.norm()>VelocityThreshold)||(LBbodies[id].pos.x()>(invdx*(sWallXp->se3.position.x())-2.*LBbodies[id].radius))) {IdOfNextErodedPtc.push_back(id);}
                        break;
                    default:
                            exit(-1);
                            break;
                }
            }
        //NumberOfDynamicParticles++;   //ModLuc: to still count only dynamic particles and not all particles
	if(b->isDynamic()) NumberOfDynamicParticles++;
        }

        LBbodies[id].force=Vector3r::Zero();
        LBbodies[id].momentum=Vector3r::Zero();
    }


    /*------------------------------------------------------------------*/
    /*------------------ detection of boundary nodes -------------------*/
    /*------------------------------------------------------------------*/
    #pragma omp parallel for
    for (int nidx=0; nidx<Nx*Ny; nidx++)
        if(nodes[nidx].isObstacle){
            for(unsigned int n=0;n<nodes[nidx].neighbour_id.size();n++){
                if(nodes[nidx].neighbour_id[n]!=-1){
                    int nidx2=nodes[nidx].neighbour_id[n];
                    if(nodes[nidx].isObstacle!=nodes[nidx2].isObstacle) {
                        nodes[nidx].isObstacleBoundary=true;
                        nodes[nidx2].isFluidBoundary=true;
                        int BodyId=nodes[nidx].body_id;
                        int lid=nodes[nidx].links_id[n];
                        links[lid].isBd=true;
                        links[lid].sid=nidx;
                        links[lid].fid=nidx2;
                        links[lid].idx_sigma_i=opp[n];
                        if(LBbodies[BodyId].isPtc()){
                            links[lid].DistMid= nodes[nidx].posb-0.5*eib[links[lid].idx_sigma_i]-LBbodies[BodyId].pos;
                            links[lid].VbMid=LBbodies[BodyId].vel+LBbodies[BodyId].AVel.cross(links[lid].DistMid);
                            if(links[lid].VbMid.norm()<VbCutOff) links[lid].VbMid=Vector3r::Zero();
                        }
                        if(LBbodies[BodyId].isBox()){
                            links[lid].DistMid= Vector3r::Zero();
                            links[lid].VbMid=Vector3r::Zero();
                        }
                        }
                }
            }
        }
     #pragma omp parallel for
     for (int nidx=0; nidx<Nx*Ny; nidx++)
        if((nodes[nidx].isObstacle)&&(!nodes[nidx].isObstacleBoundary)){
                nodes[nidx].setAsFluid();
                NbSolidNodes--;
                if(firstRun) nodes[nidx].wasObstacle=nodes[nidx].isObstacle;
        }

    NbFluidNodes=NbNodes-NbSolidNodes;
    /*----------------------------------------------------------------------*/

    if(firstRun){

        /*------------------------------------------------------------------*/
        /*------------------------------- MODE -----------------------------*/
        /*------------------------------------------------------------------*/
        if((IterSubCyclingStart<=0) && (IterMax>1) )    MODE = 1;
        if((IterSubCyclingStart<=0) && (IterMax==1) )   MODE = 2;
        if(IterSubCyclingStart>0)                       MODE = 3;

        /*------------------------------------------------------------------*/
        /*---------------------------- SUBCYCLING --------------------------*/
        /*------------------------------------------------------------------*/
        if(MODE==3&&IterMax<IterSubCyclingStart){cerr <<"Exit because Itermax<IterSubCyclingStart"<<endl;exit(-1);}
        if(DemIterLbmIterRatio==-1){
            if(MODE==1) DemIterLbmIterRatio=1;
            else DemIterLbmIterRatio=int(dt/DEMdt0);
            newDEMdt=dt/DemIterLbmIterRatio;
        }else{newDEMdt=dt/DemIterLbmIterRatio;}
        scene->dt=newDEMdt;
        if(SaveMode==2){
            IterSave=TimeSave/(dt);
            if(TimeSave<dt) {cerr <<"Warning SaveMode==2 and TimeSave<dt."<<endl; exit(-1);}
        }
        writelogfile();

        /*------------------------------------------*/
        /* Initialization of distribution functions */
        /*------------------------------------------*/
        for (int nidx=0; nidx<Nx*Ny; nidx++){
            nodes[nidx].rhob=1.;
            nodes[nidx].velb=Vector3r::Zero();
            for (int didx=0; didx<NbDir; didx++){
                cub = 3.0* eib[didx].dot(nodes[nidx].velb);
                feqb = w[didx]*nodes[nidx].rhob*(1.0 + cub + 0.5*(cub*cub) - 1.5*((nodes[nidx].velb.x()*nodes[nidx].velb.x()) + (nodes[nidx].velb.y()*nodes[nidx].velb.y())));
                nodes[nidx].f.push_back(feqb);
                nodes[nidx].fpostcol.push_back(0.);
                nodes[nidx].fprecol.push_back(0.);
            }
        }
        firstRun  = false;
    }

    /*----------------------------------------------------------------------*/
    /*                            FOR ALL ITERATIONS                        */
    /*----------------------------------------------------------------------*/
    Real Error = 1.0;
    DEMdt=scene->dt;
    DEM_TIME=DEM_ITER*DEMdt;
    for (iter=0; iter<IterMax; iter++){
        step += 1;
        LBM_ITER++;
        LBM_TIME=LBM_ITER*dt;

        /*------------------------------------------------------------------*/
        /*         REINITIALIZATION RELATIVE TO THE MODE 1                  */
        /*------------------------------------------------------------------*/
        if(MODE==1){
            FOREACH(const shared_ptr<Body>& b, *scene->bodies){
                if(!b) continue;
                const int id=b->getId();
                LBbodies[id].force=Vector3r::Zero();
                LBbodies[id].momentum=Vector3r::Zero();}
        }

        /*------------------------------------------------------------------*/
        /*                  GENERAL REINITIALIZATION                        */
        /*------------------------------------------------------------------*/
        Vector3r WallBottomVel=Vector3r::Zero();//m s-1
        FmoyCur=0.;
        VmeanFluidC=0.; VmaxC=-1000000.;    VminC=1000000.;
        RhomaxC=-1000000.;  RhominC=1000000.;RhoTot=0.;
        /*------------------------------------------------------------------*/
        /*                          Loop on nodes                           */
        /*------------------------------------------------------------------*/
        #pragma omp parallel for
        for (int nidx=0; nidx<Nx*Ny; nidx++){
            if(nodes[nidx].checkIsNewObstacle()) {newObstacleCells_couter++;}
            else{if(nodes[nidx].checkIsNewFluid()) {newFluidCells_couter++;}}
            if(nodes[nidx].applyBC){
                Vector3r U=Vector3r::Zero();
                Real density=0.;
                /*----------- inlet ------------*/
                if(nodes[nidx].applyXmBC){
                    if(XmBCType==1){
						density=1.0 + dP.x()/(Rho*cs*cs);
                        U=Vector3r(1.0-((nodes[nidx].f[0]+nodes[nidx].f[2]+nodes[nidx].f[4]) +  2.0*(nodes[nidx].f[3]+nodes[nidx].f[6]+nodes[nidx].f[7]))/density,0.,0.);
                    }else if(XmBCType==2){
                        U=Vector3r::Zero();
                        density=(nodes[nidx].f[0]+nodes[nidx].f[2]+nodes[nidx].f[4]+2.*(nodes[nidx].f[3]+nodes[nidx].f[6]+nodes[nidx].f[7]))/(1.-U.x());
                    }
                    nodes[nidx].MixteBC(model,density,U,"Xm");
                /*----------- outlet ------------*/
                }else if( nodes[nidx].applyXpBC){
                    if(XpBCType==1){
                        density=1.0;
                        U=Vector3r(-1.0 + ((nodes[nidx].f[0]+nodes[nidx].f[2]+nodes[nidx].f[4]) +  2.0*(nodes[nidx].f[1]+nodes[nidx].f[5]+nodes[nidx].f[8]))/density,0.,0.);
                    }else if(XpBCType==2){
                        U=Vector3r::Zero();
                        density=(nodes[nidx].f[0]+nodes[nidx].f[2]+nodes[nidx].f[4]+2.*(nodes[nidx].f[1]+nodes[nidx].f[5]+nodes[nidx].f[8]))/(1.+U.x());
                    }
                    nodes[nidx].MixteBC(model,density,U,"Xp");
                /*----------- top ------------*/
                } else if( nodes[nidx].applyYpBC){
                    if(YpBCType==1){
                        density=1.0;
                        U=Vector3r(0.,-1.0 + ((nodes[nidx].f[0]+nodes[nidx].f[1]+nodes[nidx].f[3]) +  2.0*(nodes[nidx].f[2]+nodes[nidx].f[5]+nodes[nidx].f[6]))/density,0.);
                    }else if(YpBCType==2){
                        U=Vector3r::Zero();
                        density=(nodes[nidx].f[0]+nodes[nidx].f[1]+nodes[nidx].f[3]+2.*(nodes[nidx].f[2]+nodes[nidx].f[5]+nodes[nidx].f[6]))/(1.+U.y());
                    }
                    nodes[nidx].MixteBC(model,density,U,"Yp");
                /*----------- bottom ------------*/
                }else if( nodes[nidx].applyYmBC){
                    if(YmBCType==1){
                        density=1.0;
                        U=Vector3r(0.,1.0-((nodes[nidx].f[0]+nodes[nidx].f[1]+nodes[nidx].f[3]) +  2.0*(nodes[nidx].f[4]+nodes[nidx].f[7]+nodes[nidx].f[8]))/density,0.);
                    }else if(YmBCType==2){
                        U=Vector3r::Zero();
                        density=(nodes[nidx].f[0]+nodes[nidx].f[1]+nodes[nidx].f[3]+2.*(nodes[nidx].f[4]+nodes[nidx].f[7]+nodes[nidx].f[8]))/(1.-U.y());
                    }
                    nodes[nidx].MixteBC(model,density,U,"Ym");
                /*----------- bottom-left ------------*/
                }else if(nodes[nidx].applyYmXmBC){
                    if(XmYmZpBCType==1){
                        cerr <<"XmYmZpType=1 not implemented . Exit"<<endl;
                        exit(-1);
                    }else if(XmYmZpBCType==2){
                        U=Vector3r::Zero();
                        density=nodes[nidx+1].rhob;
                    }
                    nodes[nidx].MixteBC(model,density,U,"XmYmZp");
                /*----------- top-left ------------*/
                }else if(nodes[nidx].applyYpXmBC){
                    if(XmYpZpBCType==1){
                        cerr <<"XmYpZpBCType=1 not implemented . Exit"<<endl;
                        exit(-1);
                    }else if(XmYpZpBCType==2){
                        U=Vector3r::Zero();
                        density=nodes[nidx+1].rhob;
                    }
                    nodes[nidx].MixteBC(model,density,U,"XmYpZp");
                /*----------- bottom-right ------------*/
                }else if(nodes[nidx].applyYmXpBC){
                    if(XpYmZpBCType==1){
                        cerr <<"XpYmZpBCType=1 not implemented . Exit"<<endl;
                        exit(-1);
                    }else if(XpYmZpBCType==2){
                        U=Vector3r::Zero();
                        density=nodes[nidx-1].rhob;
                    }
                    nodes[nidx].MixteBC(model,density,U,"XpYmZp");
                /*----------- top-right ------------*/
                }else if(nodes[nidx].applyYpXpBC){
                    if(XpYpZpBCType==1){
                        cerr <<"XpYpZpBCType=1 not implemented . Exit"<<endl;
                        exit(-1);
                    }else if(XpYpZpBCType==2){
                        U=Vector3r::Zero();
                        density=nodes[nidx-1].rhob;
                    }
                    nodes[nidx].MixteBC(model,density,U,"XpYpZp");
                }else{
                    cerr << "ERROR: node "<<nidx<<". Looking for a BC to apply ..."<<endl;
                    exit(-1);
                }
            }

            nodes[nidx].rhob=0.;
            nodes[nidx].velb=Vector3r::Zero();
            nodes[nidx].IsolNb=0;
            if(nodes[nidx].isFluidBoundary){nodes[nidx].IsolNb=8;}

            for (int dndx=0; dndx<NbDir; dndx++){
              nodes[nidx].fprecol[dndx] = nodes[nidx].f[dndx];
              nodes[nidx].velb += eib[dndx]*nodes[nidx].f[dndx];
              nodes[nidx].rhob += nodes[nidx].f[dndx];
              if((nodes[nidx].isFluidBoundary)&&(nodes[nidx].neighbour_id[dndx]!=-1)){
                int ns=nodes[nidx].neighbour_id[dndx];
                if(!nodes[ns].isObstacle) nodes[nidx].IsolNb=nodes[nidx].IsolNb-1;
                if(nodes[nidx].IsolNb<0) {cerr<<"isolNb<0"<<endl;exit(-1);}}

            }
            nodes[nidx].velb /= nodes[nidx].rhob;

            Real temp0=1.5*((nodes[nidx].velb.x()*nodes[nidx].velb.x())+(nodes[nidx].velb.y()*nodes[nidx].velb.y()));
            Real cub0 = 3.0* eib[0].dot(nodes[nidx].velb);
            nodes[nidx].fpostcol[0]= nodes[nidx].f[0] - omega * (nodes[nidx].f[0]-(nodes[nidx].rhob* w[0]*( 1. + cub0 + 0.5*(cub0*cub0) - temp0)));


            nodes[nidx].fpostcol[0] = nodes[nidx].fpostcol[0] + (nodes[nidx].rhob* w[0])/c2 * eib[0].dot(CstBodyForce);
            nodes[nidx].f[0]=nodes[nidx].fpostcol[0];
            RhoTot+=nodes[nidx].rhob;
            if(nodes[nidx].body_id==-1)if(VmaxC<c*nodes[nidx].velb.norm()) VmaxC=c*nodes[nidx].velb.norm();
            if(VminC>c*nodes[nidx].velb.norm()) VminC=c*nodes[nidx].velb.norm();
            if(RhomaxC<Rho*nodes[nidx].rhob)    RhomaxC=Rho*nodes[nidx].rhob;
            if(RhominC>Rho*nodes[nidx].rhob)    RhominC=Rho*nodes[nidx].rhob;
            if(!nodes[nidx].isObstacle)         VmeanFluidC+=c*nodes[nidx].velb.norm();
        }

        #pragma omp parallel for
        for(unsigned int lid=0;lid<links.size();lid++){
            int nidx1 = links[lid].nid1;
            int nidx2 = links[lid].nid2;
            int dndx1 = links[lid].i;
            int dndx2 = opp[links[lid].i];

            /*-------------------------------------------------- ---------------*/
            /* equilibrium functions and collisions                             */
            /*------------------------------------------------------------------*/
            Real temp1=1.5*((nodes[nidx1].velb.x()*nodes[nidx1].velb.x())+(nodes[nidx1].velb.y()*nodes[nidx1].velb.y()));
            Real cub1 = 3.0* eib[dndx1].dot(nodes[nidx1].velb);
            nodes[nidx1].fpostcol[dndx1] = nodes[nidx1].fprecol[dndx1] - omega * (nodes[nidx1].fprecol[dndx1]-(nodes[nidx1].rhob* w[dndx1]*( 1. + cub1 + 0.5*(cub1*cub1) - temp1)));
            nodes[nidx1].fpostcol[dndx1] = nodes[nidx1].fpostcol[dndx1] + (nodes[nidx1].rhob* w[dndx1])/c2 * eib[dndx1].dot(CstBodyForce);
            if(!links[lid].PointingOutside){
                Real temp2=1.5*((nodes[nidx2].velb.x()*nodes[nidx2].velb.x())+(nodes[nidx2].velb.y()*nodes[nidx2].velb.y()));
                Real cub2 = 3.0* eib[dndx2].dot(nodes[nidx2].velb);
                nodes[nidx2].fpostcol[dndx2] = nodes[nidx2].fprecol[dndx2] - omega * (nodes[nidx2].fprecol[dndx2]-(nodes[nidx2].rhob* w[dndx2]*( 1. + cub2 + 0.5*(cub2*cub2) - temp2)));
                nodes[nidx2].fpostcol[dndx2] = nodes[nidx2].fpostcol[dndx2] + (nodes[nidx2].rhob* w[dndx2])/c2 * eib[dndx2].dot(CstBodyForce);
            }

            /*-------------------------------------------------- ---------------*/
            /* Streaming                                                        */
            /*------------------------------------------------------------------*/
            if(links[lid].PointingOutside){
                /// Periodicity is currently disabled until it is implemented through the link list.
///FIXME
                I=nodes[nidx1].i+eib[dndx1].x();
                J=nodes[nidx1].j+eib[dndx1].y();
                if(Xperiodicity){ if (I==Nx) {I=0;} else {if (I==-1) { I=Nx-1;}} }
                if(Yperiodicity){ if (J==Ny) {J=0;} else {if (J==-1) { J=Ny-1;}} }
            }else{
                nodes[nidx1].f[dndx2]=nodes[nidx2].fpostcol[dndx2];
                nodes[nidx2].f[dndx1]=nodes[nidx1].fpostcol[dndx1];
            }

            if(links[lid].isBd==false) continue;

            int idx_sigma_i=links[lid].idx_sigma_i;
            int sid= links[lid].sid;
            int fid= links[lid].fid;
            int BodyId=nodes[sid].body_id;


            /*--- forces and momenta for this boundary link ---*/
            links[lid].ct=3.0*w[idx_sigma_i]*nodes[sid].rhob*eib[links[lid].idx_sigma_i].dot(links[lid].VbMid);
            Vector3r force_ij          = eib[links[lid].idx_sigma_i] * (nodes[fid].fpostcol[idx_sigma_i] - links[lid].ct);
            Vector3r lubforce_ij       = Vector3r::Zero();
            Vector3r totalforce_ij     = force_ij+lubforce_ij;
            Vector3r totalmomentum_ij  = links[lid].DistMid.cross(totalforce_ij);

            /* Sum over all boundary links of all boundary nodes  */
            LBbodies[BodyId].force=LBbodies[BodyId].force+totalforce_ij;
            LBbodies[BodyId].momentum=LBbodies[BodyId].momentum+totalmomentum_ij;

            /*------------------------------------------------------*/
            /*              Modified Bounce back rule               */
            if(nodes[fid].IsolNb>=5) {links[lid].VbMid=Vector3r::Zero();links[lid].ct=0.;}
            nodes[fid].f[opp[idx_sigma_i]]  = nodes[fid].fpostcol[idx_sigma_i] - 2.0*links[lid].ct;
            nodes[sid].f[idx_sigma_i]      = nodes[sid].fpostcol[opp[idx_sigma_i]]+ 2.0*links[lid].ct;
            if( (MODE==2)||((MODE==3)&&(IterMax==1)) ) {links[lid].ReinitDynamicalProperties();}

        }
        VmeanFluidC=VmeanFluidC/NbFluidNodes;


    /*---------------------------------------------*/
    /*     Stop criteria                           */
    /*---------------------------------------------*/
//    if(use_ConvergenceCriterion){
//        switch(ErrorCriterion){
//            case 1:
//                /*--------------------------------------------------------*/
//                /*              Criterion based on the mean force         */
//                /*--------------------------------------------------------*/
//                if((LBM_ITER > 1000) & (LBM_ITER % 10 == 0)){
//                for (int s=NB_WALLS ; s<NB_BODIES; s++)	{FmoyCur = FmoyCur + LBbodies[s].force.norm();}
//                FmoyCur = FmoyCur/(NB_DYNGRAINS);
//                if (FmoyCur!=0.){
//                    Real ErrorA = std::abs(FmoyCur-FmoyPrev)/std::abs(FmoyCur);
//                    Real ErrorB = std::abs(FmoyCur-FmoyPrevPrev)/std::abs(FmoyCur);
//                    Error=max(ErrorA,ErrorB);
//                    FmoyPrevPrev=FmoyPrev;
//                    FmoyPrev=FmoyCur;
//                    }
//                }
//                break;
//            case 2:
//                /*--------------------------------------------------------*/
//                /*           Criterion based on the mean velocity         */
//                /*--------------------------------------------------------*/
//                if((LBM_ITER > 100) & (LBM_ITER % 10 == 0)){
//                if (VmeanFluidC!=0.){
//                    Real ErrorA = std::abs(VmeanFluidC-PrevVmeanFluidC)/std::abs(VmeanFluidC);
//                    //Real ErrorB = std::abs(VmeanFluidC-PrevPrevVmeanFluidC)/std::abs(VmeanFluidC);
//                    //Error=max(ErrorA,ErrorB);
//                    Error= ErrorA;
//                    PrevPrevVmeanFluidC=PrevVmeanFluidC;
//                    PrevVmeanFluidC=VmeanFluidC;
//                    }
//                }
//                break;
//            default:
//                cerr <<"Unknow ErrorCriterion value ! "<<endl;
//                exit(-1);
//                break;
//        }
//    }

    if(LBM_ITER%IterPrint==0){
        cerr.precision(6);
        cerr <<"__________________________________________________________________________"<<endl;
        cerr << "| Run in mode : "<<MODE<<endl;
        cerr <<"| New Obstacle nodes: "<<newObstacleCells_couter<<" New Fluid nodes: "<<newFluidCells_couter<<endl;
        cout <<"| height/width/depth (m) "<<height<<" / "<<width<<" / "<<depth<<endl;
        cerr <<"|------------------------------------------------------------------------|"<<endl;
        cerr <<"| \t\tDEM\t\t | \t\tLBM\t\t |"<<endl;
        cerr <<"|------------------------------------------------------------------------|"<<endl;
        cerr <<"| t (s)\t: "<<DEM_TIME<<"\t\t | t (s)\t\t: "<<LBM_TIME<<"\t\t "<<endl;
        cerr <<"| Iter\t\t: "<<DEM_ITER<<"\t\t | Iter\t\t: "<<LBM_ITER<<"\t\t "<<endl;
        cerr <<"| Nb dyn ptc\t: "<<NumberOfDynamicParticles<<"\t\t | M\t\t: "<<VmaxC/c<<"\t "<<endl;
        cerr <<"| \t\t\t\t | "<<VminC<<"\t<  <V(t)> (m/s)= "<<VmeanFluidC <<"\t< "<<VmaxC<<"\t "<<endl;
        cerr <<"| \t\t\t\t | "<<RhominC<<"\t<  rho(t) (m3/kg) <"<<RhomaxC<<"\t "<<endl;
        cerr <<"| \t\t\t\t | RhoTot (adim) \t: "<<RhoTot<<"\t "<<endl;
        if(ObservedPtc!=-1){
        Vector3r tmp=2.*Rho*c2*dx*LBbodies[ObservedPtc].force;   cerr <<"| \t\t\t\t\t | F (N) \t: "<<tmp<<"\t "<<endl;
        tmp=LBbodies[ObservedPtc].pos*dx;                        cerr <<"| \t\t\t\t\t | pos (m) \t: "<<tmp<<"\t "<<endl;
        tmp=LBbodies[ObservedPtc].vel*c;   cerr <<"| \t\t\t\t\t | VPtc (m/s)\t: "<<tmp<<"\t \t\t\t\t "<<endl;
        }
        if(((MODE==2)||(MODE==3))&&(NB_DYNGRAINS>0)){
        cerr <<"| VminPtcC (m/s)\t: "<<CurMinVelOfPtc<<"\t |\t\t\t\t "<<endl;
        cerr <<"| VmaxPtcC (m/s)\t: "<<CurMaxVelOfPtc<<"\t |\t\t\t\t "<<endl;
        }
        if(  ((MODE==1)&&firstRun) || (MODE==2) || (MODE==3)){
        cerr <<"| \t\t\t\t\t | NbNodes\t: "<<NbNodes<<"\t "<<endl;
        cerr <<"| \t\t\t\t\t | Fluid nodes\t: "<<NbFluidNodes<<"\t\t "<<endl;
        cerr <<"| \t\t\t\t\t | Solid nodes\t: "<<NbSolidNodes<<"\t\t "<<endl;
            }
        if(ObservedNode!=-1){
        cerr <<"| \t\t\t\t\t | Track node\t: "<<ObservedNode<<"\t\t "<<endl;
        cerr <<"| \t\t\t\t\t | \t V(m/s)\t: "<<nodes[ObservedNode].velb.norm()<<"\t\t "<<endl;
        cerr <<"| \t\t\t\t\t | \t rho\t: "<<Rho*nodes[ObservedNode].rhob<<"\t\t "<<endl;
        }
        if(use_ConvergenceCriterion){
        cerr <<"| \t\t\t\t\t | Error\t: "<<Error<<"\t\t "<<endl;
        cerr <<"| \t\t\t\t\t |  with criterion  "<<ErrorCriterion<<" \t\t "<<endl;
        }
        cerr <<"|------------------------------------------------------------------------|"<<endl;
    }


    /*-------------------------------------------------------------------------------*/
    /*------------- RESULT RECORDING DURING COMPUTATION  (MODE 1)--------------------*/
    /*-------------------------------------------------------------------------------*/
    if(((iter % (IterSave*DemIterLbmIterRatio) == 0)||(firstRun))&&MODE==1) {
        if(((iter % (IterSave*SaveGridRatio*DemIterLbmIterRatio) == 0)||(firstRun))&&MODE==1) save(iter,dt);
        saveStats(iter,dt);
        if(SAVE_OBSERVEDPTC) {
            CalculateAndApplyForcesAndTorquesOnBodies(true,false);
            saveObservedPtc(iter,dt);
        }
        if(SAVE_OBSERVEDNODE) saveObservedNode(iter,dt);
    }


    if((IterSubCyclingStart>0)&&(iter+1>=IterSubCyclingStart)) {modeTransition();}
    if ((Error < ConvergenceThreshold)&&(use_ConvergenceCriterion)) {
        if(MODE==1) break;
        if(MODE==3) modeTransition();
    }
    if((EndTime>0)&&(LBM_TIME>EndTime)) LbmEnd();
    //if((InitialNumberOfDynamicParticles!=0)&&(NumberPtcEroded==InitialNumberOfDynamicParticles)) LbmEnd();
}
/*------------------- End of the LBM loop (iterations) --------------------*/


if(MODE==1) {
    cerr << "LBM ended after " << step << " iterations";
    cerr <<" | LBM_ITER = "<< LBM_ITER<<" | Error = " << Error << endl;
}

/*--------------------------------------------------------------------------------*/
/*-------------- APPLICATION OF HYDRODYNAMIC FORCES ON SPHERES -------------------*/
/*--------------------------------------------------------------------------------*/
if(applyForcesAndTorques) CalculateAndApplyForcesAndTorquesOnBodies(true,true);

/*----------------------------------------------------------------------------*/
/*----------------- SPHERES ARE MOVED OUTSIDE THE SIMULATION DOMAIN --------- */
/*----------------------------------------------------------------------------*/
//if(removingCriterion!=0){
//    for(unsigned int aa=0; aa<IdOfNextErodedPtc.size();aa++){
//        int IdRemoved=IdOfNextErodedPtc[aa];
//        LBbodies[IdRemoved].isEroded=true;
//        shared_ptr<Body> b=Body::byId(IdRemoved);
//        const shared_ptr<Sphere>& sphere = YADE_PTR_CAST<Sphere> ( b->shape );
//        Real r=sphere->radius;
//        outside_limit=outside_limit+1.1*r;
//        b->state->pos=Vector3r(outside_limit,0.,0.);
//        b->setDynamic(false);
//        NB_DYNGRAINS--;
//        NB_DYNBODIES--;
//        outside_limit=outside_limit+1.1*r;
//        cerr <<"Eroded Ptc: "<<IdRemoved<<endl;
//        NumberPtcEroded+=1;
//        if(!strcmp(model.c_str(), "d2q9" )) Vr += Mathr::PI*(r*r);
//        else Vr += 4./3.*Mathr::PI*(r*r*r);
//    }
//    if(IdOfNextErodedPtc.size()!=0) saveEroded(DEM_ITER,DEMdt);
//}

/*----------------------------------------------------------------------------*/
/*------------- RESULT RECORDING AT THE END OF THE COMPUTATION ---------------*/
/*----------------------------------------------------------------------------*/
 if(((DEM_ITER % (IterSave*DemIterLbmIterRatio) == 0)||(firstRun))&&IterMax==1 ){
    if(((DEM_ITER % (IterSave*SaveGridRatio*DemIterLbmIterRatio) == 0)||(firstRun))&&IterMax==1 ) save(DEM_ITER,DEMdt);
    saveStats(DEM_ITER,DEMdt);
    //saveEroded(DEM_ITER,DEMdt);
    if(SAVE_OBSERVEDPTC) saveObservedPtc(DEM_ITER,DEMdt);
    if(SAVE_OBSERVEDNODE) saveObservedNode(DEM_ITER,DEMdt);
    }
}

void HydrodynamicsLawLBM::save(int iter_number, Real timestep)
{
    
    /*--------------------------------------------*/
    /*   Recording of properties of LBM nodes     */
    /*--------------------------------------------*/

    bool saveLBMdata = false;  // Flag to avoid to loop over all the lbm node if no lbm data are saved but only spheres data

    std::stringstream Vfile_name;
    std::stringstream Vxfile_name; 
    std::stringstream Vyfile_name;
    std::ofstream Vfile;
    std::ofstream Vxfile;
    std::ofstream Vyfile;

    std::stringstream FxLBMfile_name;
    std::stringstream FyLBMfile_name;
    std::stringstream MzLBMfile_name;
    std::ofstream FxLBMfile;
    std::ofstream FyLBMfile;
    std::ofstream MzLBMfile;

    std::stringstream Rhofile_name;
    std::ofstream Rhofile;

    std::stringstream Bodiesfile_name;
    std::ofstream Bodiesfile;

    std::stringstream NodeBoundaryfile_name;
    std::ofstream NodeBoundaryfile;

    std::stringstream NewNodefile_name;
    std::ofstream NewNodefile;
	

    /*--- VELOCITIES ---*/
    if(SAVE_VELOCITY){
    	Vfile_name<<lbm_dir<<"/V"<<"_"; Vfile_name.width(10);
	Vfile_name.fill('0');   Vfile_name<<iter_number;
	Vfile.open(Vfile_name.str().c_str());
	
	saveLBMdata = true;
    }

    if(SAVE_VELOCITYCOMP){
    	Vxfile_name<<lbm_dir<<"/Vx"<<"_";Vxfile_name.width(10);
    	Vyfile_name<<lbm_dir<<"/Vy"<<"_";Vyfile_name.width(10);
    
    	Vxfile_name.fill('0');  Vxfile_name<<iter_number;
    	Vyfile_name.fill('0');  Vyfile_name<<iter_number;
    
	Vxfile.open(Vxfile_name.str().c_str());Vyfile.open(Vyfile_name.str().c_str());

	saveLBMdata = true;
    }

    /*--- FORCES ---*/
    if(SAVE_FORCES){
    	FxLBMfile_name<<lbm_dir<<"/Fx"<<"_";FxLBMfile_name.width(10);
    	FyLBMfile_name<<lbm_dir<<"/Fy"<<"_";FyLBMfile_name.width(10);
    	MzLBMfile_name<<lbm_dir<<"/Mz"<<"_";MzLBMfile_name.width(10);

    	FxLBMfile_name.fill('0'); FxLBMfile_name<<iter_number;
    	FyLBMfile_name.fill('0'); FyLBMfile_name<<iter_number;
    	MzLBMfile_name.fill('0'); MzLBMfile_name<<iter_number;

	FxLBMfile.open(FxLBMfile_name.str().c_str());
        FyLBMfile.open(FyLBMfile_name.str().c_str());
        MzLBMfile.open(MzLBMfile_name.str().c_str());

	saveLBMdata = true;
    }

    /*--- DENSITY ---*/
    if(SAVE_RHO){	
    	Rhofile_name<<lbm_dir<<"/Rho"<<"_";Rhofile_name.width(10);
    	Rhofile_name.fill('0'); Rhofile_name<<iter_number;
	Rhofile.open(Rhofile_name.str().c_str());

	saveLBMdata = true;
    }

    /*--- BODIES ---*/
    if(SAVE_BODIES){
	Bodiesfile_name<<lbm_dir<<"/Bodies"<<"_";Bodiesfile_name.width(10);
    	Bodiesfile_name.fill('0'); Bodiesfile_name<<iter_number;
	Bodiesfile.open(Bodiesfile_name.str().c_str());

	saveLBMdata = true;
    }

    /*--- BOUNDARY NODES ---*/
    if(SAVE_NODEBD){	
    	NodeBoundaryfile_name<<lbm_dir<<"/NodeBoundary"<<"_";
    	NodeBoundaryfile_name.width(10);
    	NodeBoundaryfile_name.fill('0');    NodeBoundaryfile_name<<iter_number;
	NodeBoundaryfile.open(NodeBoundaryfile_name.str().c_str());

	saveLBMdata = true;
    }

    /*--- NEW NODES ---*/	
    if(SAVE_NODEISNEW){	
    	NewNodefile_name<<lbm_dir<<"/NewNode"<<"_";
    	NewNodefile_name.width(10);
    	NewNodefile_name.fill('0');            NewNodefile_name<<iter_number;
	NewNodefile.open(NewNodefile_name.str().c_str());

	saveLBMdata = true;
    }



    if(saveLBMdata){  //Condition to avoid to loop over all the lbm node if no lbm data are saved but only spheres data
	
	cerr << "| Saving ("<<iter_number<<")"<<endl;

    	for (int nidx=0; nidx<Nx*Ny; nidx++){
        	if(SAVE_BODIES) Bodiesfile <<nodes[nidx].body_id<<" ";
        	if(SAVE_VELOCITY)  Vfile<< c*nodes[nidx].velb.norm()<< " ";
        	if(SAVE_VELOCITYCOMP){Vxfile<< c*nodes[nidx].velb.x()<< " ";Vyfile<< c*nodes[nidx].velb.y()<< " ";}
        	if(SAVE_FORCES){
        	    if(nodes[nidx].body_id>=0){
        	        FxLBMfile<< 2.*Rho*c2*dx*(LBbodies[nodes[nidx].body_id].force.x()) << " ";
        	        FyLBMfile<< 2.*Rho*c2*dx*(LBbodies[nodes[nidx].body_id].force.y()) << " ";
        	        MzLBMfile<< 2.*Rho*c2*dx2*LBbodies[nodes[nidx].body_id].momentum.z() << " ";
        	    }
            	    else{FxLBMfile<< 0. << " ";FyLBMfile<< 0. << " ";MzLBMfile<< 0. << " ";}}
        	if(SAVE_RHO) Rhofile	<< Rho*nodes[nidx].rhob 	<< " ";
        	if(SAVE_NODEBD) {
            		int tmp=0;
            		if(nodes[nidx].isObstacleBoundary) tmp=nodes[nidx].isObstacleBoundary;
            		if(nodes[nidx].isFluidBoundary)    tmp= -1;
            		NodeBoundaryfile	<< tmp	<< " ";}
       		if(SAVE_NODEISNEW) {
            		int tmp=0;
            		if(nodes[nidx].isNewObstacle) tmp=nodes[nidx].isNewObstacle;
            		if(nodes[nidx].isNewFluid)    tmp=-1;
            		NewNodefile	<< tmp << " ";}

        	if(nodes[nidx].i==Nx-1){
            		if(SAVE_BODIES) Bodiesfile<< endl;
            		if(SAVE_VELOCITY)  Vfile<< endl;
            		if(SAVE_VELOCITYCOMP){Vxfile<< endl;Vyfile<< endl;}
            		if(SAVE_FORCES){FxLBMfile << endl;FyLBMfile << endl;MzLBMfile << endl;}
            		if(SAVE_RHO) Rhofile << endl;
            		if(SAVE_NODEBD) NodeBoundaryfile << endl;
            		if(SAVE_NODEISNEW) NewNodefile	<< endl;
       		}
    	}
    }

    std::stringstream cmd;
    cmd<<"bzip2";
    if(SAVE_BODIES) {Bodiesfile.close();cmd<<" "<<Bodiesfile_name.str().c_str();}
    if(SAVE_VELOCITY) {Vfile.close();cmd<<" "<<Vfile_name.str().c_str();}
    if(SAVE_VELOCITYCOMP) {Vxfile.close(); Vyfile.close();cmd<<" "<<Vxfile_name.str().c_str()<<" "<<Vyfile_name.str().c_str();}
    if(SAVE_FORCES){
        FxLBMfile.close();FyLBMfile.close();MzLBMfile.close();
        cmd<<" "<<FxLBMfile_name.str().c_str()<<" "<<FyLBMfile_name.str().c_str()<<" "<<MzLBMfile_name.str().c_str();}
    if(SAVE_RHO) {Rhofile.close();cmd<<" "<<Rhofile_name.str().c_str();}
    if(SAVE_NODEBD) {NodeBoundaryfile.close();cmd<<" "<<NodeBoundaryfile_name.str().c_str();}
    if(SAVE_NODEISNEW) {NewNodefile.close();cmd<<" "<<NewNodefile_name.str().c_str();}
    //cmd<<"&";
    if(COMPRESS_DATA) {if(std::system(cmd.str().c_str())) cerr<<"bzip error"<<endl;}


    /*--------------------------------------------*/
    /*   Recording of properties of DEM objects   */
    /*--------------------------------------------*/
    if(SAVE_SPHERES){   //condition to save spheres properties only if it is required by the operator, checking if NB_GRAINS > 0 is done earlier when recording configuration is done
    	spherefile_name.str("");
   	spherefile_name<<dem_dir<<"/spheres";
   	cerr <<" Write DEM data in "<<spherefile_name.str().c_str()<<endl;
        spherefile_name<<"_";
        spherefile_name.width(10);
        spherefile_name.fill('0');
        spherefile_name<<iter_number;
        std::ofstream spherefile;
        spherefile.open(spherefile_name.str().c_str());

        for (unsigned int l=0;l<LBbodies.size();l++){
            if(LBbodies[l].isPtc()){
                if((!LBbodies[l].isEroded)&&(LBbodies[l].saveProperties)){
                    spherefile<<l<<" ";                                                                                      /* Id*/
                    spherefile<<dx*LBbodies[l].pos.x()<<" "<<dx*LBbodies[l].pos.y()<<" "<<dx*LBbodies[l].pos.z()<<" ";             /* x, y, z*/
                    spherefile<<dx*LBbodies[l].radius<<" ";                                                                    /* r*/
                    spherefile<<c*LBbodies[l].vel.x()<<" "<<c*LBbodies[l].vel.y()<<" "<<c*LBbodies[l].vel.z()<<" ";                /* Vx, Vy, Vz*/
                    spherefile<<invdt*LBbodies[l].AVel.x()<<" "<<invdt*LBbodies[l].AVel.y()<<" "<<invdt*LBbodies[l].AVel.z()<<" "; /* Wx, Wy, Wz*/
                    spherefile<<LBbodies[l].Fh.x()<<" "<<LBbodies[l].Fh.y()<<" "<<LBbodies[l].Fh.z()<<" ";                         /* Fhx, Fhy, Fhz */
                    spherefile<<LBbodies[l].Mh.x()<<" "<<LBbodies[l].Mh.y()<<" "<<LBbodies[l].Mh.z()<<endl;                        /* Mhx, Mhy, Mhz */
                }
            }
        }
    spherefile.close();
    }

#ifdef LBM_VERBOSE
cerr <<"END: HydrodynamicsLawLBM::save()"<<endl;
#endif
return;
}

void HydrodynamicsLawLBM::saveStats(int iter_number, Real timestep)
{
    #ifdef LBM_VERBOSE
    cerr <<"START: HydrodynamicsLawLBM::saveStats()"<<endl;
    #endif
    cerr << "| Save stats ..."<<endl;
    ofstream file(LBMmachFile.c_str(), ios::app);
    file <<iter_number<<" "<<iter_number*timestep<<" "<<VmaxC<<" "<<VmaxC/c<<endl;
    #ifdef LBM_VERBOSE
    cerr <<"END: HydrodynamicsLawLBM::saveStats()"<<endl;
    #endif
    return;
}
//void HydrodynamicsLawLBM::saveEroded(int iter_number, Real timestep)
//{
//
//    cerr << "| Save Eroded Ptc ..."<<endl;
//    ofstream file(RemovedPtcFile.c_str(), ios::app);
//    file <<iter_number<<" "<<iter_number*timestep<<" "<<NumberPtcEroded<<" "<<Vr<<" "<<Vr/Vo<<" "<<FhTotale<<endl;
//
//    return;
//}
void HydrodynamicsLawLBM::saveObservedPtc(int iter_number, Real timestep)
{

    cerr << "| Save Observed Ptc ..."<<endl;
    ofstream file(ObservedPtcFile.c_str(), ios::app);
    file <<iter_number<<" "<<iter_number*timestep<<" ";
    file <<dx*LBbodies[ObservedPtc].pos.x()<<" "<<dx*LBbodies[ObservedPtc].pos.y()<<" "<<dx*LBbodies[ObservedPtc].pos.z()<<" ";
    file <<dx*LBbodies[ObservedPtc].radius<<" ";
    file <<c*LBbodies[ObservedPtc].vel.x()<<" "<<c*LBbodies[ObservedPtc].vel.y()<<" "<<c*LBbodies[ObservedPtc].vel.z()<<" ";
    file <<invdt*LBbodies[ObservedPtc].AVel.x()<<" "<<invdt*LBbodies[ObservedPtc].AVel.y()<<" "<<invdt*LBbodies[ObservedPtc].AVel.z()<<" ";
    file <<LBbodies[ObservedPtc].Fh.x()<<" "<<LBbodies[ObservedPtc].Fh.y()<<" "<<LBbodies[ObservedPtc].Fh.z()<<" ";
    file <<LBbodies[ObservedPtc].Mh.x()<<" "<<LBbodies[ObservedPtc].Mh.y()<<" "<<LBbodies[ObservedPtc].Mh.z()<<endl;

    return;
}
void HydrodynamicsLawLBM::saveObservedNode(int iter_number, Real timestep)
{
    return;
}
/*
_________________________________________________________________________
*/

void HydrodynamicsLawLBM::createNewFiles()
{

    //spherefile_name<<dem_dir<<"/spheres"; //Useless here

    ofstream file(LBMmachFile.c_str());
    file <<"#iter_number\t time\t VmaxC\t VmaxC/c"<<endl;
    file.close();

    if(removingCriterion!=0){
        ofstream file2(RemovedPtcFile.c_str());
        file2 <<"#Iter time NumberPtcEroded Vr Vr/Vo FhTotale"<<endl;
        file2.close();
    }

    if(SAVE_CONTACTINFO){
        ofstream file3(LBMcontactsFile.c_str());
        file3 <<"#Iter time NumberOfContact"<<endl;
        file3.close();
    }
    //if(NB_GRAINS>0) {ofstream file3(spherefile_name.str().c_str());file3.close();}  //For what this line is used for? It seems to work without
    
    if(SAVE_OBSERVEDPTC){  //Condition to create observedPtc file only if the recording is required by the operator
    	ofstream file4(ObservedPtcFile.c_str());
    	file4 <<"#iter t x y z r Vx Vy Vz Wx Wy Wz Fx Fy Fz Mx My Mz"<<endl;
    	file4.close();
    }

    if(SAVE_OBSERVEDNODE){ //Condition to create observedNode file only if the recording is required by the operator
    	ofstream file5(ObservedNodeFile.c_str());file5.close();
    }
    

    return;
}

void HydrodynamicsLawLBM::createDirectories(bool dirLBM, bool dirDem, bool dirCntct)
{

    //bfs::create_directory(bfs::path(lbm_dir));  //ModLuc: to create only necessary directory
    if(dirLBM) bfs::create_directory(bfs::path(lbm_dir));
    //if(NB_GRAINS>0) bfs::create_directory(bfs::path(dem_dir));  //ModLuc: to create only necessary directory
    if(dirDem) bfs::create_directory(bfs::path(dem_dir)); 
    //if(SAVE_CONTACTINFO) bfs::create_directory(bfs::path(cntct_dir)); //ModLuc: to create only necessary directory
    if(dirCntct) bfs::create_directory(bfs::path(cntct_dir));

    return;
}

void HydrodynamicsLawLBM::writelogfile()
{
    ofstream file(LBMlogFile.c_str());
    file <<"File format: 1"<<endl;
    file <<"System parameters: "<<endl;
        file <<"\t Lx0= "<<Lx0<<endl;
        file <<"\t Ly0= "<<Ly0<<endl;
        file <<"\t Lz0= "<<Lz0<<endl;
        file <<"\t Wallthickness= "<<Wallthickness<<endl;
        file <<"\t dP= "<<dP<<endl;
        file <<"\t Nu= "<<Nu<<endl;
        file <<"\t Rho= "<< Rho<<endl;
        file <<"\t dx= "<< dx<<endl;
        file <<"\t Nx= "<< Nx<<endl;
        file <<"\t Ny= "<< Ny<<endl;
        file <<"\t Nz= "<< Nz<<endl;
    file <<"LBM parameters: "<<endl;
        file <<"\t tau= "<<tau<<" omega= "<< omega <<endl;
        file <<"\t IterMax= "<<IterMax<<endl;
        file <<"\t SaveMode= "<<SaveMode<<endl;
        file <<"\t IterSave= "<<IterSave<<endl;
        file <<"\t SaveGridRatio= "<<SaveGridRatio<<endl;
        file <<"\t DemIterLbmIterRatio= "<<DemIterLbmIterRatio<<endl;
        file <<"\t ConvergenceThreshold= "<<ConvergenceThreshold<<endl;
        file <<"\t Predicted Mach number (may be false)= "<<UMaxtheo/c<<endl;
        file <<"\t LBM dt= "<<dt<<endl;
    file <<"DEM parameters: "<<endl;
        file <<"\t DEM dt= "<<DEMdt0<<endl;
        file <<"\t DEM adjusted dt= "<<newDEMdt<<endl;
    file <<"Particles: "<<endl;
        file <<"\t InitialNumberOfDynamicParticles= "<<InitialNumberOfDynamicParticles<<endl;
        file <<"\t NB_BODIES= "<<NB_BODIES<<" NB_GRAINS= "<<NB_GRAINS<<endl;
        file <<"\t NB_DYNBODIES= "<<NB_DYNBODIES<<" NB_DYNGRAINS= "<<NB_DYNGRAINS<<endl;
        file <<"\t Rmin / Rmax / Rmean = "<<dx*MinBodyRadius<<" / "<<dx*MaxBodyRadius<<" / "<<dx*MeanBodyRadius<<endl;
        if(NB_GRAINS>0) file <<"\t NbNodePerPtc= "<<NbParticleNodes/NB_GRAINS<<endl;
        else file <<"\t NbNodePerPtc= "<<-1<<endl;
        file <<"\t Vo= "<<Vo<<endl;
    file <<"Misc :"<<endl;
        file <<"\t VbCutOff= "<<VbCutOff<<endl;
        //file <<"\t CstBodyForceDensity= "<<CstBodyForceDensity<<endl;
    file <<"Memory usage"<<endl;
        file <<"\t Nodes= "<<nodes.size()<<endl;
        file <<"\t links= "<<links.size()<<endl;

    file.close();
    return;
}

void HydrodynamicsLawLBM::modeTransition(){
    cerr << "Mode transition "<<endl;
    IterMax=1;
    IterSubCyclingStart=-1;
    LBM_ITER=1;
    use_ConvergenceCriterion=false;
    return;
}

void HydrodynamicsLawLBM::LbmEnd(){
    if(MODE==1) IterMax=iter;
    Omega::instance().stop();
    Omega::instance().saveSimulation ("end.xml");
}

void HydrodynamicsLawLBM::CalculateAndApplyForcesAndTorquesOnBodies(bool mean,bool apply){
    /*--------------------------------------------------------------------------------*/
    /*---------------- APPLICATION OF HYDRODYNAMIC FORCES ON SPHERES -----------------*/
    /*--------------------------------------------------------------------------------*/
    if(mean) FhTotale=Vector3r::Zero();
    FOREACH(const shared_ptr<Body>& b, *scene->bodies){
        if(!b) continue;
        const int id=b->getId();
            //if ( ((b->isDynamic())&&(b->shape->getClassName()=="Sphere")) || (b->shape->getClassName()=="Box") ){  //ModLuc: remove the condition (b->isDynamic()) to be able to apply force and torque on non dynamic bodies, by this way hydrodynamic force and torque on bodies can be read through python even if bodies are non dynamic.
            if ( (b->shape->getClassName()=="Sphere") || (b->shape->getClassName()=="Box") ){
	    if(mean){
                LBbodies[id].fp=LBbodies[id].force;
                LBbodies[id].force=0.5*(LBbodies[id].fp+LBbodies[id].fm);
                LBbodies[id].fm=LBbodies[id].fp;
                LBbodies[id].mp=LBbodies[id].momentum;
                LBbodies[id].momentum=0.5*(LBbodies[id].mp+LBbodies[id].mm);
                LBbodies[id].mm=LBbodies[id].mp;
                LBbodies[id].Fh=2.*Rho*c2*dx*LBbodies[id].force;
                LBbodies[id].Mh=2.*Rho*c2*dx2*LBbodies[id].momentum;
                FhTotale=FhTotale+LBbodies[id].Fh;
            }
            if(apply){
                scene->forces.addForce(id, LBbodies[id].Fh);
                scene->forces.addTorque(id, LBbodies[id].Mh);
            }
        }
    }
    return;
}
YADE_PLUGIN((HydrodynamicsLawLBM));

#endif //LBM_ENGINE

