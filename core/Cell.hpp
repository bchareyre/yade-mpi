// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
// #include<yade/lib-base/yadeWm3Extra.hpp>

/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The Cell has reference box configuration (*refSize*) which is transformed (using *trsf*) to the current parallelepiped configuration. Using notation from http://en.wikipedia.org/wiki/Finite_strain_theory:

* Vector3r *refSize* is undeformed cell size (box)
* Matrix3r *trsf* is "deformation gradient tensor" F (written as matrix) 
* Matrix3r *velGrad* is …

*/
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
	//! stretch ratio Λ(n) (http://en.wikipedia.org/wiki/Finite_strain_theory#Stretch_ratio)
	Vector3r getStretchRatio() const { return Vector3r(trsf[0][0],trsf[1][1],trsf[2][2]); }
	//! return vector of consines of skew angle in yz, xz, xy planes between respective transformed base vectors
	const Vector3r& getCos() const {return _cos;}
	//! transformation matrix applying puse shear (normal strain removed: ones on the diagonal)
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
		void fillGlShearTrsfMatrix(double m[16]){
			m[0]=trsf[0][0]; m[4]=trsf[0][1]; m[8]=trsf[0][2]; m[12]=0;
			m[1]=trsf[1][0]; m[5]=trsf[1][1]; m[9]=trsf[1][2]; m[13]=0;
			m[2]=trsf[2][0]; m[6]=trsf[2][1]; m[10]=trsf[2][2];m[14]=0;
			m[3]=0;          m[7]=0;          m[11]=0;         m[15]=1;
		}

	public:

	//! "integrate" velGrad, update cached values used by public getter
	void integrateAndUpdate(Real dt){
		//incremental displacement gradient
		_trsfInc=dt*velGrad;
		// total transformation; M = (Id+G).M = F.M
		trsf+=_trsfInc*trsf;
		// Hsize will contain colums with transformed base vectors
		Matrix3r Hsize(refSize[0],refSize[1],refSize[2]);
		Hsize=trsf*Hsize;
		// lengths of transformed cell vectors, skew cosines
		Vector3r Hnorm[3]; // normalized transformed base vectors
		for(int i=0; i<3; i++){
			_size[i]=Vector3r(Hsize[i][0],Hsize[i][1],Hsize[i][2]).Length();
			Hnorm[i]=Vector3r(Hsize[i][0],Hsize[i][1],Hsize[i][2]); Hnorm[i].Normalize();
		};
		//cerr<<"Hsize="<<endl<<Hsize[0][0]<<" "<<Hsize[0][1]<<" "<<Hsize[0][2]<<endl<<Hsize[1][0]<<" "<<Hsize[1][1]<<" "<<Hsize[1][2]<<endl<<Hsize[2][0]<<" "<<Hsize[2][1]<<" "<<Hsize[2][2]<<endl;
		//cerr<<"Hnorm="<<Hnorm[0]<<" | "<<Hnorm[1]<<" | "<<Hnorm[2]<<endl;
		// skew cosines
		for(int i=0; i<3; i++){
			int i1=(i+1)%3, i2=(i+2)%3;
			// sin between axes is cos of skew
			_cos[i]=(Hnorm[i1].Cross(Hnorm[i2])).SquaredLength();
		}
		// pure shear trsf: ones on diagonal
		_shearTrsf=trsf; _shearTrsf[0][0]=_shearTrsf[1][1]=_shearTrsf[2][2]=1.;
		// pure unshear transformation
		_unshearTrsf=_shearTrsf.Inverse();
		// some parts can branch depending on presence/absence of shear
		_hasShear=(trsf[0][1]!=0 || trsf[0][2]!=0 || trsf[1][0]!=0 || trsf[1][2]!=0 || trsf[2][0]!=0 || trsf[2][1]!=0);
		// OpenGL shear matrix (used frequently)
		fillGlShearTrsfMatrix(_glShearTrsfMatrix);
	}
	
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
	REGISTER_ATTRIBUTES(Serializable,(refSize)(trsf)(velGrad));
	REGISTER_CLASS_AND_BASE(Cell,Serializable);
};
REGISTER_SERIALIZABLE(Cell);
