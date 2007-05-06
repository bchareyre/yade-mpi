// © 2007 Václav Šmilauer <eudoxos@arcig.cz>

#include"Tetra.hpp"

char* yadePluginClasses[]={
	// self-contained in hpp:
	"TetraMold",
	"TetraBang", 
	"Tetrahedron2TetraMold",
	"TetraAABB", 
	// some code in cpp (this file):
	"TetraLaw",	 
	"Tetra2TetraBang",
	"TetraDraw",
	NULL /*sentinel*/
};

#include <boost/shared_ptr.hpp>

#include <yade/core/Interaction.hpp>
#include <yade/core/PhysicalAction.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>

#include <yade/lib-opengl/OpenGLWrapper.hpp>

#include <yade/pkg-common/AABB.hpp>
#include <yade/pkg-common/Tetrahedron.hpp>
#include <yade/pkg-common/ElasticBodyParameters.hpp>
#include <yade/pkg-common/SimpleElasticInteraction.hpp>


bool Tetra2TetraBang::go(const shared_ptr<InteractingGeometry>& cm1,const shared_ptr<InteractingGeometry>& cm2,const Se3r& se31,const Se3r& se32,const shared_ptr<Interaction>& c){
	TetraMold* A = static_cast<TetraMold*>(cm1.get());
	TetraMold* B = static_cast<TetraMold*>(cm2.get());
	return false;
	
/*	shared_ptr<TetraBang> imt;
	// depending whether it's a new interaction: create new one, or use the existing one.
	if (c->isNew)
		imt = shared_ptr<TetraBang>(new TetraBang());
	else
		imt = dynamic_pointer_cast<TetraBang>(c->interactionGeometry);	

	bool isInteracting = false;
	for(int i=0 ; i<4 ; ++i )
		for(int j=0 ; j<4 ; ++j)
		{
			// both spheres must be calculated in global coordinate system
			//
			// c1,c2,c3,c4 are sphere centers in local coordinate system of tetrahedron,
			//             they must by rotated, so that they have the rotation of tetrahedron
			//             they must be translated, so that they have global coordinates of tetrahedron
			calcTwoSpheres(	
				// (Quaternionr)orientation * (Vector3r)(c1)     : rotates the sphere center from local into global coordinate system.
				//                             + se31.position   : translates it, to stay in the x,y,z in global coordinate system
				se31.orientation*(&(t1->c1))[i]+se31.position,

				(&(t1->r1))[i], // radius of sphere from first TetraMold

				se32.orientation*(&(t2->c1))[j]+se32.position,
				(&(t2->r1))[j],

				imt->penetrationDepths[i][j], imt->normals[i][j], imt->contactPoints[i][j] );

			if( imt->penetrationDepths[i][j] > 0 )
				isInteracting = true;
		}


	c->interactionGeometry = imt;
	return isInteracting; */
}

bool Tetra2TetraBang::goReverse(const shared_ptr<InteractingGeometry>& cm1,const shared_ptr<InteractingGeometry>& cm2,const Se3r& se31,const Se3r& se32,const shared_ptr<Interaction>& c){
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	/*if (isInteracting)
	{
		TetraBang* itm = static_cast<TetraBang*>(c->interactionGeometry.get());
		// reverse direction of normals
		for(int i=0 ; i<4 ; ++i )
			for(int j=0 ; j<4 ; ++j)
				itm->normals[i][j] =  -1.0*( itm->normals[i][j] );
	}*/
	return isInteracting;
}


void TetraLaw::action(Body* body)
{
	return;
#if 0
 	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)	// isReal means that Tetra2TetraBang returned true
					//                or Tetra2BoxBang           returned true
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
// you can use groups if you want for example a simulation that has different
// types of InteractionSolvers that talk with different bodies, for example
// if you want together TetraLaw (solving tetrahedrons) and ElasticContactLaw (solving spheres)
//
// I already have a better solution for that, so you will not need to check
// groups inside this function, but they will be checked outside. I just need
// time to implement this :)
//
//			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
//				continue;
	
			ElasticBodyParameters* de1 				= dynamic_cast<ElasticBodyParameters*>((*bodies)[id1]->physicalParameters.get());
			ElasticBodyParameters* de2 				= dynamic_cast<ElasticBodyParameters*>((*bodies)[id2]->physicalParameters.get());
			TetraBang* currentContactGeometry	= dynamic_cast<TetraBang*>(contact->interactionGeometry.get());
			SimpleElasticInteraction* currentContactPhysics		= dynamic_cast<SimpleElasticInteraction*>(contact->interactionPhysics.get());

			for(int i=0 ; i<4 ; ++i )
				for(int j=0 ; j<4 ; ++j)
				{
					Real un					= currentContactGeometry->penetrationDepths[i][j];
					if(un > 0)
					{
						Vector3r force			= currentContactPhysics->kn*un*currentContactGeometry->normals[i][j];

						Vector3r x			= currentContactGeometry->contactPoints[i][j];
						Vector3r c1x			= (x - de1->se3.position);
						Vector3r c2x			= (x - de2->se3.position);

						// it will be some macro(	body->physicalActions,	ActionType , bodyId )
						static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= force;
						static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += force;

						static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(force);
						static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(force);
					}
				}

		}
	}
#endif
}


