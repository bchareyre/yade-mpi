#ifdef FLOW_ENGINE

#include "def_types.h"
#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
#include <CGAL/Width_3.h>
#include <iostream>
#include <fstream>
#include <new>
#include <utility>
#include "vector"
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Network.h"



#define FAST

const double ONE_THIRD = 1.0/3.0;
const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};
const int permut3 [3][3] = {{0,1,2},{1,2,0},{2,0,1}};
const int permut4 [4][4] = {{0,1,2,3},{1,2,3,0},{2,3,0,1},{3,0,1,2}};

int facetF1=0, facetF2=0, facetRe1=0, facetRe2=0, facetRe3=0;
int F1=0, F2=0, Re1=0, Re2=0;
int fictious_vertex, real_vertex;
bool facet_detected = false;

const double FAR = 500;

using namespace std;
using namespace boost;
namespace CGT {
  
Network::~Network(){}

Network::Network(){}

double Network::Volume_Pore ( Cell_handle cell)
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	Real v;
	switch (cell->info().fictious()) {
                        case(0) :
				v=std::abs(Tri.tetrahedron(cell).volume());
				for (int k=0;k<4;k++) v-= spherical_triangle_volume(cell->vertex(permut4[k][0])->point(),
					cell->vertex(permut4[k][1])->point(),
					cell->vertex(permut4[k][2])->point(),
					cell->vertex(permut4[k][3])->point());
				return v;
                        case(1) : return 0;
                        case(2) : return 0;
			case(3) : return 0;}
}

int Network::Detect_facet_fictious_vertices (Cell_handle& cell, int& j)
{
	fictious_vertex = 0;
	int real_vertex=0;
	Sphere v [3];
        Vertex_handle W [3];
	
        for (int kk=0; kk<3; kk++) {
                W[kk] = cell->vertex(facetVertices[j][kk]);
                v[kk] = cell->vertex(facetVertices[j][kk])->point();
                if (W[kk]->info().isFictious) {
                        if (fictious_vertex==0) {F1=facetVertices[j][kk];facetF1=kk;} else 
			{F2 = facetVertices[j][kk];facetF2=kk;}
                        fictious_vertex +=1;
                } else {
                        if (real_vertex==0) {Re1=facetVertices[j][kk];facetRe1=kk;} else if (real_vertex==1) 
			{Re2=facetVertices[j][kk];facetRe2=kk;} else if (real_vertex==2)
			{Re2=facetVertices[j][kk];facetRe3=kk;}
                        real_vertex+=1;}}
        facet_detected = true;
	return fictious_vertex;
}

double Network::Volume_Pore_VoronoiFraction (Cell_handle& cell, int& j)
{
  Point& p1 = cell->info();
  Point& p2 = cell->neighbor(j)->info();
  
  fictious_vertex = Detect_facet_fictious_vertices (cell,j);
  
  Sphere v [3];
  Vertex_handle W [3];
  for (int kk=0; kk<3; kk++) {W[kk] = cell->vertex(facetVertices[j][kk]);v[kk] = cell->vertex(facetVertices[j][kk])->point();}

  switch (fictious_vertex) {
    case (0) : {
		Vertex_handle& SV1 = W[0];
                Vertex_handle& SV2 = W[1];
                Vertex_handle& SV3 = W[2];
		
                cell->info().facetSurfaces[j]=0.5*CGAL::cross_product(SV1->point()-SV3->point(),SV2->point()-SV3->point());
                if (cell->info().facetSurfaces[j]*(p2-p1) > 0) cell->info().facetSurfaces[j] = -1.0*cell->info().facetSurfaces[j];
                Real Vtot = abs(ONE_THIRD*cell->info().facetSurfaces[j]*(p1-p2));
		Vtotalissimo += Vtot;

                double Vsolid1=0, Vsolid2=0;
                for (int i=0;i<3;i++) {
                Vsolid1 += spherical_triangle_volume(v[permut3[i][0]],v[permut3[i][1]],p1,p2);
                Vsolid2 += spherical_triangle_volume(v[permut3[i][0]],v[permut3[i][2]],p1,p2);}
                
		Vsolid_tot += Vsolid1 + Vsolid2;
		Vporale += Vtot - (Vsolid1 + Vsolid2);

		/**Vpore**/ return Vtot - (Vsolid1 + Vsolid2); 
    }; break;
    case (1) : {return volume_single_fictious_pore(cell->vertex(facetVertices[j][facetF1]), cell->vertex(facetVertices[j][facetRe1]), cell->vertex(facetVertices[j][facetRe2]), p1,p2, cell->info().facetSurfaces[j]);}; break;
    case (2) : {return volume_double_fictious_pore(cell->vertex(facetVertices[j][facetF1]), cell->vertex(facetVertices[j][facetF2]), cell->vertex(facetVertices[j][facetRe1]), p1,p2, cell->info().facetSurfaces[j]);}; break;
    }
}

