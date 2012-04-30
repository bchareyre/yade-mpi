#ifdef FLOW_ENGINE
/*#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
#include <CGAL/Width_3.h>
#include <iostream>
#include <fstream>
#include <new>
#include <utility>
#include "vector"
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>*/
// #include "Network.hpp"
// #include <omp.h>

// #define XVIEW
#include"PeriodicFlow.hpp"
// #include<yade/core/Cell.hpp>

#ifdef XVIEW
// #include "Vue3D.h" //FIXME implicit dependencies will look for this class (out of tree) even ifndef XVIEW
#endif

#define FAST
#define TESS_BASED_FORCES
#define FACET_BASED_FORCES 1

#ifdef YADE_OPENMP
//   #define GS_OPEN_MP //It should never be defined if Yade is not using openmp
#endif


namespace CGT {

void PeriodicFlow::Interpolate(Tesselation& Tes, Tesselation& NewTes)
{
        Cell_handle old_cell;
        RTriangulation& NewTri = NewTes.Triangulation();
        RTriangulation& Tri = Tes.Triangulation();
        Finite_cells_iterator cell_end = NewTri.finite_cells_end();
        for (Finite_cells_iterator new_cell = NewTri.finite_cells_begin(); new_cell != cell_end; new_cell++) {
		if (new_cell->info().Pcondition || new_cell->info().isGhost) continue;
                old_cell = Tri.locate((Point) new_cell->info());
                new_cell->info().p() = old_cell->info().shiftedP();
// 		new_cell->info().dv() = old_cell->info().dv();
        }
//  	Tes.Clear();//Don't reset to avoid segfault when getting pressure in scripts just after interpolation
}
	


void PeriodicFlow::ComputeFacetForcesWithCache()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	Finite_cells_iterator cell_end = Tri.finite_cells_end();
	Vecteur nullVect(0,0,0);
	static vector<Vecteur> oldForces;
	if (oldForces.size()<=Tri.number_of_vertices()) oldForces.resize(Tri.number_of_vertices()+1);
	//reset forces
	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		if (noCache) {oldForces[v->info().id()]=nullVect; v->info().forces=nullVect;}
		else {oldForces[v->info().id()]=v->info().forces; v->info().forces=nullVect;}
	}

	Cell_handle neighbour_cell;
	Vertex_handle mirror_vertex;
	Vecteur tempVect;
	//FIXME : Ema, be carefull with this (noCache), it needs to be turned true after retriangulation
	if (noCache) for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			//reset cache
			if (cell->info().isGhost) continue;
			for (int k=0;k<4;k++) cell->info().unitForceVectors[k]=nullVect;
			for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j))) {
				neighbour_cell = cell->neighbor(j);
				const Vecteur& Surfk = cell->info().facetSurfaces[j];
				//FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
				//The ratio void surface / facet surface
				Real area = sqrt(Surfk.squared_length());
				Vecteur facetNormal = Surfk/area;
				const std::vector<Vecteur>& crossSections = cell->info().facetSphereCrossSections;
				Vecteur fluidSurfk = cell->info().facetSurfaces[j]*cell->info().facetFluidSurfacesRatio[j];
				/// handle fictious vertex since we can get the projected surface easily here
				if (cell->vertex(j)->info().isFictious) {
					Real projSurf=abs(Surfk[boundary(cell->vertex(j)->info().id()).coordinate]);
					tempVect=-projSurf*boundary(cell->vertex(j)->info().id()).normal;
					//define the cached value for later use with cache*p
					cell->info().unitForceVectors[j]=cell->info().unitForceVectors[j]+ tempVect;
				}
				/// Apply weighted forces f_k=sqRad_k/sumSqRad*f
				Vecteur Facet_Unit_Force = -fluidSurfk*cell->info().solidSurfaces[j][3];
				for (int y=0; y<3;y++) {
					//add to cached value
					cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]+Facet_Unit_Force*cell->info().solidSurfaces[j][y];
					//uncomment to get total force / comment to get only viscous forces (Bruno)
					if (!cell->vertex(facetVertices[j][y])->info().isFictious) {
						//add to cached value
						cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]-facetNormal*crossSections[j][y];
					}
					
				}
			}
	}
	//use cached values
	//First define products that will be used below for all cells:
	Real pDeltas [3];
	for (unsigned int k=0; k<3;k++) pDeltas[k]=PeriodicCellInfo::hSize[k]*PeriodicCellInfo::gradP;
	//Then compute the forces
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++){
		if (cell->info().isGhost) continue;
		for (int yy=0;yy<4;yy++) {
			Vertex_Info& vhi = cell->vertex(yy)->info();
			Real unshiftedP = cell->info().p();
// 			cerr <<"p="<<cell->info().p()<<" shifted="<<cell->info().shiftedP()<<endl;
			//the pressure translated to a ghost cell adjacent to the non-ghost vertex
			//FIXME: the hsize[k]*gradP products could be computed only once for all cells and facets
			/*if (vhi.isGhost) */unshiftedP -= pDeltas[0]*vhi.period[0] + pDeltas[1]*vhi.period[1] +pDeltas[2]*vhi.period[2];
			T[currentTes].vertexHandles[vhi.id()]->info().forces=T[currentTes].vertexHandles[vhi.id()]->info().forces + cell->info().unitForceVectors[yy]*unshiftedP;
// 			vhi.forces = vhi.forces + cell->info().unitForceVectors[yy]*unshiftedP;
		}
	}
	noCache=false;//cache should always be defined after execution of this function
	if (DEBUG_OUT) {
// 		cout << "Facet cached scheme" <<endl;
		Vecteur TotalForce = nullVect;
		for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); v++)
		{
			if (!v->info().isFictious /*&& !v->info().isGhost*/ ){
				TotalForce = TotalForce + v->info().forces;
// 				cerr<< "real_id = " << v->info().id() << " force = "<< v->info().forces<< " ghost:"<< v->info().isGhost <<endl;
			}
// 				cout << "real_id = " << v->info().id() << " force = " << v->info().forces << endl;
			else /*if (!v->info().isGhost)*/{
				if (boundary(v->info().id()).flowCondition==1) TotalForce = TotalForce + v->info().forces;
// 				cout << "fictious_id = " << v->info().id() << " force = " << v->info().forces << " ghost:"<< v->info().isGhost<< endl;
			}
		}
		cout << "TotalForce = "<< TotalForce << endl;}
}

