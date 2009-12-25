// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
// #include<yade/lib-base/yadeWm3Extra.hpp>

/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The cell has size and shear, which are independent. 

*/
class Cell: public Serializable{
	public:
		Cell(): refSize(Vector3r(1,1,1)), strain(Matrix3r::ZERO), velGrad(Matrix3r::ZERO){ integrateAndUpdate(0); }
				
		//! size of the cell, projected on axes (for non-zero shear)
		Vector3r refSize;
		//! Strain matrix (current total strain)
		//! Ordering see http://www.efunda.com/formulae/solid_mechanics/mat_mechanics/strain.cfm#matrix
		Matrix3r strain;
		Matrix3r velGrad;
		//Matrix3r Hsize;

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
	//! transformation increment matrix applying arbitrary field
	const Matrix3r& getStrainIncrMatrix() const { return _strainInc; }
	
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
		Matrix3r _strainInc;
		Vector3r _size;
		bool _hasShear;
		Matrix3r _shearTrsfMatrix, _unshearTrsfMatrix, _cosMatrix;
		double _glShearTrsfMatrix[16];
		void fillGlShearTrsfMatrix(double m[16]){
			m[0]=1;            m[4]=strain[0][1]; m[8]=strain[0][2]; m[12]=0;
			m[1]=strain[1][0]; m[5]=1;            m[9]=strain[1][2]; m[13]=0;
			m[2]=strain[2][0]; m[6]=strain[2][1]; m[10]=1;           m[14]=0;
			m[3]=0;            m[7]=0;            m[11]=0;           m[15]=1;
		}

	public:

	//! "integrate" velGrad, update cached values used by public getter
	void integrateAndUpdate(Real dt){
		#if 0
			//initialize Hsize for "lazy" default scripts, after that Hsize is free to change
			if (refSize[0]!=1 && Hsize[0][0]==0) {Hsize[0][0]=refSize[0]; Hsize[1][1]=refSize[1]; Hsize[2][2]=refSize[2];}
			//update Hsize (redundant with total transformation perhaps)
			Hsize=Hsize+_shearInc*Hsize;
		#endif

		//incremental displacement gradient
		_strainInc=dt*velGrad;
		// total transformation; M = (Id+G).M = F.M
		strain+=_strainInc*_shearTrsfMatrix;
		#if 0 
			cerr<<"velGrad "; for(int i=0; i<3; i++) for(int j=0; j<3; j++) cerr<<velGrad[i][j]<<endl;
			cerr<<"_strainInc "; for(int i=0; i<3; i++) for(int j=0; j<3; j++) cerr<<_strainInc[i][j]<<endl;
			cerr<<"strain "; for(int i=0; i<3; i++) for(int j=0; j<3; j++) cerr<<strain[i][j]<<endl;
		#endif
		// pure shear transformation: strain with 1s on the diagonal
		_shearTrsfMatrix=strain; _shearTrsfMatrix[0][0]=_shearTrsfMatrix[1][1]=_shearTrsfMatrix[2][2]=1.;
		// pure unshear transformation
		_unshearTrsfMatrix=_shearTrsfMatrix.Inverse();
		// some parts can branch depending on presence/absence of shear
		_hasShear=!(strain[0][1]==0 && strain[0][2]==0 && strain[1][0]==0 && strain[1][2]==0 && strain[2][0]==0 && strain[2][1]==0);
		// current cell size (in units on physical space axes)
		_size=refSize+diagMult(getExtensionalStrain(),refSize);
		// OpenGL shear matrix (used frequently)
		fillGlShearTrsfMatrix(_glShearTrsfMatrix);
		// precompute cosines of angles, used for enlarge factor when computing Aabb's
		for(int i=0; i<3; i++) for(int j=0; j<3; j++) _cosMatrix[i][j]=(i==j?0:cos(atan(strain[i][j])));
	}
	
	/*! Return point inside periodic cell, even if shear is applied */
	Vector3r wrapShearedPt(const Vector3r& pt){ return shearPt(wrapPt(unshearPt(pt))); }
	/*! Return point inside periodic cell, even if shear is applied; store cell coordinates in period. */
	Vector3r wrapShearedPt(const Vector3r& pt, Vector3<int>& period){ return shearPt(wrapPt(unshearPt(pt),period)); }
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
	void postProcessAttributes(bool deserializing){ if(deserializing) integrateAndUpdate(0); }
	REGISTER_ATTRIBUTES(Serializable,(refSize)(strain)(velGrad));
	REGISTER_CLASS_AND_BASE(Cell,Serializable);
};
REGISTER_SERIALIZABLE(Cell);
