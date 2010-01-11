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
          $(OPENGL_LIBS) -lglut -lglui

#---------------------------------------------------------------------#
#                    filename related flags
#---------------------------------------------------------------------#

STATIC_LIB_SUFFIX = .a




#---------------------------------------------------------------------#
#                    target entries
#---------------------------------------------------------------------#

all:            \
                TesselationWrapper$(OBJ_EXT)\
                libTesselationWrapper.a \
		test$(EXE_EXT) \
                

Operations$(OBJ_EXT): Operations.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Operations.cpp

RegularTriangulation$(OBJ_EXT): RegularTriangulation.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) RegularTriangulation.cpp

Tenseur3$(OBJ_EXT): Tenseur3.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Tenseur3.cpp
	
TriaxialState$(OBJ_EXT): TriaxialState.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) TriaxialState.cpp
	
KinematicLocalisationAnalyser$(OBJ_EXT): KinematicLocalisationAnalyser.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) KinematicLocalisationAnalyser.cpp

Tesselation$(OBJ_EXT): Tesselation.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) Tesselation.cpp

test$(OBJ_EXT): test.cpp
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) test.cpp
	
test$(EXE_EXT): test$(OBJ_EXT)\
		libTesselationWrapper.a
	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT) test 	test$(OBJ_EXT) \
	-L. -lTesselationWrapper -lCGAL
	
libTesselationWrapper.a : 	TesselationWrapper$(OBJ_EXT) \
				Tenseur3$(OBJ_EXT) \
				Tesselation$(OBJ_EXT) \
				RegularTriangulation$(OBJ_EXT) \
				TriaxialState$(OBJ_EXT) \
				KinematicLocalisationAnalyser$(OBJ_EXT)
	 		ar cr	libTesselationWrapper.a \
	 			TesselationWrapper$(OBJ_EXT) \
				Tenseur3$(OBJ_EXT) \
				Tesselation$(OBJ_EXT) \
				RegularTriangulation$(OBJ_EXT) \
				TriaxialState$(OBJ_EXT) \
				KinematicLocalisationAnalyser$(OBJ_EXT)
	
TesselationWrapper$(OBJ_EXT): TesselationWrapper.cpp 
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) TesselationWrapper.cpp $(LDFLAGS)
	

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
                   Vue3D.clean 

#---------------------------------------------------------------------#
#                    suffix rules
#---------------------------------------------------------------------#

.C$(OBJ_EXT):
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) $<


.cpp$(OBJ_EXT):
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) $<

