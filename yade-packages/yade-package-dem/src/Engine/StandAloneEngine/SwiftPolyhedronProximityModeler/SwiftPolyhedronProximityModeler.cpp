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
#include "SpheresContactGeometry.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/MetaBody.hpp>

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

		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		for(  ; bi!=biEnd ; ++bi )
		{
			b = *bi;
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

/*		scene->Activate();
		int nbPairs;
		int * oids;
		int * nbContacts;
		SWIFT_Real * nearestPts;
		SWIFT_Real * distances;
		SWIFT_Real * normals;
		scene->Query_Contact_Determination(true,SWIFT_INFINITY,nbPairs,&oids,&nbContacts,&distances,&nearestPts,&normals);*/
	}
		
	scene->Deactivate();

// FIXME : object trnasfor can be initilized 2 or more times		
	
	set<int> bodiesToUpdates;
	shared_ptr<InteractionContainer>& potentialCollisions = ncb->volatileInteractions;

	InteractionContainer::iterator ii    = ncb->volatileInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->volatileInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		const shared_ptr<Interaction>& interaction = *ii;
		bodiesToUpdates.insert(interaction->getId1());
		bodiesToUpdates.insert(interaction->getId2());
		scene->Activate(interaction->getId1(),interaction->getId2());
	}
	
	set<int>::iterator b2ui = bodiesToUpdates.begin();
	set<int>::iterator b2uiEnd = bodiesToUpdates.end();
	for( ; b2ui != b2uiEnd ; ++b2ui)
	{	
		shared_ptr<Body>& b = (*bodies)[*b2ui];

		Vector3r rotColumn[3];
		Vector3r p;
				
		b->physicalParameters->se3.orientation.toRotationMatrix(rotColumn);
		R[0] = rotColumn[0][0]; R[1] = rotColumn[1][0]; R[2] = rotColumn[2][0];
		R[3] = rotColumn[0][1]; R[4] = rotColumn[1][1]; R[5] = rotColumn[2][1];
		R[6] = rotColumn[0][2]; R[7] = rotColumn[1][2]; R[8] = rotColumn[2][2];
		p = b->physicalParameters->se3.position;
		T[0] = p[0]; T[1] = p[1]; T[2] = p[2];
		scene->Set_Object_Transformation( *b2ui, (SWIFT_Real*)R, (SWIFT_Real*)T );
	}

		
	int nbPairs;
	int * oids;
	int * nbContacts;
	SWIFT_Real * nearestPts;
	SWIFT_Real * distances;
	SWIFT_Real * normals;
	int * featuresType;
	int * featuresId;
	scene->Query_Contact_Determination(true,SWIFT_INFINITY,nbPairs,&oids,&nbContacts,&distances,&nearestPts,&normals,&featuresType,&featuresId);

	for(int i=0 ; i<nbPairs ; i++)
	{
		int id1 = oids[2*i];
		int id2 = oids[2*i+1];
		//cout << id1 << " " << id2 << endl;
		if (oids[2*i]>oids[2*i+1]) cout << "greater" << endl;
		shared_ptr<Body> b1, b2;
			
		bodies->find(id1,b1);
		bodies->find(id2,b2);
		
		PolyhedralSweptSphere * pss1 = static_cast<PolyhedralSweptSphere*>(b1->interactionGeometry.get());
		PolyhedralSweptSphere * pss2 = static_cast<PolyhedralSweptSphere*>(b2->interactionGeometry.get());

		if (distances[i]<pss1->radius+pss2->radius)
		{
			shared_ptr<Interaction> collision = potentialCollisions->find(id1,id2);

			collision->isReal = true;
			
			shared_ptr<SpheresContactGeometry> cg;
			if (collision->interactionGeometry)
				cg = dynamic_pointer_cast<SpheresContactGeometry>(collision->interactionGeometry);
			else
				cg = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());

			Vector3r p1(nearestPts[6*i],nearestPts[6*i+1],nearestPts[6*i+2]);
			Vector3r p2(nearestPts[6*i+3],nearestPts[6*i+4],nearestPts[6*i+5]);

			p1 = b1->physicalParameters->se3*p1;
			p2 = b2->physicalParameters->se3*p2;

			cg->normal[0] = normals[3*i+0];
			cg->normal[1] = normals[3*i+1];
			cg->normal[2] = normals[3*i+2];
			
			//cg->normal = p1-p2;
			//cg->normal.normalize();

			//cout << cg->normal << " | " << normals[3*i+0] << " " << normals[3*i+1] << " "  << normals[3*i+2] << endl;
			
			if (featuresType[2*i]==SWIFT_VERTEX && featuresType[2*i+1]==SWIFT_VERTEX)
				cout << "vv" << endl;
			else if (featuresType[2*i]==SWIFT_EDGE && featuresType[2*i+1]==SWIFT_VERTEX)
				cout << "ev" << endl;
			else if (featuresType[2*i]==SWIFT_VERTEX && featuresType[2*i+1]==SWIFT_EDGE)
				cout << "ve" << endl;
			else if (featuresType[2*i]==SWIFT_VERTEX && featuresType[2*i+1]==SWIFT_FACE)
				cout << "vf" << endl;
			else if (featuresType[2*i]==SWIFT_FACE && featuresType[2*i+1]==SWIFT_VERTEX)
				cout << "fv" << endl;
			else if (featuresType[2*i]==SWIFT_EDGE && featuresType[2*i+1]==SWIFT_EDGE)
				cout << "ee" << endl;

			p1 -= cg->normal*pss1->radius;
			p2 += cg->normal*pss2->radius;
			
// 			if (featuresType[2*i]==SWIFT_VERTEX && featuresType[2*i+1]==SWIFT_EDGE ||
// 			    featuresType[2*i]==SWIFT_EDGE && featuresType[2*i+1]==SWIFT_VERTEX ||
// 			    featuresType[2*i]==SWIFT_EDGE && featuresType[2*i+1]==SWIFT_EDGE)
// 			{
// 				Vector3r tmpp=p1;
// 				p1 = p2;
// 				p2 = tmpp;
// 				cg->normal = -cg->normal;
//  			}
// 			else
// 			{
// 				p1 -= cg->normal*pss1->radius;
// 				p2 += cg->normal*pss2->radius;
// 				cg->normal = -cg->normal;				
// 			}
			cg->contactPoint = 0.5*(p1+p2);

			cg->penetrationDepth = fabs(distances[i]-pss1->radius-pss2->radius);//(p2-p1).length();
			
			cg->radius1 = 0.5*(b1->boundingVolume->max-b1->boundingVolume->min).length();
			cg->radius2 = 0.5*(b2->boundingVolume->max-b2->boundingVolume->min).length();

			if (!collision->interactionGeometry)
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
	for(unsigned int i=0;i<pss->faces.size();i++)
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
	for(unsigned int i=0;i<pss->faces.size();i++)
		for(unsigned int j=0;j<pss->faces[i].size();j++)
		{
			f[k] = pss->faces[i][j];
			k++;
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

