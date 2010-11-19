project(Yade C CXX)
cmake_minimum_required(VERSION 2.8)

include(FindPythonLibs)
include(FindPythonInterp)
include(FindOpenMP)
include(FindOpenGL)
include(FindQt4)
include(FindBoost)
include(FindVTK)
include(FindPkgConfig)
include(FeatureSummary)

find_package(Boost COMPONENTS python thread date_time filesystem iostreams regex serialization)
find_package(Qt4 COMPONENTS QtCore QtGgui QtOpenGL)
find_package(VTK COMPONENTS Common REQUIRED) 

add_definitions(-DYADE_PTR_CAST=static_pointer_cast -DYADE_CAST=static_cast)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/include/eigen2)
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extra/floating_point_utilities_v3)

find_path(CGAL_INCLUDE_DIR CGAL/Exact_predicates_inexact_constructions_kernel.h REQUIRED)
find_library(CGAL_FOUND CGAL)
if(${CGAL_FOUND})
	add_definitions(-frounding-math)
	INCLUDE_DIRECTORIES(${CGAL_INCLUDE_DIR})
endif()

pkg_search_module(GTS gts)

# ccache distcc clang++: must check compiler and its first 2 "args"
if((CMAKE_CXX_COMPILER MATCHES ".*clang.*") OR (CMAKE_CXX_COMPILER_ARG1 MATCHES ".*clang.*") OR (CMAKE_CXX_COMPILER_ARG2 MATCHES ".*clang.*"))
	add_definitions(-Wno-unused-variable -Wno-mismatched-tags -Wno-constant-logical-operand -Qunused-arguments -Wno-empty-body)
	MESSAGE("Using clang, suppressing some warnings.")
endif()
	

#set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_INSTALL_PREFIX /usr/local)

#"OPENMP" 
FOREACH(FEAT "OPENGL" "VTK" "LOG4CXX" "GTS" "CGAL")
	if("${FEAT}_FOUND")
		add_definitions(-DYADE_${FEAT})
		message("Feature ${FEAT} enabled")
	endif()
ENDFOREACH()
PRINT_ENABLED_FEATURES()
PRINT_DISABLED_FEATURES()

# this is only a temporary hack, headers should be installed in the build-dir directly
# perhaps there is a proper way, have to ask
if(NOT EXISTS "${CMAKE_BINARY_DIR}/yade")
	EXECUTE_PROCESS(COMMAND ln -s ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/yade)
endif()
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})


# include several source files (in SRCS) in one or more
# combined files; each combined file holds maximally 
# MAXNUM files.
# BASE gives basename for created files;
# files corresponding to the pattern are deleted
# first, so that there are no relicts from previous runs
# with possibly different MAXNUM
MACRO(COMBINE_SOURCES BASE SRCS MAXNUM)
	LIST(LENGTH SRCS SRCS_LENGTH)
	SET(COMB_COUNTER 0)
	FILE(GLOB EXISTING "${BASE}.*.cpp")
	IF("$EXISTING")
		FILE(REMOVE ${EXISTING})
	ENDIF()
	SET(OUT "${BASE}.${COMB_COUNTER}.cpp")
	FILE(WRITE ${OUT})
	SET(COUNTER 0)
	FOREACH(SRC ${SRCS})
		if(${SRC} MATCHES "^/.*$") # absolute filename
			FILE(APPEND ${OUT} "#include<${SRC}>\n")
		else()
			FILE(APPEND ${OUT} "#include<${CMAKE_SOURCE_DIR}/${SRC}>\n")
		endif()
		MATH(EXPR COUNTER "${COUNTER}+1")
		IF(${COUNTER} GREATER ${MAXNUM})
			SET(COUNTER 0)
			MATH(EXPR COMB_COUNTER ${COMB_COUNTER}+1)
			SET(OUT "${BASE}.${COMB_COUNTER}.cpp")
			FILE(WRITE ${OUT})
		ENDIF()
	ENDFOREACH()
ENDMACRO()

FILE(GLOB         SRC_CORE "core/*.cpp")
FILE(GLOB_RECURSE SRC_PKG "pkg/*.cpp")
# without lib/, will be added below
SET(SRC_LIB "lib/base/Math.cpp;lib/factory/ClassFactory.cpp;lib/factory/DynLibManager.cpp;lib/multimethods/Indexable.cpp;lib/serialization/Serializable.cpp;lib/pyutil/gil.cpp")
IF(${CGAL_FOUND})
	SET(SRC_LIB "${SRC_LIB};lib/triangulation/KinematicLocalisationAnalyser.cpp;lib/triangulation/Operations.cpp;lib/triangulation/RegularTriangulation.cpp;lib/triangulation/Timer.cpp;lib/triangulation/basicVTKwritter.cpp;lib/triangulation/FlowBoundingSphere.cpp;lib/triangulation/Network.cpp;lib/triangulation/Deformation.cpp;lib/triangulation/Empilement.cpp;lib/triangulation/stdafx.cpp;lib/triangulation/Tenseur3.cpp;lib/triangulation/Tesselation.cpp;lib/triangulation/TriaxialState.cpp")
