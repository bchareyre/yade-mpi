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

IF (NOT LIB_EIGEN_PATH)
 SET(LIB_EIGEN_PATH "/usr/include/eigen2")
 MESSAGE("Headers of eigen library will be included from  ${LIB_EIGEN_PATH}, if you want to override it use -DLIB_EIGEN_PATH option.")
ENDIF (NOT LIB_EIGEN_PATH)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LIB_EIGEN_PATH})
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

IF (NOT CHUNKSIZE)
 SET(CHUNKSIZE 100)
 MESSAGE("Maximum files to compile in one translation unit ${CHUNKSIZE}, if you want to override it use -DCHUNKSIZE option.")
ENDIF (NOT CHUNKSIZE)


IF (NOT INSTALL_PREFIX)
 SET(CMAKE_INSTALL_PREFIX "/usr/local")
 MESSAGE("Yade will be installed to default path ${CMAKE_INSTALL_PREFIX}, if you want to override it use -DINSTALL_PREFIX option.")
ELSE (NOT INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
	MESSAGE("Yade will be installed to ${CMAKE_INSTALL_PREFIX}")
ENDIF (NOT CMAKE_INSTALL_PREFIX)


SET(YADE_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/yade-${SUFFIX})
SET(YADE_EXEC_PATH ${CMAKE_INSTALL_PREFIX}/bin)
SET(YADE_PY_PATH ${YADE_LIB_PATH}/py)

#"OPENMP" 
FOREACH(FEAT "OPENGL" "VTK" "GTS" "CGAL")
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
		IF(${COUNTER} EQUAL ${MAXNUM})
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
COMBINE_SOURCES(${CMAKE_BINARY_DIR}/core "${SRC_CORE}" 1000)
COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pkg "${SRC_PKG}" ${CHUNKSIZE})
COMBINE_SOURCES(${CMAKE_BINARY_DIR}/lib "${SRC_LIB}" ${CHUNKSIZE})
FILE(GLOB SRC_CORE_COMBINED "${CMAKE_BINARY_DIR}/core.*.cpp")
FILE(GLOB SRC_PKG_COMBINED  "${CMAKE_BINARY_DIR}/pkg.*.cpp")
FILE(GLOB SRC_LIB_COMBINED  "${CMAKE_BINARY_DIR}/lib.*.cpp")

#add_library(core SHARED ${SRC_CORE_COMBINED})
#add_library(pkg SHARED ${SRC_PKG_COMBINED})
#add_library(support SHARED ${SRC_LIB_COMBINED})
#target_link_libraries(core support)
#target_link_libraries(pkg core)
SET(SRC_PYMODULES py/WeightedAverage2d.cpp;py/_eudoxos.cpp;py/log.cpp;py/_utils.cpp;py/wrapper/customConverters.cpp)

SET(SRC_PACK ${CMAKE_SOURCE_DIR}/py/pack/_packPredicates.cpp;${CMAKE_SOURCE_DIR}/py/pack/_packObb.cpp;${CMAKE_SOURCE_DIR}/py/pack/_packSpheres.cpp) # ;${CMAKE_SOURCE_DIR}/py/pack/_packSpherePadder.cpp;${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/SpherePadder.cpp;${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/TetraMesh.cpp;${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/CellPartition.cpp
#IF(${CGAL_FOUND})
#	SET(SRC_PACK ${SRC_PACK};${CMAKE_SOURCE_DIR}/py/pack/SpherePadder/SpherePackingTriangulation.cpp)
#ENDIF()

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pyPack "${SRC_PACK}" 100)
SET(SRC_PACK_COMBINED "${CMAKE_BINARY_DIR}/pyPack.0.cpp")
#add_library(pyPack STATIC ${CMAKE_BINARY_DIR}/pyPack.0.cpp)
#add_library(pyWrapper STATIC py/wrapper/yadeWrapper.cpp)

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pyModules "${SRC_PYMODULES}" 100)
FILE(GLOB SRC_PYMODULES_COMBINED "${CMAKE_BINARY_DIR}/pyModules.*.cpp")
#add_library(pyModules SHARED ${SRC_PYMODULES_COMBINED})
#target_link_libraries(pyModules pyWrapper pyPack pyEigen _gts)

