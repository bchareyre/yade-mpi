// 2009 © Václav Šmilauer <eudoxos@arcig.cz>

/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The Cell has reference box configuration (*refSize*) which is transformed (using *trsf*) to the current parallelepiped configuration. Using notation from http://en.wikipedia.org/wiki/Finite_strain_theory:

* Vector3r *refSize* is undeformed cell size (box)
* Matrix3r *trsf* is "deformation gradient tensor" F (written as matrix) 
* Matrix3r *velGrad* is …

The transformation has normal part and rotation/shear part. the shearPt, unshearPt, getShearTrsf etc functions refer to both shear and rotation.

*/

#pragma once

#include<yade/lib/serialization/Serializable.hpp>
#include<yade/lib/base/Math.hpp>

class Cell: public Serializable{
	public:
	//! Get current size (refSize × normal strain)
	const Vector3r& getSize() const { return _size; }
	//! Return copy of the current size (used only by the python wrapper)
	Vector3r getSize_copy() const { return _size; }
	//! return vector of consines of skew angle in yz, xz, xy planes between respective transformed base vectors
	const Vector3r& getCos() const {return _cos;}
	//! transformation matrix applying pure shear&rotation (scaling removed)
	const Matrix3r& getShearTrsf() const { return _shearTrsf; }
	//! inverse of getShearTrsfMatrix().
	const Matrix3r& getUnshearTrsf() const {return _unshearTrsf;}
	//! transformation increment matrix applying arbitrary field (remove if not used in NewtonIntegrator! )
	// const Matrix3r& getTrsfInc() const { return _trsfInc; }
	
	/*! return pointer to column-major OpenGL 4x4 matrix applying pure shear. This matrix is suitable as argument for glMultMatrixd.

	Note: the order of OpenGL transoformations matters; for instance, if you draw sheared wire box of size *size*,
	centered at *center*, the order is:

		1. translation: glTranslatev(center);
		3. shearing: glMultMatrixd(scene->cell->getGlShearTrsfMatrix());
		2. scaling: glScalev(size);
		4. draw: glutWireCube(1);
	
	See also http://www.songho.ca/opengl/gl_transform.html#matrix .
	*/
	const double* getGlShearTrsfMatrix() const { return _glShearTrsfMatrix; }
	//! Whether any shear (non-diagonal) component of the strain matrix is nonzero.
	bool hasShear() const {return _hasShear; }

	// caches; private
	private:
		Matrix3r _trsfInc;
		Vector3r _size, _cos;
		bool _hasShear;
		Matrix3r _shearTrsf, _unshearTrsf;
		double _glShearTrsfMatrix[16];
		void fillGlShearTrsfMatrix(double m[16]);
	public:

	//! "integrate" velGrad, update cached values used by public getter. If initH, Hsize is defined on the basis of refSize and trsf (only used if refSize is modified); else it is incremented the same way as for trsf.
	void integrateAndUpdate(Real dt, bool initH=false);
	/*! Return point inside periodic cell, even if shear is applied */
	Vector3r wrapShearedPt(const Vector3r& pt) const { return shearPt(wrapPt(unshearPt(pt))); }
	/*! Return point inside periodic cell, even if shear is applied; store cell coordinates in period. */
	Vector3r wrapShearedPt(const Vector3r& pt, Vector3i& period) const { return shearPt(wrapPt(unshearPt(pt),period)); }
	/*! Apply inverse shear on point; to put it inside (unsheared) periodic cell, apply wrapPt on the returned value. */
	Vector3r unshearPt(const Vector3r& pt) const { return _unshearTrsf*pt; }
	//! Apply shear on point. 
	Vector3r shearPt(const Vector3r& pt) const { return _shearTrsf*pt; }
	/*! Wrap point to inside the periodic cell; don't compute number of periods wrapped */
	Vector3r wrapPt(const Vector3r& pt) const {
		Vector3r ret; for(int i=0;i<3;i++) ret[i]=wrapNum(pt[i],_size[i]); return ret;}
	/*! Wrap point to inside the periodic cell; period will contain by how many cells it was wrapped. */
	Vector3r wrapPt(const Vector3r& pt, Vector3i& period) const {
		Vector3r ret; for(int i=0; i<3; i++){ ret[i]=wrapNum(pt[i],_size[i],period[i]); } return ret;}
	/*! Wrap number to interval 0…sz */
	static Real wrapNum(const Real& x, const Real& sz) {
		Real norm=x/sz; return (norm-floor(norm))*sz;}
	/*! Wrap number to interval 0…sz; store how many intervals were wrapped in period */
	static Real wrapNum(const Real& x, const Real& sz, int& period) {
		Real norm=x/sz; period=(int)floor(norm); return (norm-period)*sz;}