ENDIF()
IF(${OPENGL_FOUND})
	SET(SRC_LIB "${SRC_LIB};lib/opengl/GLUtils.cpp")
ENDIF()

# the last number is "chunkSize"
COMBINE_SOURCES(${CMAKE_BINARY_DIR}/core "${SRC_CORE}" 100)
COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pkg "${SRC_PKG}" 15)
COMBINE_SOURCES(${CMAKE_BINARY_DIR}/lib "${SRC_LIB}" 100)
FILE(GLOB SRC_CORE_COMBINED "${CMAKE_BINARY_DIR}/core.*.cpp")
FILE(GLOB SRC_PKG_COMBINED  "${CMAKE_BINARY_DIR}/pkg.*.cpp")
FILE(GLOB SRC_LIB_COMBINED  "${CMAKE_BINARY_DIR}/lib.*.cpp")

add_library(core SHARED ${SRC_CORE_COMBINED})
add_library(pkg SHARED ${SRC_PKG_COMBINED})
add_library(support SHARED ${SRC_LIB_COMBINED})
target_link_libraries(core support)
target_link_libraries(pkg core)

# python modules
# we compile gts separately, but all are linked to a single library, which links to pkg
# that makes it significantly smaller, faster to build and easier to link
IF(${GTS_FOUND})
	add_library(_gts STATIC py/3rd-party/pygts-0.3.1/cleanup.c;py/3rd-party/pygts-0.3.1/edge.c;py/3rd-party/pygts-0.3.1/face.c;py/3rd-party/pygts-0.3.1/object.c;py/3rd-party/pygts-0.3.1/point.c;py/3rd-party/pygts-0.3.1/pygts.c;py/3rd-party/pygts-0.3.1/segment.c;py/3rd-party/pygts-0.3.1/surface.c;py/3rd-party/pygts-0.3.1/triangle.c;py/3rd-party/pygts-0.3.1/vertex.c)
	include_directories(${GTS_INCLUDE_DIRS})
	add_definitions(-DPYGTS_HAS_NUMPY)
	# see http://www.mail-archive.com/cmake@cmake.org/msg24812.html
	#set_target_properties(_gts
	#	PROPERTIES COMPILE_FLAGS "-Wno-all"
	#	COMPILE_DEFINES -DPYGTS_HAS_NUMPY )
	FOREACH(_libname ${GTS_LIBRARIES})
		target_link_libraries(_gts ${_libname})
	ENDFOREACH()
ENDIF()

add_library(pyEigen STATIC py/mathWrap/miniEigen.cpp)

SET(SRC_PYMODULES py/WeightedAverage2d.cpp;py/_eudoxos.cpp;py/log.cpp;py/_utils.cpp;py/wrapper/customConverters.cpp)

SET(SRC_PADDER ${CMAKE_SOURCE_DIR}/py/pack/_packPredicates.cpp;${CMAKE_SOURCE_DIR}/py/pack/_packObb.cpp;${CMAKE_SOURCE_DIR}/py/pack/_packSpheres.cpp;${CMAKE_SOURCE_DIR}/py/pack/_packSpherePadder.cpp;${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/SpherePadder.cpp;${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/TetraMesh.cpp;${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/CellPartition.cpp)
IF(${CGAL_FOUND})
	SET(SRC_PADDER ${SRC_PADDER};${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/SpherePackingTriangulation.cpp)
ENDIF()

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pyPadder "${SRC_PADDER}" 100)
add_library(pyPadder STATIC ${CMAKE_BINARY_DIR}/pyPadder.0.cpp)

add_library(pyWrapper STATIC py/wrapper/yadeWrapper.cpp)

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pyModules "${SRC_PYMODULES}" 100)
FILE(GLOB SRC_PYMODULES_COMBINED "${CMAKE_BINARY_DIR}/pyModules.*.cpp")
add_library(pyModules SHARED ${SRC_PYMODULES_COMBINED})
target_link_libraries(pyModules pyWrapper pyPadder pyEigen _gts)

# will be different for debugging builds!
SET(YADE_LIBDIR lib/yade${YADE_SUFFIX})

#INSTALL(TARGETS pyEigen _gts 
#	LIBRARY DESTINATION ${YADE_LIBDIR}/py
#)
INSTALL(TARGETS pyModules
	LIBRARY DESTINATION ${YADE_LIBDIR}/py/yade
)
INSTALL(TARGETS core support pkg
	LIBRARY DESTINATION ${YADE_LIBDIR}/
)

