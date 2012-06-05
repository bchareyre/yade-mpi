# cmake build file for yade
# the way to build yade
# 1. gather all source files in chunks (using COMBINE_SOURCES) that will be compiled together
#    (otherwise each source file would be compiled separately, which is too slow)
#    Unlike in scons compilations, also python modules are gathered in chunks.
# 2. create yade-all library that is compiled from all the chunks
# 3. Create symlinks pointing to yade-all for all python modules we want to create
#    (this will have to be 
#
#
project(Yade C CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cMake")

IF (NOT CHUNKSIZE)
 SET(CHUNKSIZE 5)
 MESSAGE("Maximum files to compile in one translation unit ${CHUNKSIZE}, if you want to override it use -DCHUNKSIZE option.")
ENDIF (NOT CHUNKSIZE)

INCLUDE(FindPythonLibs)
INCLUDE(FindPythonInterp)
INCLUDE(FindOpenMP)
INCLUDE(FindQt4)
INCLUDE(FindBoost)
INCLUDE(FindVTK)
INCLUDE(FindPkgConfig)
INCLUDE(CombineSources)
INCLUDE(GetVersion)
INCLUDE(FindOpenGL)
INCLUDE(FindGTS)
INCLUDE(FindGL2PS)


FIND_PACKAGE(Boost COMPONENTS python thread date_time filesystem iostreams regex serialization REQUIRED)
FIND_PACKAGE(Eigen3)

SET(DEFAULT ON CACHE INTERNAL "Default value for enabled by default options")
SET(LINKLIBS "")
SET(CONFIGURED_FEATS "")
SET(CMAKE_LD_FLAGS  "${CMAKE_LD_FLAGS} -lrt")


OPTION(ENABLE_VTK "Enable VTK" ${DEFAULT})
OPTION(ENABLE_OPENMP "Enable OpenMP" ${DEFAULT})
OPTION(ENABLE_GTS "Enable GTS" ${DEFAULT})
OPTION(ENABLE_GUI "Enable GUI" ${DEFAULT})
OPTION(ENABLE_CGAL "Enable CGAL" ${DEFAULT})
OPTION(ENABLE_GL2PS "Enable GL2PS" ${DEFAULT})
#===========================================================
# Use Eigen3 by default
IF (EIGEN3_FOUND)
  INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
  MESSAGE(STATUS "Found Eigen3")
  SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} Eigen3")
ELSE ()
  FIND_PACKAGE(Eigen2)
  if (EIGEN2_FOUND)
    INCLUDE_DIRECTORIES(${EIGEN2_INCLUDE_DIR})
    MESSAGE(STATUS "Found Eigen2")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} Eigen2")
  ELSE ()
    MESSAGE(FATAL_ERROR "None Eigen2 or Eigen3 were found!")
  ENDIF ()
ENDIF()
#===========================================================
IF(ENABLE_VTK)
  FIND_PACKAGE(VTK COMPONENTS Common) 
  IF(VTK_FOUND)
    INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
    LINK_DIRECTORIES( ${VTK_LIBRARY_DIRS} )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DYADE_VTK")
    SET(features "${features},vtk")
    MESSAGE(STATUS "Found VTK")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} VTK")
  ELSE(VTK_FOUND)
    MESSAGE(STATUS "VTK NOT found")
    SET(${ENABLE_VTK} OFF)
  ENDIF(VTK_FOUND)
ENDIF(ENABLE_VTK)
#===========================================================
IF(ENABLE_OPENMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DYADE_OPENMP -fopenmp")
    MESSAGE(STATUS "Found OpenMP")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} OpenMP")
    SET(features "${features},openmp")
  ELSE(OPENMP_FOUND)
    MESSAGE(STATUS "OpenMP NOT found")
    SET(${ENABLE_OPENMP} OFF)
  ENDIF(OPENMP_FOUND)
ENDIF(ENABLE_OPENMP)
#===========================================================
IF(ENABLE_GTS)
  FIND_PACKAGE(GTS)
  FIND_PACKAGE(glib2)
  IF(GTS_FOUND AND GLIB2_FOUND)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DYADE_GTS ${CMAKE_GTS_CXX_FLAGS}")
    SET(CMAKE_LD_FLAGS  "${CMAKE_LD_FLAGS} ${GTS_LIBRARIES}")
    INCLUDE_DIRECTORIES(${GTS_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS})
    MESSAGE(STATUS "Found GTS")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} GTS")
    SET(features "${features},gts")
  ELSE(GTS_FOUND AND GLIB2_FOUND)
    MESSAGE(STATUS "GTS NOT found")
    SET(${ENABLE_GTS} OFF)
  ENDIF(GTS_FOUND AND GLIB2_FOUND)
