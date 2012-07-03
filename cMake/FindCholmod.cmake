# - Try to find CHOLMOD
# This will define
#
#  CHOLMOD_FOUND            - system has CHOLMOD
#  CHOLMOD_LIBRARIES 	    - Link these to use CHOLMOD_FOUND
#  AMD_LIBRARY	 	    - needed by CHOLMOD
#  COLAMD_LIBRARY 	    - needed by CHOLMOD
#  CCOLAMD_LIBRARY 	    - needed by CHOLMOD
#  CAMD_LIBRARY 	    - needed by CHOLMOD

IF (CHOLMOD_LIBRARIES)
	SET(CHOLMOD_FOUND TRUE)
ELSE(CHOLMOD_LIBRARIES)
  FIND_LIBRARY(CHOLMOD_LIBRARIES NAMES cholmod libcholmod
        PATHS
        /usr/lib
        /usr/local/lib
        /usr/lib/CGAL
        /usr/lib64
        /usr/local/lib64
        /usr/lib64/CGAL
    )
    IF(CHOLMOD_LIBRARIES)
	SET(CHOLMOD_FOUND TRUE)
	FIND_LIBRARY(AMD_LIBRARY NAMES amd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL)
	FIND_LIBRARY(CAMD_LIBRARY NAMES camd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL)
	FIND_LIBRARY(COLAMD_LIBRARY NAMES colamd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL)
	FIND_LIBRARY(CCOLAMD_LIBRARY NAMES ccolamd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL)
    ELSE(CHOLMOD_LIBRARIES)
        SET(CHOLMOD_FOUND FALSE)
        MESSAGE(STATUS "CHOLMOD not found.")
    ENDIF(CHOLMOD_LIBRARIES)
    MARK_AS_ADVANCED(CHOLMOD_LIBRARIES)
ENDIF (CHOLMOD_LIBRARIES)
