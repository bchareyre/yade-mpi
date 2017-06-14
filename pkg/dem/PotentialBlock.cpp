/*CWBoon 2015 */
#ifdef YADE_POTENTIAL_BLOCKS
 //! To implement potential particles (Houlsby 2009) using sphere
#include "PotentialBlock.hpp"

YADE_PLUGIN((PotentialBlock));


PotentialBlock::~PotentialBlock()
{
}


 void PotentialBlock::addPlaneStruct() {
		   planeStruct.push_back(Planes());
}

 void PotentialBlock::addVertexStruct() {
		   vertexStruct.push_back(Vertices());
}

 void PotentialBlock::addEdgeStruct() {
		   edgeStruct.push_back(Edges());
}

//#if 0
void PotentialBlock::postLoad(PotentialBlock&)
{
	 int planeNo = a.size();
	 for (int i=0; i<planeNo; i++){
		addPlaneStruct();
	 }
	 double D[3]; double Ax[9]; Eigen::Matrix3d Aplanes;
	 for (int i=0; i<planeNo; i++ ){
		for (int j=i+1; j<planeNo; j++){
			for(int k=j+1; k<planeNo; k++){
				
				Vector3r plane1 = Vector3r(a[i],b[i], c[i]);	
				Vector3r plane2 = Vector3r(a[j],b[j], c[j]);	
				Vector3r plane3 = Vector3r(a[k],b[k], c[k]);	
				
				
				double d1 = d[i]+r;	
				double d2 = d[j]+r;
				double d3 = d[k]+r;
	
				D[0]=d1;
				D[1]=d2;
				D[2]=d3;
				Ax[0]=plane1.x(); Ax[3]=plane1.y(); Ax[6]=plane1.z();  Aplanes(0,0)= Ax[0]; Aplanes(0,1) = Ax[3]; Aplanes(0,2) = Ax[6];
				Ax[1]=plane2.x(); Ax[4]=plane2.y(); Ax[7]=plane2.z();  Aplanes(1,0) = Ax[1]; Aplanes(1,1) = Ax[4];Aplanes(1,2) = Ax[7];
				Ax[2]=plane3.x(); Ax[5]=plane3.y(); Ax[8]=plane3.z();  Aplanes(2,0) = Ax[2]; Aplanes(2,1) = Ax[5];Aplanes(2,2) = Ax[8];
				bool parallel = false;
				if (fabs(plane1.dot(plane2))<1.0002 && fabs(plane1.dot(plane2))>0.9998){parallel = true;}
				if (fabs(plane1.dot(plane3))<1.0002 && fabs(plane1.dot(plane3))>0.9998){parallel = true;}
				if (fabs(plane2.dot(plane3))<1.0002 && fabs(plane2.dot(plane3))>0.9998){parallel = true;}
				double det = Aplanes.determinant();
				
				if(fabs(det)>pow(10,-15) ){
				//if (parallel == false){
					int ipiv[3];  int bColNo=1; int info=0; /* LU */ int three =3;
					dgesv_( &three, &bColNo, Ax, &three, ipiv, D, &three, &info);
					if (info!=0){
						//std::cout<<"linear algebra error"<<endl;
					}else{
						bool inside = true; Vector3r vertex(D[0],D[1],D[2]);
						
						for (int i=0; i<planeNo; i++){
							Real plane = a[i]*vertex.x() + b[i]*vertex.y() + c[i]*vertex.z() - d[i]- r; if (plane>pow(10,-3)){inside = false;} 	
						}
						#if 0
						double planeV1 = d1 - (plane1.x()*vertex[0] + plane1.y()*vertex[1] + plane1.z()*vertex[2]);
						double planeV2 = d2 - (plane2.x()*vertex[0] + plane2.y()*vertex[1] + plane2.z()*vertex[2]);
						double planeV3 = d3 - (plane3.x()*vertex[0] + plane3.y()*vertex[1] + plane3.z()*vertex[2]);
						if (planeV1 <-pow(10,-3) ){ inside = false;}
						if (planeV2 <-pow(10,-3) ){ inside = false;}
						if (planeV3 <-pow(10,-3) ){ inside = false;}
						#endif
						
						if (inside == true){
							//std::cout<<"vertex: "<<vertex<<", planeV1: "<<planeV1<<", planeV2: "<<planeV2<<", planeV3: "<<planeV3<<", plane1: "<<plane1<<", plane2: "<<plane2<<", plane3: "<<plane3<<", det: "<<det<<endl;

							/*Vertices */

							addVertexStruct();
							int vertexID = vertexStruct.size()-1;
							vertexStruct[vertexID].planeID.push_back(i);	/*Note that the planeIDs are arranged from small to large! */
							vertexStruct[vertexID].planeID.push_back(j);    /* planeIDs are arranged in the same sequence as [a,b,c] and d */
							vertexStruct[vertexID].planeID.push_back(k);    /* vertices store information on planeIDs */
							
							/*Planes */
							planeStruct[i].vertexID.push_back(vertexID);	/* planes store information on vertexIDs */
							planeStruct[j].vertexID.push_back(vertexID);	
							planeStruct[k].vertexID.push_back(vertexID);	
							

						}
					}
				}
			}		
		}
	  }
}
//#endif

#endif // YADE_POTENTIAL_BLOCKS
