
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

#ifndef __SDECDYNAMICENGINE_HPP__
#define __SDECDYNAMICENGINE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "DynamicEngine.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <set>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SDECDynamicEngine : public DynamicEngine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Typedef											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	struct interactionInfo
	{
		//int id1;
		//int id2;
		//int id;
		float	kn;				// normal elastic constant.
		float	ks;				// shear elastic constant.
		float	initialKn;			// initial normal elastic constant.
		float	initialKs;			// initial shear elastic constant.
		float	equilibriumDistance;		// equilibrium distance
		float initialEquilibriumDistance;	// initial equilibrium distance
		Vector3 prevNormal;			// unit normal of the contact plane.
		Vector3	normal;				// new unit normal of the contact plane.
		Vector3 normalForce;			// normal force applied on a DE
		Vector3 shearForce;			// shear force applied on a DE
		bool accessed;
	};

	//map<pair<int,int> , interactionInfo > originalInteractions;

	/*struct lessThanPair
	{
		bool operator()(const pair<int,int> p1, const pair<int,int> p2) const
		{
			return (p1.first<p2.first || (p1.first==p2.first && p1.second<p2.second));
		}
	};*/

	struct lessThanPair
	{
		bool operator()(const pair<int,interactionInfo>& p1, const pair<int,interactionInfo>& p2) const
		{
			return (p1.first<p2.first);
		}
	};
	
	//map<pair<int,int> , interactionInfo, lessThanPair> prevInteractions;
	//list<interactionInfo> prevInteractions;
	vector<set<pair<int,interactionInfo>,lessThanPair > > interactionsPerBody;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : std::vector<Vector3> forces;
	private : std::vector<Vector3> moments;
	private : bool first;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor								        ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : SDECDynamicEngine();

	/*! Destructor */
	public : virtual ~SDECDynamicEngine();

	public : void processAttributes();
	public : void registerAttributes();

	public : void respondToCollisions(Body* body, const std::list<shared_ptr<Interaction> >& interactions);

	REGISTER_CLASS_NAME(SDECDynamicEngine);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(SDECDynamicEngine,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SDECDYNAMICENGINE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
