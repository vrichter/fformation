/********************************************************************
**                                                                 **
** File   : src/Settings.cpp                                       **
** Authors: Viktor Richter                                         **
**                                                                 **
**                                                                 **
** GNU LESSER GENERAL PUBLIC LICENSE                               **
** This file may be used under the terms of the GNU Lesser General **
** Public License version 3.0 as published by the                  **
**                                                                 **
** Free Software Foundation and appearing in the file LICENSE.LGPL **
** included in the packaging of this file.  Please review the      **
** following information to ensure the license requirements will   **
** be met: http://www.gnu.org/licenses/lgpl-3.0.txt                **
**                                                                 **
********************************************************************/

#include "Settings.h"
#include "JsonReader.h"

using fformation::Settings;
using fformation::JsonReader;
using fformation::Json;
using fformation::Exception;

Settings::Matrix3D readMatrix(const Json &js) {
  auto value = js.at("params").at("covmat");
  Exception::check(value.is_array(),
                   "Expected array of arrays. Got: " + value.dump());
  Exception::check(value.size() == 3,
                   "Expected size = 3. Got: " + value.dump());
  Settings::Matrix3D result;
  size_t matrix_position = 0;
  for (auto row : value) {
    Exception::check(row.is_array(), "Expected array. Got: " + row.dump());
    Exception::check(row.size() == 3, "Expected size = 3. Got: " + row.dump());
    for (auto column : row) {
      result[matrix_position++] = column;
    }
  }
  return result;
}

Settings Settings::readMatlabJson(const std::string &filename) {
  Json js = JsonReader::readFile(filename);
  return Settings(readMatrix(js), js.at("params").at("empty"),
                  js.at("params").at("radius"), js.at("params").at("nsamples"),
                  js.at("params").at("quant"), js.at("mdl"), js.at("stride"));
}