void PeriodicFlow::Compute_Permeability()
{
	if (DEBUG_OUT)  cout << "----Computing_Permeability------" << endl;
	RTriangulation& Tri = T[currentTes].Triangulation();
	Vsolid_tot = 0, Vtotalissimo = 0, Vporale = 0, Ssolid_tot = 0, V_totale_porosity=0, V_porale_porosity=0;
	Finite_cells_iterator cell_end = Tri.finite_cells_end();

	Cell_handle neighbour_cell;

	double k=0, distance = 0, radius = 0, viscosity = VISCOSITY;
	int surfneg=0;
	int NEG=0, POS=0, pass=0;

	bool ref = Tri.finite_cells_begin()->info().isvisited;
// 	Vecteur n;
//         std::ofstream oFile( "Radii",std::ios::out);
// 	std::ofstream fFile( "Radii_Fictious",std::ios::out);
//         std::ofstream kFile ( "LocalPermeabilities" ,std::ios::app );
	Real meanK=0, STDEV=0, meanRadius=0, meanDistance=0;
	Real infiniteK=1e3;

	double volume_sub_pore = 0.f;

	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++){
		if (cell->info().isGhost==false){
			Point& p1 = cell->info();
			for (int j=0; j<4; j++){
				neighbour_cell = cell->neighbor(j); 
				Point& p2 = neighbour_cell->info();
				if (!Tri.is_infinite(neighbour_cell) && (neighbour_cell->info().isvisited==ref || computeAllCells)) {
					//Compute and store the area of sphere-facet intersections for later use
					Vertex_handle W [3];
					for (int kk=0; kk<3; kk++) {
						W[kk] = cell->vertex(facetVertices[j][kk]);
					}
					Sphere& v0 = W[0]->point();
					Sphere& v1 = W[1]->point();
					Sphere& v2 = W[2]->point();
#ifdef USE_FAST_MATH
					//FIXME : code not compiling,, do the same as in "else"
					assert((W[permut3[jj][1]]->point()-W[permut3[jj][0]]->point())*(W[permut3[jj][2]]->point()-W[permut3[jj][0]]->point())>=0 && (W[permut3[jj][1]]->point()-W[permut3[jj][0]]->point())*(W[permut3[jj][2]]->point()-W[permut3[jj][0]]->point())<=1);
					for (int jj=0;jj<3;jj++)
						cell->info().facetSphereCrossSections[j][jj]=0.5*W[jj]->point().weight()*Wm3::FastInvCos1((W[permut3[jj][1]]->point()-W[permut3[jj][0]]->point())*(W[permut3[jj][2]]->point()-W[permut3[jj][0]]->point()));
#else
					cell->info().facetSphereCrossSections[j]=Vecteur(
					W[0]->info().isFictious ? 0 : 0.5*v0.weight()*acos((v1-v0)*(v2-v0)/sqrt((v1-v0).squared_length()*(v2-v0).squared_length())),
					W[1]->info().isFictious ? 0 : 0.5*v1.weight()*acos((v0-v1)*(v2-v1)/sqrt((v1-v0).squared_length()*(v2-v1).squared_length())),
					W[2]->info().isFictious ? 0 : 0.5*v2.weight()*acos((v0-v2)*(v1-v2)/sqrt((v1-v2).squared_length()*(v2-v0).squared_length())));
#endif
					pass+=1;
					Vecteur l = p1 - p2;
					distance = sqrt(l.squared_length());
// 					n = distance ? l/distance : ;
					if (!RAVERAGE) radius = 2* Compute_HydraulicRadius(cell, j);
					else radius = (Compute_EffectiveRadius(cell, j)+Compute_EquivalentRadius(cell,j))*0.5;
					if (radius<0) NEG++;
					else POS++;
					if (radius==0) {
						cout << "INS-INS PROBLEM!!!!!!!" << endl;
					}
// 					Real h,d;
					Real fluidArea=0;
					int test=0;
					if (distance!=0) {
						if (minPermLength>0 && distance_correction) distance=max(minPermLength,distance);
						const Vecteur& Surfk = cell->info().facetSurfaces[j];
						Real area = sqrt(Surfk.squared_length());
						const Vecteur& crossSections = cell->info().facetSphereCrossSections[j];
						if (areaR2Permeability){
//  							Real m1=sqrt((cross_product((v0-v1),v2-v1)).squared_length()/(v2-v1).squared_length());
							Real S0=0;
							S0=checkSphereFacetOverlap(v0,v1,v2);
							if (S0==0) S0=checkSphereFacetOverlap(v1,v2,v0);
							if (S0==0) S0=checkSphereFacetOverlap(v2,v0,v1);
							//take absolute value, since in rare cases the surface can be negative (overlaping spheres)
							fluidArea=abs(area-crossSections[0]-crossSections[1]-crossSections[2]+S0);
							cell->info().facetFluidSurfacesRatio[j]=fluidArea/area;
							k=(fluidArea * pow(radius,2)) / (8*viscosity*distance);
						}
						else {
						cout << "WARNING! if !areaR2Permeability, facetFluidSurfacesRatio will not be defined correctly. Don't use that."<<endl;
						k = (M_PI * pow(radius,4)) / (8*viscosity*distance);
						}

					if (k<0 && DEBUG_OUT) {surfneg+=1;
					cout<<"__ k<0 __"<<k<<" "<<" fluidArea "<<fluidArea<<" area "<<area<<" "<<crossSections[0]<<" "<<crossSections[1]<<" "<<crossSections[2] <<" "<<W[0]->info().id()<<" "<<W[1]->info().id()<<" "<<W[2]->info().id()<<" "<<p1<<" "<<p2<<" test "<<test<<endl;}
					     
					} 
					else  {
						cout <<"infinite K2!"<<endl; k = infiniteK;
					}//Will be corrected in the next loop

					(cell->info().k_norm())[j]= k*k_factor;
					(neighbour_cell->info().k_norm())[Tri.mirror_index(cell, j)]= k*k_factor;
					meanDistance += distance;
					meanRadius += radius;
					meanK += (cell->info().k_norm())[j];
				
					if(permeability_map){
						Cell_handle c = cell;
						cell->info().s = cell->info().s + k*distance/fluidArea*Volume_Pore_VoronoiFraction (c,j);
						volume_sub_pore += Volume_Pore_VoronoiFraction (c,j);
					}
				}
			}
			cell->info().isvisited = !ref;
			if(permeability_map){
				cell->info().s = cell->info().s/volume_sub_pore;
				volume_sub_pore = 0.f;
			}
		}
		else cell->info().isvisited = !ref;
	} 
	
	if (DEBUG_OUT) cout<<"surfneg est "<<surfneg<<endl;
	meanK /= pass;
	meanRadius /= pass;
	meanDistance /= pass;
	double maxKdivKmean=MAXK_DIV_KMEAN;
	if (DEBUG_OUT) {
		cout << "PassCompK = " << pass << endl;
		cout << "meanK = " << meanK << endl;
		cout << "maxKdivKmean = " << maxKdivKmean << endl;
		cout << "meanTubesRadius = " << meanRadius << endl;
		cout << "meanDistance = " << meanDistance << endl;
	}
	ref = Tri.finite_cells_begin()->info().isvisited;
	pass=0;
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
		for (int j=0; j<4; j++) {
			neighbour_cell = cell->neighbor(j);
			if (!Tri.is_infinite(neighbour_cell) && neighbour_cell->info().isvisited==ref) {
				pass++;
				(cell->info().k_norm())[j] = min((cell->info().k_norm())[j], maxKdivKmean*meanK);
// 				(cell->info().k_norm())[j] = max(MINK_DIV_KMEAN*meanK ,min((cell->info().k_norm())[j], maxKdivKmean*meanK));
				(neighbour_cell->info().k_norm())[Tri.mirror_index(cell, j)]=(cell->info().k_norm())[j];
// 				cout<<(cell->info().k_norm())[j]<<endl;
// 				kFile << (cell->info().k_norm())[j] << endl;
			}
		}
		cell->info().isvisited = !ref;
	}
	if (DEBUG_OUT) cout << "PassKcorrect = " << pass << endl;

	if (DEBUG_OUT) cout << "POS = " << POS << " NEG = " << NEG << " pass = " << pass << endl;

