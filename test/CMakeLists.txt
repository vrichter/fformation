#*********************************************************************
#**                                                                 **
#** File   : test/CMakeLists.txt                                    **
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

cmake_minimum_required(VERSION 2.8.2)

# build gtest
add_subdirectory(gtest)

# for every cpp file create a test
FILE(GLOB TESTS "${PROJECT_SOURCE_DIR}/test/*.cpp")

foreach(TEST ${TESTS})
  STRING(REGEX REPLACE "/.*/" "" TEST ${TEST})
  STRING(REGEX REPLACE "[.]cpp" "" TEST ${TEST})
  message(STATUS "-- Adding test: ${TEST}")

  add_executable("${PROJECT_NAME}-${TEST}"
    "${PROJECT_SOURCE_DIR}/test/${TEST}.cpp"
  )

target_link_libraries("${PROJECT_NAME}-${TEST}"
    ${PROJECT_NAME}
    gtest_main
  )
add_test("Test::${TEST}" "${PROJECT_NAME}-${TEST}")
endforeach(TEST)
