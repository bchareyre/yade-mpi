/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "STLImporter.hpp"

STLImporter::STLImporter()  
{
    import_vertices = true;
    import_edges = true;
    import_facets = true;
    facets_wire=false;
    import_flat_edges_flag = false;
    import_flat_vertices_flag = false;
    max_vertices_in_facet = 4;
}

bool STLImporter::open(const char* filename)
{
    vector<double> vtmp, ntmp;
    vector<int>  etmp, ftmp;
    STLReader::tolerance = Math<Real>::ZERO_TOLERANCE;
    
    STLReader reader;
    if(!reader.open(filename, back_inserter(vtmp), back_inserter(etmp), back_inserter(ftmp), back_inserter(ntmp)))
	return false;

    vts.clear(); egs.clear(); fcs.clear();

    for(int i=0,e=vtmp.size(); i<e; i+=3)
	vts.push_back(Vrtx(vtmp[i], vtmp[i+1], vtmp[i+2]));

    for(int i=0,e=etmp.size(); i<e; i+=2)
	egs.push_back(Edg(etmp[i], etmp[i+1]));

    for(int i=0,e=ftmp.size(); i<e; i+=3)
	fcs.push_back(Fct(ftmp[i], ftmp[i+1], ftmp[i+2]));

    nbFlatEdges=0;
    for(int i=0,e=egs.size(); i<e; ++i)
	processEdge(i);

    facetsConsolidation();

    nbFlatVertices=0;
    for(int i=0,e=vts.size(); i<e; ++i)
	processVertex(i);

    return true;
}

void STLImporter::import(vector<shared_ptr<Body> > &bds)
{
    int index=0;
    if (import_vertices) 
	for(int j=0,e=vts.size(); j<e; ++j)
	{
	    if (!import_flat_vertices_flag && vts[j].flat) continue;

	    shared_ptr<Vertex> gVertex(new Vertex);
	    gVertex->diffuseColor		= Vector3r(0.7,0.7,0.7);
	    gVertex->wire			= false;
	    gVertex->visible			= true;
	    gVertex->shadowCaster		= true;

	    shared_ptr<InteractingVertex> iVertex(new InteractingVertex);
	    iVertex->normals=vts[j].normals;
	    iVertex->flat=vts[j].flat;
	    iVertex->diffuseColor		= Vector3r(0.8,0.3,0.3);

	    bds[index]->physicalParameters->se3 = Se3r( vts[j].point, Quaternionr( 1,0,0,0 ) );
	    bds[index]->geometricalModel	= gVertex;
	    bds[index]->interactingGeometry	= iVertex;

	    ++index;
	}

    if (import_edges) 
	for(int j=0,e=egs.size(); j<e; ++j)
	{ 
	    if (!import_flat_edges_flag && egs[j].flat) continue;

	    Vector3r p0 = vts[egs[j].first].point
		    ,p1 = vts[egs[j].second].point;

	    shared_ptr<GeometricalEdge> gEdge(new GeometricalEdge);
	    gEdge->edge			= p1-p0;
	    gEdge->diffuseColor		= Vector3r(0.8,0.8,0.8);
	    gEdge->wire			= false;
	    gEdge->visible		= true;
	    gEdge->shadowCaster		= true;

	    shared_ptr<InteractingEdge> iEdge(new InteractingEdge);
	    iEdge->edge			= p1-p0;
	    iEdge->normal1	        = egs[j].normal1;
	    iEdge->normal2	        = egs[j].normal2;
	    iEdge->both			= egs[j].both;
	    iEdge->flat			= egs[j].flat;
	    iEdge->diffuseColor		= Vector3r(0.8,0.3,0.3);

	    bds[index]->physicalParameters->se3 = Se3r(p0 , Quaternionr( 1,0,0,0 ) );
	    bds[index]->geometricalModel	= gEdge;
	    bds[index]->interactingGeometry	= iEdge;

	    ++index;
	}

    if (import_facets) 
	for(int i=0,e=fcs.size(); i<e; ++i)
	{
	    const Fct &f = fcs[i];
	    Vector3r p0 (vts[f[0]].point);

	    shared_ptr<InteractingFacet> iFacet(new InteractingFacet);
	    iFacet->diffuseColor    = Vector3r(0.8,0.3,0.3);

	    shared_ptr<Facet> gFacet(new Facet);
	    gFacet->diffuseColor    = Vector3r(0.5,0.5,0.5);
	    gFacet->wire	    = facets_wire;
	    gFacet->visible	    = true;
	    gFacet->shadowCaster    = true;

	    for (int j=1,ej=f.size(); j<ej; ++j)
	    {   
		iFacet->vertices.push_back(vts[f[j]].point - p0);
		gFacet->vertices.push_back(vts[f[j]].point - p0);
	    }

	    bds[index]->physicalParameters->se3 = Se3r( p0, Quaternionr( 1,0,0,0 ) );
	    bds[index]->geometricalModel	= gFacet;
	    bds[index]->interactingGeometry	= iFacet;

	    ++index;
	}
}

