# Define version or set it from git


IF (NOT VERSION)
  IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RELEASE )
    #Release file is found
    exec_program(
        "less"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "RELEASE"
        OUTPUT_VARIABLE VERSION 
      )
  ELSE (EXISTS(${CMAKE_CURRENT_SOURCE_DIR}/RELEASE))
    IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git/config )
      #Use git for version defining
      exec_program(
        "git"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "describe"
        OUTPUT_VARIABLE VERSION
      )
      string( REGEX MATCH "-g.*$" VERSION_SHA1 ${VERSION} )
    ELSE (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git/config )
      SET (VERSION "Unknown")
    ENDIF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git/config )
  ENDIF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RELEASE)
ENDIF (NOT VERSION)


MESSAGE (STATUS "Version is set to " ${VERSION})
