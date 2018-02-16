# - Try to find CHOLMOD
# This will define
#
#  CHOLMOD_FOUND          - system has CHOLMOD
#  CHOLMOD_LIBRARIES 	    - library to link against to use Cholmod 
#  CHOLMOD_INCLUDE_DIR    - where to find cholmod.h, etc.
#  AMD_LIBRARY	 	        - needed by CHOLMOD
#  COLAMD_LIBRARY 	      - needed by CHOLMOD
#  CCOLAMD_LIBRARY 	      - needed by CHOLMOD
#  CAMD_LIBRARY 	        - needed by CHOLMOD


FIND_LIBRARY(CHOLMOD_LIBRARIES NAMES cholmod libcholmod
        PATHS
        /usr/lib
        /usr/local/lib
        /usr/lib/CGAL
        /usr/lib64
        /usr/local/lib64
        /usr/lib64/CGAL
	/usr/local/SuiteSparse/lib
    )

FIND_LIBRARY(AMD_LIBRARY NAMES amd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL /usr/local/SuiteSparse/lib)
FIND_LIBRARY(CAMD_LIBRARY NAMES camd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL /usr/local/SuiteSparse/lib)
FIND_LIBRARY(COLAMD_LIBRARY NAMES colamd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL /usr/local/SuiteSparse/lib)
FIND_LIBRARY(CCOLAMD_LIBRARY NAMES ccolamd PATHS /usr/lib /usr/local/lib /usr/lib/CGAL /usr/lib64 /usr/local/lib64 /usr/lib64/CGAL /usr/local/SuiteSparse/lib)

FIND_PATH(CHOLMOD_INCLUDE_DIR cholmod.h PATH /usr/include /usr/include/suitesparse /usr/local/SuiteSparse/include)

# Check the suitesparse library version and set #ifdefs accordingly
if (CHOLMOD_INCLUDE_DIR AND CHOLMOD_LIBRARIES)
  # SuiteSparse version >= 4.
  set(SUITESPARSE_VERSION_FILE
    ${CHOLMOD_INCLUDE_DIR}/SuiteSparse_config.h)
  if (NOT EXISTS ${SUITESPARSE_VERSION_FILE})
	MESSAGE(STATUS "Could not find file ${SUITESPARSE_VERSION_FILE} containing version information for SuiteSparse version >=4. Direct CHOLMOD solver for CPU will be deactivated.")
  else (NOT EXISTS ${SUITESPARSE_VERSION_FILE})
    file(READ ${SUITESPARSE_VERSION_FILE} SUITESPARSE_CONFIG_CONTENTS)

    string(REGEX MATCH "#define SUITESPARSE_MAIN_VERSION [0-9]+"
      SUITESPARSE_MAIN_VERSION "${SUITESPARSE_CONFIG_CONTENTS}")
    string(REGEX REPLACE "#define SUITESPARSE_MAIN_VERSION ([0-9]+)" "\\1"
      SUITESPARSE_MAIN_VERSION "${SUITESPARSE_MAIN_VERSION}")

    string(REGEX MATCH "#define SUITESPARSE_SUB_VERSION [0-9]+"
      SUITESPARSE_SUB_VERSION "${SUITESPARSE_CONFIG_CONTENTS}")
    string(REGEX REPLACE "#define SUITESPARSE_SUB_VERSION ([0-9]+)" "\\1"
      SUITESPARSE_SUB_VERSION "${SUITESPARSE_SUB_VERSION}")

    string(REGEX MATCH "#define SUITESPARSE_SUBSUB_VERSION [0-9]+"
      SUITESPARSE_SUBSUB_VERSION "${SUITESPARSE_CONFIG_CONTENTS}")
    string(REGEX REPLACE "#define SUITESPARSE_SUBSUB_VERSION ([0-9]+)" "\\1"
      SUITESPARSE_SUBSUB_VERSION "${SUITESPARSE_SUBSUB_VERSION}")

    set(SUITESPARSE_VERSION
      "${SUITESPARSE_MAIN_VERSION}.${SUITESPARSE_SUB_VERSION}.${SUITESPARSE_SUBSUB_VERSION}")
	MESSAGE(STATUS "SuiteSparse version ${SUITESPARSE_VERSION} found, CHOLMOD direct solver for CPU activated.")
  endif (NOT EXISTS ${SUITESPARSE_VERSION_FILE})
endif (CHOLMOD_INCLUDE_DIR AND CHOLMOD_LIBRARIES)

if ((SUITESPARSE_MAIN_VERSION GREATER 4) OR (SUITESPARSE_MAIN_VERSION EQUAL 4))
	ADD_DEFINITIONS("-DSUITESPARSE_VERSION_4")
endif ((SUITESPARSE_MAIN_VERSION GREATER 4) OR (SUITESPARSE_MAIN_VERSION EQUAL 4))


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Cholmod DEFAULT_MSG CHOLMOD_LIBRARIES CHOLMOD_INCLUDE_DIR AMD_LIBRARY CAMD_LIBRARY COLAMD_LIBRARY CCOLAMD_LIBRARY)
MARK_AS_ADVANCED(CHOLMOD_LIBRARIES CHOLMOD_INCLUDE_DIR AMD_LIBRARY CAMD_LIBRARY COLAMD_LIBRARY CCOLAMD_LIBRARY)
