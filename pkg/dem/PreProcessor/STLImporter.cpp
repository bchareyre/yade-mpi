/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include"STLImporter.hpp"
#include<yade/pkg-dem/Shop.hpp>
#include<yade/lib-import/STLReader.hpp>

CREATE_LOGGER(STLImporter);

vector<shared_ptr<Body> > STLImporter::import(const char* filename)
{
	vector<Vector3r> tr; 
	vector<shared_ptr<Body> > imported;

	// Load geometry
    vector<double> vtmp, ntmp; vector<int>  etmp, ftmp;
    STLReader reader; reader.tolerance=Math<Real>::ZERO_TOLERANCE;
    if(!reader.open(filename, back_inserter(vtmp), back_inserter(etmp), back_inserter(ftmp), back_inserter(ntmp))) 
	{
		LOG_ERROR("Can't open file: " << filename);
		return imported; // zero size
	}
	for(int i=0,e=ftmp.size(); i<e; ++i)
		tr.push_back(Vector3r(vtmp[3*ftmp[i]],vtmp[3*ftmp[i]+1],vtmp[3*ftmp[i]+2])); 

	// Create facets
	for(int i=0,e=tr.size(); i<e; i+=3)
	{
		Vector3r v[3]={tr[i],tr[i+1],tr[i+2]};
		Vector3r icc = Shop::inscribedCircleCenter(v[0],v[1],v[2]);
		shared_ptr<InteractingFacet> iFacet(new InteractingFacet);
		iFacet->diffuseColor    = Vector3r(0.8,0.3,0.3);
		#ifdef YADE_GEOMETRICALMODEL
			shared_ptr<FacetModel> gFacet(new FacetModel);
			gFacet->diffuseColor    = Vector3r(0.5,0.5,0.5);
			gFacet->wire	    = wire;
			gFacet->shadowCaster    = true;
			(*bodies)[b_id]->geometricalModel	= gFacet;
		#endif
		for (int j=0; j<3; ++j)
		{   
				iFacet->vertices.push_back(v[j]-icc);
				#ifdef YADE_GEOMETRICALMODEL
					gFacet->vertices.push_back(v[j]-icc);
				#endif
		}
		//iFacet->postProcessAttributes(true); //postProcessAttributes is protected
		shared_ptr<Body> b(new Body());
		b->state->pos=b->state->refPos=icc;
		b->state->ori=b->state->refOri=Quaternionr::IDENTITY;
		b->shape	= iFacet;
		imported.push_back(b);
	}
	return imported;
}

