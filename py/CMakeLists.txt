FILE(GLOB SRC_PYMODULES_TEMP "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES_TEMP}")
FILE(GLOB SRC_PYMODULES_TEMP "${CMAKE_CURRENT_SOURCE_DIR}/mathWrap/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES};${SRC_PYMODULES_TEMP}")
FILE(GLOB_RECURSE SRC_PYMODULES_TEMP "${CMAKE_CURRENT_SOURCE_DIR}/wrapper/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES};${SRC_PYMODULES_TEMP}")
FILE(GLOB_RECURSE SRC_PYMODULES_TEMP "${CMAKE_CURRENT_SOURCE_DIR}/pack/*.cpp")
SET (SRC_PYMODULES "${SRC_PYMODULES};${CMAKE_CURRENT_SOURCE_DIR}/pack/_packObb.cpp;${CMAKE_CURRENT_SOURCE_DIR}/pack/_packPredicates.cpp;${CMAKE_CURRENT_SOURCE_DIR}/pack/_packSpheres.cpp;${CMAKE_CURRENT_SOURCE_DIR}/pack/SpherePadder/CellPartition.cpp;${CMAKE_CURRENT_SOURCE_DIR}/pack/SpherePadder/TetraMesh.cpp;")
#py/pack/_packSpherePadder.cpp;py/pack/SpherePadder/SpherePadder.cpp;py/pack/SpherePadder/SpherePackingTriangulation.cpp;

FILE(GLOB SRC_PYGTS "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pygts-0.3.1/*.c")

SET(PY_COMPILED_MODULES mathWrap;gts/_gts;yade/boot;yade/log;yade/wrapper;yade/_customConverters;yade/_eudoxos;yade/_packPredicates;yade/_packSpheres;yade/_packObb;yade/_utils;yade/WeightedAverage2d) # ;yade/_packSpherePadder

COMBINE_SOURCES(${CMAKE_BINARY_DIR}/pyModules "${SRC_PYMODULES}" 1000)
FILE(GLOB SRC_PYMODULES_COMBINED "${CMAKE_BINARY_DIR}/pyModules.*.cpp")

ADD_LIBRARY(pyModules SHARED ${SRC_PYMODULES_COMBINED})
ADD_LIBRARY(_gts SHARED ${SRC_PYGTS})

SET_TARGET_PROPERTIES(_gts PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_gts gts)


CONFIGURE_FILE(config.py.in "${CMAKE_BINARY_DIR}/config.py")
CONFIGURE_FILE(__init__.py.in "${CMAKE_BINARY_DIR}/__init__.py")

INSTALL(FILES 3rd-party/pygts-0.3.1/__init__.py DESTINATION ${YADE_LIB_PATH}/py/gts)
INSTALL(FILES 3rd-party/pygts-0.3.1/pygts.py DESTINATION ${YADE_LIB_PATH}/py/gts)
INSTALL(FILES 3rd-party/mtTkinter-0.3/mtTkinter.py DESTINATION ${YADE_LIB_PATH}/py)

FILE(GLOB filesPY "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
INSTALL(FILES ${filesPY} DESTINATION ${YADE_PY_PATH}/yade)
FILE(GLOB filesPYTests "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py")
INSTALL(FILES ${filesPYTests} DESTINATION ${YADE_PY_PATH}/yade/tests)
INSTALL(TARGETS pyModules DESTINATION ${YADE_LIB_PATH}/lib)
INSTALL(TARGETS _gts DESTINATION ${YADE_LIB_PATH}/py/gts)


