# - Find CuBlas library
# 
# This module defines
#  CUBBLAS_LIBRARY, libraries to link against to use Cublas.
#  CUBLAS_FOUND, If false, do not try to use Cublas.

FIND_LIBRARY(CUBLAS_LIBRARY NAMES cublas libcublas PATHS /usr/lib/x86_64-linux-gnu )
FIND_LIBRARY(CUDART_LIBRARY NAMES cudart libcudart PATHS /usr/lib/x86_64-linux-gnu )

# handle the QUIETLY and REQUIRED arguments and set LOKI_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CuBlas  DEFAULT_MSG  CUBLAS_LIBRARY CUDART_LIBRARY)

MARK_AS_ADVANCED(CUBLAS_LIBRARY CUDART_LIBRARYR)
