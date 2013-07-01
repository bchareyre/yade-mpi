#==================pygts=========================================
IF(ENABLE_GTS)
  find_python_module(gts)
  IF (PY_gts)
    MESSAGE(STATUS "Use system gts version")
  ELSE (PY_gts)
    MESSAGE(STATUS "Use embedded version of gts. Please, consider installing the corresponding package")
    FILE(GLOB SRC_PYGTS "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pygts-0.3.1/*.c")
    ADD_LIBRARY(_gts SHARED ${SRC_PYGTS})
    SET_TARGET_PROPERTIES(_gts PROPERTIES PREFIX "")
    TARGET_LINK_LIBRARIES(_gts gts)
    INSTALL(FILES 3rd-party/pygts-0.3.1/__init__.py DESTINATION ${YADE_LIB_PATH}/py/gts)
    INSTALL(FILES 3rd-party/pygts-0.3.1/pygts.py DESTINATION ${YADE_LIB_PATH}/py/gts)
    INSTALL(TARGETS _gts DESTINATION ${YADE_LIB_PATH}/py/gts)
  ENDIF (PY_gts)
ENDIF(ENABLE_GTS)
#==================pygts=========================================

#==================pyModules=========================================

FILE(GLOB filesPY "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
SET(filesPY "${filesPY};${CMAKE_CURRENT_SOURCE_DIR}/pack/pack.py")
INSTALL(FILES ${filesPY} DESTINATION ${YADE_PY_PATH}/yade)
INSTALL(FILES 3rd-party/mtTkinter-0.3/mtTkinter.py DESTINATION ${YADE_LIB_PATH}/py)
FILE(GLOB filesPYTests "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py")
INSTALL(FILES ${filesPYTests} DESTINATION ${YADE_PY_PATH}/yade/tests)
FILE(GLOB filesPYPerf "${CMAKE_CURRENT_SOURCE_DIR}/../examples/test/performance/*")
INSTALL(FILES ${filesPYPerf} DESTINATION ${YADE_PY_PATH}/yade/tests/checks/performance)
ADD_LIBRARY(WeightedAverage2d SHARED "${CMAKE_CURRENT_SOURCE_DIR}/WeightedAverage2d.cpp")
SET_TARGET_PROPERTIES(WeightedAverage2d PROPERTIES PREFIX "")
INSTALL(TARGETS WeightedAverage2d DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_eudoxos SHARED "${CMAKE_CURRENT_SOURCE_DIR}/_eudoxos.cpp")
SET_TARGET_PROPERTIES(_eudoxos PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_eudoxos yade)
INSTALL(TARGETS _eudoxos DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_utils SHARED "${CMAKE_CURRENT_SOURCE_DIR}/_utils.cpp")
SET_TARGET_PROPERTIES(_utils PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_utils yade)
INSTALL(TARGETS _utils DESTINATION "${YADE_PY_PATH}/yade/")


ADD_LIBRARY(_packPredicates SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pack/_packPredicates.cpp")
SET_TARGET_PROPERTIES(_packPredicates PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_packPredicates yade)
IF(ENABLE_GTS AND NOT(PY_gts))
  TARGET_LINK_LIBRARIES(_packPredicates _gts)
ENDIF(ENABLE_GTS AND NOT(PY_gts))


INSTALL(TARGETS _packPredicates DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_packSpheres SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pack/_packSpheres.cpp")
SET_TARGET_PROPERTIES(_packSpheres PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_packSpheres yade)
INSTALL(TARGETS _packSpheres DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_packObb SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pack/_packObb.cpp")
SET_TARGET_PROPERTIES(_packObb PROPERTIES PREFIX "")
INSTALL(TARGETS _packObb DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(wrapper SHARED "${CMAKE_CURRENT_SOURCE_DIR}/wrapper/yadeWrapper.cpp")
SET_TARGET_PROPERTIES(wrapper PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(wrapper yade)
INSTALL(TARGETS wrapper DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_customConverters SHARED "${CMAKE_CURRENT_SOURCE_DIR}/wrapper/customConverters.cpp")
SET_TARGET_PROPERTIES(_customConverters PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_customConverters yade)
INSTALL(TARGETS _customConverters DESTINATION "${YADE_PY_PATH}/yade/")