void TetraDraw::go(const shared_ptr<InteractingGeometry>& cm, const shared_ptr<PhysicalParameters>&,bool)
{
  	glMaterialv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,Vector3f(cm->diffuseColor[0],cm->diffuseColor[1],cm->diffuseColor[2]));
	glColor3v(cm->diffuseColor);
	TetraMold* t=static_cast<TetraMold*>(cm.get());
	if (0) { // wireframe, as for Tetrahedron
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			#define __ONEWIRE(a,b) glVertex3dv(t->v[a]);glVertex3dv(t->v[b])
				__ONEWIRE(0,1);__ONEWIRE(0,2);__ONEWIRE(0,3);__ONEWIRE(1,2);__ONEWIRE(1,3);__ONEWIRE(2,3);
			#undef __ONEWIRE
		glEnd();
	}
	else
	{
		Vector3r center = (t->v[0]+t->v[1]+t->v[2]+t->v[3])*.25, faceCenter, n;
		glDisable(GL_CULL_FACE); glEnable(GL_LIGHTING);
		glBegin(GL_TRIANGLES);
			#define __ONEFACE(a,b,c) n=(t->v[b]-t->v[a]).UnitCross(t->v[c]-t->v[a]); faceCenter=(t->v[a]+t->v[b]+t->v[c])/3.; if((faceCenter-center).Dot(n)<0)n=-n; glNormal3dv(n); glVertex3dv(t->v[a]); glVertex3dv(t->v[b]); glVertex3dv(t->v[c]);
				__ONEFACE(3,0,1);
				__ONEFACE(0,1,2);
				__ONEFACE(1,2,3);
				__ONEFACE(2,3,0);
			#undef __ONEFACE
		glEnd();
	}
	
}



/*! calculates tetrahedron inertia relative to the origin (0,0,0), with unit density (scales linearly)
See article F. Tonon, "Explicit Exact Formulas for the 3-D Tetrahedron Inertia Tensor in Terms of its Vertex Coordinates", http://www.scipub.org/fulltext/jms2/jms2118-11.pdf

Numerical example to check:

vertices:
	(8.33220, 11.86875, 0.93355)
	(0.75523 ,5.00000, 16.37072)
	(52.61236, 5.00000, 5.38580)
	(2.00000, 5.00000, 3.00000)
centroid:
	(15.92492, 0.78281, 3.72962)
intertia/density WRT centroid:
	a/μ = 43520.33257 m⁵
	b/μ = 194711.28938 m⁵
	c/μ = 191168.76173 m⁵
	a’/μ= 4417.66150 m⁵
	b’/μ=-46343.16662 m⁵
	c’/μ= 11996.20119 m⁵

@fixme: failing numerical testcase (in TetraTestGen::generate) ?! centroid is correct, for inertia we get:

63509.2
193465
191169
4417.66
-52950.8
-11971.3

I checked "a" charcter by character and it is correct; it the author wrong (doubtful)?

*/
Matrix3r TetrahedronInertiaTensor(vector<Vector3r> v){
	#define x1 v[0][0]
	#define y1 v[0][1]
	#define z1 v[0][2]
	#define x2 v[1][0]
	#define y2 v[1][1]
	#define z2 v[1][2]
	#define x3 v[2][0]
	#define y3 v[2][1]
	#define z3 v[2][2]
	#define x4 v[3][0]
	#define y4 v[3][1]
	#define z4 v[3][2]

	// Jacobian of transformation to the reference 4hedron
	double detJ=(x2-x1)*(y3-y1)*(z4-z1)+(x3-x1)*(y4-y1)*(z2-z1)+(x4-x1)*(y2-y1)*(z3-z1)
		-(x2-x1)*(y4-y1)*(z3-z1)-(x3-x1)*(y2-y1)*(z4-z1)-(x4-x1)*(y3-y1)*(z2-z1);
	detJ=fabs(detJ);
	double a=detJ*(y1*y1-y1*y2+y2*y2+y1*y3+y2*y3+
		y3*y3+y1*y4+y2*y4+y3*y4+y4*y4+z1*z1+z1*z2+
		z2*z2+z1*z3+z2*z3+z3*z3+z1*z4+z2*z4+z3*z4+z4*z4)/60.;
	double b=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+
		x1*x4+x2*x4+x3*x4+x4*x4+z1*z1+z1*z2+z2*z2+z1*z3+
		z2*z3+z3*z3+z1*z4+z2*z4+z4*z4)/60.;
	double c=detJ*(x1*x1+x1*x2+x2*x2+x1*x3+x2*x3+x3*x3+x1*x4+
		x2*x4+x3*x4+x4*x4+y1*y1+y1*y2+y2*y2+y1*y3+
		y2*y3+y3*y3+y1*y4+y2*y4+y3*y4+y4*y4)/60.;
	// a' in the article etc.
	double a__=detJ*(2*y1*z1+y2*z1+y3*z1+y4*z1+y1*z2+
		2*y2*z2+y3*z2+y4*z2+y1*z3+y2*z3+2*y3*z3+
		y4*z3+y1*z4+y2*z4+y3*z4+2*y4*z4)/120.;
	double b__=detJ*(2*z1*z1+x2*z1+x3*z1+x4*z1+x1*z2+
		2*x2*z2+x3*z2+x4*z2+z1*z3+x2*z3+2*x3*z3+
		x4*z3+x1*z4+x2*z4+x3*z4+2*x4*z4)/120.;
	double c__=detJ*(2*x1*y1+x2*y2+x3*y1+x4*y1+x1*y2+
		2*x2*y2+x3*y2+x4*y2+x1*y3+x2*y3+2*x3*y3+
		x4*y3+x1*y4+x2*y4+x3*y4+2*x4*y4)/120.;

	return Matrix3r(
		a   , -b__, -c__,
		-b__, b   , -a__,
		-c__, -a__, c    );

	#undef x1
	#undef y1
	#undef z1
	#undef x2
	#undef y2
	#undef z2
	#undef x3
	#undef y3
	#undef z3
	#undef x4
	#undef y4
	#undef z4

}

