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
set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_BUILD_TYPE Release)

IF (NOT CHUNKSIZE)
 SET(CHUNKSIZE 5)
 MESSAGE("Maximum files to compile in one translation unit ${CHUNKSIZE}, if you want to override it use -DCHUNKSIZE option.")
ENDIF (NOT CHUNKSIZE)

IF (NOT SUFFIX)
  SET(SUFFIX version)
ENDIF ()

SET(YADE_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/yade-${SUFFIX})
SET(YADE_EXEC_PATH ${CMAKE_INSTALL_PREFIX}/bin)
SET(YADE_PY_PATH ${YADE_LIB_PATH}/py)


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -fPIC -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Werror=format-security -fopenmp")

INCLUDE(FindPythonLibs)
INCLUDE(FindPythonInterp)
INCLUDE(FindOpenMP)
INCLUDE(FindOpenGL)
INCLUDE(FindQt4)
INCLUDE(FindBoost)
INCLUDE(FindVTK)
INCLUDE(FindPkgConfig)
INCLUDE(FeatureSummary)
INCLUDE(CombineSources)


FIND_PACKAGE(Boost COMPONENTS python thread date_time filesystem iostreams regex serialization)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)
FIND_PACKAGE(VTK COMPONENTS Common REQUIRED) 
FIND_PACKAGE(OpenMP REQUIRED) 
FIND_PACKAGE(Eigen3)
FIND_PACKAGE(glib2 REQUIRED)

# Use Eigen3 by default
IF (EIGEN3_FOUND)
  INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
ELSE ()
  FIND_PACKAGE(Eigen2)
  if (EIGEN2_FOUND)
    INCLUDE_DIRECTORIES(${EIGEN2_INCLUDE_DIR})
  ELSE ()
    MESSAGE(FATAL_ERROR "None Eigen2 or Eigen3 were found!")
  ENDIF ()
ENDIF()

ADD_DEFINITIONS(-DYADE_PTR_CAST=static_pointer_cast -DYADE_CAST=static_cast -DYADE_OPENMP)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extra/floating_point_utilities_v3)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS})


FIND_PATH(CGAL_INCLUDE_DIR CGAL/Exact_predicates_inexact_constructions_kernel.h REQUIRED)
FIND_LIBRARY(CGAL_FOUND CGAL)
IF(${CGAL_FOUND})
	ADD_DEFINITIONS(-frounding-math)
	INCLUDE_DIRECTORIES(${CGAL_INCLUDE_DIR})
ENDIF()

PKG_SEARCH_MODULE(GTS gts)

IF (NOT INSTALL_PREFIX)
 SET(CMAKE_INSTALL_PREFIX "/usr/local")
 MESSAGE("Yade will be installed to default path ${CMAKE_INSTALL_PREFIX}, if you want to override it use -DINSTALL_PREFIX option.")
