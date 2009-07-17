/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMTetrahedronData.hpp"
#include "FEMNodeData.hpp"


FEMTetrahedronData::FEMTetrahedronData() : PhysicalParameters()
{ 
	createIndex();
	ids.clear();
}


FEMTetrahedronData::~FEMTetrahedronData()
{

}



void FEMTetrahedronData::calcKeMatrix(MetaBody* femBody)
{
	ublas::matrix<Real> nodesCoordinates;
	nodesCoordinates.resize(3,4);
	nodesCoordinates.clear();
	
	nodesCoordinates(0,0) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[0]]->physicalParameters.get())->initialPosition[0];
	nodesCoordinates(1,0) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[0]]->physicalParameters.get())->initialPosition[1];
	nodesCoordinates(2,0) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[0]]->physicalParameters.get())->initialPosition[2];

	nodesCoordinates(0,1) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[1]]->physicalParameters.get())->initialPosition[0];
	nodesCoordinates(1,1) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[1]]->physicalParameters.get())->initialPosition[1];
	nodesCoordinates(2,1) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[1]]->physicalParameters.get())->initialPosition[2];

	nodesCoordinates(0,2) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[2]]->physicalParameters.get())->initialPosition[0];
	nodesCoordinates(1,2) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[2]]->physicalParameters.get())->initialPosition[1];
	nodesCoordinates(2,2) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[2]]->physicalParameters.get())->initialPosition[2];

	nodesCoordinates(0,3) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[3]]->physicalParameters.get())->initialPosition[0];
	nodesCoordinates(1,3) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[3]]->physicalParameters.get())->initialPosition[1];
	nodesCoordinates(2,3) = YADE_CAST<FEMNodeData*>((*(femBody->bodies))[ids[3]]->physicalParameters.get())->initialPosition[2]; 
	
	localCalcKeMatrix(nodesCoordinates);
	
//	std::cerr << Ke_ << std::endl;
}

