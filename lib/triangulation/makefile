# Created by the script cgal_create_makefile
# This is the makefile for compiling a CGAL application.

#---------------------------------------------------------------------#
#                    include platform specific settings
#---------------------------------------------------------------------#
# Choose the right include file from the <cgalroot>/make directory.

# CGAL_MAKEFILE = ENTER_YOUR_INCLUDE_MAKEFILE_HERE
ifndef CGAL_MAKEFILE
CGAL_MAKEFILE = /usr/share/cgal/cgal.mk
endif
include $(CGAL_MAKEFILE)

#---------------------------------------------------------------------#
#                    compiler flags
#---------------------------------------------------------------------#

CXXFLAGS = \
           $(CGAL_CXXFLAGS) \
           $(LONG_NAME_PROBLEM_CXXFLAGS) \
           -DCGAL_NDEBUG -DNDEBUG -DNO_REAL_CHECK -O3
           


#---------------------------------------------------------------------#
#                    linker flags
#---------------------------------------------------------------------#

LIBPATH = \
          $(CGAL_LIBPATH)

LDFLAGS = \
          $(LONG_NAME_PROBLEM_LDFLAGS) \
          $(CGAL_LDFLAGS) \
          $(OPENGL_LIBS) -lglut -lglui -lboost_iostreams

#---------------------------------------------------------------------#
#                    filename related flags
#---------------------------------------------------------------------#

STATIC_LIB_SUFFIX = .a




#---------------------------------------------------------------------#
#                    target entries
#---------------------------------------------------------------------#

all:            \
                TesselationWrapperOOT$(OBJ_EXT)\
                libTesselationWrapperOOT.a \
                Deviateur_ToTxt$(EXE_EXT) \
                Deviateur$(EXE_EXT) \
		test$(EXE_EXT) \
                
Vue3D$(OBJ_EXT): Vue3D.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Vue3D.cpp
	
GLset$(OBJ_EXT): GLset.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) GLset.cpp
	
glBMP$(OBJ_EXT): glBMP.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) glBMP.cpp

KinematicLocalisationAnalyser$(OBJ_EXT): KinematicLocalisationAnalyser.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) KinematicLocalisationAnalyser.cpp
	
Operations$(OBJ_EXT): Operations.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Operations.cpp

RegularTriangulation$(OBJ_EXT): RegularTriangulation.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) RegularTriangulation.cpp

Tenseur3$(OBJ_EXT): Tenseur3.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Tenseur3.cpp
	
basicVTKwritter$(OBJ_EXT): basicVTKwritter.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) basicVTKwritter.cpp
	
TriaxialState$(OBJ_EXT): TriaxialState.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) TriaxialState.cpp

Tesselation$(OBJ_EXT): Tesselation.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Tesselation.cpp

test$(OBJ_EXT): test.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) test.cpp
	
test$(EXE_EXT): test$(OBJ_EXT)\
		libTesselationWrapperOOT.a
	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT) test 	test$(OBJ_EXT) \
	-L. -lTesselationWrapperOOT -lCGAL
	
libTesselationWrapperOOT.a : 	TesselationWrapperOOT$(OBJ_EXT) \
				Tenseur3$(OBJ_EXT) \
				Tesselation$(OBJ_EXT) \
				RegularTriangulation$(OBJ_EXT) \
				TriaxialState$(OBJ_EXT) \
				KinematicLocalisationAnalyser$(OBJ_EXT) \
				basicVTKwritter$(OBJ_EXT)
	 		ar cr	libTesselationWrapperOOT.a \
	 			TesselationWrapperOOT$(OBJ_EXT) \
				Tenseur3$(OBJ_EXT) \
				Tesselation$(OBJ_EXT) \
				RegularTriangulation$(OBJ_EXT) \
				TriaxialState$(OBJ_EXT) \
				KinematicLocalisationAnalyser$(OBJ_EXT) \
				basicVTKwritter$(OBJ_EXT)
	
TesselationWrapperOOT$(OBJ_EXT): TesselationWrapperOOT.cpp 
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) TesselationWrapperOOT.cpp $(LDFLAGS)
	
Deviateur$(EXE_EXT): Deviateur$(OBJ_EXT) \
			Vue3D$(OBJ_EXT) \
			Tesselation$(OBJ_EXT) \
			Tenseur3$(OBJ_EXT) \
			KinematicLocalisationAnalyser$(OBJ_EXT) \
			RegularTriangulation$(OBJ_EXT) \
			glBMP$(OBJ_EXT) \
			GLset$(OBJ_EXT) \
			Operations$(OBJ_EXT) \
			Timer$(OBJ_EXT) \
			TriaxialState$(OBJ_EXT) \
			basicVTKwritter$(OBJ_EXT)
	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT)Deviateur 	Deviateur$(OBJ_EXT) \
							Vue3D$(OBJ_EXT) \
							Tesselation$(OBJ_EXT) \
							Tenseur3$(OBJ_EXT) \
							KinematicLocalisationAnalyser$(OBJ_EXT) \
							RegularTriangulation$(OBJ_EXT) \
							glBMP$(OBJ_EXT) \
							GLset$(OBJ_EXT) \
							Operations$(OBJ_EXT) \
							TriaxialState$(OBJ_EXT) \
							Timer$(OBJ_EXT) \
							basicVTKwritter$(OBJ_EXT) \
							$(LDFLAGS)
							
Deviateur_ToTxt$(EXE_EXT):Deviateur_ToTxt$(OBJ_EXT) \
			Vue3D$(OBJ_EXT) \
			Tesselation$(OBJ_EXT) \
			Tenseur3$(OBJ_EXT) \
			KinematicLocalisationAnalyser$(OBJ_EXT) \
			RegularTriangulation$(OBJ_EXT) \
			glBMP$(OBJ_EXT) \
			GLset$(OBJ_EXT) \
			Operations$(OBJ_EXT) \
			Timer$(OBJ_EXT) \
			TriaxialState$(OBJ_EXT) \
			basicVTKwritter$(OBJ_EXT)
	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT)Deviateur_ToTxt$ 	Deviateur_ToTxt$(OBJ_EXT) \
							Vue3D$(OBJ_EXT) \
							Tesselation$(OBJ_EXT) \
							Tenseur3$(OBJ_EXT) \
							KinematicLocalisationAnalyser$(OBJ_EXT) \
							RegularTriangulation$(OBJ_EXT) \
							glBMP$(OBJ_EXT) \
							GLset$(OBJ_EXT) \
							Operations$(OBJ_EXT) \
							TriaxialState$(OBJ_EXT) \
							Timer$(OBJ_EXT) \
							basicVTKwritter$(OBJ_EXT) \
							$(LDFLAGS)					
	

#	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT)
#	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT)
	
		




clean: \
                   Deformation.clean \
                   Deviateur.clean \
                   Deviateur_ToTxt.clean \
                   Empilement.clean \
                   GLset.clean \
                   GLview.clean \
                   KinematicLocalisationAnalyser.clean \
                   KinematicLocalisationAnalysis.clean \
                   Operations.clean \
                   RegularTriangulation.clean \
                   stdafx.clean \
                   Tenseur3.clean \
                   Tesselation.clean \
                   test.clean \
                   Timer.clean \
                   TriaxialState.clean \
                   Voronoi.clean \
                   Vue3D.clean \
                   basicVTKwritter.clean

#---------------------------------------------------------------------#
#                    suffix rules
#---------------------------------------------------------------------#

.C$(OBJ_EXT):
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) $<


.cpp$(OBJ_EXT):
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) $<