ELSE (NOT INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
	MESSAGE("Yade will be installed to ${CMAKE_INSTALL_PREFIX}")
ENDIF (NOT INSTALL_PREFIX)


SET(YADE_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/yade-${SUFFIX})
SET(YADE_EXEC_PATH ${CMAKE_INSTALL_PREFIX}/bin)
SET(YADE_PY_PATH ${YADE_LIB_PATH}/py)

FOREACH(FEAT "OPENGL" "VTK" "GTS" "CGAL")
	IF("${FEAT}_FOUND")
		add_definitions(-DYADE_${FEAT})
		message("Feature ${FEAT} enabled")
	ENDIF()
ENDFOREACH()

# this is only a temporary hack, headers should be installed in the build-dir directly
# perhaps there is a proper way, have to ask
IF(NOT EXISTS "${CMAKE_BINARY_DIR}/yade")
  EXECUTE_PROCESS(COMMAND ln -s ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/yade)
ENDIF ()


FILE(GLOB SRC_CORE "core/*.cpp")
FILE(GLOB_RECURSE SRC_PKG  "pkg/common/*.cpp")
FILE(GLOB SRC_LIB  "lib/*.cpp")

SET(SRC_LIB "${SRC_LIB};lib/base/Math.cpp;lib/factory/ClassFactory.cpp;lib/factory/DynLibManager.cpp;lib/multimethods/Indexable.cpp;lib/serialization/Serializable.cpp;lib/pyutil/gil.cpp;core/main/pyboot.cpp")


IF(CGAL_FOUND)
  FILE(GLOB SRC_LIB "${SRC_LIB}; triangulation/*.cpp")
ENDIF ()

FILE(GLOB SRC_PYMODULES_TEMP "py/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES_TEMP}")
FILE(GLOB SRC_PYMODULES_TEMP "py/mathWrap/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES};${SRC_PYMODULES_TEMP}")
FILE(GLOB_RECURSE SRC_PYMODULES_TEMP "py/wrapper/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES};${SRC_PYMODULES_TEMP}")
FILE(GLOB_RECURSE SRC_PYMODULES_TEMP "py/pack/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES};py/pack/_packObb.cpp;py/pack/_packPredicates.cpp;py/pack/_packSpheres.cpp;py/pack/SpherePadder/CellPartition.cpp;py/pack/SpherePadder/TetraMesh.cpp;")
#py/pack/_packSpherePadder.cpp;py/pack/SpherePadder/SpherePadder.cpp;py/pack/SpherePadder/SpherePackingTriangulation.cpp;

FILE(GLOB SRC_PYGTS "py/3rd-party/pygts-0.3.1/*.c")

IF(${OPENGL_FOUND})
	SET(SRC_LIB "${SRC_LIB};lib/opengl/GLUtils.cpp")
ENDIF()

SET(PY_COMPILED_MODULES mathWrap;gts/_gts;yade/boot;yade/log;yade/wrapper;yade/_customConverters;yade/_eudoxos;yade/_packPredicates;yade/_packSpheres;yade/_packObb;yade/_utils;yade/WeightedAverage2d) # ;yade/_packSpherePadder

IF(${QT4_FOUND})
	SET(PY_COMPILED_MODULES ${PY_COMPILED_MODULES};yade/qt/_GLViewer)
ENDIF()


COMBINE_SOURCES(${CMAKE_BINARY_DIR}/core "${SRC_CORE}" 1000)
FILE(GLOB SRC_CORE_COMBINED "${CMAKE_BINARY_DIR}/core.*.cpp")

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pkg "${SRC_PKG}" ${CHUNKSIZE})
FILE(GLOB SRC_PKG_COMBINED "${CMAKE_BINARY_DIR}/pkg.*.cpp")

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/lib "${SRC_LIB}" ${CHUNKSIZE})
FILE(GLOB SRC_LIB_COMBINED "${CMAKE_BINARY_DIR}/lib.*.cpp")

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pyModules "${SRC_PYMODULES}" 1000)
FILE(GLOB SRC_PYMODULES_COMBINED "${CMAKE_BINARY_DIR}/pyModules.*.cpp")


ADD_LIBRARY(core SHARED ${SRC_CORE_COMBINED})
ADD_LIBRARY(plugins SHARED ${SRC_PKG_COMBINED})
ADD_LIBRARY(support SHARED ${SRC_LIB_COMBINED})
ADD_LIBRARY(pyModules SHARED ${SRC_PYMODULES_COMBINED})

ADD_LIBRARY(_gts SHARED ${SRC_PYGTS})
SET_TARGET_PROPERTIES(_gts PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_gts gts)

INSTALL(TARGETS core DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS plugins DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS support DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS pyModules DESTINATION ${YADE_LIB_PATH}/py)
INSTALL(TARGETS _gts DESTINATION ${YADE_LIB_PATH}/py/gts)


INSTALL(FILES py/3rd-party/pygts-0.3.1/__init__.py DESTINATION ${YADE_LIB_PATH}/py/gts)
INSTALL(FILES py/3rd-party/pygts-0.3.1/pygts.py DESTINATION ${YADE_LIB_PATH}/py/gts)
INSTALL(FILES py/3rd-party/mtTkinter-0.3/mtTkinter.py DESTINATION ${YADE_LIB_PATH}/py)
