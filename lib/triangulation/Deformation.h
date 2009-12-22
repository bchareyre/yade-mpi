#ifndef _DEFORMATION_H
#define _DEFORMATION_H

#include "Tesselation.h"
#include "Tenseur3.h"
 

// using namespace CGT;
namespace CGT{
// DEF_l_vertices : d�finition de l'ordre de parcours des sommets
// pour la facette k, les indices des 3 sommets sont dans la colonne k
const int DEF_l_vertices [4][3] = { {1, 2, 3}, {0, 3, 2}, {3, 0, 1}, {2, 1, 0} };



//void Grad_u (Facet &f, Vecteur &V, Tenseur3& T); //Prend une facette et un vecteur d�placement moyen pour
												// calculer le gradient, orient� + pour la cellule point�e
												// par facet.first
void Grad_u (Finite_cells_iterator cell, int facet, Vecteur &V, Tenseur3& T);

Vecteur deplacement (Cell_handle cell);		//donne le d�placement d'un sommet de voronoi
Vecteur deplacement (Point &p);				//d�finit un champ homog�ne (pour test)

// Calcul du tenseur d'orientation des voisins
Tenseur_sym3 Orientation_voisins (Tesselation& Tes);



Vecteur deplacement (Point &p) {return (p-CGAL::ORIGIN)/100;}

void Grad_u (Finite_cells_iterator cell, int facet, Vecteur &V, Tenseur3& T)
{	
	Vecteur S = cross_product((cell->vertex(DEF_l_vertices[facet][1])->point()) - (cell->vertex(DEF_l_vertices[facet][0])->point()),
        (cell->vertex(DEF_l_vertices[facet][2])->point()) - (cell->vertex(DEF_l_vertices[facet][1])->point()))/2.f;
    Somme (T, V, S);
}


void Grad_u (Point &p1, Point &p2, Point &p3, Vecteur &V, Tenseur3& T) // rotation 1->2->3 orient�e vers l'ext�rieur
{		
	Vecteur S = cross_product(p2-p1, p3-p2);
    Somme (T, V, S);
};

Tenseur3 Grad_u (Point &p1, Point &p2, Point &p3)
{
	Tenseur3 T;
	Vecteur V = (deplacement(p1)+deplacement(p2)+deplacement(p3))/3;
	Grad_u(p1, p2, p3, V, T);
	return T;
	//Vecteur V = (deplacement(p1)+deplacement(p2)+deplacement(p3))/3;
}



// Calcul du tenseur d'orientation des voisins
Tenseur_sym3 Orientation_voisins (Tesselation &Tes)
{
	RTriangulation& T = Tes.Triangulation();
	Tenseur3 Tens;
	Vecteur v;
	long Nv = 0; //nombre de voisins
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != T.edges_end(); ed_it++)
	{	
		if (!T.is_infinite(*ed_it))
		{
			Nv++;
			v = T.segment(*ed_it).to_vector()/ sqrt(T.segment(*ed_it).squared_length());
			for (int i=1; i<4; i++) for (int j=3; j>=i; j--) Tens(i,j) += v[i-1]*v[j-1];
		}
	} 
	Tens /= Nv;
	return Tens;
}


} //namespace CGT
#endif