// A loop to compute the standard deviation of the local K distribution, and use it to include/exclude K values higher then (meanK +/- K_opt_factor*STDEV)
	if (meanK_STAT)
	{
		std::ofstream k_opt_file("k_stdev.txt" ,std::ios::out);
		ref = Tri.finite_cells_begin()->info().isvisited;
		pass=0;
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			for (int j=0; j<4; j++) {
				neighbour_cell = cell->neighbor(j);
				if (!Tri.is_infinite(neighbour_cell) && neighbour_cell->info().isvisited==ref) {
					pass++;
					STDEV += pow(((cell->info().k_norm())[j]-meanK),2);
				}
			}
			cell->info().isvisited = !ref;
		}
		STDEV = sqrt(STDEV/pass);
		if (DEBUG_OUT) cout << "PassSTDEV = " << pass << endl;
		cout << "STATISTIC K" << endl;
		double k_min = 0, k_max = meanK + K_opt_factor*STDEV;
		cout << "Kmoy = " << meanK << " Standard Deviation = " << STDEV << endl;
		cout << "kmin = " << k_min << " kmax = " << k_max << endl;
		ref = Tri.finite_cells_begin()->info().isvisited;
		pass=0;
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			for (int j=0; j<4; j++) {
				neighbour_cell = cell->neighbor(j);
				if (!Tri.is_infinite(neighbour_cell) && neighbour_cell->info().isvisited==ref) {
					pass+=1;
					if ((cell->info().k_norm())[j]>k_max) {
						(cell->info().k_norm())[j]=k_max;
						(neighbour_cell->info().k_norm())[Tri.mirror_index(cell, j)]= (cell->info().k_norm())[j];
					}
					k_opt_file << K_opt_factor << " " << (cell->info().k_norm())[j] << endl;
				}
			}
			cell->info().isvisited=!ref;
		}
		if (DEBUG_OUT) cout << "PassKopt = " << pass << endl;
	}

	Finite_vertices_iterator vertices_end = Tri.finite_vertices_end();
	Real Vgrains = 0;
	int grains=0;

	for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it !=  vertices_end; V_it++) {
		if (!V_it->info().isFictious && !V_it->info().isGhost) {
			grains +=1;
			Vgrains += 1.33333333 * M_PI * pow(V_it->point().weight(),1.5);
		}
	}
	if (DEBUG_OUT) {
		cout<<grains<<"grains - " <<"Vtotale = " << Vtotale << " Vgrains = " << Vgrains << " Vporale1 = " << (Vtotale-Vgrains) << endl;
		cout << "Vtotalissimo = " << Vtotalissimo/2 << " Vsolid_tot = " << Vsolid_tot/2 << " Vporale2 = " << Vporale/2  << " Ssolid_tot = " << Ssolid_tot << endl<< endl;

		if (!RAVERAGE) cout << "------Hydraulic Radius is used for permeability computation------" << endl << endl;
		else cout << "------Average Radius is used for permeability computation------" << endl << endl;
		cout << "-----Computed_Permeability Periodic-----" << endl;
	}
