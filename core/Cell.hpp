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

#define CELL_BACKW_COMPAT

class Cell: public Serializable{
	public:
	//! Get current size
	const Vector3r& getSize() const { return _size; }
	
	void setSize(const Vector3r& s){for (int k=0;k<3;k++) hSize.col(k)*=s[k]/hSize.col(k).norm(); postLoad(*this);}
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
		Matrix3r _invTrsf;
		Matrix3r _trsfInc;
		Vector3r _size, _cos;
		Vector3r _refSize;
		bool _hasShear;
		Matrix3r _shearTrsf, _unshearTrsf;
		double _glShearTrsfMatrix[16];
		void fillGlShearTrsfMatrix(double m[16]);
	public:

	DECLARE_LOGGER;

	//! "integrate" velGrad, update cached values used by public getter.
	void integrateAndUpdate(Real dt);
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
	Vector3r intrShiftPos(const Vector3i& cellDist) const { return hSize*cellDist.cast<Real>(); }
	Vector3r intrShiftVel(const Vector3i& cellDist) const { if(homoDeform==HOMO_VEL || homoDeform==HOMO_VEL_2ND) return velGrad*hSize*cellDist.cast<Real>(); return Vector3r::Zero(); }
	// return body velocity while taking away mean field velocity (coming from velGrad) if the mean field velocity is applied on velocity
	Vector3r bodyFluctuationVel(const Vector3r& pos, const Vector3r& vel) const { if(homoDeform==HOMO_VEL || homoDeform==HOMO_VEL_2ND) return (vel-velGrad*pos); return vel; }
	// get/set mechanically undeformed shape; setting resets trsf to identity
	Matrix3r getHSize() const { return hSize; }
	void setHSize(const Matrix3r& m){ hSize=refHSize=m; trsf=Matrix3r::Identity(); postLoad(*this); }
	// set current transformation; has no influence on current configuration (hSize); sets display refHSize as side-effect
	Matrix3r getTrsf() const { return trsf; }
	void setTrsf(const Matrix3r& m){ refHSize=hSize; trsf=m; postLoad(*this); }
#ifdef CELL_BACKW_COMPAT
	// get undeformed shape
	Matrix3r getHSize0() const { return _invTrsf*hSize; }
	// edge lengths of the undeformed shape
	Vector3r getRefSize() const { Matrix3r h=getHSize0(); return Vector3r(h.col(0).norm(),h.col(1).norm(),h.col(2).norm()); }
	// temporary, will be removed in favor of more descriptive setBox(...)
	void setRefSize(const Vector3r& s){
		// if refSize is set to the current size and the cell is a box (used in older scripts), say it is not necessary
		if(s==_size && hSize==hSize.diagonal().asDiagonal()){ LOG_WARN("Setting O.cell.refSize=O.cell.size is useless, O.trsf=Matrix3.Identity is enough now."); }
		else {LOG_WARN("Setting Cell.refSize is deprecated, use Cell.setBox(...) instead.");}
		setBox(s); postLoad(*this);
	} 
	// set box shape of the cell
	void setBox(const Vector3r& size){ setHSize(size.asDiagonal()); postLoad(*this); }
	void setBox3(const Real& s0, const Real& s1, const Real& s2){ setBox(Vector3r(s0,s1,s2)); }
#endif
	// return current cell volume
	Real getVolume() const {return hSize.determinant();}
	void postLoad(Cell&){ integrateAndUpdate(0); }

	// to resolve overloads
	Vector3r wrapShearedPt_py(const Vector3r& pt) const { return wrapShearedPt(pt);}
	Vector3r wrapPt_py(const Vector3r& pt) const { return wrapPt(pt);}