double Network::volume_single_fictious_pore(const Vertex_handle& SV1, const Vertex_handle& SV2, const Vertex_handle& SV3, const Point& PV1,  const Point& PV2, Vecteur& facetSurface)
{
        double A [3], B[3];
	
        Boundary &bi1 =  boundaries [SV1->info().id()];

        for (int m=0;m<3;m++) {A[m]= (SV2->point())[m];}
        for (int m=0;m<3;m++) {B[m]= (SV3->point())[m];}

        A[bi1.coordinate]=bi1.p[bi1.coordinate];
        B[bi1.coordinate]=bi1.p[bi1.coordinate];

        Point AA(A[0],A[1],A[2]);
        Point BB(B[0],B[1],B[2]);
        facetSurface = surface_single_fictious_facet(SV1,SV2,SV3);
        if (facetSurface*(PV2-PV1) > 0) facetSurface = -1.0*facetSurface;
        Real Vtot=ONE_THIRD*abs(facetSurface*(PV1-PV2));
	Vtotalissimo += Vtot;

        Sphere A1(AA, 0);
        Sphere B1(BB, 0);
        Sphere& SW2 = SV2->point();
        Sphere& SW3 = SV3->point();

        Real Vsolid1 = spherical_triangle_volume(SW2, AA, PV1, PV2)+spherical_triangle_volume(SW2, SW3, PV1, PV2);
        Real Vsolid2 = spherical_triangle_volume(SW3, BB, PV1, PV2)+spherical_triangle_volume(SW3, SW2, PV1, PV2);

	Vsolid_tot += Vsolid1 + Vsolid2;
	Vporale += Vtot - (Vsolid1 + Vsolid2);
        
        return (Vtot - (Vsolid1 + Vsolid2));
}

double Network::volume_double_fictious_pore(const Vertex_handle& SV1, const Vertex_handle& SV2, const Vertex_handle& SV3, const Point& PV1, const Point& PV2, Vecteur& facetSurface)
{
        double A [3], B[3];

	Boundary &bi1 =  boundary(SV1->info().id());
        Boundary &bi2 =  boundary(SV2->info().id());
        for (int m=0;m<3;m++) {A[m]=B[m]= SV3->point()[m];}
        
        A[bi1.coordinate]=bi1.p[bi1.coordinate];
        B[bi2.coordinate]=bi2.p[bi2.coordinate];
        Point AA(A[0],A[1],A[2]);
        Point BB(B[0],B[1],B[2]);
	
        facetSurface = CGAL::cross_product(SV3->point()-AA,SV3->point()-BB);
        if (facetSurface*(PV2-PV1) > 0) facetSurface = -1.0*facetSurface;
        Real Vtot = abs(facetSurface*(PV1-PV2))*ONE_THIRD;
	Vtotalissimo += Vtot;
  
        Real Vsolid1 = spherical_triangle_volume(SV3->point(), AA, PV1, PV2);
        Real Vsolid2 = spherical_triangle_volume(SV3->point(), BB, PV1, PV2);
	
	Vporale += (Vtot - Vsolid1 - Vsolid2);
	Vsolid_tot += Vsolid1 + Vsolid2;

        return (Vtot - Vsolid1 - Vsolid2);
}

double Network::spherical_triangle_volume(const Sphere& ST1, const Point& PT1, const Point& PT2, const Point& PT3)
{
        double rayon = sqrt(ST1.weight());
        if (rayon == 0.0) return 0.0;

        return ((ONE_THIRD * rayon) * (fast_spherical_triangle_area(ST1, PT1, PT2, PT3))) ;
}