// 	cout << "Negative Permeabilities = " << count_k_neg << endl; 
}

void PeriodicFlow::Initialize_pressures( double P_zero )
{
        RTriangulation& Tri = T[currentTes].Triangulation();
        Finite_cells_iterator cell_end = Tri.finite_cells_end();

        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++){
		cell->info().setP(P_zero); cell->info().dv()=0;}

        for (int bound=0; bound<6;bound++) {
                int& id = *boundsIds[bound];
		if (id<0) continue;
		Boundary& bi = boundary(id);
		if (!bi.flowCondition) {
			Vector_Cell tmp_cells;
			tmp_cells.resize(10000);
			VCell_iterator cells_it = tmp_cells.begin();
			VCell_iterator cells_end = Tri.incident_cells(T[currentTes].vertexHandles[id],cells_it);
			for (VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
				{(*it)->info().p() = bi.value;(*it)->info().Pcondition=true;
				boundingCells[bound].push_back(*it);}
		}
        }
        IPCells.clear();
        for (unsigned int n=0; n<imposedP.size();n++) {
		Cell_handle cell=Tri.locate(imposedP[n].first);
		
		//check redundancy
		for (unsigned int kk=0;kk<IPCells.size();kk++){
			if (cell==IPCells[kk]) cerr<<"Two imposed pressures fall in the same cell."<<endl;
			else if  (cell->info().Pcondition) cerr<<"Imposed pressure fall in a boundary condition."<<endl;}
// 		cerr<<"cell found : "<<cell->vertex(0)->point()<<" "<<cell->vertex(1)->point()<<" "<<cell->vertex(2)->point()<<" "<<cell->vertex(3)->point()<<endl;
// 		assert(cell);
		IPCells.push_back(cell);
		cell->info().p()=imposedP[n].second;
		cell->info().Pcondition=true;}
	pressureChanged=false;
}

