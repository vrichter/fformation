#*********************************************************************
#**                                                                 **
#** File   : CMakeLists.txt                                         **
#** Authors: Viktor Richter                                         **
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
#*********************************************************************

cmake_minimum_required(VERSION 3.6)
cmake_policy(SET CMP0048 NEW)

# this sets the projects name
project(fformation VERSION 3.0.1)

# some output
message(STATUS "")
message(STATUS "==============================================================")
message(STATUS "Configuring ${PROJECT_NAME}:")

set_property(GLOBAL PROPERTY CXX_STANDARD 11)

option(BUILD_TEST "Build unit tests" ON)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR "share/${PROJECT_NAME}/" CACHE PATH "Installation directory for CMake files")
set(INSTALL_DOCUMENTATION_DIR "share/${PROJECT_NAME}/doc" CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
set(INSTALL_CMAKE_DIR_REL "${INSTALL_CMAKE_DIR}")
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# boost
find_package(Boost 1.54 COMPONENTS program_options REQUIRED)

message(STATUS "Looking for doxygen")
find_program(DOXYGEN_BIN NAMES doxygen)
if(NOT "${DOXYGEN_BIN}" STREQUAL "DOXYGEN_BIN-NOTFOUND")
  message(STATUS "found Doxygen, a documentation can be built with 'make doc'")
  add_custom_target(doc ALL
    COMMAND ${DOXYGEN_BIN} ${PROJECT_BINARY_DIR}/Doxyfile
    COMMENT "Creating html and tex documentation in doc/"
  )
  install(
    DIRECTORY ${PROJECT_BINARY_DIR}/doc/html
    DESTINATION "${INSTALL_DOCUMENTATION_DIR}/"
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
 )
else()
  message(STATUS "  doxygen not found, documentation will not be available")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_BINARY_DIR}/Doxyfile"
)

# get all header files
FILE(GLOB HEADERS "${PROJECT_SOURCE_DIR}/src/*.h")
# get all sources
FILE(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
# get all apps
FILE(GLOB APPS "${PROJECT_SOURCE_DIR}/app/*.cpp")

# add all headers
include_directories("${PROJECT_SOURCE_DIR}/src/")
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#create library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  PUBLIC_HEADER "${HEADERS}")
target_link_libraries(${PROJECT_NAME})
#install library
install(TARGETS "${PROJECT_NAME}"
  EXPORT  ${PROJECT_NAME}Targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
        ARCHIVE DESTINATION lib/static
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
)

######  creating executables #####
set(APPLICATIONS "")
foreach(APP ${APPS})
  STRING(REGEX REPLACE "/.*/" "" APP ${APP})
  STRING(REGEX REPLACE "[.]cpp" "" APP ${APP})
  message(STATUS "-- Adding executable: ${APP}")

  link_directories(${Boost_LIBRARY_DIRS})

  add_executable("${PROJECT_NAME}-${APP}"
    "${PROJECT_SOURCE_DIR}/app/${APP}.cpp"
  )

  target_link_libraries("${PROJECT_NAME}-${APP}"
    ${PROJECT_NAME}
    ${Boost_LIBRARIES}
  )

  install(TARGETS "${PROJECT_NAME}-${APP}"
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib/static
          PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
         )
  list(APPEND APPLICATIONS "${PROJECT_NAME}-${APP}")
endforeach(APP)

if(${BUILD_TEST})
  message(STATUS "Testing turned on. Add -DBUILD_TEST=OFF to build without unit tests.")
  enable_testing()
  add_subdirectory(test)
else()
  message(STATUS "Testing turned off. Add -DBUILD_TEST=ON to build with unit tests.")
endif()

##### setup cmake config #####
# Project name in caps
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
# Add all targets to the build-tree export set
export(TARGETS ${PROJECT_NAME} ${APPLICATIONS}
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

# Create the Config.cmake and ConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${FFORMATION_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake/Config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake" @ONLY)
# ... for both
configure_file(cmake/ConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

# Install the Config.cmake and ConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ${PROJECT_NAME}Targets DESTINATION
  "${INSTALL_CMAKE_DIR_REL}" COMPONENT dev)

