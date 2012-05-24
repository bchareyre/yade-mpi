#==================pyModules=========================================
ADD_LIBRARY(WeightedAverage2d SHARED "${CMAKE_CURRENT_SOURCE_DIR}/WeightedAverage2d.cpp")
SET_TARGET_PROPERTIES(WeightedAverage2d PROPERTIES PREFIX "")
INSTALL(TARGETS WeightedAverage2d DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_eudoxos SHARED "${CMAKE_CURRENT_SOURCE_DIR}/_eudoxos.cpp")
SET_TARGET_PROPERTIES(_eudoxos PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_eudoxos plugins support core ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
INSTALL(TARGETS _eudoxos DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_utils SHARED "${CMAKE_CURRENT_SOURCE_DIR}/_utils.cpp")
SET_TARGET_PROPERTIES(_utils PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_utils plugins support core ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
INSTALL(TARGETS _utils DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_packPredicates SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pack/_packPredicates.cpp")
SET_TARGET_PROPERTIES(_packPredicates PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_packPredicates plugins support core _gts ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
INSTALL(TARGETS _packPredicates DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_packSpheres SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pack/_packSpheres.cpp")
SET_TARGET_PROPERTIES(_packSpheres PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_packSpheres plugins support core ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
INSTALL(TARGETS _packSpheres DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_packObb SHARED "${CMAKE_CURRENT_SOURCE_DIR}/pack/_packObb.cpp")
SET_TARGET_PROPERTIES(_packObb PROPERTIES PREFIX "")
INSTALL(TARGETS _packObb DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(wrapper SHARED "${CMAKE_CURRENT_SOURCE_DIR}/wrapper/yadeWrapper.cpp")
SET_TARGET_PROPERTIES(wrapper PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(wrapper plugins support ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
INSTALL(TARGETS wrapper DESTINATION "${YADE_PY_PATH}/yade/")

ADD_LIBRARY(_customConverters SHARED "${CMAKE_CURRENT_SOURCE_DIR}/wrapper/customConverters.cpp")
SET_TARGET_PROPERTIES(_customConverters PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_customConverters plugins support ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
INSTALL(TARGETS _customConverters DESTINATION "${YADE_PY_PATH}/yade/")



#==================pyModules=========================================

#==================pygts=========================================
FILE(GLOB SRC_PYGTS "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pygts-0.3.1/*.c")
ADD_LIBRARY(_gts SHARED ${SRC_PYGTS})
SET_TARGET_PROPERTIES(_gts PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(_gts gts)
INSTALL(FILES 3rd-party/pygts-0.3.1/__init__.py DESTINATION ${YADE_LIB_PATH}/py/gts)
INSTALL(FILES 3rd-party/pygts-0.3.1/pygts.py DESTINATION ${YADE_LIB_PATH}/py/gts)
INSTALL(TARGETS _gts DESTINATION ${YADE_LIB_PATH}/py/gts)
#==================pygts=========================================


CONFIGURE_FILE(config.py.in "${CMAKE_BINARY_DIR}/config.py")
CONFIGURE_FILE(__init__.py.in "${CMAKE_BINARY_DIR}/__init__.py")

FILE(GLOB filesPY "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
SET(filesPY "${filesPY};${CMAKE_CURRENT_SOURCE_DIR}/pack/pack.py")
INSTALL(FILES ${filesPY} DESTINATION ${YADE_PY_PATH}/yade)
INSTALL(FILES 3rd-party/mtTkinter-0.3/mtTkinter.py DESTINATION ${YADE_LIB_PATH}/py)
FILE(GLOB filesPYTests "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py")
INSTALL(FILES ${filesPYTests} DESTINATION ${YADE_PY_PATH}/yade/tests)