void PeriodicFlow::GaussSeidel(Real dt)
{
    RTriangulation& Tri = T[currentTes].Triangulation();
    int j = 0;
    double m, n, dp_max, p_max, sum_p, p_moy, dp_moy, dp, sum_dp;
    double compFlowFactor=0;
    vector<Real> previousP;
    previousP.resize(Tri.number_of_finite_cells());
    double tolerance = TOLERANCE;
    double relax = RELAX;
    const int num_threads=1;
    bool compressible= fluidBulkModulus>0;

    vector<Real> t_sum_p, t_dp_max, t_sum_dp, t_p_max;
    t_sum_dp.resize(num_threads);
    t_dp_max.resize(num_threads);
    t_p_max.resize(num_threads);
    t_sum_p.resize(num_threads);
    Finite_cells_iterator cell_end = Tri.finite_cells_end();
    do {
        int cell2=0;
        dp_max = 0;
        p_max = 0;
        p_moy=0;
        dp_moy=0;
        sum_p=0;
        sum_dp=0;
        int bb=-1;
        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
            bb++;
            if ( !cell->info().Pcondition && !cell->info().isGhost) {
		cell2++;
		if (compressible && j==0) previousP[bb]=cell->info().shiftedP();
		m=0, n=0;
		for (int j2=0; j2<4; j2++) {
		  if (!Tri.is_infinite(cell->neighbor(j2))) {
			if ( compressible ) {
				compFlowFactor = fluidBulkModulus*dt*cell->info().invVoidVolume();
				m += compFlowFactor*(cell->info().k_norm())[j2]*cell->neighbor(j2)->info().shiftedP();
				if (j==0) n += compFlowFactor*(cell->info().k_norm())[j2];
			} else {
				m += (cell->info().k_norm())[j2]*cell->neighbor(j2)->info().shiftedP();
				if ( isinf(m) && j<10 ) cout << "(cell->info().k_norm())[j2] = " << (cell->info().k_norm())[j2] << " cell->neighbor(j2)->info().shiftedP() = " << cell->neighbor(j2)->info().shiftedP() << endl;
				if (j==0) n += (cell->info().k_norm())[j2];
			} 
		  }
		}
		dp = cell->info().p();
		if (n!=0 || j!=0) {
			if (j==0) { if (compressible) cell->info().inv_sum_k=1/(1+n); else cell->info().inv_sum_k=1/n; }
			if ( compressible ) { 
				cell->info().setP( ( ((previousP[bb] - ((fluidBulkModulus*dt*cell->info().invVoidVolume())*(cell->info().dv()))) + m) * cell->info().inv_sum_k - cell->info().shiftedP()) * relax + cell->info().shiftedP());
			} else {
				cell->info().setP((-(cell->info().dv()-m)*cell->info().inv_sum_k-cell->info().p())*relax+cell->info().shiftedP());
			}
		}
		dp -= cell->info().p();
		dp_max = std::max(dp_max, std::abs(dp));
		p_max = std::max(p_max, std::abs(cell->info().shiftedP()));
		sum_p += cell->info().shiftedP();
		sum_dp += std::abs(dp);
            }
        }
        p_moy = sum_p/cell2;
        dp_moy = sum_dp/cell2;
	j++;
// 	if (j%100==0) cerr <<"j="<<j<<" p_moy="<<p_moy<<" dp="<< dp_moy<<" p_max="<<p_max<<" dp_max="<<dp_max<<endl;
// 	if (j>=40000) cerr<<"\r GS not converged after 40k iterations, break";

    } while ((dp_max/p_max) > tolerance && j<40000 /*&& ( dp_max > tolerance )*//* &&*/ /*( j<50 )*/); //while (j<2); //