double Network::fast_spherical_triangle_area(const Sphere& STA1, const Point& STA2, const Point& STA3, const Point& PTA1)
{
        using namespace CGAL;
#ifndef FAST
        return spherical_triangle_area(STA1, STA2, STA3, PTA1);
#endif

        double rayon2 = STA1.weight();
        if (rayon2 == 0.0) return 0.0;
        return rayon2 * fast_solid_angle(STA1,STA2,STA3,PTA1);
}

double Network::spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 )
{
 double rayon = STA1.weight();
 if ( rayon == 0.0 ) return 0.0;

 Vecteur v12 = STA2.point() - STA1.point();
 Vecteur v13 = STA3.point() - STA1.point();
 Vecteur v14 = PTA1 - STA1.point();

 double norme12 = ( v12.squared_length() );
 double norme13 = ( v13.squared_length() );
 double norme14 = ( v14.squared_length() );

 double cosA = v12*v13 / sqrt ( ( norme13 * norme12 ) );
 double cosB = v12*v14 / sqrt ( ( norme14 * norme12 ) );
 double cosC = v14*v13 / sqrt ( ( norme13 * norme14 ) );

 double A = acos ( cosA );
 double B = acos ( cosB );
 double C = acos ( cosC );
 if ( A==0 || B==0 || C==0 ) return 0;

 double a = acos ( ( cosA - cosB * cosC ) / ( sin ( B ) * sin ( C ) ) );
 double b = acos ( ( cosB - cosC * cosA ) / ( sin ( C ) * sin ( A ) ) );
 double c = acos ( ( cosC - cosA * cosB ) / ( sin ( A ) * sin ( B ) ) );

 double aire_triangle_spherique = rayon * ( a + b + c - M_PI );

 return  aire_triangle_spherique;
}

Real Network::fast_solid_angle(const Point& STA1, const Point& PTA1, const Point& PTA2, const Point& PTA3)
{
        //! This function needs to be fast because it is used heavily. Avoid using vector operations which require constructing vectors (~50% of cpu time in the non-fast version), and compute angle using the 3x faster formula of Oosterom and StrackeeVan Oosterom, A; Strackee, J (1983). "The Solid Angle of a Plane Triangle". IEEE Trans. Biom. Eng. BME-30 (2): 125-126. (or check http://en.wikipedia.org/wiki/Solid_angle)
        using namespace CGAL;
        double M[3][3];
        M[0][0] = PTA1.x() - STA1.x();
        M[0][1] = PTA2.x() - STA1.x();
        M[0][2] = PTA3.x() - STA1.x();
        M[1][0] = PTA1.y() - STA1.y();
        M[1][1] = PTA2.y() - STA1.y();
        M[1][2] = PTA3.y() - STA1.y();
        M[2][0] = PTA1.z() - STA1.z();
        M[2][1] = PTA2.z() - STA1.z();
        M[2][2] = PTA3.z() - STA1.z();

        double detM = M[0][0]* (M[1][1]*M[2][2]-M[2][1]*M[1][2]) +
                      M[1][0]* (M[2][1]*M[0][2]-M[0][1]*M[2][2]) +
                      M[2][0]* (M[0][1]*M[1][2]-M[1][1]*M[0][2]);

        double pv12N2 = pow(M[0][0],2) +pow(M[1][0],2) +pow(M[2][0],2);
        double pv13N2 = pow(M[0][1],2) +pow(M[1][1],2) +pow(M[2][1],2);
        double pv14N2 = pow(M[0][2],2) +pow(M[1][2],2) +pow(M[2][2],2);

        double pv12N = sqrt(pv12N2);
        double pv13N = sqrt(pv13N2);
        double pv14N = sqrt(pv14N2);

        double cp12 = (M[0][0]*M[0][1]+M[1][0]*M[1][1]+M[2][0]*M[2][1]);
        double cp13 = (M[0][0]*M[0][2]+M[1][0]*M[1][2]+M[2][0]*M[2][2]);
        double cp23 = (M[0][1]*M[0][2]+M[1][1]*M[1][2]+M[2][1]*M[2][2]);

        double ratio = detM/ (pv12N*pv13N*pv14N+cp12*pv14N+cp13*pv13N+cp23*pv12N);
        return abs(2*atan(ratio));
}