ENDIF(ENABLE_GTS)
#===========================================================
IF(ENABLE_GUI)
  FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtOpenGL)
  FIND_PACKAGE(OpenGL)
  FIND_PACKAGE(GLUT)
  FIND_PACKAGE(glib2)
  FIND_PACKAGE(QGLVIEWER)
  IF(QT4_FOUND AND OPENGL_FOUND AND GLUT_FOUND AND GLIB2_FOUND AND QGLVIEWER_FOUND)
    SET(GUI_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${QGLVIEWER_LIBRARIES})
    SET(GUI_SRC_LIB "lib/opengl/GLUtils.cpp")
    SET(features "${features},qt4,opengl")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DYADE_OPENGL")
    INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS})
    MESSAGE(STATUS "Found GUI-LIBS")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} GUI")
  ELSE(QT4_FOUND AND OPENGL_FOUND AND GLUT_FOUND AND GLIB2_FOUND AND QGLVIEWER_FOUND)
    MESSAGE(STATUS "GUI-LIBS NOT found")
    SET(${ENABLE_GUI} OFF)
  ENDIF(QT4_FOUND AND OPENGL_FOUND AND GLUT_FOUND AND GLIB2_FOUND AND QGLVIEWER_FOUND)
ENDIF(ENABLE_GUI)
#===========================================================
IF(ENABLE_CGAL)
  FIND_PATH(CGAL_INCLUDE_DIR CGAL/Exact_predicates_inexact_constructions_kernel.h)
  FIND_LIBRARY(CGAL_FOUND CGAL)
  IF(CGAL_FOUND)
    INCLUDE_DIRECTORIES(${CGAL_INCLUDE_DIR})
    FILE(GLOB CGAL_SRC_LIB "triangulation/*.cpp")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math -DYADE_CGAL")
    MESSAGE(STATUS "Found CGAL")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} CGAL")
  ELSE(CGAL_FOUND)
    MESSAGE(STATUS "CGAL NOT found")
    SET(${ENABLE_CGAL} OFF)
  ENDIF(CGAL_FOUND)
ENDIF(ENABLE_CGAL)
#===========================================================
IF(ENABLE_GL2PS)
  FIND_PACKAGE(GL2PS)
  IF(GL2PS_FOUND)
    INCLUDE_DIRECTORIES(${GL2PS_INCLUDE_DIR})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DYADE_GL2PS")
    SET(LINKLIBS  "${LINKLIBS}${GL2PS_LIBRARIES}")
    MESSAGE(STATUS "Found GL2PS")
    SET(CONFIGURED_FEATS "${CONFIGURED_FEATS} GL2PS")
  ELSE(GL2PS_FOUND)
    MESSAGE(STATUS "GL2PS NOT found")
    SET(${ENABLE_GL2PS} OFF)
  ENDIF(GL2PS_FOUND)
ENDIF(ENABLE_GL2PS)
#===========================================================

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extra/floating_point_utilities_v3)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

IF (DEBUG)
  SET(CMAKE_VERBOSE_MAKEFILE 1)
  SET(CMAKE_BUILD_TYPE Debug)
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DYADE_PTR_CAST=static_pointer_cast -DYADE_CAST=static_cast -DYADE_DEBUG -Wall -fPIC -g -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Werror=format-security -ggdb2")
ELSE (DEBUG)
  SET(CMAKE_BUILD_TYPE Release)
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DYADE_PTR_CAST=static_pointer_cast -DYADE_CAST=static_cast -Wall -fPIC -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Werror=format-security -s")
ENDIF (DEBUG)


#===========================================================

IF (NOT INSTALL_PREFIX)
 SET(CMAKE_INSTALL_PREFIX "/usr/local")
 MESSAGE("Yade will be installed to default path ${CMAKE_INSTALL_PREFIX}, if you want to override it use -DINSTALL_PREFIX option.")
ELSE (NOT INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
	MESSAGE("Yade will be installed to ${CMAKE_INSTALL_PREFIX}")
ENDIF (NOT INSTALL_PREFIX)

IF (NOT SUFFIX)
  SET (SUFFIX "-${VERSION}")
ENDIF (NOT SUFFIX)

IF (NOT runtimePREFIX)
  SET (runtimePREFIX ${CMAKE_INSTALL_PREFIX})
ENDIF (NOT runtimePREFIX)

MESSAGE (STATUS "Suffix is set to " ${SUFFIX})
MESSAGE (STATUS "runtimePREFIX is set to " ${runtimePREFIX})
#===========================================================

SET(YADE_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/yade${SUFFIX})
SET(YADE_EXEC_PATH ${CMAKE_INSTALL_PREFIX}/bin)
SET(YADE_PY_PATH ${YADE_LIB_PATH}/py)


SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${YADE_LIB_PATH}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#===========================================================
IF(ENABLE_GUI)
  ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/gui")
ENDIF(ENABLE_GUI)
ADD_SUBDIRECTORY("${CMAKE_SOURCE_DIR}/py")
#===========================================================

# this is only a temporary hack, headers should be installed in the build-dir directly
# perhaps there is a proper way, have to ask
IF(NOT EXISTS "${CMAKE_BINARY_DIR}/yade")
  EXECUTE_PROCESS(COMMAND ln -s ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/yade)
ENDIF ()
#===========================================================

FILE(GLOB SRC_CORE "core/*.cpp")
FILE(GLOB_RECURSE SRC_PKG  "pkg/*.cpp")
FILE(GLOB SRC_LIB  "lib/*.cpp")

SET(SRC_LIB "${SRC_LIB};lib/base/Math.cpp;lib/factory/ClassFactory.cpp;lib/factory/DynLibManager.cpp;lib/multimethods/Indexable.cpp;lib/serialization/Serializable.cpp;lib/pyutil/gil.cpp;core/main/pyboot.cpp;${GUI_SRC_LIB};${CGAL_SRC_LIB}")

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/core "${SRC_CORE}" 1000)
FILE(GLOB SRC_CORE_COMBINED "${CMAKE_BINARY_DIR}/core.*.cpp")

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pkg "${SRC_PKG}" ${CHUNKSIZE})
FILE(GLOB SRC_PKG_COMBINED "${CMAKE_BINARY_DIR}/pkg.*.cpp")

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/lib "${SRC_LIB}" ${CHUNKSIZE})
FILE(GLOB SRC_LIB_COMBINED "${CMAKE_BINARY_DIR}/lib.*.cpp")