void FEMTetrahedronData::localCalcKeMatrix( ublas::matrix<Real>& coord )
{
	double h1, h2, G, h3;
	double Youn, nu, rho, Damp;
// FIXME - those are parameters
	Youn = 150; // young modulus I guess?
	nu = 0.28 ; // and what is this one?
	rho = 1; // what is this ? density maybe?
	Damp = 1; // how this one works?


//	Youn = 1; // young modulus I guess?
//	nu = 0.28 ; // and what is this one?
//	rho = 1; // what is this ?
//	Damp = 1; // how this one works?

//	Youn = 210E9;
//	nu = 0.28 ;
//	rho = 7800;
//	Damp = 1000;
	
	h1=Youn/((1+nu)*(1-nu));
	h2=nu*h1;
	G=Youn/(2*(1+nu));
	ublas::matrix<double> H2;
	H2.resize(3,3);
	H2.clear();
	H2(0,0)=h1;
	H2(0,1)=h2;
	H2(0,2)=0;
	H2(1,0)=h2;
	H2(1,1)=h1;
	H2(1,2)=0;
	H2(2,0)=0;
	H2(2,1)=0;
	H2(2,2)=G;
	h1=(Youn*(1-nu))/((1+nu)*(1-2*nu));
	h2=(Youn*nu)/((1+nu)*(1-2*nu));
	h3=(Youn*0.5)/((1+nu));
	ublas::matrix<double> H3;
	H3.resize(6,6);
	H3.clear();
	for (int i=0 ; i<3; ++i)
		for (int j=0 ; j<3; ++j)
			H3(i,j)=h2;
	for (int i=0 ; i<3; ++i)
	{	//for (int j=0 ; j<2; ++j)
		H3(i,i)=h1;
		H3(i+3,i+3)=h3;
	}
	ublas::matrix<double> nialpha;
	nialpha.resize(4,3);
	nialpha.clear();
	for (int i=0 ; i<3; ++i)
		nialpha(0,i)=-1;
	nialpha(1,0)=1;
	nialpha(2,1)=1;
	nialpha(3,2)=1;
	ublas::matrix<double> jac;
	jac.resize(3,3);
	jac.clear();
	jac = prod(coord,nialpha);
	double det  ;
	det = jac(0,0) * jac(1,1) * jac(2,2) + jac(0,1) * jac(1,2) *jac(2,0) + jac(1,0) * jac(2,1) *jac(0,2) - jac(0,0) * jac(2,1) *jac(1,2) - jac(1,0) * jac(0,1) *jac(2,2) - jac(2,0) * jac(1,1) *jac(0,2);
	double volume = det/6 ;
	if (volume<0)
	{volume = -volume;}
	ublas::matrix<double> invjac ;
	invjac.resize( 3,3);
	invjac.clear();
	invjac(0,0) = 1/det * (jac(1,1) * jac(2,2) - jac(1,2) * jac(2,1) );
	invjac(0,1) = -1/det * (jac(1,0) * jac(2,2) - jac(1,2) * jac(2,0) );
	invjac(0,2) = 1/det * (jac(1,0) * jac(2,1) - jac(1,1) * jac(2,0) );
	invjac(1,0) = -1/det * (jac(0,1) * jac(2,2) - jac(0,2) * jac(2,1) );
	invjac(1,1) = 1/det * (jac(0,0) * jac(2,2) - jac(0,2) * jac(2,0) );
	invjac(1,2) = -1/det * (jac(0,0) * jac(2,1) - jac(0,1) * jac(2,0) );
	invjac(2,0) = 1/det * (jac(0,1) * jac(1,2) - jac(0,2) * jac(1,1) );
	invjac(2,1) = -1/det * (jac(0,0) * jac(1,2) - jac(0,2) * jac(1,0) );
	invjac(2,2) = 1/det * (jac(0,0) * jac(1,1) - jac(0,1) * jac(1,0) );
	ublas::matrix<double> invJacT;
	invJacT.resize( 3,3);
	invJacT.clear();
	for (int i = 0 ; i<3 ; i++)
		for (int j = 0 ; j<3 ; j++)
			invJacT(i,j) = invjac (j,i);
//	ublas::matrix<double> toto ;
//	toto.resize( 3,3);
	ublas::matrix<double> resultinv ;
	resultinv.resize( 3,3);
	ublas::matrix<double> verifInv2 ;
	verifInv2.resize( 3,3);
	verifInv2 = prod (invJacT, jac);
	for (int i = 0 ; i<3 ; i++)
		for (int j = 0 ; j<3 ; j++)
			resultinv(i,j) = verifInv2(i,j);//-toto(i,j);
	ublas::matrix<double> nix;
	nix.resize(  4,3);
	nix.clear();
	nix= prod (nialpha , invJacT);
	ublas::matrix<double> be;
	be.resize( 6,12);
	be.clear();
	for (int l = 0 ; l < 4  ;l++)
	{	be(0,l*3)=nix(l,0);
		be(3,l*3)=nix(l,1);
		be(4,l*3)=nix(l,2);
		be(1,l*3+1)=nix(l,1);
		be(3,l*3+1)=nix(l,0);
		be(5,l*3+1)=nix(l,2);
		be(2,l*3+2)=nix(l,2);
		be(4,l*3+2)=nix(l,0);
		be(5,l*3+2)=nix(l,1);
	}
	ublas::matrix<double> beTranspose;
	beTranspose.resize( 12,6);
	beTranspose.clear();
	for (int i = 0 ; i<12 ; i++)
		for (int j = 0 ; j<6 ; j++)
		{beTranspose(i,j) = be (j,i);}
	ublas::matrix<double> ke1;
	ke1.resize( 12,12);
	ke1.clear();
	Ke_.resize( 12,12);
	Ke_.clear();
	ke1 = prod(H3, be);
	Ke_ = volume * prod (beTranspose , ke1);
	// Concentred mass
	mass = rho * volume / 4 ;
	// Concentred damping
	damping = Damp * mass ;
}

YADE_PLUGIN();
