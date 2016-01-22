/*CWBoon 2015 */
#pragma once
#ifdef YADE_POTENTIAL_PARTICLES
#include <pkg/dem/PotentialParticle.hpp>
#include <pkg/dem/PotentialParticle2AABB.hpp>
#include <pkg/common/GLDrawFunctors.hpp>
#include <lib/computational-geometry/MarchingCube.hpp>
#include <vector>
#include <pkg/common/PeriodicEngines.hpp>

#include <vtkImplicitFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>

#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkTriangle.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkSampleFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkWriter.h>
#include <vtkExtractVOI.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkActor.h>
#include <vtkAppendPolyData.h>

#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkVectorText.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkTextActor3D.h>
#include <vtkCylinderSource.h>



class ImpFunc : public vtkImplicitFunction {
	public:
		vtkTypeMacro(ImpFunc,vtkImplicitFunction);
		//void PrintSelf(ostream& os, vtkIndent indent);

		// Description
		// Create a new function
		static ImpFunc * New(void);
		vector<Real>a;
		vector<Real>b;
		vector<Real>c;
		vector<Real>d;
		Real k;
		Real r;
		Real R;
		Eigen::Matrix3d rotationMatrix;
		bool clump;
		Real clumpMemberCentreX;
		Real clumpMemberCentreY;
		Real clumpMemberCentreZ;
		// Description
		// Evaluate function
		Real FunctionValue(Real x[3]);
		Real EvaluateFunction(Real x[3]) {
			//return this->vtkImplicitFunction::EvaluateFunction(x);
			return FunctionValue(x);
		};

		Real EvaluateFunction(Real x, Real y, Real z) {
			return this->vtkImplicitFunction::EvaluateFunction(x, y, z);
		};




		// Description
		// Evaluate gradient for function
		void EvaluateGradient(Real x[3], Real n[3]) { };

		// If you need to set parameters, add methods here

	protected:
		ImpFunc();
		~ImpFunc();
		ImpFunc(const ImpFunc&) {}
		void operator=(const ImpFunc&) {}

		// Add parameters/members here if you need
};

#ifdef YADE_OPENGL
class Gl1_PotentialParticle : public GlShapeFunctor {
	private :
		MarchingCube mc;
		Vector3r min,max;
		vector<vector<vector<Real > > > scalarField,weights;
		void generateScalarField(const PotentialParticle& pp);
		void calcMinMax(const PotentialParticle& pp);
		Real oldIsoValue,oldIsoSec,oldIsoThick;
		Vector3r isoStep;
		struct Leaf {
			Vector3r centre;
			Leaf(Vector3r pos) {
				centre = pos;
			}
			Leaf() {
				centre = Vector3r(0,0,0);
			}
		};
		struct scalarF {
			vector<vector<vector<Real > > > scalarField2;
			vector<Vector3r> triangles;
			vector<Vector3r> normals;
			int nbTriangles;
		};
		Real evaluateF(const PotentialParticle& pp, Real x, Real y, Real z);
		static vector<scalarF> SF ;

	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);

		YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_PotentialParticle,GlShapeFunctor,"Renders :yref:`PotentialParticle` object",
			((int,sizeX,10,,"Number of divisions in the x direction for triangulation"))
			((int,sizeY,10,,"Number of divisions in the y direction for triangulation"))
			((int,sizeZ,10,,"Number of divisions in the z direction for triangulation"))
			((bool,store,false,,"store computed triangulation or not"))
			((bool,initialized,false,,"if triangulation is initialized"))
			((Real,aabbEnlargeFactor,1.3,,"some factor for displaying algorithm, try different value if you have problems with displaying"))
		);
		RENDERS(PotentialParticle);
};
REGISTER_SERIALIZABLE(Gl1_PotentialParticle);
#endif // YADE_OPENGL


class PotentialParticleVTKRecorder: public PeriodicEngine {
	public:
		vtkSmartPointer<ImpFunc> function;

		virtual void action(void);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(PotentialParticleVTKRecorder,PeriodicEngine,"Engine recording potential blocks as surfaces into files with given periodicity.",
			((string,fileName,,,"File prefix to save to"))
			((int,sampleX,30,,"Number of divisions in the x direction for triangulation"))
			((int,sampleY,30,,"Number of divisions in the y direction for triangulation"))
			((int,sampleZ,30,,"Number of divisions in the z direction for triangulation"))
			((Real,maxDimension,30,,"max dimension"))
			((bool,twoDimension,false,,"2D or not"))
			((bool,REC_INTERACTION,false,,"contact point and forces"))
			((bool,REC_COLORS,false,,"colors"))
			((bool,REC_VELOCITY,false,,"velocity"))
			((bool,REC_ID,true,,"id"))
			,
			function = ImpFunc::New();
			,

		);
};
REGISTER_SERIALIZABLE(PotentialParticleVTKRecorder);

#endif // YADE_POTENTIAL_PARTICLES
