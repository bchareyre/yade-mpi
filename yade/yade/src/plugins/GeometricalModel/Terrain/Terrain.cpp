#include "Terrain.hpp"

#include <iostream>
#include <GL/gl.h>
#include <set>


Terrain::Terrain () : CollisionGeometry()
{
}

Terrain::~Terrain ()
{

}

void Terrain::processAttributes()
{

}

void Terrain::registerAttributes()
{
	REGISTER_ATTRIBUTE(fileName);
}

bool Terrain::loadFromFile(char * fileName)
{
	if (fileName!=NULL)
	{
		loadWrl(fileName);		
		buildCollisionGeometry();
		return true;
	}
	else	
		return false;
}

void Terrain::glDraw()
{
	glColor3f(diffuseColor[0],diffuseColor[1],diffuseColor[2]);
	//if (_glListId==0)
	//{
		std::vector<std::vector<int> >::iterator fsi	= faces.begin();
		std::vector<std::vector<int> >::iterator fsiEnd	= faces.end();
		std::vector<Vector3r>::iterator ni = normals.begin();	

		for( ; fsi!=fsiEnd; ++fsi, ++ni)
		{
			std::vector<int>::iterator fi		= (*fsi).begin();
			std::vector<int>::iterator fiEnd	= (*fsi).end();
			
			glBegin(GL_POLYGON);
				glNormal3fv(*ni);
				for( ; fi!=fiEnd; ++fi)
					glVertex3fv(vertices[(*fi)]);
			glEnd();
		}
	//}
	//else
	//	glCallList(_glListId);
	
	glEnable(GL_LIGHTING);
	glColor3f(1.0,0.0,0.0);
	
	for(unsigned int k=0;k<testedFaces.size();k++)
	{
		int faceId = testedFaces[k];
		glBegin(GL_POLYGON);
			glNormal3fv(normals[faceId]);
			glVertex3fv(vertices[faces[faceId][0]]+Vector3r(0,0.1,0));
			glVertex3fv(vertices[faces[faceId][1]]+Vector3r(0,0.1,0));
			glVertex3fv(vertices[faces[faceId][2]]+Vector3r(0,0.1,0));
		glEnd();
	}
	testedFaces.clear();
}	

