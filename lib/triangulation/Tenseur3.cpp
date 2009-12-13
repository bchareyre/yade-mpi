// Tenseur3.cpp : d�finit le point d'entr�e pour l'application console.
//

#include "stdafx.h"
#include "Tenseur3.h"
#include "def_types.h" //pour d�finition de la classe "Vecteur"

using namespace std;
namespace CGT {


//const Tenseur3 NULL_TENSEUR3 = Tenseur3(0,0,0,0,0,0,0,0,0);




// Classe m�re "Tens" dont d�rive toutes les classes tenseur

Real Tens::Norme2(void)
{
	Real N=0;
    for (int i=1; i<=3; i++)
		for (int j=1; j<=3; j++)
			N+= pow(operator ()(i,j), 2);
    return N;
}


// D�finition d'op�rations compl�mentaires sur vecteurs et matrices


Vecteur operator* (Tens& tens, Vecteur& vect)
{
	Vecteur result;
	result = Vecteur( tens(1,1)*vect.x()+ tens(1,2)*vect.y()+ tens(1,3)*vect.z(),
		tens(2,1)*vect.x()+ tens(2,2)*vect.y()+ tens(2,3)*vect.z(),
		tens(3,1)*vect.x()+ tens(3,2)*vect.y()+ tens(3,3)*vect.z() );
	return result;
}

Vecteur& NormalizedVecteur (Vecteur& vect)
{
	vect = vect*(1/sqrt(pow(vect.x(),2)+pow(vect.y(),2)+pow(vect.z(),2)));
	return vect;
}


///////////		 Classe Tenseur3		////////////

Tenseur3::Tenseur3(bool init)
{
	if (init)
	{
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++) T[i][j] = 0;
		}
	}
}

Tenseur3::~Tenseur3(void)
{
}

Tenseur3::Tenseur3(const Tenseur3& source)
{
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++) T[i][j] = source.T[i][j];
	}
}

Tenseur3::Tenseur3(Real a11, Real a12, Real a13,
				   Real a21, Real a22, Real a23,
				   Real a31, Real a32, Real a33)
{
	T[0][0] = a11;
	T[0][1] = a12;
	T[0][2] = a13;
	T[1][0] = a21;
	T[1][1] = a22;
	T[1][2] = a23;
	T[2][0] = a31;
	T[2][1] = a32;
	T[2][2] = a33;
}


Tenseur3 &Tenseur3::operator=(const Tenseur3& source)
{
	if (&source != this)
	{
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++) T[i][j] = source.T[i][j];
		}
	}
	return *this;
}

Tenseur3 &Tenseur3::operator/=(Real d)
{	
	if (d!=0)
	{
		d = 1/d;
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++) T[i][j] *= d;
		}
	}
	return *this;
}


Tenseur3 & Tenseur3::operator +=(const Tenseur3 & source)
{
	for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++) T[i][j] += source.T[i][j];
		}
	return *this;
}



///////////		 Classe Tenseur_sym3		////////////

Tenseur_sym3::Tenseur_sym3(bool init)
{
	if (init)
	{
		for (int i=0; i<6; i++) T[i] = 0;
	}
}

Tenseur_sym3::~Tenseur_sym3(void)
{
}

Tenseur_sym3::Tenseur_sym3(const Tenseur_sym3& source)
{
	for (int i=0; i<6; i++)
	{
		for (int i=0; i<6; i++) T[i] = source.T[i];
	}
}

Tenseur_sym3::Tenseur_sym3(const Tenseur3& source)
{
	for (int i=1; i<=3; i++)
	{
		T[i-1]=source(i,i);
		for (int j=3; j>i; j--) T[i+j] = (source(i,j)+source(j,i))*0.5;
	}
}

Tenseur_sym3::Tenseur_sym3(Real a11, Real a22, Real a33,
				   Real a12, Real a13, Real a23)
{
	T[0] = a11;
	T[1] = a22;
	T[2] = a33;
	T[3] = a12;
	T[4] = a13;
	T[5] = a23;
}


Tenseur_sym3 &Tenseur_sym3::operator=(const Tenseur_sym3& source)
{
	if (&source != this)
	{
		for (int i=0; i<6; i++)	T[i] = source.T[i];
	}
	return *this;
}

Tenseur_sym3 &Tenseur_sym3::operator/=(Real d)
{
	if (d!=0) {
		d=1/d;
		for (int i=0; i<6; i++)	T[i]*= d;}
	return *this;
}

Real Tenseur_sym3::operator() (int i, int j) const 
{
	if (i==j) return T[i-1];
	else return T[i+j];
}

Real &Tenseur_sym3::operator() (int i, int j)
{	
	if (i==j) return T[i-1];
	else return T[i+j];
}

Tenseur_sym3 Tenseur_sym3::Deviatoric (void) const	//retourne la partie d�viatoire
{
	Tenseur_sym3 temp(*this);
	Real spheric = temp.Trace()/3;
	temp(1,1)-= spheric;
	temp(2,2)-= spheric;
	temp(3,3)-= spheric;
	return temp;
}

///////////		 Classe Tenseur_anti3		////////////