#===========================================================
ADD_LIBRARY(core SHARED ${SRC_CORE_COMBINED})
ADD_LIBRARY(plugins SHARED ${SRC_PKG_COMBINED})
ADD_LIBRARY(support SHARED ${SRC_LIB_COMBINED})

ADD_LIBRARY(boot SHARED ${CMAKE_CURRENT_SOURCE_DIR}/core/main/pyboot.cpp)
SET_TARGET_PROPERTIES(boot PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(support ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${LINKLIBS})
TARGET_LINK_LIBRARIES(core support ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${LINKLIBS})

TARGET_LINK_LIBRARIES(plugins support core miniEigen ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${LINKLIBS})
IF(ENABLE_VTK)
  TARGET_LINK_LIBRARIES(plugins vtkHybrid)
ENDIF(ENABLE_VTK)
IF(ENABLE_GUI)
  TARGET_LINK_LIBRARIES(core ${GUI_LIBS})
  TARGET_LINK_LIBRARIES(plugins _GLViewer ${GUI_LIBS})
  TARGET_LINK_LIBRARIES(support ${GUI_LIBS})
ENDIF(ENABLE_GUI)

TARGET_LINK_LIBRARIES(boot plugins support core ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
ADD_LIBRARY(miniEigen SHARED py/mathWrap/miniEigen.cpp)
SET_TARGET_PROPERTIES(miniEigen PROPERTIES PREFIX "")

#====================================
#Back compatibility with scons
SET (realVersion ${VERSION})
SET (version ${VERSION})
SET (pyExecutable ${PYTHON_EXECUTABLE})
SET (profile "default")
SET (sourceRoot "${CMAKE_CURRENT_SOURCE_DIR}")
#====================================

CONFIGURE_FILE(core/main/yade-batch.in "${CMAKE_BINARY_DIR}/yade${SUFFIX}-batch")
CONFIGURE_FILE(core/main/main.py.in "${CMAKE_BINARY_DIR}/yade${SUFFIX}")
CONFIGURE_FILE(py/config.py.in "${CMAKE_BINARY_DIR}/config.py")
CONFIGURE_FILE(py/__init__.py.in "${CMAKE_BINARY_DIR}/__init__.py")

#===========================================================

INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/yade${SUFFIX}-batch" DESTINATION ${YADE_EXEC_PATH}/)
INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/yade${SUFFIX}" DESTINATION ${YADE_EXEC_PATH}/)
INSTALL(FILES "${CMAKE_BINARY_DIR}/config.py" DESTINATION ${YADE_PY_PATH}/yade/)
INSTALL(FILES "${CMAKE_BINARY_DIR}/__init__.py" DESTINATION ${YADE_PY_PATH}/yade/)
FILE(GLOB filesPYChecks "${CMAKE_CURRENT_SOURCE_DIR}/scripts/checks-and-tests/checks/*.py")
INSTALL(FILES ${filesPYChecks} DESTINATION ${YADE_PY_PATH}/yade/tests/checks)
FILE(GLOB filesPYChecksData "${CMAKE_CURRENT_SOURCE_DIR}/scripts/checks-and-tests/checks/data/*")
INSTALL(FILES ${filesPYChecksData} DESTINATION ${YADE_PY_PATH}/yade/tests/checks/data)

INSTALL(TARGETS boot DESTINATION "${YADE_PY_PATH}/yade/")
INSTALL(TARGETS core DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS plugins DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS support DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS miniEigen DESTINATION ${YADE_LIB_PATH}/py)
#===========================================================
MESSAGE(STATUS "===========================================================")
MESSAGE(STATUS "Yade configured with following features:${CONFIGURED_FEATS}")
MESSAGE(STATUS "Chunksize: ${CHUNKSIZE}")
IF (DEBUG)
  MESSAGE(STATUS "Debug build")
ELSE (DEBUG)
  MESSAGE(STATUS "Optimized build")
ENDIF (DEBUG)
MESSAGE(STATUS "===========================================================")
#===========================================================