void STLImporter::processVertex(int id)
{
    Vrtx &vrtx = vts[id];

    Vector3r p0 (vrtx.point);

    for(int i=0,ei=egs.size(); i<ei; ++i)
    {
//	if (egs[i].flat) continue; //FIXME: skip flat edges 

	Vector3r v0(vts[egs[i].first].point)
		,v1(vts[egs[i].second].point)
		,normal;
	if ( p0 == v0 ) normal = v0-v1; 
	else if (p0 == v1) normal = v1-v0; 
	else continue;
	normal.Normalize();
	if ( vrtx.normals.size() < 2 ) // the first two normal
	{
	    vrtx.normals.push_back(normal);
	    if ( vrtx.normals.size()==2 && (vrtx.normals[0].UnitCross(vrtx.normals[1])).Length() == 0)
	    { // vertex lies on the edge
		vrtx.flat = true;
		break;
	    }
	    continue;
	}
	// third normal, etc.
	bool add_normal_flag=true;
	for (int ii=0,eii=vrtx.normals.size()-1; ii<eii; ++ii)
	{
	    for (int jj=ii+1, ejj=vrtx.normals.size(); jj<ejj; ++jj)
	    {
		Vector3r n1n2 = vrtx.normals[ii].UnitCross(vrtx.normals[jj]); // turn from ni to nj
		if ( Math<Real>::FAbs(n1n2.Dot(normal)) < Math<Real>::ZERO_TOLERANCE )
		{ // нормаль n лежит в плоскости ni, nj
		    add_normal_flag = false;

		    Vector3r n1normal = vrtx.normals[ii].UnitCross(normal); // turn from ni to n

		    if ( n1n2.Dot(n1normal) < Math<Real>::ZERO_TOLERANCE) 
		    { // turns ni->nj and ni->n directed to different sides
			Vector3r normaln1 = normal.UnitCross(vrtx.normals[ii]); // turn from n to ni
			Vector3r normaln2 = normal.UnitCross(vrtx.normals[jj]); // turn form n to nj
			if ( normaln1.Dot(normaln2) < Math<Real>::ZERO_TOLERANCE) 
			    // turns n->ni and n->nj directed to different sides =>
			    vrtx.flat = true; // vertex lies in the inner corner
			else
			    // turns n->ni and n->nj directed to one side =>
			    vrtx.normals[ii] = normal; // ni <- n
		    }
		    else // turns ni->nj and ni->n directed to one side
		       	if (vrtx.normals[ii].Dot(normal) < vrtx.normals[ii].Dot(vrtx.normals[jj]))
			    // the angle between ni and n are greater than between ni and nj =>
			    vrtx.normals[jj]=normal; // nj <- n
		}
	    }
	}
	if (add_normal_flag) vrtx.normals.push_back(normal);
    }

    if (vrtx.flat)
	nbFlatVertices++;
}

void STLImporter::processEdge(int id)
{
    Edg &edg = egs[id];

    Vector3r p0 (vts[edg.first].point);
    Vector3r p1 (vts[edg.second].point);

    edg.normal1 = edg.normal2 = Vector3r::ZERO;

    pair<Vector3r,Vector3r> edge( minmax(p0, p1) );
    for(int i=0,ei=fcs.size(); i<ei; ++i)
    {
	Vector3r v0(vts[fcs[i][0]].point)  // triangular facet
		,v1(vts[fcs[i][1]].point)
	        ,v2(vts[fcs[i][2]].point);
	Vector3r N(0,0,0);
	if ( edge == minmax(v0,v1) ) // find normal to edge
	{
	    v1 -= v0;
	    v2 -= v0;
	    Vector3r ne = v1; ne.Normalize();
	    N = ne * v2.Dot(ne) - v2;  
	}
	else if ( edge == minmax(v0,v2) ) 
	{
	    v1 -= v0;
	    v2 -= v0;
	    Vector3r ne = v2; ne.Normalize();
	    N = ne * v1.Dot(ne) - v1;
	}
	else if ( edge == minmax(v1,v2) ) 
	{
	    v1 -= v0;
	    v2 -= v0;
	    Vector3r ne = v2-v1; ne.Normalize();
	    N = v1 + ne*(-v1).Dot(ne);
	}

	if (N.Normalize() > 0)
	{
	    if (edg.normal1 == Vector3r::ZERO) // setup first normal
	    {
		edg.normal1 = N;
		continue;
	    }
	    if (edg.normal2 == Vector3r::ZERO) // setup second normal
	    {
		edg.normal2 = N;
		edg.both=true;
		continue;
	    }
	    // the edge involves more than two faces
	    cerr << "WARNING: Edge have over two facets" << endl;
	}
    }
    if ( Math<Real>::FAbs(edg.normal1.Dot(edg.normal2) + 1) <= Math<Real>::ZERO_TOLERANCE)
    { // scalar product normals is -1 => facets lies in the same plane, edge is flat 
	edg.flat=true;
	nbFlatEdges++;
    }
}

