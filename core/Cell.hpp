// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/lib-base/yadeWm3Extra.hpp>

/*! Periodic cell parameters and routines. Usually instantiated as Scene::cell.

The cell has size and skew, which are independent. 

*/
class Cell{
	public:
		Vector3r size;
		Vector3r skew;
		Vector3r refSize;

	// caches
	Vector3r _skewAngle;
	Vector3r _skewSine;
	Matrix3r _skewTrsf;
	Matrix3r _unskewTrsf;
	// should be called before every step
	void updateCache(){
		for(int i=0; i<3; i++) {
			_skewAngle[i]=atan(skew[i]);
			_skewSine[i]=sin(_skewAngle);
		}
		_skewTrsf=Matrix3r(1,skew[1],skew[2],skew[0],1,skew[2],skew[0],skew[1],1);
		_unskewTrsf=_skewTrst.Inverse();
	}

	/*! Apply inverse skew on point; to put it inside (unskewed) periodic cell, apply wrapPt on the returned value. */
	Vector3r unskewPt(Vector3r pt){ return _unskewTrsf*pt; }
	/*! Wrap number to interval 0…sz */
	Real wrapNum(const Real& x, const Real& sz){
		Real norm=x/mx; return (norm-floor(norm))*sz;
	}
	/*! Wrap number to interval 0…sz; store how many intervals were wrapped in period */
	Real wrapNum(const Real& x, const Real& sz, int& period){
		Real norm=x/mx; period=(int)floor(norm); return (norm-period[i])*sz[i];
	}
	/*! Wrap point to inside the periodic cell; don't compute number of periods wrapped */
	Vector3r wrapPt(const Vector3r pt){
		Vector3r ret; for(int i=0;i<3;i++) ret[i]=wrapNum(pt[i],size[i]); return ret;
	}
	/*! Wrap point to inside the periodic cell; period will contain by how many cells it was wrapped. */
	Vector3r wrapPt(const Vector3r pt, Vector3<int>& period){
		Vector3r ret; for(int i=0; i<3; i++){ ret[i]=wrapNum(pt[i],size[i],period[i]); } return ret;
	}

	REGISTER_ATTRIBUTES(Serializable,(size)(skew));
	REGISTER_CLASS_AND_BASE(Cell,Serializable);
};
REGISTER_SERIALIZABLE(Cell);
