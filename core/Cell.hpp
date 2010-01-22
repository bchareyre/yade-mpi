// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
// #include<yade/lib-base/yadeWm3Extra.hpp>

/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The Cell has reference box configuration (*refSize*) which is transformed (using *trsf*) to the current parallelepiped configuration. Using notation from http://en.wikipedia.org/wiki/Finite_strain_theory:

* Vector3r *refSize* is undeformed cell size (box)
* Matrix3r *trsf* is "deformation gradient tensor" F (written as matrix) 
* Matrix3r *velGrad* is …

The transformation has normal part and rotation/shear part. the shearPt, unshearPt, getShearTrsf etc functions refer to both shear and rotation.

*/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>

class Cell: public Serializable{
	public:
		Cell(): refSize(Vector3r(1,1,1)), trsf(Matrix3r::IDENTITY), velGrad(Matrix3r::ZERO){ integrateAndUpdate(0); }
				
		//! size of the reference cell
		Vector3r refSize;
		Matrix3r trsf;
		Matrix3r velGrad;
		//Matrix3r Hsize;

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
	//! transformation increment matrix applying arbitrary field (remove if not used in NewtonIntegrator!)
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

	//! "integrate" velGrad, update cached values used by public getter
	void integrateAndUpdate(Real dt);
	/*! Return point inside periodic cell, even if shear is applied */
	Vector3r wrapShearedPt(const Vector3r& pt){ return shearPt(wrapPt(unshearPt(pt))); }
	/*! Return point inside periodic cell, even if shear is applied; store cell coordinates in period. */
	Vector3r wrapShearedPt(const Vector3r& pt, Vector3<int>& period){ return shearPt(wrapPt(unshearPt(pt),period)); }
	/*! Apply inverse shear on point; to put it inside (unsheared) periodic cell, apply wrapPt on the returned value. */
	Vector3r unshearPt(const Vector3r& pt){ return _unshearTrsf*pt; }
	//! Apply shear on point. 
	Vector3r shearPt(const Vector3r& pt){ return _shearTrsf*pt; }
	/*! Wrap point to inside the periodic cell; don't compute number of periods wrapped */
	Vector3r wrapPt(const Vector3r pt)const{
		Vector3r ret; for(int i=0;i<3;i++) ret[i]=wrapNum(pt[i],_size[i]); return ret;
	}
	/*! Wrap point to inside the periodic cell; period will contain by how many cells it was wrapped. */
	Vector3r wrapPt(const Vector3r pt, Vector3<int>& period)const{
		Vector3r ret; for(int i=0; i<3; i++){ ret[i]=wrapNum(pt[i],_size[i],period[i]); } return ret;
	}
	/*! Wrap number to interval 0…sz */
	static Real wrapNum(const Real& x, const Real& sz){
		Real norm=x/sz; return (norm-floor(norm))*sz;
	}
	/*! Wrap number to interval 0…sz; store how many intervals were wrapped in period */
	static Real wrapNum(const Real& x, const Real& sz, int& period){
		Real norm=x/sz; period=(int)floor(norm); return (norm-period)*sz;
	}
	void postProcessAttributes(bool deserializing){ if(deserializing) integrateAndUpdate(0); }
	YADE_CLASS_BASE_DOC_ATTRS_PY(Cell,Serializable,"Parameters of periodic boundary conditions. Only applies if O.isPeriodic==True.",
		((refSize,"Reference size of the cell"))((trsf,"Current transformation matrix of the cell"))((velGrad,"Velocity gradient of the transformation; used in NewtonIntegrator.")),
		.def_readonly("size",&Cell::getSize_copy,"Current size of the cell, i.e. lengths of 3 cell lateral vectors after applying current trsf. Update automatically at every step.")
	);
};
REGISTER_SERIALIZABLE(Cell);