double Network::Surface_Solid_Pore(Cell_handle cell, int j, bool SLIP_ON_LATERALS)
{
  if (!facet_detected) fictious_vertex=Detect_facet_fictious_vertices(cell,j);
  
  RTriangulation& Tri = T[currentTes].Triangulation();
  Point& p1 = cell->info();
  Point& p2 = cell->neighbor(j)->info();
  
  double Ssolid = 0;
  double Ssolid1= 0, Ssolid1n= 0, Ssolid2= 0, Ssolid2n= 0, Ssolid3= 0, Ssolid3n= 0;
  
  Sphere v [3];
  Vertex_handle W [3];
  
  for (int kk=0; kk<3; kk++) {
	  W[kk] = cell->vertex(facetVertices[j][kk]);
	  v[kk] = cell->vertex(facetVertices[j][kk])->point();}
  
  switch (fictious_vertex) {
    case (0) : {
		Vertex_handle& SV1 = W[0];
                Vertex_handle& SV2 = W[1];
                Vertex_handle& SV3 = W[2];
		
		Ssolid1 = fast_spherical_triangle_area(SV1->point(), SV2->point(), p1, p2);
                Ssolid1n = fast_spherical_triangle_area(SV1->point(), SV3->point(), p1, p2);
                cell->info().solidSurfaces[j][0]=Ssolid1+Ssolid1n;
                Ssolid2 = fast_spherical_triangle_area(SV2->point(),SV1->point(),p1, p2);
                Ssolid2n = fast_spherical_triangle_area(SV2->point(),SV3->point(),p1, p2);
                cell->info().solidSurfaces[j][1]=Ssolid2+Ssolid2n;
                Ssolid3 = fast_spherical_triangle_area(SV3->point(),SV2->point(),p1, p2);
                Ssolid3n = fast_spherical_triangle_area(SV3->point(),SV1->point(),p1, p2);
                cell->info().solidSurfaces[j][2]=Ssolid3+Ssolid3n;

    }; break;
    case (1) : {
		Vertex_handle SV1 = cell->vertex(facetVertices[j][facetF1]);
		Vertex_handle SV2 = cell->vertex(facetVertices[j][facetRe1]);
		Vertex_handle SV3 = cell->vertex(facetVertices[j][facetRe2]);
		
		Boundary &bi1 =  boundary(SV1->info().id());
                Ssolid1 = 0;
		if (bi1.flowCondition && ! SLIP_ON_LATERALS) {
                        Ssolid1 = abs(0.5*CGAL::cross_product(p1-p2, SV2->point()-SV3->point())[bi1.coordinate]);
                        cell->info().solidSurfaces[j][facetF1]=Ssolid1;
                }
                Ssolid2 = fast_spherical_triangle_area(SV2->point(),SV1->point(),p1, p2);
                Ssolid2n = fast_spherical_triangle_area(SV2->point(),SV3->point(),p1, p2);
                cell->info().solidSurfaces[j][facetRe1]=Ssolid2+Ssolid2n;
                Ssolid3 = fast_spherical_triangle_area(SV3->point(),SV2->point(),p1, p2);
                Ssolid3n = fast_spherical_triangle_area(SV3->point(),SV1->point(),p1, p2);
                cell->info().solidSurfaces[j][facetRe2]=Ssolid3+Ssolid3n;
    }; break;
    case (2) : {
      
		double A [3], B[3], C[3];
		
		Vertex_handle SV1 = cell->vertex(facetVertices[j][facetF1]);
		Vertex_handle SV2 = cell->vertex(facetVertices[j][facetF2]);
		Vertex_handle SV3 = cell->vertex(facetVertices[j][facetRe1]);
		
		Boundary &bi1 =  boundary(SV1->info().id());
                Boundary &bi2 =  boundary(SV2->info().id());
                for (int m=0;m<3;m++) {
                        A[m]=B[m]=C[m]= (SV3->point())[m];
                }
                A[bi1.coordinate]=bi1.p[bi1.coordinate];
                B[bi2.coordinate]=bi2.p[bi2.coordinate];
                C[bi1.coordinate]=bi1.p[bi1.coordinate];
                C[bi2.coordinate]=bi2.p[bi2.coordinate];
                Point AA(A[0],A[1],A[2]);
                Point BB(B[0],B[1],B[2]);
                Point CC(C[0],C[1],C[2]);

                Sphere A1(AA, 0);
                Sphere B1(BB, 0);
                Sphere C1(CC, 0);
                //FIXME : we are computing triangle area twice here, because its computed in volume_double_fictious already -> optimize
                Ssolid1 = fast_spherical_triangle_area(SV3->point(), AA, p1, p2);
                Ssolid1n = fast_spherical_triangle_area(SV3->point(), BB, p1, p2);
                cell->info().solidSurfaces[j][facetRe1]=Ssolid1+Ssolid1n;
                //area vector of triangle (p1,sphere,p2)
                Vecteur p1p2v1Surface = 0.5*CGAL::cross_product(p1-p2,SV3->point()-p2);
                if (bi1.flowCondition && ! SLIP_ON_LATERALS) {
                        //projection on boundary 1
                        Ssolid2 = abs(p1p2v1Surface[bi1.coordinate]);
                        cell->info().solidSurfaces[j][facetF1]=Ssolid2;
                } else cell->info().solidSurfaces[j][facetF1]=0;

                if (bi2.flowCondition && ! SLIP_ON_LATERALS) {
                        //projection on boundary 2
                        Ssolid3 = abs(p1p2v1Surface[bi2.coordinate]);
                        cell->info().solidSurfaces[j][facetF2]=Ssolid3;
                } else cell->info().solidSurfaces[j][facetF2]=0;
    }; break;
    }
    
    Ssolid = Ssolid1+Ssolid1n+Ssolid2+Ssolid2n+Ssolid3+Ssolid3n;
    
    cell->info().solidSurfaces[j][3]=1.0/Ssolid;
    Ssolid_tot += Ssolid;
    
    return Ssolid;

}

