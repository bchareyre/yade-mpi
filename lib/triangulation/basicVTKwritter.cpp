#include "basicVTKwritter.hpp"

using namespace std;

void basicVTKwritter::open(const char * filename, const char * comment)
{
  file.open(filename,ios_base::out);
  if (!file)
  {
	cerr << "Cannot open file [" << filename << "]" << endl;
	return;
  }

  // Header
  file << "# vtk DataFile Version 2.0" << endl;
  file << comment << endl;
  file << "ASCII" << endl;
  file << "DATASET UNSTRUCTURED_GRID" << endl;
  file << endl;

}

void basicVTKwritter::begin_vertices()
{
  file << "POINTS " << nbVertices << " float" << endl;
}

void basicVTKwritter::begin_cells()
{
  file << "CELLS " << nbCells << " " << 5*nbCells << endl;
}

void basicVTKwritter::write_point(float x, float y, float z)
{
  file << x << " " << y << " " << z << endl;
}

// Note that identifiers must be defined with 0-offset
void basicVTKwritter::write_cell(unsigned int id1, unsigned int id2, unsigned int id3, unsigned int id4)
{
  file << "4 " << id1 << " " << id2 << " " << id3 << " " << id4 << endl;
}

void basicVTKwritter::end_vertices()
{
  file << endl;
}

void basicVTKwritter::end_cells()
{
  file << "CELL_TYPES " << nbCells << endl;
  for (unsigned int n = 0 ; n < nbCells ; ++n)
  {
	file << "10" << endl;
  }
  file << endl;
}

void basicVTKwritter::begin_data(const char * dataname, DataPosition pos, DataName name, DataType type)
{
switch(pos)
{
  case POINT_DATA : file << "POINT_DATA " << nbVertices << endl; break;
  case CELL_DATA  : file << "CELL_DATA " << nbCells << endl; break;
}

  switch (name)
  {
	case SCALARS : file << "SCALARS " << dataname; break;
	case VECTORS : file << "VECTORS " << dataname; break;
	case TENSORS : file << "TENSORS " << dataname; break;
  }

switch (type)
{
  case INT   : file << " int"; break;
  case FLOAT : file << " float"; break;
}

  if (name == SCALARS) file << " 1" << endl;
  else                file << endl;
  
  file << "LOOKUP_TABLE default" << endl;
}


void basicVTKwritter::write_data(float value)
{
  file << value << endl;
}

void basicVTKwritter::write_data(float x, float y, float z)
{
  file << x << " " << y << " " << z << endl;
}

void basicVTKwritter::write_data( float t11, float t12, float t13,
								  float t21, float t22, float t23,
								  float t31, float t32, float t33)
{
  file << t11 << " " << t12 << " " << t13 << endl;
  file << t21 << " " << t22 << " " << t23 << endl;
  file << t31 << " " << t32 << " " << t33 << endl;
  file << endl;
}

void basicVTKwritter::end_data()
{
  file << endl;
}






