#*********************************************************************
#**                                                                 **
#** File   : cmake/FindDependency.cmake                             **
#** Authors: Sergius Gaulik, Viktor Richter                         **
#**                                                                 **
#**                                                                 **
#** GNU LESSER GENERAL PUBLIC LICENSE                               **
#** This file may be used under the terms of the GNU Lesser General **
#** Public License version 3.0 as published by the                  **
#**                                                                 **
#** Free Software Foundation and appearing in the file LICENSE.LGPL **
#** included in the packaging of this file.  Please review the      **
#** following information to ensure the license requirements will   **
#** be met: http://www.gnu.org/licenses/lgpl-3.0.txt                **
#**                                                                 **
#** The major part of this file is taken frim the FindPackage macro **
#** definition of the ICL www.iclcv.org                             **
#**                                                                 **
#*********************************************************************

# Input:
#   NAME                 - name of the package
#   HEADERS              - will try to find include directory for these headers
#   LIBS                 - will try to find these libraries
#   PATHS                - these paths will be used to find the include directory and the libraries
#   HEADER_PATH_SUFFIXES - path suffixes will be used for the include directory search
#   LIB_PATH_SUFFIXES    - path suffixex will be used for the libraries search
#   OPTIONAL             - if set the package is optional and there will be no error message
# Output:
#   NAME_FOUND           - shows if the package was found
#   HAVE_NAME            - shows if the package was found too
#   NAME_INCLUDE_DIRS    - include directory
#   NAME_LIBRARIES       - all found libraries
#   HAVE_NAME            - this variable is added to the icl definitions
# Global input:
#  PKG_SEARCH_PATHS         - contains directories, which should be searched
#  ARCH_DEPENDENT_LIB_PATHS - suffixes should be used for the libraries search
include(CMakeParseArguments)
MACRO(FIND_DEPENDENCY)

  # Get all arguments
  SET(options OPTIONAL)
  SET(oneValueArgs NAME)
  SET(multiValueArgs HEADERS LIBS PATHS HEADER_PATH_SUFFIXES LIB_PATH_SUFFIXES)
  CMAKE_PARSE_ARGUMENTS(_PKG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  STRING(TOUPPER ${_PKG_NAME} _PKG_NAME_UPPER)

  SET(${_PKG_NAME_UPPER}_FOUND TRUE)

  # Set all search paths
  SET(_PKG_SEARCH_PATHS "")
  IF(${_PKG_NAME_UPPER}_ROOT)
    LIST(APPEND _PKG_SEARCH_PATHS ${${_PKG_NAME_UPPER}_ROOT})
  ENDIF()
  FOREACH(_PATH ${PKG_SEARCH_PATHS})
    LIST(APPEND _PKG_SEARCH_PATHS ${_PATH})
  ENDFOREACH()
  FOREACH(_PATH ${_PKG_PATHS})
    LIST(APPEND _PKG_SEARCH_PATHS ${_PATH})
  ENDFOREACH()
  SET(_PKG_MISSING "")

  # Search for the include directory
  FIND_PATH(${_PKG_NAME_UPPER}_INCLUDE_DIR
    NAMES ${_PKG_HEADERS}
    PATHS ${_PKG_SEARCH_PATHS}
    PATH_SUFFIXES "include" ${_PKG_HEADER_PATH_SUFFIXES}
    DOC "The path to ${_PKG_NAME} header files"
    )

  IF(NOT ${_PKG_NAME_UPPER}_INCLUDE_DIR)
    IF(NOT ${_PKG_HEADERS} STREQUAL "")
      LIST(APPEND _PKG_MISSING ${_PKG_NAME_UPPER}_INCLUDE_DIR)
    ENDIF()
  ELSE()
    SET(${_PKG_NAME_UPPER}_INCLUDE_DIRS ${${_PKG_NAME_UPPER}_INCLUDE_DIR})
  ENDIF()

  # Search for all listed libraries
  FOREACH(_LIB ${_PKG_LIBS})
    FIND_LIBRARY(${_LIB}_LIBRARY
      NAMES ${_LIB}
      PATHS ${_PKG_SEARCH_PATHS}
      PATH_SUFFIXES "lib" ${ARCH_DEPENDENT_LIB_PATHS} ${_PKG_LIB_PATH_SUFFIXES}
      )

    IF(NOT ${_LIB}_LIBRARY)
      LIST(APPEND _PKG_MISSING ${_LIB}_LIBRARY)
    ELSE()
      LIST(APPEND ${_PKG_NAME_UPPER}_LIBRARIES ${${_LIB}_LIBRARY})
    ENDIF()
  ENDFOREACH()

  IF(_PKG_MISSING)
    SET(${_PKG_NAME_UPPER}_FOUND FALSE)
    IF(_PKG_OPTIONAL)
      MESSAGE(STATUS "Could NOT find ${_PKG_NAME} (optional | missing: ${_PKG_MISSING})")
    ELSE(_PKG_OPTIONAL)
      MESSAGE(SEND_ERROR "Could NOT find ${_PKG_NAME} (missing: ${_PKG_MISSING})")
    ENDIF(_PKG_OPTIONAL)
    # Ask the root directory of the package
    SET(${_PKG_NAME_UPPER}_ROOT ${${_PKG_NAME_UPPER}_ROOT} CACHE PATH "Root directory of ${_PKG_NAME}")
  ELSE()
    ADD_DEFINITIONS(-DHAVE_${_PKG_NAME_UPPER})
    MESSAGE(STATUS "Found ${_PKG_NAME}: ${${_PKG_NAME_UPPER}_INCLUDE_DIRS} ${${_PKG_NAME_UPPER}_LIBRARIES}")
  ENDIF(_PKG_MISSING)

  SET(HAVE_${_PKG_NAME_UPPER} ${${_PKG_NAME_UPPER}_FOUND})

ENDMACRO()
