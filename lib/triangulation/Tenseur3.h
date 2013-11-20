#pragma once

#include "def_types.h" 
#include <iostream>
#include <fstream>

namespace CGT {

#define NORMALIZE(vecteur) ((vecteur) = (vecteur)*(1.0/sqrt(pow((vecteur)[0],2)+pow((vecteur)[1],2)+pow((vecteur)[2],2))))

class Tens;
class Tenseur3;
class Tenseur_sym3;
class Tenseur_anti3;

Vecteur operator* ( Tens& tens, Vecteur& vect );
Vecteur& NormalizedVecteur ( Vecteur& vect );


void Tenseur_produit ( Vecteur &v1, Vecteur &v2, Tenseur3 &result );
void Somme ( Tenseur3 &result, Vecteur &v1, Vecteur &v2 );

std::ostream& operator<< ( std::ostream& os,const Tenseur3& T );
std::ostream& operator<< ( std::ostream& os,const Tenseur_sym3& T );
std::ostream& operator<< ( std::ostream& os,const Tenseur_anti3& T );

class Tens
{
	public:
		Tens ( void ) {}
		virtual ~Tens ( void ) {}
		virtual Real operator() ( int i, int j ) const {return 0;}
		Real Norme2 ( void );
		Real Norme ( void ) {return sqrt ( Norme2() );}
		Real Trace ( void )
		{
			return this->operator () ( 1,1 )
				   + this->operator () ( 2,2 )
				   + this->operator () ( 3,3 );
		}
};

class Tenseur3 : public Tens
{
	private:
		Real T [3] [3];

	public:
		Tenseur3 ( bool init = true );// Sp�cifier "false" pour �conomiser le temps d'initialisation du tableau
		virtual ~Tenseur3 ( void );
		Tenseur3 ( const Tenseur3& source );
		Tenseur3 ( Real a11, Real a12, Real a13,
				   Real a21, Real a22, Real a23,
				   Real a31, Real a32, Real a33 );

		Tenseur3& operator= ( const Tenseur3& source );
		Tenseur3& operator/= ( Real d );
		Tenseur3& operator+= ( const Tenseur3& source );
		Real operator() ( int i, int j ) const {return T[i-1][j-1];}
		Real &operator() ( int i, int j ) {return T[i-1][j-1];}

		virtual void reset ( void ) {for ( int i=0; i<3; i++ ) for ( int j=0; j<3; j++ ) T[i][j] = 0;}

};

class Tenseur_sym3 : public Tens
{
	private:
		Real T [6];

	public:
		Tenseur_sym3 ( bool init = true );// Sp�cifier "false" pour �conomiser le temps d'initialisation du tableau
		~Tenseur_sym3 ( void );
		Tenseur_sym3 ( const Tenseur_sym3& source );
		Tenseur_sym3 ( const Tenseur3& source );
		Tenseur_sym3 ( Real a11, Real a22, Real a33,
					   Real a12, Real a13, Real a23 );

		Tenseur_sym3& operator= ( const Tenseur_sym3& source );
		Tenseur_sym3& operator/= ( Real d );
		Tenseur_sym3 Deviatoric ( void ) const; //retourne la partie d�viatoire
		Real operator() ( int i, int j ) const;
		Real &operator() ( int i, int j );

		void reset ( void ) {for ( int i=0; i<6; i++ ) T[i] = 0;}

};

class Tenseur_anti3 : public Tens
{
	private:
		Real T [6];

	public:
		Tenseur_anti3 ( bool init = true );// Sp�cifier "false" pour �conomiser le temps d'initialisation du tableau
		virtual ~Tenseur_anti3 ( void );
		Tenseur_anti3 ( const Tenseur_anti3& source );
		Tenseur_anti3 ( const Tenseur3& source );
		Tenseur_anti3 ( Real a11, Real a22, Real a33,
						Real a12, Real a13, Real a23 );

		Tenseur_anti3& operator= ( const Tenseur_anti3& source );
		Tenseur_anti3& operator/= ( Real d );
		Real operator() ( int i, int j ) const;
		//Real &operator() (int i, int j); //Supprim�e car pb. pour retourner une r�f�rence vers -T[i+j]

		void reset ( void ) {for ( int i=0; i<6; i++ ) T[i] = 0;}

};

static const Tenseur3 NULL_TENSEUR3 ( 0,0,0,0,0,0,0,0,0 );

}