void STLImporter::facetsConsolidation()
{
    int nbFlatEdgesOld = nbFlatEdges;

    vector<Edg>::iterator edge_it = egs.begin(), edge_end = egs.end();
    while( edge_it != edge_end ) // loop on all edges
    {
	if (!edge_it->flat) // find the flat edge
	{
	    ++edge_it;
	    continue;
	}
	// flat edge
	pair<int,int> edge = minmax(edge_it->first, edge_it->second); 

	// looking for a faces, which shares this edge
	int facet1=-1, facet2=-1;
	int edge_in_facet1, edge_in_facet2;
	for (int facet_it=0, facet_end=fcs.size(); facet_it<facet_end && facet2<0; ++facet_it) // loop on the facets
	{
	    for (int edge_in_facet_it=0, edge_in_facet_end=fcs[facet_it].size(); // loop on the facet edges
		    edge_in_facet_it < edge_in_facet_end; ++edge_in_facet_it)
	    {
		pair<int,int> edge_in_facet = fcs[facet_it].edge(edge_in_facet_it);
		if ( edge == minmax(edge_in_facet.first, edge_in_facet.second) )
		{ // edge coincides with one of the facet
		    if (facet1<0)
		    { 
		      facet1 = facet_it;
		      edge_in_facet1 = edge_in_facet_it;
		    }
		    else
		    {
		      facet2 = facet_it;
		      edge_in_facet2 = edge_in_facet_it;
		    }
		    break;
		}
	    } 
	} //for

	// join the facets 
	Fct f1 = fcs[facet1], f2 = fcs[facet2];
	if (f1.vertex.size()<max_vertices_in_facet && f2.vertex.size()<max_vertices_in_facet)
	{
	    f2.remove_edge(edge_in_facet2);
	    f1.vertex.insert(f1.vertex.begin()+edge_in_facet1+1, f2.vertex.begin(), f2.vertex.end() );
	    reduce_facet_vertices(f1);
	    
	    // if we get a convex shape, accept join
	    if (convex_test(f1))
	    {
		fcs[facet1]=f1;
		fcs.erase(fcs.begin()+facet2);
		edge_it = egs.erase(edge_it);
		nbFlatEdges--;
		continue;
	    } 
	}
	++edge_it;
    } // while

    // repeat if the joins occurred
    if (nbFlatEdgesOld != nbFlatEdges) 
	facetsConsolidation();

    return;
}

bool STLImporter::convex_test(const Fct& f) const
{
    const vector<int>& v = f.vertex;
    int N = v.size();
    Vector3r R,T = (vts[v[0]].point-vts[v[N-1]].point).UnitCross(vts[v[1]].point-vts[v[0]].point);
    for (int i=1; i<N; ++i)
    {
	if (i==N-1)
	    R = (vts[v[N-1]].point-vts[v[N-2]].point).UnitCross(vts[v[0]].point-vts[v[N-1]].point);
	else
	    R = (vts[v[i]].point-vts[v[i-1]].point).UnitCross(vts[v[i+1]].point-vts[v[i]].point);

	if ( T.Dot(R) < Math<Real>::ZERO_TOLERANCE ) // T oppositely directed R
	    return false;
    }
    return true;
}

void STLImporter::reduce_facet_vertices(Fct& f)
{
    vector<int>& v = f.vertex;
    int i=0, N = v.size();
    while (i<N)
    {
	int k,l,m;
	if (i == 0)
	{
	    k=0; l=N-1; m=1;
	}
	else if (i==N-1)
	{
	    k=N-1; l=N-2; m=0;
	}
	else
	{
	    k=i; l=i-1; m=i+1;
	}

	Real L = ((vts[v[k]].point-vts[v[l]].point).UnitCross(vts[v[m]].point-vts[v[k]].point)).Length();
	if (L == 0)
	{ // three vertices lies on a line => remove middle vertex
	    v.erase(v.begin()+i);
	    N--;
	}
	else i++;
    }
}

ostream& operator<<(ostream& os, const STLImporter::Fct& f)
{
    for (int i=0,e=f.vertex.size(); i<e; ++i)
	os << f.vertex[i] << ' ';
    return os;
}
