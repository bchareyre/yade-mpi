/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "STLImporter.hpp"
#include <yade/extra/Shop.hpp>

CREATE_LOGGER(STLImporter);

STLImporter::STLImporter()  
{
    wire=false;
	number_of_facets=0;
}
 
bool STLImporter::open(const char* filename)
{
    vector<double> vtmp, ntmp;
    vector<int>  etmp, ftmp;
    STLReader::tolerance = Math<Real>::ZERO_TOLERANCE;
    STLReader reader;
    if(!reader.open(filename, back_inserter(vtmp), back_inserter(etmp), back_inserter(ftmp), back_inserter(ntmp))) 
	{
		LOG_ERROR("Can't open file: " << filename);
		return false;
	}
	
	for(int i=0,e=ftmp.size(); i<e; ++i)
		tr.push_back(Vector3r(vtmp[3*ftmp[i]],vtmp[3*ftmp[i]+1],vtmp[3*ftmp[i]+2])); 
	number_of_facets = tr.size()/3;
	LOG_INFO("Open file: " << filename << ": Number of facets: " << number_of_facets);
	return true;
}

void STLImporter::import(shared_ptr<BodyContainer> bodies, unsigned int begin)
{
	unsigned int b_id = begin;
	for(int i=0,e=tr.size(); i<e; i+=3)
	{
		Vector3r v[3]={tr[i],tr[i+1],tr[i+2]};

		Vector3r icc = Shop::inscribedCircleCenter(v[0],v[1],v[2]);

		shared_ptr<InteractingFacet> iFacet(new InteractingFacet);
		iFacet->diffuseColor    = Vector3r(0.8,0.3,0.3);

		shared_ptr<Facet> gFacet(new Facet);
		gFacet->diffuseColor    = Vector3r(0.5,0.5,0.5);
		gFacet->wire	    = wire;
		gFacet->visible	    = true;
		gFacet->shadowCaster    = true;

		for (int j=0; j<3; ++j)
		{   
				iFacet->vertices.push_back(v[j]-icc);
				gFacet->vertices.push_back(v[j]-icc);
		}

		(*bodies)[b_id]->physicalParameters->se3 = Se3r( icc, Quaternionr( 1,0,0,0 ) );
		(*bodies)[b_id]->geometricalModel	= gFacet;
		(*bodies)[b_id]->interactingGeometry	= iFacet;

		++b_id;
	}
}