//     cerr << "j" << j << endl;
    int cel=0;
    double Pav=0;
    for (Finite_cells_iterator cell = Tri.finite_cells_begin();
            cell != cell_end;
            cell++) {
        cel++;
        Pav+=cell->info().shiftedP();
    }
    Pav/=cel;
}

void PeriodicFlow::DisplayStatistics()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	int Zero =0, Inside=0, Fictious=0, ghostC=0,realC=0, ghostV=0, realV=0;
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
		} 
		else {
			  Fictious+=1;
		}
		if (cell->info().isGhost)  ghostC+=1; else realC+=1;
	}
	int fict=0, real=0;
	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		if (v->info().isFictious) fict+=1;
		else real+=1;
		if (v->info().isGhost)  {ghostV+=1; /*cerr<<"ghost v "<< v->info().id() <<": period=("<<v->info().period[0]<<","<<v->info().period[1]<<","<<v->info().period[2]<<")"<<endl;*/} else realV+=1;
	}
	long Vertices = Tri.number_of_vertices();
	long Cells = Tri.number_of_finite_cells();
	long Facets = Tri.number_of_finite_facets();
	if(DEBUG_OUT) {
		cout << "zeros = " << Zero << endl;
		cout << "There are " << Vertices << " vertices, dont " << fict << " fictious et " << real << " reeeeeel" << std::endl;
		cout << "There are " << ghostV+realV << " vertices, dont " << ghostV << " ghost et " << realV << " reeeeeel" << std::endl;
		cout << "There are " << ghostC+realC << " cells, dont " << ghostC << " ghost et " << realC << " reeeeeel" << std::endl;
		cout << "There are " << Cells << " cells " << std::endl;
		cout << "There are " << Facets << " facets " << std::endl;
		cout << "There are " << Inside << " cells INSIDE." << endl;
		cout << "There are " << Fictious << " cells FICTIOUS." << endl;
	}

	vtk_infinite_vertices = fict;
	vtk_infinite_cells = Fictious;
	num_particles = real;
}