Vecteur Network::surface_double_fictious_facet(Vertex_handle fSV1, Vertex_handle fSV2, Vertex_handle SV3)
{
        //This function is correct only with axis-aligned boundaries
        const Boundary &bi1 = boundary(fSV1->info().id());
        const Boundary &bi2 = boundary(fSV2->info().id());
        double area=(bi1.p[bi1.coordinate]-SV3->point()[bi1.coordinate])*(bi2.p[bi2.coordinate]-SV3->point()[bi2.coordinate]);
        double surf [3] = {1,1,1};
        surf[bi1.coordinate]=0;
        surf[bi2.coordinate]=0;
        return area*Vecteur(surf[0],surf[1],surf[2]);
}

Vecteur Network::surface_single_fictious_facet(Vertex_handle fSV1, Vertex_handle SV2, Vertex_handle SV3)
{
        //This function is correct only with axis-aligned boundaries
        const Boundary &bi1 =  boundary(fSV1->info().id());
//  const Boundary &bi2 = boundary ( fSV2->info().id() );
        Vecteur mean_height = (bi1.p[bi1.coordinate]-0.5*(SV3->point()[bi1.coordinate]+SV2->point()[bi1.coordinate]))*bi1.normal;
	
        return CGAL::cross_product(mean_height,SV3->point()-SV2->point());
}

double Network::surface_solid_double_fictious_facet(Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3)
{
        double A [3], B [3];

        for (int m=0;m<3;m++) {
                A[m]=B[m]= (SV3->point())[m];
        }

        const Boundary &bi1 = boundary(SV1->info().id());
        const Boundary &bi2 = boundary(SV2->info().id());

        A[bi1.coordinate]=bi1.p[bi1.coordinate];
        B[bi2.coordinate]=bi2.p[bi2.coordinate];

        double board1=0, board2=0, total_surface=0;
        for (int p=0;p<3;p++) {
                board1 += pow((SV3->point()[p]-A[p]),2);
                board2 += pow((SV3->point()[p]-B[p]),2);
        }
        total_surface = sqrt(board1 * board2);

        double solid_surface = surface_solid_facet(SV3->point(),SV2->point(),SV1->point());

        return total_surface - solid_surface;
}

double Network::surface_solid_facet(Sphere ST1, Sphere ST2, Sphere ST3)
{
        double Area;

        double squared_radius = ST1.weight();

        Vecteur v12 = ST2.point() - ST1.point();
        Vecteur v13 = ST3.point() - ST1.point();

        double norme12 =  v12.squared_length();
        double norme13 =  v13.squared_length();

        double cosA = v12*v13 / (sqrt(norme13 * norme12));
        double A = acos(cosA);

        Area = (A/ (2*M_PI)) * (M_PI * squared_radius);
        return Area;
}

