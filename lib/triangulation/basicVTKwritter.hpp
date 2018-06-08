#pragma once
#include <fstream>
#include <iostream>

enum DataPosition {POINT_DATA,CELL_DATA};
enum DataName     {SCALARS,VECTORS,TENSORS};
enum DataType     {INT,FLOAT};

using std::ios_base;
using std::endl;
using std::cout;
using std::cerr;

// Really simplistic struct for vtk file creation
struct basicVTKwritter
{
	std::ofstream file;
	unsigned int nbVertices, nbCells;
	bool hasPointData;
	bool hasCellData;
	
	
	basicVTKwritter(unsigned int nV, unsigned int nC) : nbVertices(nV),nbCells(nC),hasPointData(false),hasCellData(false) {}
	void setNums(unsigned int nV, unsigned int nC) {nbVertices=nV; nbCells=nC;}
	bool open(const char * filename, const char * comment);
	bool close();
	
	void begin_vertices();
	void write_point(float x, float y, float z);
	void end_vertices();

	void begin_cells();
	void write_cell(unsigned int id1, unsigned int id2, unsigned int id3, unsigned int id4);
	void end_cells();

	void begin_data(const char * dataname, DataPosition pos, DataName name, DataType type);
    	void write_data(float value);
	void write_data(float x, float y, float z);
	void write_data(float t11, float t12, float t13,
					float t21, float t22, float t23,
					float t31, float t32, float t33);
	void end_data();
};