# will be different for debugging builds!
SET(YADE_LIBDIR lib/yade${YADE_SUFFIX})

SET(SRC_PYGTS "py/3rd-party/pygts-0.3.1/cleanup.c;py/3rd-party/pygts-0.3.1/edge.c;py/3rd-party/pygts-0.3.1/face.c;py/3rd-party/pygts-0.3.1/object.c;py/3rd-party/pygts-0.3.1/point.c;py/3rd-party/pygts-0.3.1/pygts.c;py/3rd-party/pygts-0.3.1/segment.c;py/3rd-party/pygts-0.3.1/surface.c;py/3rd-party/pygts-0.3.1/triangle.c;py/3rd-party/pygts-0.3.1/vertex.c")


SET(YADE_ALL_SRCS ${SRC_CORE_COMBINED};${SRC_PKG_COMBINED};${SRC_LIB_COMBINED};${SRC_PYMODULES_COMBINED};${SRC_PACK_COMBINED};${SRC_PYGTS};py/mathWrap/miniEigen.cpp)

python_add_module(yade-all "${YADE_ALL_SRCS}")

IF(${GTS_FOUND})
	#add_library(_gts STATIC py/3rd-party/pygts-0.3.1/cleanup.c;py/3rd-party/pygts-0.3.1/edge.c;py/3rd-party/pygts-0.3.1/face.c;py/3rd-party/pygts-0.3.1/object.c;py/3rd-party/pygts-0.3.1/point.c;py/3rd-party/pygts-0.3.1/pygts.c;py/3rd-party/pygts-0.3.1/segment.c;py/3rd-party/pygts-0.3.1/surface.c;py/3rd-party/pygts-0.3.1/triangle.c;py/3rd-party/pygts-0.3.1/vertex.c)
	include_directories(${GTS_INCLUDE_DIRS})
	add_definitions(-DPYGTS_HAS_NUMPY)
	# see http://www.mail-archive.com/cmake@cmake.org/msg24812.html
	#set_target_properties(_gts
	#	PROPERTIES COMPILE_FLAGS "-Wno-all"
	#	COMPILE_DEFINES -DPYGTS_HAS_NUMPY )
	FOREACH(_libname ${GTS_LIBRARIES})
		target_link_libraries(yade-all ${_libname})
	ENDFOREACH()
ENDIF()

## add_library(pyEigen STATIC py/mathWrap/miniEigen.cpp)


SET(PY_COMPILED_MODULES mathWrap;gts/_gts;yade/boot;yade/log;yade/wrapper;yade/_customConverters;yade/_eudoxos;yade/_packPredicates;yade/_packSpheres;yade/_packObb;yade/_utils;yade/WeightedAverage2d) # ;yade/_packSpherePadder
IF(${QT4_FOUND})
	SET(PY_COMPILED_MODULES ${PY_COMPILED_MODULES};yade/qt/_GLViewer)
ENDIF()


install(TARGETS yade-all DESTINATION ${YADE_LIB_PATH})

FOREACH(_mod ${PY_COMPILED_MODULES})
	install(CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink ${YADE_LIB_PATH}/libyade-all.so ${YADE_PY_PATH}/${_mod}.so)")
ENDFOREACH()


# install our python scripts, keeping the hierarchy under py/
install(DIRECTORY py/ DESTINATION ${YADE_PY_PATH}
	FILES_MATCHING PATTERN "*.py"
	PATTERN "3rd-party" EXCLUDE
)

# install 3rd party python files
install(FILES py/3rd-party/pygts-0.3.1/__init__.py DESTINATION ${YADE_LIB_PATH}/gts)
install(FILES py/3rd-party/pygts-0.3.1/pygts.py DESTINATION ${YADE_LIB_PATH}/gts)
install(FILES py/3rd-party/mtTkinter-0.3/mtTkinter.py DESTINATION ${YADE_LIB_PATH})

# install the files that need to be pre-processed (FIXME: this will need to define all variables, and call them the same as they are in scons -- for compatibility)
configure_file(core/main/yade-batch.in ${YADE_EXEC_PATH}/yade-batch-${SUFFIX})
configure_file(core/main/main.py.in ${YADE_EXEC_PATH}/yade-${SUFFIX})
configure_file(py/config.py.in ${YADE_PY_PATH}/yade/config.py)