void Network::AddBoundingPlanes()
{
	Tesselation& Tes = T[currentTes];
	
	y_min_id = Tes.Max_id() + 1;
        boundsIds[0]=&y_min_id;
        y_max_id = Tes.Max_id() + 2;
        boundsIds[1]=&y_max_id;
        x_min_id = Tes.Max_id() + 3;
        boundsIds[2]=&x_min_id;
        x_max_id = Tes.Max_id() + 4;
        boundsIds[3]=&x_max_id;
        z_min_id = Tes.Max_id() + 5;
        boundsIds[4]=&z_min_id;
        z_max_id = Tes.Max_id() + 6;
        boundsIds[5]=&z_max_id;
	 
	id_offset = Tes.Max_id() +1;//so that boundaries[vertex->id - offset] gives the ordered boundaries (also see function Boundary& boundary(int b))
	
	AddBoundingPlanes(true);
}

void Network::AddBoundingPlanes(bool yade)
{
        Tesselation& Tes = T[currentTes];
        Corner_min = Point(x_min, y_min, z_min);
        Corner_max = Point(x_max, y_max, z_max);

        boundsIds[0]= &y_min_id;
        boundsIds[1]= &y_max_id;
        boundsIds[2]= &x_min_id;
        boundsIds[3]= &x_max_id;
        boundsIds[4]= &z_min_id;
        boundsIds[5]= &z_max_id;

        Tes.insert(0.5*(Corner_min.x() +Corner_max.x()), Corner_min.y()-FAR*(Corner_max.x()-Corner_min.x()), 0.5*(Corner_max.z()-Corner_min.z()), FAR*(Corner_max.x()-Corner_min.x()), y_min_id, true);
        boundaries[y_min_id-id_offset].p = Corner_min;
        boundaries[y_min_id-id_offset].normal = Vecteur(0,1,0);
        boundaries[y_min_id-id_offset].coordinate = 1;
        cout << "Bottom boundary has been created. ID = " << y_min_id << endl;

        Tes.insert(0.5*(Corner_min.x() +Corner_max.x()), Corner_max.y() +FAR*(Corner_max.x()-Corner_min.x()), 0.5*(Corner_max.z()-Corner_min.z()), FAR*(Corner_max.x()-Corner_min.x()), y_max_id, true);
        boundaries[y_max_id-id_offset].p = Corner_max;
        boundaries[y_max_id-id_offset].normal = Vecteur(0,-1,0);
        boundaries[y_max_id-id_offset].coordinate = 1;
        cout << "Top boundary has been created. ID = " << y_max_id << endl;

        Tes.insert(Corner_min.x()-FAR*(Corner_max.y()-Corner_min.y()), 0.5*(Corner_max.y()-Corner_min.y()), 0.5*(Corner_max.z()-Corner_min.z()), FAR*(Corner_max.y()-Corner_min.y()), x_min_id, true);
        boundaries[x_min_id-id_offset].p = Corner_min;
        boundaries[x_min_id-id_offset].normal = Vecteur(1,0,0);
        boundaries[x_min_id-id_offset].coordinate = 0;
        cout << "Left boundary has been created. ID = " << x_min_id << endl;

        Tes.insert(Corner_max.x() +FAR*(Corner_max.y()-Corner_min.y()), 0.5*(Corner_max.y()-Corner_min.y()), 0.5*(Corner_max.z()-Corner_min.z()), FAR*(Corner_max.y()-Corner_min.y()), x_max_id, true);
        boundaries[x_max_id-id_offset].p = Corner_max;
        boundaries[x_max_id-id_offset].normal = Vecteur(-1,0,0);
        boundaries[x_max_id-id_offset].coordinate = 0;
        cout << "Right boundary has been created. ID = " << x_max_id << endl;

        Tes.insert(0.5*(Corner_min.x() +Corner_max.x()), 0.5*(Corner_max.y()-Corner_min.y()), Corner_min.z()-FAR*(Corner_max.y()-Corner_min.y()), FAR*(Corner_max.y()-Corner_min.y()), z_min_id, true);
        boundaries[z_min_id-id_offset].p = Corner_min;
        boundaries[z_min_id-id_offset].normal = Vecteur(0,0,1);
        boundaries[z_min_id-id_offset].coordinate = 2;
        cout << "Front boundary has been created. ID = " << z_min_id << endl;

        Tes.insert(0.5*(Corner_min.x() +Corner_max.x()), 0.5*(Corner_max.y()-Corner_min.y()), Corner_max.z() +FAR*(Corner_max.y()-Corner_min.y()), FAR*(Corner_max.y()-Corner_min.y()), z_max_id, true);
        boundaries[z_max_id-id_offset].p = Corner_max;
        boundaries[z_max_id-id_offset].normal = Vecteur(0,0,-1);
        boundaries[z_max_id-id_offset].coordinate = 2;
        cout << "Back boundary has been created. ID = " << z_max_id << endl;

        for (int k=0;k<6;k++) {
                boundaries[k].flowCondition = 1;
                boundaries[k].value = 0;
        }
}