void PeriodicFlow::Average_Relative_Cell_Velocity()
{  
        RTriangulation& Tri = T[currentTes].Triangulation();
        Point pos_av_facet;
        int num_cells = 0;
        double facet_flow_rate = 0;
	double volume_facet_translation = 0;
        Finite_cells_iterator cell_end = Tri.finite_cells_end();
        for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ ) {
		if (cell->info().isGhost) continue;
		cell->info().av_vel() =CGAL::NULL_VECTOR;
                num_cells++;
                for ( int i=0; i<4; i++ ) {
		  volume_facet_translation = 0;
		  if (!Tri.is_infinite(cell->neighbor(i))){
                        Vecteur Surfk = cell->info()-cell->neighbor(i)->info();
                        Real area = sqrt ( Surfk.squared_length() );
			Surfk = Surfk/area;
//                         Vecteur facetNormal = Surfk/area;
                        Vecteur branch = cell->vertex ( facetVertices[i][0] )->point() - cell->info();
                        pos_av_facet = (Point) cell->info() + ( branch*Surfk ) *Surfk;
			facet_flow_rate = (cell->info().k_norm())[i] * (cell->info().shiftedP() - cell->neighbor (i)->info().shiftedP());
			cell->info().av_vel() = cell->info().av_vel() + (facet_flow_rate) * ( pos_av_facet-CGAL::ORIGIN );
		  }}
		cell->info().av_vel() = cell->info().av_vel() /abs(cell->info().volume());
	}
}
void  PeriodicFlow::ComputeEdgesSurfaces()
{
  RTriangulation& Tri = T[currentTes].Triangulation();
  Edge_normal.clear(); Edge_Surfaces.clear(); Edge_ids.clear(); Edge_HydRad.clear();
  Finite_edges_iterator ed_it;
  for ( Finite_edges_iterator ed_it = Tri.finite_edges_begin(); ed_it!=Tri.finite_edges_end();ed_it++ )
  {
    Real Rh;
    if (((ed_it->first)->vertex(ed_it->second)->info().isFictious) && ((ed_it->first)->vertex(ed_it->third)->info().isFictious)) continue;
    else if (((ed_it->first)->vertex(ed_it->second)->info().isFictious) && ((ed_it->first)->vertex(ed_it->third)->info().isGhost)) continue;
    else if (((ed_it->first)->vertex(ed_it->second)->info().isGhost) && ((ed_it->first)->vertex(ed_it->third)->info().isFictious)) continue;
    else if (((ed_it->first)->vertex(ed_it->second)->info().isGhost) && ((ed_it->first)->vertex(ed_it->third)->info().isGhost)) continue;
    int id1 = (ed_it->first)->vertex(ed_it->second)->info().id();
    int id2 = (ed_it->first)->vertex(ed_it->third)->info().id();
    double area = T[currentTes].ComputeVFacetArea(ed_it);
    Edge_Surfaces.push_back(area);
    Edge_ids.push_back(pair<int,int>(id1,id2));
    double radius1 = sqrt((ed_it->first)->vertex(ed_it->second)->point().weight());
    double radius2 = sqrt((ed_it->first)->vertex(ed_it->third)->point().weight());
    Vecteur x = (ed_it->first)->vertex(ed_it->third)->point().point() - (ed_it->first)->vertex(ed_it->second)->point().point();
    Vecteur n = x / sqrt(x.squared_length());
    Edge_normal.push_back(Vector3r(n[0],n[1],n[2]));
    double d = x*n - radius1 - radius2;
    if (radius1<radius2)  Rh = d + 0.45 * radius1;
    else  Rh = d + 0.45 * radius2;
    Edge_HydRad.push_back(Rh);
//     if (DEBUG_OUT) cout<<"id1= "<<id1<<", id2= "<<id2<<", area= "<<area<<", R1= "<<radius1<<", R2= "<<radius2<<" x= "<<x<<", n= "<<n<<", Rh= "<<Rh<<endl;
    
  }
  if (DEBUG_OUT)cout << "size of Edge_ids "<< Edge_ids.size()<< ", size of area "<< Edge_Surfaces.size() << ", size of Rh "<< Edge_HydRad.size()<< ", size of normal "<<Edge_normal.size() << endl;
}

} //namespace CGT

#endif //FLOW_ENGINE


#ifdef LINSOLV
#include "PeriodicFlowLinSolv.ipp"
#endif
