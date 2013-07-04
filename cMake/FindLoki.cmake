# - Find Loki library
# 
# This module defines
#  LOKI_INCLUDE_DIR, where to find loki/Typelist.h, etc.
#  LOKI_LIBRARY, libraries to link against to use GL2PS.
#  LOKI_FOUND, If false, do not try to use GL2PS.

FIND_PATH(LOKI_INCLUDE_DIR loki/Typelist.h)
FIND_LIBRARY(LOKI_LIBRARY NAMES loki )

# handle the QUIETLY and REQUIRED arguments and set LOKI_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Loki  DEFAULT_MSG  LOKI_INCLUDE_DIR LOKI_LIBRARY)

MARK_AS_ADVANCED(LOKI_INCLUDE_DIR LOKI_LIBRARY)