void Network::Define_fictious_cells()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
  
	Finite_cells_iterator cell_end = Tri.finite_cells_end();     
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
	  cell->info().fictious()=0;}
  
	for (int bound=0; bound<6;bound++) {
                int& id = *boundsIds[bound];
                Tesselation::Vector_Cell tmp_cells;
                tmp_cells.resize(10000);
                Tesselation::VCell_iterator cells_it = tmp_cells.begin();
                Tesselation::VCell_iterator cells_end = Tri.incident_cells(T[currentTes].vertexHandles[id],cells_it);
                for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
		{
		  Cell_handle& cell = *it;
		  (cell->info().fictious())+=1;
		}
	}
}

void Network::DisplayStatistics()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
        int Zero =0, Inside=0, Fictious=0;
        Finite_cells_iterator cell_end = Tri.finite_cells_end();
        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
                int zeros =0;
                for (int j=0; j!=4; j++) {
                        if ((cell->info().k_norm())[j]==0) {
                                zeros+=1;
                        }
                }
                if (zeros==4) {
                        Zero+=1;
                }
                if (!cell->info().fictious()) {
                        Inside+=1;
                } else {
                        Fictious+=1;
                }
        }
        cout << "zeros = " << Zero << endl;
        int fict=0, real=0;
        for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                if (v->info().isFictious) fict+=1;
                else real+=1;
        }
        long Vertices = Tri.number_of_vertices();
        cout << "There are " << Vertices << " vertices, dont " << fict << " fictious et " << real << " reeeeeel" << std::endl;
        long Cells = Tri.number_of_finite_cells();
        cout << "There are " << Cells << " cells " << std::endl;
        long Facets = Tri.number_of_finite_facets();
        cout << "There are " << Facets << " facets " << std::endl;
        cout << "There are " << Inside << " cells INSIDE." << endl;
        cout << "There are " << Fictious << " cells FICTIOUS." << endl;
	
	vtk_infinite_vertices = fict;
	vtk_infinite_cells = Fictious;
}

#endif //FLOW_ENGINE

// double Network::spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 )
// {
//  double rayon = STA1.weight();
//  if ( rayon == 0.0 ) return 0.0;
//
//  Vecteur v12 = STA2.point() - STA1.point();
//  Vecteur v13 = STA3.point() - STA1.point();
//  Vecteur v14 = PTA1 - STA1.point();
//
//  double norme12 = ( v12.squared_length() );
//  double norme13 = ( v13.squared_length() );
//  double norme14 = ( v14.squared_length() );
//
//  double cosA = v12*v13 / sqrt ( ( norme13 * norme12 ) );
//  double cosB = v12*v14 / sqrt ( ( norme14 * norme12 ) );
//  double cosC = v14*v13 / sqrt ( ( norme13 * norme14 ) );
//
//  double A = acos ( cosA );
//  double B = acos ( cosB );
//  double C = acos ( cosC );
//  if ( A==0 || B==0 || C==0 ) return 0;
//
//  double a = acos ( ( cosA - cosB * cosC ) / ( sin ( B ) * sin ( C ) ) );
//  double b = acos ( ( cosB - cosC * cosA ) / ( sin ( C ) * sin ( A ) ) );
//  double c = acos ( ( cosC - cosA * cosB ) / ( sin ( A ) * sin ( B ) ) );
//
//  double aire_triangle_spherique = rayon * ( a + b + c - M_PI );
//
//  return  aire_triangle_spherique;
// }
} //namespace CGT