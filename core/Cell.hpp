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

	
//#define VELGRAD
	
/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The cell has size and shear, which are independent. 

*/
class Cell: public Serializable{
	public:
		Cell(): refSize(Vector3r(1,1,1)), strain(Matrix3r::ZERO)
#ifdef VELGRAD
				, velGrad(Matrix3r::ZERO) 
				, Hsize(Matrix3r::IDENTITY)
				,_shearTrsfMatrix(Matrix3r::IDENTITY)
				{ updateCache(0); }
#else
				{ updateCache(); }
#endif
				
		//! size of the cell, projected on axes (for non-zero shear)
		Vector3r refSize;
		//! 3 non-diagonal components of the shear matrix, ordered by axis normal to the shear plane,
		//! i.e. (εyz=εzy,εxz=εzx,εxy=εyx). Shear of the cell is always symmetric (transforms
		//! a box into a parallelepiped).
		//! See http://www.efunda.com/formulae/solid_mechanics/mat_mechanics/strain.cfm#matrix
		//! for details
		// Vector3r shear;
		Matrix3r strain;
#ifdef VELGRAD
		Matrix3r velGrad;
		Matrix3r Hsize;
		Matrix3r _shearIncrt;
#endif

		//! reference values of size and shear (for rendering, mainly)
		// Vector3r refShear, refCenter;

	//! Get current size (refSize × normal strain)
	const Vector3r& getSize() const { return _size; }
	//! Return copy of the current size (used only by the python wrapper)
	Vector3r getSize_copy() const { return _size; }
	//! return normal strain component (strain matrix diagonal as Vector3r)
	Vector3r getExtensionalStrain() const { return Vector3r(strain[0][0],strain[1][1],strain[2][2]); }
	//! return matrix containing cosines of shear angles
	const Matrix3r& getCosMatrix() const {return _cosMatrix;}
	//! transformation matrix applying puse shear (normal strain removed: ones on the diagonal)
	const Matrix3r& getShearTrsfMatrix() const { return _shearTrsfMatrix; }
	//! inverse of getShearTrsfMatrix().
	const Matrix3r& getUnshearTrsfMatrix() const {return _unshearTrsfMatrix;}
#ifdef VELGRAD	
	//! transformation increment matrix applying arbitrary field
	const Matrix3r& getShearIncrMatrix() const { return _shearIncrt; }
#endif	
	
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

	// caches
	private:
		Vector3r _size;
		bool _hasShear;
		Matrix3r _shearTrsfMatrix, _unshearTrsfMatrix, _cosMatrix;
		// Vector3r _shearAngle, _shearSin, _shearCos;
		//Matrix3r _shearTrsf, _unshearTrsf;
		double _glShearTrsfMatrix[16];
		void fillGlShearTrsfMatrix(double m[16]){
			m[0]=1;            m[4]=strain[0][1]; m[8]=strain[0][2]; m[12]=0;
			m[1]=strain[1][0]; m[5]=1;            m[9]=strain[1][2]; m[13]=0;
			m[2]=strain[2][0]; m[6]=strain[2][1]; m[10]=1;           m[14]=0;
			m[3]=0;            m[7]=0;            m[11]=0;           m[15]=1;
		}

	public:

	// should be called before every step
#ifdef VELGRAD
	void updateCache(double dt){	
		//incremental disp gradient
		_shearIncrt=dt*velGrad;
		
		//update Hsize (redundant with total transformation perhaps)
		Hsize=Hsize+_shearIncrt*Hsize;
		//total transformation
		_shearTrsfMatrix = _shearTrsfMatrix + _shearIncrt*_shearTrsfMatrix;// M = (Id+G).M = F.M
		//compatibility with Vaclav's code :
		_size[0]=Hsize[0][0]; _size[1]=Hsize[1][1]; _size[2]=Hsize[2][2];
		_hasShear = false;
		_unshearTrsfMatrix=_shearTrsfMatrix.Inverse();
		strain=_shearTrsfMatrix;
#else
	void updateCache(){
		/*
		for(int i=0; i<3; i++) {
			_shearAngle[i]=atan(shear[i]);
			_shearSin[i]=sin(_shearAngle[i]);
			_shearCos[i]=cos(_shearAngle[i]);
		}
		*/
		_size=refSize+diagMult(getExtensionalStrain(),refSize);
		_hasShear=(strain[0][1]==0 && strain[0][2]==0 && strain[1][0]==0 && strain[1][2]==0 && strain[2][0]==0 && strain[2][1]==0);
		_shearTrsfMatrix=strain;
		_shearTrsfMatrix[0][0]=_shearTrsfMatrix[1][1]=_shearTrsfMatrix[2][2]=1.;
		_unshearTrsfMatrix=_shearTrsfMatrix.Inverse();
		// _shearTrsf=Matrix3r(1,shear[2],shear[1],shear[2],1,shear[0],shear[1],shear[0],1);
		// _unshearTrsf=strain.Inverse();
#endif
		fillGlShearTrsfMatrix(_glShearTrsfMatrix);
		for(int i=0; i<3; i++) for(int j=0; j<3; j++) _cosMatrix[i][j]=(i==j?0:cos(atan(strain[i][j])));
	}
	
// #ifdef VELGRAD		
// 	void updateCache(){ updateCache(0);}
// #endif

	// doesn't seem to be really useful
	#if 0
		/*! Prepare OpenGL matrix with current shear and given translation and scale.
		
		This matrix is ccumulated product of this sequence:

			glTranslatev(translation);
			glScalev(scale);
			glMultMatrixd(scene->cell->_glShearMatrix);

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
	Vector3r unshearPt(const Vector3r& pt){ return _unshearTrsfMatrix*pt; }
	//! Apply shear on point. 
	Vector3r shearPt(const Vector3r& pt){ return _shearTrsfMatrix*pt; }
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
#ifdef VELGRAD
	void postProcessAttributes(bool deserializing){ if(deserializing) updateCache(0); }
#else
	void postProcessAttributes(bool deserializing){ if(deserializing) updateCache(); }
#endif
	REGISTER_ATTRIBUTES(Serializable,(refSize)(strain)
#ifdef VELGRAD
			(velGrad)(Hsize)
#endif
			   );
	REGISTER_CLASS_AND_BASE(Cell,Serializable);
};
REGISTER_SERIALIZABLE(Cell);
