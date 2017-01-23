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
#include <boost/property_tree/json_parser.hpp>

using fformations::JsonReader;

boost::property_tree::ptree JsonReader::readFile(const std::string filename) {
  boost::property_tree::ptree data;
  try {
    boost::property_tree::read_json(filename, data);
  } catch (const boost::property_tree::json_parser_error &e) {
    throw Exception(std::string("Error while parsing data: ") +
                    std::string(e.what()));
  }
  return data;
}