void Terrain::loadWrl(const char * fileName)
{
	vertices.clear(); 
	faces.clear();      
	normals.clear();  

	ifstream *file;
	
	file = new ifstream(fileName);
 
 	min[0] = min[1] = min[2] = FLT_MAX;
	max[0] = max[1] = max[2] = -FLT_MAX+1;
	
	if (!findTag(file,"#VRML V1.0 ascii\0"))
		cout << "Error : This file is not a VRML 1.0 ascii file" << endl;
		
	findTag(file,"Coordinate3\0");
	findTag(file,"point\0");
	findTag(file,"[\0");	

	Vector3r p,barycenter;
	char c;
	
	barycenter[0] = barycenter[1] = barycenter[2] = 0;
	bool stop = false;
	while (!file->eof() && !stop)
	{
		*file >> p[0] >> p[1] >> p[2];
		
		if (p[0]>max[0])
			max[0] = p[0];
		if (p[0]<min[0])
			min[0] = p[0];
			
		if (p[1]>max[1])
			max[1] = p[1];
		if (p[1]<min[1])
			min[1] = p[1];
			
		if (p[2]>max[2])
			max[2] = p[2];
		if (p[2]<min[2])
			min[2] = p[2];
		
		barycenter += p;
		
		vertices.push_back(p);
		
		c = file->peek();
		while (c!='-' && (c<'0' || c>'9') && c!=']' )
		{
			file->get(); 
			c = file->peek();
		}
		
		if (c==']')
			stop = true;
	}
	
	barycenter /= vertices.size();
	
	findTag(file,"IndexedFaceSet\0");
	findTag(file,"coordIndex\0");
	findTag(file,"[\0");	
	
	int n;
	
	std::vector<int> vId;
	Vector3r v1,v2,normal,faceBarycenter;
	stop = false;
	while (!file->eof() && !stop)	
	{
		vId.clear();
		*file >> n;		
		faceBarycenter[0] = faceBarycenter[1] = faceBarycenter[2] = 0;
		while (n!=-1)
		{		
			file->get(); 
			vId.push_back(n);
			faceBarycenter += vertices[n];
			*file >> n;		
		}
		
		faceBarycenter /= vId.size();
		
		faces.push_back(vId);
		
		v1 = vertices[vId[1]]-vertices[vId[0]];
 		v2 = vertices[vId[vId.size()-1]]-vertices[vId[0]];		 
		normal = v1.unitCross(v2);	
 		//if (normal.Dot(barycenter-faceBarycenter)>0)
 		//	normal = -normal;		
 		normals.push_back(normal);

		c = file->peek();
		while (c!='-' && (c<'0' || c>'9') && c!=']' )
		{
			file->get(); 
			c = file->peek();
		}
		
		if (c==']')
			stop = true;
	}

	file->close();
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Terrain::findTag(ifstream * file, char * tag)
{
	char * c;
	int tagLength,i;
	bool found;
	
	tagLength=0;
	while (tag[tagLength]!='\0')
		tagLength++;
	
	c = new char[tagLength];
	
	if (!file->eof())
	{
		c[0] = file->get();
		found = false;
		while (!found)
		{
			while (!file->eof() && c[0]!=tag[0])
				c[0] = file->get();			
			i=0;
			while (!file->eof() && c[i]==tag[i] && i<tagLength-1)
			{			
				i++;
				c[i] = file->get();
			}
			if (c[i]==tag[i] || file->eof())
				found = true;
			else if (!file->eof())
				c[0] = file->get();

		}
		c[tagLength] = '\0';
		return (strcmp(c,tag)==0);
	}
	else
		return false;


}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Terrain::glDrawNormals()
{
	std::vector<std::vector<int> >::iterator fsi	= faces.begin();
	std::vector<std::vector<int> >::iterator fsiEnd	= faces.end();
	std::vector<Vector3r>::iterator ni = normals.begin();	

	Vector3r bary;

	for( ; fsi!=fsiEnd; ++fsi, ++ni)
	{
		std::vector<int>::iterator fi		= (*fsi).begin();
		std::vector<int>::iterator fiEnd	= (*fsi).end();

		bary[0] = bary[1] = bary[2] = 0;

		for( ; fi!=fiEnd; ++fi)
			bary += vertices[(*fi)];

		bary /= (*fsi).size();
		
		glBegin(GL_LINES);
			glVertex3fv(bary);
			glVertex3fv(bary+*(ni)*0.2);
		glEnd();
		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Terrain::reOrientFaces()
{
	
	Vector3r bary = Vector3r(0,0,0);
	unsigned int i;
	for(i=0;i<vertices.size();i++)
		bary += vertices[i];
	bary /= vertices.size();

	Vector3r n;
	int tmp;
	float d;
	for(i=0;i<faces.size();i++)
	{	
		d = normals[i].dot(bary) - (normals[i][0]*vertices[faces[i][0]][0]+normals[i][1]*vertices[faces[i][0]][1]+normals[i][2]*vertices[faces[i][0]][2]);
		if (d>0)
		{
			for(unsigned int j=0;j<faces[i].size()/2;j++)
			{
				tmp = faces[i][j];
				faces[i][j] = faces[i][faces[i].size()-j-1];
				faces[i][faces[i].size()-j-1] = tmp;
			}
			normals[i] = -normals[i];
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int Terrain::buildDisplayList()
{
	if (_glListId==0)
	{
		unsigned int tmpListId = glGenLists(1);
		glNewList( tmpListId, GL_COMPILE );	
		glDraw();
		glEndList();
		_glListId = tmpListId;
	}
	
	return _glListId;
}        

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Terrain::buildCollisionGeometry()
{
	nbTriPerCell = 40;
	nbCells = (int)(sqrt(faces.size()/(float)nbTriPerCell));
	cellSizeX = (max[0]-min[0])/(float)nbCells;
	cellSizeZ = (max[2]-min[2])/(float)nbCells;

	boundingBoxes.resize(nbCells);
	triLists.resize(nbCells);	
	facesAABB.resize(faces.size());
	
	for(int i=0;i<nbCells;i++)
	{
		boundingBoxes[i].resize(nbCells);
		triLists[i].resize(nbCells);
	}
	
	for(unsigned int i=0;i<faces.size();i++)
	{
		Vector3r p1 = vertices[faces[i][0]]+Vector3r(0,-70,0); // beurk
		Vector3r p2 = vertices[faces[i][1]]+Vector3r(0,-70,0);
		Vector3r p3 = vertices[faces[i][2]]+Vector3r(0,-70,0);
		Vector3r inf = p1;
		Vector3r sup = p1;
		for(int j=0;j<3;j++)
		{
			if (p2[j]<inf[j]) inf[j]=p2[j];
			if (p2[j]>sup[j]) sup[j]=p2[j];
			if (p3[j]<inf[j]) inf[j]=p3[j];
			if (p3[j]>sup[j]) sup[j]=p3[j];
		}
		facesAABB[i] = AABB((sup-inf)*0.5,(sup+inf)*0.5);
	}	
	
	for(unsigned int i=0;i<faces.size();i++)
	{
		std::vector<Vector2r> tri;
		tri.clear();
		tri.push_back(Vector2r(vertices[faces[i][0]][0],vertices[faces[i][0]][2]));
		tri.push_back(Vector2r(vertices[faces[i][1]][0],vertices[faces[i][1]][2]));
		tri.push_back(Vector2r(vertices[faces[i][2]][0],vertices[faces[i][2]][2]));
		
		Vector2r v1,v2,v3;
		
		v1[0] = (int) ((tri[0][0]-min[0])/(float)cellSizeX);
		v1[1] = (int) ((tri[0][1]-min[2])/(float)cellSizeZ);
		
		v2[0] = (int) ((tri[1][0]-min[0])/(float)cellSizeX);
		v2[1] = (int) ((tri[1][1]-min[2])/(float)cellSizeZ);
		
		v3[0] = (int) ((tri[2][0]-min[0])/(float)cellSizeX);
		v3[1] = (int) ((tri[2][1]-min[2])/(float)cellSizeZ);
		
		if (v1[0]==nbCells) v1[0] = nbCells-1;
		if (v1[1]==nbCells) v1[1] = nbCells-1;
		if (v2[0]==nbCells) v2[0] = nbCells-1;
		if (v2[1]==nbCells) v2[1] = nbCells-1;
		if (v3[0]==nbCells) v3[0] = nbCells-1;
		if (v3[1]==nbCells) v3[1] = nbCells-1;
		
		Vector2r min,max;
		min = v1;
		max = v1;
		
		if (v2[0]<min[0]) min[0] = v2[0];
		if (v2[0]>max[0]) max[0] = v2[0];
		
		if (v2[1]<min[1]) min[1] = v2[1];
		if (v2[1]>max[1]) max[1] = v2[1];
			
		if (v3[0]<min[0]) min[0] = v3[0];
		if (v3[0]>max[0]) max[0] = v3[0];
		
		if (v3[1]<min[1]) min[1] = v3[1];
		if (v3[1]>max[1]) max[1] = v3[1];
		
		
		triLists[(int)v1[0]][(int)v1[1]].push_back(i);
		if (v2!=v1)
			triLists[(int)v2[0]][(int)v2[1]].push_back(i);
		if (v3!=v1 && v3!=v2)
			triLists[(int)v3[0]][(int)v3[1]].push_back(i);
			
		for(int j=(int)min[0];j<(int)min[1];j++)
			for(int k=(int)max[0];k<(int)max[1];k++)
			{
				Vector2r current = Vector2r(j,k);
				if (current!=v1 && current!=v2 && current!=v3)
					if (	pointInTriangle(Vector2r(min[0]+(j+0)*cellSizeX,min[2]+(k+0)*cellSizeZ),tri) || 
						pointInTriangle(Vector2r(min[0]+(j+1)*cellSizeX,min[2]+(k+0)*cellSizeZ),tri) || 
						pointInTriangle(Vector2r(min[0]+(j+1)*cellSizeX,min[2]+(k+1)*cellSizeZ),tri) || 
						pointInTriangle(Vector2r(min[0]+(j+0)*cellSizeX,min[2]+(k+1)*cellSizeZ),tri) )
						triLists[j][k].push_back(i);
			}
	}
	
	for(int i=0;i<nbCells;i++)
		for(int j=0;j<nbCells;j++)
		{
			float min,max;
			min = Mathr::MAX_REAL;
			max = -Mathr::MAX_REAL;
			for(unsigned int k=0 ; k<triLists[i][j].size() ; k++)
			{
				int faceId = triLists[i][j][k];
				
				if (vertices[faces[faceId][0]][1]<min) min = vertices[faces[faceId][0]][1];
				if (vertices[faces[faceId][0]][1]>max) max = vertices[faces[faceId][0]][1];
				
				if (vertices[faces[faceId][1]][1]<min) min = vertices[faces[faceId][1]][1];
				if (vertices[faces[faceId][1]][1]>max) max = vertices[faces[faceId][1]][1];
				
				if (vertices[faces[faceId][2]][1]<min) min = vertices[faces[faceId][2]][1];
				if (vertices[faces[faceId][2]][1]>max) max = vertices[faces[faceId][2]][1];
				
				boundingBoxes[i][j] = std::pair<float,float>(min,max);
			}
		}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Terrain::pointInTriangle(Vector2r p , std::vector<Vector2r>& tri)
{

	for (int i1 = 0, i0 = 2; i1 < 3; i0 = i1++)
	{
		float nx = tri[i1][1] - tri[i0][1];
		float ny = tri[i0][0] - tri[i1][0];
		float dx = p[0] - tri[i0][0];
		float dy = p[1] - tri[i0][1];
		if ( nx*dx + ny*dy > 0.0 )
			return false;
	}

	return true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Terrain::getFaces(const AABB& aabb, std::vector<int>& faceList)
{
	Vector2r minCell,maxCell;
	
	minCell[0] = (int) ((aabb.min[0]-min[0])/(float)cellSizeX);
	minCell[1] = (int) ((aabb.min[2]-min[2])/(float)cellSizeZ);

	maxCell[0] = (int) ((aabb.max[0]-min[0])/(float)cellSizeX);
	maxCell[1] = (int) ((aabb.max[2]-min[2])/(float)cellSizeZ);

	if (minCell[0]<0) minCell[0]=0;
	if (minCell[0]>=nbCells) minCell[0]=nbCells-1;
	if (minCell[1]<0) minCell[1]=0;
	if (minCell[1]>=nbCells) minCell[1]=nbCells-1;
	
	if (maxCell[0]<0) maxCell[0]=0;
	if (maxCell[0]>=nbCells) maxCell[0]=nbCells-1;
	if (maxCell[1]<0) maxCell[1]=0;
	if (maxCell[1]>=nbCells) maxCell[1]=nbCells-1;
	
	for(int i=(int)minCell[0] ; i<=(int)maxCell[0] ; i++)
		for(int j=(int)minCell[1] ; j<=(int)maxCell[1] ; j++)
		{			
			if (aabb.min[1]<boundingBoxes[i][j].second) // test complet sur min et max utile ???
			{				
				for(unsigned int k=0;k<triLists[i][j].size();k++)
				{					
					if (facesAABB[triLists[i][j][k]].overlap(aabb))
					{
						faceList.push_back(triLists[i][j][k]);
						testedFaces.push_back(triLists[i][j][k]);
					}
				}
			}
		}
}