Tenseur_anti3::Tenseur_anti3(bool init)
{
	if (init)
	{
		for (int i=0; i<6; i++) T[i] = 0;
	}
}

Tenseur_anti3::~Tenseur_anti3(void)
{
}

Tenseur_anti3::Tenseur_anti3(const Tenseur_anti3& source)
{
	for (int i=0; i<6; i++)
	{
		for (int i=0; i<6; i++) T[i] = source.T[i];
	}
}

Tenseur_anti3::Tenseur_anti3(const Tenseur3& source)
{
	for (int i=1; i<=3; i++)
	{
		T[i-1]=0;
		for (int j=3; j>i; j--) T[i+j] = (source(i,j)-source(j,i))*0.5;
	}
}

Tenseur_anti3::Tenseur_anti3(Real a11, Real a22, Real a33,
				   Real a12, Real a13, Real a23)
{
	T[0] = a11;
	T[1] = a22;
	T[2] = a33;
	T[3] = a12;
	T[4] = a13;
	T[5] = a23;
}


Tenseur_anti3 &Tenseur_anti3::operator=(const Tenseur_anti3& source)
{
	if (&source != this)
	{
		for (int i=0; i<6; i++)	T[i] = source.T[i];
	}
	return *this;
}

Tenseur_anti3 &Tenseur_anti3::operator/=(Real d)
{
	if (d!=0)	for (int i=0; i<6; i++)	T[i]/= d;
	return *this;
}

Real Tenseur_anti3::operator() (int i, int j) const 
{
	if (i==j) return T[i-1];
	else
	{
		if (i<j) return T[i+j];
		else return -T[i+j];
	}
}

//Real &Tenseur_anti3::operator() (int i, int j) //Supprim�e car pb. pour retourner une r�f�rence vers -T[i+j]
//{	
//	if (i==j) return T[i-1];
//	else 
//	{
//		if (i<j) return T[i+j];
//		else return T[i+j];
//	}
//}




void Tenseur_produit (Vecteur &v1, Vecteur &v2, Tenseur3 &result)
{
	result(1,1) = v1.x()*v2.x();
	result(1,2) = v1.x()*v2.y(); 
	result(1,3) = v1.x()*v2.z(); 
	result(2,1) = v1.y()*v2.x();
	result(2,2) = v1.y()*v2.y(); 
	result(2,3) = v1.y()*v2.z(); 
	result(3,1) = v1.z()*v2.x();
	result(3,2) = v1.z()*v2.y(); 
	result(3,3) = v1.z()*v2.z(); 
}

void Somme (Tenseur3 &result, Vecteur &v1, Vecteur &v2)
{
	result(1,1) += v1.x()*v2.x();
	result(1,2) += v1.x()*v2.y(); 
	result(1,3) += v1.x()*v2.z(); 
	result(2,1) += v1.y()*v2.x();
	result(2,2) += v1.y()*v2.y(); 
	result(2,3) += v1.y()*v2.z(); 
	result(3,1) += v1.z()*v2.x();
	result(3,2) += v1.z()*v2.y(); 
	result(3,3) += v1.z()*v2.z(); 
}


// Fonctions d'�criture

std::ostream&
operator<<(std::ostream& os, const Tenseur3& T)
  
{
	for (int j=1 ; j<4; j++)
	{
		for (int i=1; i<4; i++)
		{
			os << T(j,i) << " ";
		}
		os  << endl;
	}	
	return os;
}

std::ostream&
operator<<(std::ostream& os, const Tenseur_sym3& T)
  
{
	for (int j=1 ; j<4; j++)
	{
		for (int i=1; i<4; i++)
		{
			os << T(j,i) << " ";
		}
		os  << endl;
	}	
	return os;
}

std::ostream&
operator<<(std::ostream& os, const Tenseur_anti3& T)
  
{
	for (int j=1 ; j<4; j++)
	{
		for (int i=1; i<4; i++)
		{
			os << (Real) T(j,i) << (string) " ";
		}
		os  << endl;
	}	
	return os;
}

// std::ofstream&
// operator<<(std::ofstream& os, Tenseur3& T)
//   
// {
// 	for (int j=1 ; j<4; j++)
// 	{
// 		for (int i=1; i<4; i++)
// 		{
// 			os << T(j,i) << " ";
// 		}
// 		os  << endl;
// 	}	
// 	return os;
// }
// 
// std::ofstream&
// operator<<(std::ofstream& os, Tenseur_sym3& T)
//   
// {
// 	for (int j=1 ; j<4; j++)
// 	{
// 		for (int i=1; i<4; i++)
// 		{
// 			os << T(j,i) << " ";
// 		}
// 		os  << endl;
// 	}	
// 	return os;
// }
// 
// std::ofstream&
// operator<<(std::ofstream& os, Tenseur_anti3& T)
//   
// {
// 	for (int j=1 ; j<4; j++)
// 	{
// 		for (int i=1; i<4; i++)
// 		{
// 			os << (Real) T(j,i) << (string) " ";
// 		}
// 		os  << endl;
// 	}	
// 	return os;
// }


} // namespace CGT