	enum { HOMO_NONE=0, HOMO_POS=1, HOMO_VEL=2, HOMO_VEL_2ND=3 };
	YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(Cell,Serializable,"Parameters of periodic boundary conditions. Only applies if O.isPeriodic==True.",
		/* overridden below to be modified by getters/setters because of intended side-effects */
		((Matrix3r,trsf,Matrix3r::Identity(),,"[overridden]")) //"Current transformation matrix of the cell, which defines how far is the current cell geometry (:yref:`hSize<Cell.hSize>`) from the reference configuration. Changing trsf will not change :yref:`hSize<Cell.hSize>`, it serves only as accumulator for transformations applied via :yref:`velGrad<Cell.velGrad>`."))
		((Matrix3r,refHSize,Matrix3r::Identity(),,"Reference cell configuration, only used with :yref:`OpenGLRenderer.dispScale`. Updated automatically when :yref:`hSize<Cell.hSize>` or :yref:`trsf<Cell.trsf>` is assigned directly; also modified by :yref:`yade.utils.setRefSe3` (called e.g. by the :gui:`Reference` button in the UI)."))
		((Matrix3r,hSize,Matrix3r::Identity(),,"[overridden below]"))
		/* normal attributes */
		((Matrix3r,velGrad,Matrix3r::Zero(),,"Velocity gradient of the transformation; used in :yref:`NewtonIntegrator`. Values of :yref:`velGrad<Cell.velGrad>` accumulate in :yref:`trsf<Cell.trsf>` at every step."))
		((Matrix3r,prevVelGrad,Matrix3r::Zero(),Attr::readonly,"Velocity gradient in the previous step."))
		((int,homoDeform,3,Attr::triggerPostLoad,"Deform (:yref:`velGrad<Cell.velGrad>`) the cell homothetically, by adjusting positions or velocities of particles. The values have the following meaning: 0: no homothetic deformation, 1: set absolute particle positions directly (when ``velGrad`` is non-zero), but without changing their velocity, 2: adjust particle velocity (only when ``velGrad`` changed) with Δv_i=Δ ∇v x_i. 3: as 2, but include a 2nd order term in addition -- the derivative of 1 (convective term in the velocity update).")),
		/*deprec*/ ((Hsize,hSize,"conform to Yade's names convention.")),
		/*init*/ ,
		/*ctor*/ _invTrsf=Matrix3r::Identity(); integrateAndUpdate(0),
		/*py*/
		// override some attributes above
		.add_property("hSize",&Cell::getHSize,&Cell::setHSize,"Base cell vectors (columns of the matrix), updated at every step from :yref:`velGrad<Cell.velGrad>` (:yref:`trsf<Cell.trsf>` accumulates applied :yref:`velGrad<Cell.velGrad>` transformations). Setting *hSize* directly results in :yref:`trsf<Cell.trsf>` being set to identity.")
#ifdef CELL_BACKW_COMPAT
		.add_property("refSize",&Cell::getRefSize,&Cell::setRefSize,"Reference size of the cell (lengths of initial cell vectors, i.e. column norms of :yref:`hSize<Cell.hSize>`).\n\n.. note:: Modifying this value is deprecated, use :yref:`setBox<Cell.setBox>` instead.\n\n")
		.add_property("hSize0",&Cell::getHSize0,"Value of untransformed hSize, with respect to current :yref:`trsf<Cell.trsf>` (computed as :yref:`invTrsf<Cell.invTrsf>` × :yref:`hSize<Cell.hSize>`.")
		.def("setBox",&Cell::setBox,"Set :yref:`Cell` shape to be rectangular, with dimensions along axes specified by given argument. Shorthand for assigning diagonal matrix with respective entries to :yref:`hSize<Cell.hSize>`.")
		.def("setBox",&Cell::setBox3,"Set :yref:`Cell` shape to be rectangular, with dimensions along $x$, $y$, $z$ specified by arguments. Shorthand for assigning diagonal matrix with the respective entries to :yref:`hSize<Cell.hSize>`.")
#endif
		.add_property("trsf",&Cell::getTrsf,&Cell::setTrsf,"Current transformation matrix of the cell with regards to the initial configuration.")
		// useful properties		
		.add_property("size",&Cell::getSize_copy,&Cell::setSize,"Current size of the cell, i.e. lengths of the 3 cell lateral vectors contained in :yref:`Cell.hSize` columns. Updated automatically at every step.")
		.add_property("volume",&Cell::getVolume,"Current volume of the cell.")
		// debugging only
		.def("wrap",&Cell::wrapShearedPt_py,"Transform an arbitrary point into a point in the reference cell")
		.def("unshearPt",&Cell::unshearPt,"Apply inverse shear on the point (removes skew+rot of the cell)")
		.def("shearPt",&Cell::shearPt,"Apply shear (cell skew+rot) on the point")
		.def("wrapPt",&Cell::wrapPt_py,"Wrap point inside the reference cell, assuming the cell has no skew+rot.")
		.def_readonly("shearTrsf",&Cell::_shearTrsf,"Current skew+rot transformation (no resize)")
		.def_readonly("unshearTrsf",&Cell::_unshearTrsf,"Inverse of the current skew+rot transformation (no resize)")
	);
};
REGISTER_SERIALIZABLE(Cell);
