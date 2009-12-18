// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
// #include<yade/lib-base/yadeWm3Extra.hpp>

// begin yade compatibility
#ifndef PREFIX
	#include<yade/lib-miniWm3/Wm3Vector3.h>
	#include<yade/lib-miniWm3/Wm3Matrix3.h>
	#define REGISTER_ATTRIBUTES(a,b)
	#define REGISTER_SERIALIZABLE(a)
	#define REGISTER_CLASS_AND_BASE(a,b)
	typedef Wm3::Vector3<double> Vector3r;
	using Wm3::Vector3;
	typedef Wm3::Matrix3<double> Matrix3r;
	typedef double Real;
	class Serializable{};
#endif
// end yade compatibility

/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The cell has size and shear, which are independent. 

*/
class Cell: public Serializable{
	public:
		Cell(): size(Vector3r(1,1,1)), shear(Vector3r::ZERO){ updateCache(); }
		//! size of the cell, projected on axes (for non-zero shear)
		Vector3r size;
		//! 3 non-diagonal components of the shear matrix, ordered by axis normal to the shear plane,
		//! i.e. (εyz=εzy,εxz=εzx,εxy=εyx). Shear of the cell is always symmetric (transforms
		//! a box into a parallelepiped).
		//! See http://www.efunda.com/formulae/solid_mechanics/mat_mechanics/strain.cfm#matrix
		//! for details
		Vector3r shear;

	// caches
	Vector3r _shearAngle;
	Vector3r _shearSin;
	Vector3r _shearCos;
	Matrix3r _shearTrsf;
	Matrix3r _unshearTrsf;

	double _glShearMatrix[16];
	// should be called before every step
	void updateCache(){
		for(int i=0; i<3; i++) {
			_shearAngle[i]=atan(shear[i]);
			_shearSin[i]=sin(_shearAngle[i]);
			_shearCos[i]=cos(_shearAngle[i]);
		}
		_shearTrsf=Matrix3r(1,shear[2],shear[1],shear[2],1,shear[0],shear[1],shear[0],1);
		_unshearTrsf=_shearTrsf.Inverse();
		fillGlShearMatrix(_glShearMatrix);
	}

	/*! Fill column-major (standard) OpenGL matrix for shear (don't use directly, use _glShearMatrix instead).

	Note: the order of OpenGL transoformations matters; for instance, if you draw sheared wire box of size *size*, centered at *center*, the order is:

		1. translation: glTranslatev(center);
		3. shearing: glMultMatrixd(scene->cell._glShearMatrix);
		2. scaling: glScalev(size);
		4. draw: glutWireCube(1);
	
	See also http://www.songho.ca/opengl/gl_transform.html#matrix
	*/
	void fillGlShearMatrix(double m[16]){
		m[0]=1;        m[4]=shear[2]; m[8]=shear[1]; m[12]=0;
		m[1]=shear[2]; m[5]=1;        m[9]=shear[0]; m[13]=0;
		m[2]=shear[1]; m[6]=shear[0]; m[10]=1;       m[14]=0;
		m[3]=0;        m[7]=0;        m[11]=0;       m[15]=1;
	}

	// doesn't seem to be really useful
	#if 0
		/*! Prepare OpenGL matrix with current shear and given translation and scale.
		
		This matrix is ccumulated product of this sequence:

			glTranslatev(translation);
			glScalev(scale);
			glMultMatrixd(scene->cell._glShearMatric);

		*/
		void glTrsfMatrix(double m[16], const Vector3r& translation, const Vector3r& scale){
			m[0]=scale[0];          m[4]=scale[2]*shear[2]; m[8]=scale[1]*shear[1]; m[12]=translation[0];
			m[1]=scale[2]*shear[2]; m[5]=scale[1];          m[9]=scale[0]*shear[0]; m[13]=translation[1];
			m[2]=scale[1]*shear[1]; m[6]=shear[0];          m[10]=scale[2];         m[14]=translation[2];
			m[3]=0;        m[7]=0;        m[11]=0;       m[15]=1;
		}
	#endif
	/*! Return point inside periodic cell, even if shear is applied */
	Vector3r wrapShearedPt(const Vector3r& pt){ return shearPt(wrapPt(unshearPt(pt))); }
	/*! Apply inverse shear on point; to put it inside (unsheared) periodic cell, apply wrapPt on the returned value. */
	Vector3r unshearPt(const Vector3r& pt){ return _unshearTrsf*pt; }
	//! Apply shear on point. 
	Vector3r shearPt(const Vector3r& pt){ return _shearTrsf*pt; }
	/*! Wrap number to interval 0…sz */
	Real wrapNum(const Real& x, const Real& sz)const{
		Real norm=x/sz; return (norm-floor(norm))*sz;
	}
	/*! Wrap number to interval 0…sz; store how many intervals were wrapped in period */
	Real wrapNum(const Real& x, const Real& sz, int& period)const{
		Real norm=x/sz; period=(int)floor(norm); return (norm-period)*sz;
	}
	/*! Wrap point to inside the periodic cell; don't compute number of periods wrapped */
	Vector3r wrapPt(const Vector3r pt)const{
		Vector3r ret; for(int i=0;i<3;i++) ret[i]=wrapNum(pt[i],size[i]); return ret;
	}
	/*! Wrap point to inside the periodic cell; period will contain by how many cells it was wrapped. */
	Vector3r wrapPt(const Vector3r pt, Vector3<int>& period)const{
		Vector3r ret; for(int i=0; i<3; i++){ ret[i]=wrapNum(pt[i],size[i],period[i]); } return ret;
	}

	REGISTER_ATTRIBUTES(Serializable,(size)(shear));
	REGISTER_CLASS_AND_BASE(Cell,Serializable);
};
REGISTER_SERIALIZABLE(Cell);