	// relative position and velocity for interaction accross multiple cells
	Vector3r intrShiftPos(const Vector3i& cellDist) const { return Hsize*cellDist.cast<Real>(); }
	Vector3r intrShiftVel(const Vector3i& cellDist) const { if(homoDeform==HOMO_VEL || homoDeform==HOMO_VEL_2ND) return velGrad*Hsize*cellDist.cast<Real>(); return Vector3r::Zero(); }
	// return body velocity while taking away mean field velocity (coming from velGrad) if the mean field velocity is applied on velocity
	Vector3r bodyFluctuationVel(const Vector3r& pos, const Vector3r& vel) const { if(homoDeform==HOMO_VEL || homoDeform==HOMO_VEL_2ND) return (vel-velGrad*pos); return vel; }

	Vector3r getRefSize() const { return refSize; }
	void setRefSize(const Vector3r& s){refSize=s; Hsize=Matrix3r::Zero(); Hsize.diagonal()=refSize; Hsize=trsf*Hsize; integrateAndUpdate(0);}
	void setInitHsize(const Matrix3r& m){Hsize=m; for (int k=0;k<3;k++) refSize[k]=Hsize.col(k).norm(); integrateAndUpdate(0);}
	Matrix3r getTrsf() const { return trsf; }
	void setTrsf(const Matrix3r& m){ Hsize=m*invTrsf*Hsize; trsf=m; integrateAndUpdate(0); }
	// return current cell volume
	Real getVolume() const {return Hsize.determinant();}
	void postLoad(Cell&){ integrateAndUpdate(0); }

	// to resolve overloads
	Vector3r wrapShearedPt_py(const Vector3r& pt) const { return wrapShearedPt(pt);}
	Vector3r wrapPt_py(const Vector3r& pt) const { return wrapPt(pt);}

	enum { HOMO_NONE=0, HOMO_POS=1, HOMO_VEL=2, HOMO_VEL_2ND=3 };
	
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Cell,Serializable,"Parameters of periodic boundary conditions. Only applies if O.isPeriodic==True.",
		((Vector3r,refSize,Vector3r(1,1,1),Attr::readonly,"Reference size of the cell. Change the value with :yref:`Cell::setRefSize`"))
		((Matrix3r,trsf,Matrix3r::Identity(),Attr::readonly,"Current transformation matrix of the cell. Change the value with :yref:`Cell::setTrsf`"))
		((Matrix3r,invTrsf,Matrix3r::Identity(),Attr::readonly,"Inverse of current transformation matrix of the cell."))
		((Matrix3r,velGrad,Matrix3r::Zero(),,"Velocity gradient of the transformation; used in NewtonIntegrator."))
		((Matrix3r,prevVelGrad,Matrix3r::Zero(),Attr::readonly,"Velocity gradient in the previous step."))
		((Matrix3r,Hsize,Matrix3r::Zero(),Attr::readonly,"The current cell size (one column per box edge), computed from *refSize* and *trsf* |yupdate|"))
		((int,homoDeform,3,Attr::triggerPostLoad,"Deform (:yref:`velGrad<Cell.velGrad>`) the cell homothetically, by adjusting positions or velocities of particles. The values have the following meaning: 0: no homothetic deformation, 1: set absolute particle positions directly (when ``velGrad`` is non-zero), but without changing their velocity, 2: adjust particle velocity (only when ``velGrad`` changed) with Δv_i=Δ ∇v x_i. 3: as 2, but include a 2nd order term in addition -- the derivative of 1 (convective term in the velocity update).")),
		/*ctor*/ integrateAndUpdate(0),
		/*py*/
		.def_readonly("size",&Cell::getSize_copy,"Current size of the cell, i.e. lengths of 3 cell lateral vectors after applying current trsf. Update automatically at every step.")
		.add_property("volume",&Cell::getVolume,"Current volume of the cell.")
		// debugging only
		.def("wrap",&Cell::wrapShearedPt_py,"Transform an arbitrary point into a point in the reference cell")
		.def("unshearPt",&Cell::unshearPt,"Apply inverse shear on the point (removes skew+rot of the cell)")
		.def("shearPt",&Cell::shearPt,"Apply shear (cell skew+rot) on the point")
		.def("wrapPt",&Cell::wrapPt_py,"Wrap point inside the reference cell, assuming the cell has no skew+rot.")
		.def("setTrsf",&Cell::setTrsf,"Set transformation (will update Hsize as if it was initialy an axis-aligned cuboïd).")
		.def("setRefSize",&Cell::setRefSize,"Set reference size of the cell (will update Hsize as if the cell was initialy an axis-aligned cuboïd).")
		.def("setInitHsize",&Cell::setInitHsize,"Define reference base vectors of the undeformed period. The shape is arbitrary: a non-rectangular parallepiped can be defined as the reference configuration, and will correspond to trsf=0.")
		.def_readonly("shearTrsf",&Cell::_shearTrsf,"Current skew+rot transformation (no resize)")
		.def_readonly("unshearTrsf",&Cell::_unshearTrsf,"Inverse of the current skew+rot transformation (no resize)")
	);
};
REGISTER_SERIALIZABLE(Cell);
