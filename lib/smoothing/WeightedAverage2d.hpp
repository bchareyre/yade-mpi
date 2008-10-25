// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<iostream>
#include<vector>
#include<cstdlib>
#include<algorithm>
#include<cassert>
#include<cmath>
#include<stdexcept>
#include<boost/foreach.hpp>
#include<boost/lexical_cast.hpp>
#include<boost/python.hpp>

#define FOREACH BOOST_FOREACH
using namespace std;
using namespace boost;

#include<yade/lib-base/yadeWm3.hpp>
#include<Wm3Vector2.h>
typedef Wm3::Vector2<int> Vector2i;


template<typename T>
struct GridContainer{
	private:
		Vector2r lo, hi;
		Vector2r cellSizes;
		Vector2i nCells;
	public:
		vector<vector<vector<T> > > grid;
	/* construct grid from lower left corner, upper right corner and number of cells in each direction */
	GridContainer(Vector2r _lo, Vector2r _hi, Vector2i _nCells): lo(_lo), hi(_hi), nCells(_nCells){
		cellSizes=Vector2r((hi[0]-lo[0])/nCells[0],(hi[1]-lo[1])/nCells[1]);
		grid.resize(nCells[0]); for(int i=0; i<nCells[0]; i++) grid[i].resize(nCells[1]);
	}
	/* turn spatial coordinates into cell coordinates; if inGrid is not given, out-of-grid throws; otherwise, it sets inGrid=false. */
	Vector2i xy2cell(Vector2r xy, bool* inGrid=NULL) const {
		Vector2i ret((int)(floor((xy[0]-lo[0])/cellSizes[0])),(int)(floor((xy[1]-lo[1])/cellSizes[1])));
		if(ret[0]<0 || ret[0]>=nCells[0] || ret[1]<0 || ret[1]>=nCells[1]){
			if(inGrid) *inGrid=false; else throw std::invalid_argument("Cell coordinates outside grid (xy="+lexical_cast<string>(xy[0])+","+lexical_cast<string>(xy[1])+", computed cell coordinates "+lexical_cast<string>(ret[0])+","+lexical_cast<string>(ret[1])+").");
		} else {if(inGrid) *inGrid=true;}
		return ret;
	}
	Vector2r cell2xyMid(Vector2i cxy) const { return Vector2r(lo[0]+cellSizes[0]*(.5+cxy[0]),lo[1]+cellSizes[1]*(.5+cxy[1])); }

	// add new element to the right cell
	void add(const T& t, Vector2r xy){Vector2i cxy=xy2cell(xy); grid[cxy[0]][cxy[1]].push_back(t);}

	/* Filters: return list of cells, based on some spatial criterion: rectangle, ellipse, circle (add more if you need that) */
	vector<Vector2i> rectangleFilter(Vector2r bbLo, Vector2r bbHi) const {
		vector<Vector2i> ret; bool dummy; Vector2i cxymin=xy2cell(bbLo,&dummy), cxymax=xy2cell(bbHi,&dummy);
		for(int cx=cxymin[0]; cx<=cxymax[0]; cx++){ for(int cy=cxymin[1]; cy<=cxymax[1]; cy++){ if(cx>=0 && cx<nCells[0] && cy>=0 && cy<nCells[1]) ret.push_back(Vector2i(cx,cy));	} }
		return ret;
	}
	vector<Vector2i> circleFilter(Vector2r xy, Real radius) const { return ellipseFilter(xy,Vector2r(radius,radius));}
	vector<Vector2i> ellipseFilter(Vector2r xy0, Vector2r radii) const {
		vector<Vector2i> rectangle=rectangleFilter(Vector2r(xy0[0]-radii[0],xy0[1]-radii[1]),Vector2r(xy0[0]+radii[0],xy0[1]+radii[1]));
		vector<Vector2i> ret; bool inGrid;
		Vector2i cxy=xy2cell(xy0,&inGrid);
		FOREACH(Vector2i mid, rectangle){
			// if we are in the cell where the middle is also, this cell passes the filter
			if(inGrid && mid[0]==cxy[0] && mid[1]==cxy[1]){ret.push_back(mid); continue;}
			Vector2r xyMid=cell2xyMid(mid);
			Vector2r xy(xyMid[0]-xy0[0],xyMid[1]-xy0[1]); // relative mid-cell coords
			// tricky: move the mid-cell to the corner (or aligned pt on edge) according to position WRT center
			if(mid[0]!=cxy[0]) xy.X()+=(mid[0]<cxy[0]?1:-1)*.5*cellSizes[0]; else xy.X()=0;
			if(mid[1]!=cxy[1]) xy.X()+=(mid[1]<cxy[1]?1:-1)*.5*cellSizes[1]; else xy.Y()=0;
			// are we inside the ellipse? pass the filter, then
			if((pow(xy[0],2)/pow(radii[0],2)+pow(xy[1],2)/pow(radii[1],2))<=1) ret.push_back(mid);
		}
		return ret;
	}

