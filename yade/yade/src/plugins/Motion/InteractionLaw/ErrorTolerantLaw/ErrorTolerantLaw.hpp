/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ERRORTOLERANTDYNAMICENGINE_HPP__
#define __ERRORTOLERANTDYNAMICENGINE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionLaw.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/banded.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost::numeric;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
@author Olivier Galizzi
*/
class ErrorTolerantLaw : public InteractionLaw
{
	typedef enum{VANISHING,CLAMPED} ContactState;
	vector<ContactState> contactStates;
	vector<int> nbReactivations;
	int maxReactivations;
	int maxIterations;
	float threshold;
	
	public : ErrorTolerantLaw();
	public : ~ErrorTolerantLaw();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : void respondToInteractions(Body* body);

	private : void multA(	ublas::vector<float>& res		, 
				ublas::sparse_matrix<float>& J		,
				ublas::banded_matrix<float>& invM	,
				ublas::sparse_matrix<float>& Jt		,
				ublas::vector<float>& v	);
	
	private : void initInitialGuess(ublas::vector<float>& v);
	
	private : void filter(const ublas::vector<float>& v, ublas::vector<float>& res) ;
	
	private : bool solved(const ublas::vector<float>& v, float threshold) ;

	private : float norm(const ublas::vector<float>& v);
	
	private : bool wrong(ublas::vector<float>& f, const ublas::vector<float>& a) ;

	private : void BCGSolve(	ublas::sparse_matrix<float>& J		,
					ublas::banded_matrix<float>& invM	,
					ublas::sparse_matrix<float>& Jt		,
					ublas::vector<float>& constantTerm	,
					ublas::vector<float>& res);

	
	REGISTER_CLASS_NAME(ErrorTolerantLaw);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(ErrorTolerantLaw,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ERRORTOLERANTDYNAMICENGINE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
