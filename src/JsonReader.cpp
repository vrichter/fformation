/********************************************************************
**                                                                 **
** File   : src/JsonReader.cpp                                     **
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

#include "JsonReader.h"
#include "Exception.h"
#include <fstream>

using fformation::JsonReader;
using fformation::Json;

Json JsonReader::readFile(const std::string filename) {
  std::fstream fs;
  Json result;
  fs.open(filename.c_str(), std::ios_base::in);
  if (fs.is_open()) {
    fs >> result;
  }
  return result;
}
