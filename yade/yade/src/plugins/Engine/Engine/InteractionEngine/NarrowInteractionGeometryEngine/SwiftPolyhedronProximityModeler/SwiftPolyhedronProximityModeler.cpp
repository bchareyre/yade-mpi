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

#include "SwiftPolyhedronProximityModeler.hpp"
#include <yade-common/MacroMicroContactGeometry.hpp>
#include <yade/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SwiftPolyhedronProximityModeler::SwiftPolyhedronProximityModeler() : Engine()
{
	first = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

SwiftPolyhedronProximityModeler::~SwiftPolyhedronProximityModeler()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SwiftPolyhedronProximityModeler::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if (first)
	{
		scene = new SWIFT_Scene( false, false );
			
		ids.clear();
		int id;

		shared_ptr<Body> b;
		PolyhedralSweptSphere * pss;
		for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext())
		{
			b = bodies->getCurrent();
			pss = static_cast<PolyhedralSweptSphere*>(b->interactionGeometry.get());
			getSwiftInfo(pss,vs,fs,fv,vn,fn); //FIXME : need to make a new inside getSwiftInfo ??
			scene->Add_Convex_Object( (SWIFT_Real*)vs, fs, vn, fn, id , 
							DEFAULT_FIXED, 
							DEFAULT_ORIENTATION, 
							DEFAULT_TRANSLATION,
							DEFAULT_SCALE,
							DEFAULT_BOX_SETTING,
							DEFAULT_BOX_ENLARGE_REL,
							DEFAULT_BOX_ENLARGE_ABS,
							fv,
							DEFAULT_CUBE_ASPECT_RATIO);
	
			ids.push_back(id);
			
			Vector3r rotColumn[3];
			b->physicalParameters->se3.orientation.toRotationMatrix(rotColumn);
			
			R[0] = rotColumn[0][0]; R[1] = rotColumn[1][0]; R[2] = rotColumn[2][0];
			R[3] = rotColumn[0][1]; R[4] = rotColumn[1][1]; R[5] = rotColumn[2][1];
			R[6] = rotColumn[0][2]; R[7] = rotColumn[1][2]; R[8] = rotColumn[2][2];
			
			Vector3r p = b->physicalParameters->se3.position;
			T[0] = p[0]; T[1] = p[1]; T[2] = p[2];
	
			scene->Set_Object_Transformation( id, (SWIFT_Real*)R, (SWIFT_Real*)T );
		}	
		first = false;
	}
		
	scene->Deactivate();

// FIXME : object trnasfor can be initilized 2 or more times		
	
	
	shared_ptr<InteractionContainer>& potentialCollisions = ncb->volatileInteractions;
	for( potentialCollisions->gotoFirstPotential() ; potentialCollisions->notAtEndPotential() ; potentialCollisions->gotoNextPotential())
	{
		const shared_ptr<Interaction>& interaction = potentialCollisions->getCurrent();
		
		shared_ptr<Body>& b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body>& b2 = (*bodies)[interaction->getId2()];
		
		scene->Activate(interaction->getId1(),interaction->getId2());

		Vector3r rotColumn[3];
		Vector3r p;
				
		b1->physicalParameters->se3.orientation.toRotationMatrix(rotColumn);
		R[0] = rotColumn[0][0]; R[1] = rotColumn[1][0]; R[2] = rotColumn[2][0];
		R[3] = rotColumn[0][1]; R[4] = rotColumn[1][1]; R[5] = rotColumn[2][1];
		R[6] = rotColumn[0][2]; R[7] = rotColumn[1][2]; R[8] = rotColumn[2][2];
		p = b1->physicalParameters->se3.position;
		T[0] = p[0]; T[1] = p[1]; T[2] = p[2];
		scene->Set_Object_Transformation( ids[interaction->getId1()], (SWIFT_Real*)R, (SWIFT_Real*)T );

		b2->physicalParameters->se3.orientation.toRotationMatrix(rotColumn);
		R[0] = rotColumn[0][0]; R[1] = rotColumn[1][0]; R[2] = rotColumn[2][0];
		R[3] = rotColumn[0][1]; R[4] = rotColumn[1][1]; R[5] = rotColumn[2][1];
		R[6] = rotColumn[0][2]; R[7] = rotColumn[1][2]; R[8] = rotColumn[2][2];
		p = b2->physicalParameters->se3.position;
		T[0] = p[0]; T[1] = p[1]; T[2] = p[2];
		scene->Set_Object_Transformation( ids[interaction->getId2()], (SWIFT_Real*)R, (SWIFT_Real*)T );
			
	}

		
	int nbPairs;
	int * oids;
	int * nbContacts;
	SWIFT_Real * nearestPts;
	SWIFT_Real * distances;
	
	scene->Query_Contact_Determination(true,SWIFT_INFINITY,nbPairs,&oids,&nbContacts,&distances,&nearestPts);
	cout << nbPairs << endl;
	for(int i=0 ; i<nbPairs;i++)
	{
		int id1 = oids[2*i];
		int id2 = oids[2*i+1];
		if (id1<id2)
			swap(id1,id2);
		
		shared_ptr<Body> b1, b2;
		bodies->find(id1,b1);
		bodies->find(id2,b2);
		
		PolyhedralSweptSphere * pss1 = static_cast<PolyhedralSweptSphere*>(b1->interactionGeometry.get());
		PolyhedralSweptSphere * pss2 = static_cast<PolyhedralSweptSphere*>(b2->interactionGeometry.get());
		
		if (distances[2*i]<pss1->radius+pss2->radius)
		{
			shared_ptr<Interaction> collision = potentialCollisions->find(id1,id2);

			collision->isReal = true;
			
			shared_ptr<MacroMicroContactGeometry> cg(new MacroMicroContactGeometry);
			
			Vector3r p1(nearestPts[3*i],nearestPts[3*i+1],nearestPts[3*i+2]);
			Vector3r p2(nearestPts[3*i+3],nearestPts[3*i+4],nearestPts[3*i+5]);
			p1 = b1->physicalParameters->se3*p1;
			p2 = b2->physicalParameters->se3*p2;
	
			cg->contactPoint = 0.5*(p1+p2);
			cg->normal = p2-p1;
			cg->penetrationDepth = cg->normal.normalize();
			cg->radius1 = 0.5*(b1->boundingVolume->max-b1->boundingVolume->min).length();
			cg->radius2 = 0.5*(b2->boundingVolume->max-b2->boundingVolume->min).length();
	
			collision->interactionGeometry = cg;
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SwiftPolyhedronProximityModeler::getSwiftInfo(const PolyhedralSweptSphere* pss, double *& v, int *& f, int*& fv, int& nbVertices, int& nbFaces)
{
	nbVertices = pss->vertices.size();
	nbFaces = pss->faces.size();
	fv = new int[nbFaces];
	
	int totValence = 0;
	for(int i=0;i<pss->faces.size();i++)
	{
		totValence += pss->faces[i].size();
		fv[i] = pss->faces[i].size();
	}
	
	v = new double[3*nbVertices];
	f = new int[totValence*nbFaces];
	
	for(int i=0 ; i<nbVertices ; i++)
	{
		v[3*i] = pss->vertices[i][0];
		v[3*i+1] = pss->vertices[i][1];
		v[3*i+2] = pss->vertices[i][2];
	}	

	int k=0;
	for(int i=0;i<pss->faces.size();i++)
		for(int j=0;j<pss->faces[i].size();j++)
		{
			f[k] = pss->faces[i][j];
			k++;
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

