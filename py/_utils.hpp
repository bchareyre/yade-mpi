#include<pkg/dem/Shop.hpp>
#include<core/Scene.hpp>
#include<core/Omega.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/DemXDofGeom.hpp>
#include<pkg/common/Facet.hpp>
#include<pkg/dem/Tetra.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/NormShearPhys.hpp>
#include<lib/computational-geometry/Hull2d.hpp>
#include<lib/pyutil/doc_opts.hpp>
#include<pkg/dem/ViscoelasticPM.hpp>

#include<numpy/ndarrayobject.h>

namespace py = boost::python;

bool isInBB(Vector3r p, Vector3r bbMin, Vector3r bbMax);

py::tuple negPosExtremeIds(int axis, Real distFactor=1.1);

BOOST_PYTHON_FUNCTION_OVERLOADS(negPosExtremeIds_overloads,negPosExtremeIds,1,2);

py::tuple coordsAndDisplacements(int axis,py::tuple Aabb=py::tuple());

void setRefSe3();

Real PWaveTimeStep();
Real RayleighWaveTimeStep();

py::tuple interactionAnglesHistogram(int axis, int mask=0, size_t bins=20, py::tuple aabb=py::tuple(), Real minProjLen=1e-6);
BOOST_PYTHON_FUNCTION_OVERLOADS(interactionAnglesHistogram_overloads,interactionAnglesHistogram,1,4);

py::tuple bodyNumInteractionsHistogram(py::tuple aabb=py::tuple());
BOOST_PYTHON_FUNCTION_OVERLOADS(bodyNumInteractionsHistogram_overloads,bodyNumInteractionsHistogram,0,1);

Vector3r inscribedCircleCenter(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2);
py::dict getViscoelasticFromSpheresInteraction(Real tc, Real en, Real es);

/* reset highlight of all bodies */
void highlightNone();

/*!Sum moments acting on given bodies
 *
 * @param ids is the calculated bodies ids
 * @param axis is the direction of axis with respect to which the moment is calculated.
 * @param axisPt is a point on the axis.
 *
 * The computation is trivial: moment from force is is by definition r×F, where r
 * is position relative to axisPt; moment from moment is m; such moment per body is
 * projected onto axis.
 */
Real sumTorques(py::list ids, const Vector3r& axis, const Vector3r& axisPt);

/* Sum forces acting on bodies within mask.
 *
 * @param ids list of ids
 * @param direction direction in which forces are summed
 *
 */
Real sumForces(py::list ids, const Vector3r& direction);

/* Sum force acting on facets given by their ids in the sense of their respective normals.
   If axis is given, it will sum forces perpendicular to given axis only (not the the facet normals).
*/
Real sumFacetNormalForces(vector<Body::id_t> ids, int axis=-1);

/* Set wire display of all/some/none bodies depending on the filter. */
void wireSome(string filter);
void wireAll();
void wireNone();
void wireNoSpheres();


/* Tell us whether a point lies in polygon given by array of points.
 *  @param xy is the point that is being tested
 *  @param vertices is Numeric.array (or list or tuple) of vertices of the polygon.
 *         Every row of the array is x and y coordinate, numer of rows is >= 3 (triangle).
 *
 * Copying the algorithm from http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
 * is gratefully acknowledged:
 *
 * License to Use:
 * Copyright (c) 1970-2003, Wm. Randolph Franklin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimers.
 *   2. Redistributions in binary form must reproduce the above copyright notice in the documentation and/or other materials provided with the distribution.
 *   3. The name of W. Randolph Franklin may not be used to endorse or promote products derived from this Software without specific prior written permission. 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://numpy.scipy.org/numpydoc/numpy-13.html told me how to use Numeric.array from c
 */
bool pointInsidePolygon(py::tuple xy, py::object vertices);

/* Compute area of convex hull when when taking (swept) spheres crossing the plane at coord, perpendicular to axis.

	All spheres that touch the plane are projected as hexagons on their circumference to the plane.
	Convex hull from this cloud is computed.
	The area of the hull is returned.

*/
Real approxSectionArea(Real coord, int axis);

/* Find all interactions deriving from NormShearPhys that cross plane given by a point and normal
	(the normal may not be normalized in this case, though) and sum forces (both normal and shear) on them.
	
	Returns a 3-tuple with the components along global x,y,z axes, which can be viewed as "action from lower part, towards
	upper part" (lower and upper parts with respect to the plane's normal).

	(This could be easily extended to return sum of only normal forces or only of shear forces.)
*/
Vector3r forcesOnPlane(const Vector3r& planePt, const Vector3r&  normal);

/* Less general than forcesOnPlane, computes force on plane perpendicular to axis, passing through coordinate coord. */
Vector3r forcesOnCoordPlane(Real coord, int axis);

py::tuple spiralProject(const Vector3r& pt, Real dH_dTheta, int axis=2, Real periodStart=std::numeric_limits<Real>::quiet_NaN(), Real theta0=0);

shared_ptr<Interaction> Shop__createExplicitInteraction(Body::id_t id1, Body::id_t id2);

Real Shop__unbalancedForce(bool useMaxForce /*false by default*/);
py::tuple Shop__totalForceInVolume();
Real Shop__getSpheresVolume(int mask=-1);
Real Shop__getSpheresMass(int mask=-1);
py::object Shop__kineticEnergy(bool findMaxId=false);

Real maxOverlapRatio();

Real Shop__getPorosity(Real volume=-1);
Real Shop__getVoxelPorosity(int resolution=200, Vector3r start=Vector3r(0,0,0),Vector3r end=Vector3r(0,0,0));

//Matrix3r Shop__stressTensorOfPeriodicCell(bool smallStrains=false){return Shop::stressTensorOfPeriodicCell(smallStrains);}
py::tuple Shop__fabricTensor(bool splitTensor=false, bool revertSign=false, Real thresholdForce=NaN);
py::tuple Shop__normalShearStressTensors(bool compressionPositive=false, bool splitNormalTensor=false, Real thresholdForce=NaN);

py::list Shop__getStressLWForEachBody();

py::list Shop__getBodyIdsContacts(Body::id_t bodyID=-1);

Real shiftBodies(py::list ids, const Vector3r& shift);

void Shop__calm(int mask=-1);

void setNewVerticesOfFacet(const shared_ptr<Body>& b, const Vector3r& v1, const Vector3r& v2, const Vector3r& v3);

py::list intrsOfEachBody();

py::list numIntrsOfEachBody();