	// graphical representation of a set of filtered cells
	string dumpGrid(vector<Vector2i> v){
		vector<vector<bool> > vvb; string ret; vvb.resize(nCells[0]); for(size_t i=0; i<(size_t)nCells[0]; i++) vvb[i].resize(nCells[1],false); FOREACH(Vector2i& vv, v) vvb[vv[0]][vv[1]]=true;
		for(int cy=nCells[1]-1; cy>=0; cy--){ ret+="|"; for(int cx=0; cx<nCells[0]; cx++){ ret+=vvb[cx][cy]?"@":"."; }	ret+="|\n";	}
		return ret;
	}
};
/* Abstract class for averages templated by elements stored in the grid and by the type of average value.
 *
 * To define your own class, you need to override getPosition (returns exact position of an element),
 * getWeight (return weight associated with given point and element -- may be 0, but should not be negative),
 * getValue (for an element),and filterCells (returns vector of cells of which elements will be used in computation).
 *
 * computeAverage computes weighted average using the overridden methods.
 */
template<typename T, typename Tvalue>
struct WeightedAverage{
	const shared_ptr<GridContainer<T> > grid;
	WeightedAverage(const shared_ptr<GridContainer<T> >& _grid):grid(_grid){};
	virtual Vector2r getPosition(const T&)=0;
	virtual Real getWeight(const Vector2r& refPt, const T&)=0;
	virtual Tvalue getValue(const T&)=0;
	virtual vector<Vector2i> filterCells(const Vector2r& refPt)=0;
	Tvalue computeAverage(const Vector2r& refPt){
		vector<Vector2i> filtered=filterCells(refPt);
		Real sumValues=0, sumWeights=0;
		FOREACH(Vector2i cell, filtered){
			FOREACH(const T& element, grid->grid[cell[0]][cell[1]]){
				Real weight=getWeight(refPt,element);
				sumValues+=weight*getValue(element); sumWeights+=weight;
			}
		}
		return sumValues/sumWeights;
	}
};

/* Compute nonlocal average with isotropic Gaussian kernel.
 *
 */
template<typename T, typename Tvalue>
struct SymmGaussianKernelAverage: public WeightedAverage<T,Tvalue> {
	Real stDev, relThreshold;
	SymmGaussianKernelAverage(const shared_ptr<GridContainer<T> >& _grid, Real _stDev, Real _relThreshold=3): WeightedAverage<T,Tvalue>(_grid), stDev(_stDev), relThreshold(_relThreshold){}
	virtual Real getWeight(const Vector2r& meanPt, const T& e){	
		Vector2r pos=getPosition(e);
		Real rSq=pow(meanPt[0]-pos[0],2)+pow(meanPt[1]-pos[1],2);
		if(rSq>relThreshold*stDev) return 0.;
		return (1./sqrt(2*M_PI*stDev))*exp(-rSq/(2*stDev));
	}
	virtual vector<Vector2i> filterCells(const Vector2r& refPt){return WeightedAverage<T,Tvalue>::grid->circleFilter(refPt,stDev*relThreshold);}
};

/* Class for doing template specialization of gaussian kernel average  on SGKA_dataPt and for testing */
struct dataPt{
	Vector2r pos;
	int val;
};

/* Final specialization; it only needs to define getValue and getPosition -- these functions contain knowledge about the element class itself */
struct SGKA_dataPt: public SymmGaussianKernelAverage<dataPt,Real>{
	SGKA_dataPt(const shared_ptr<GridContainer<dataPt> >& _grid, Real _stDev, Real _relThreshold=3): SymmGaussianKernelAverage<dataPt,Real>(_grid,_stDev,_relThreshold){}
	virtual Real getValue(const dataPt& dp){return (Real)dp.val;}
	virtual Vector2r getPosition(const dataPt& dp){return dp.pos;}
};

/* simplified interface for python:
 * 
 * ga=GaussAverage((0,0),(100,100),(10,10),5)
 * ga.add(53.444,(15.6,43.0))
 * ...
 * ga.avg((10,10))
 * ...
 *
 * */
class pyGaussAverage{
	//struct dataPt{Vector2r pos; Real val;};
	Vector2r tuple2vec2r(const python::tuple& t){return Vector2r(python::extract<Real>(t[0])(),python::extract<Real>(t[1])());}
	Vector2i tuple2vec2i(const python::tuple& t){return Vector2i(python::extract<int>(t[0])(),python::extract<int>(t[1])());}
	shared_ptr<SGKA_dataPt> sgka;
	public:
	pyGaussAverage(python::tuple lo, python::tuple hi, python::tuple nCells, Real stDev){
		shared_ptr<GridContainer<dataPt> > g(new GridContainer<dataPt>(tuple2vec2r(lo),tuple2vec2r(hi),tuple2vec2i(nCells)));
		sgka=shared_ptr<SGKA_dataPt>(new SGKA_dataPt(g,stDev));
	}
	void addPt(Real val, python::tuple pos){dataPt d; d.pos=tuple2vec2r(pos); d.val=val; sgka->grid->add(d,d.pos); }
	Real avg(python::tuple pt){return sgka->computeAverage(tuple2vec2r(pt));}

	Real stDev_get(){return sgka->stDev;} void stDev_set(Real s){sgka->stDev=s;}
	Real relThreshold_get(){return sgka->relThreshold;} void relThreshold_set(Real rt){sgka->relThreshold=rt;}
};

