/********************************************************************
**                                                                 **
** File   : src/JsonReader.h                               **
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

#pragma once
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace fformations {

class JsonReader {
public:
  static boost::property_tree::ptree readFile(const std::string filename);

  template <typename T>
  static T createFromTree(const boost::property_tree::ptree &tree);

  template <typename T>
  std::vector<T> static node_to_vector(
      const boost::property_tree::ptree &tree) {
    std::vector<T> result;
    result.reserve(tree.size());
    for (auto node : tree) {
      result.push_back(node.second.get_value<T>());
    }
    return result;
  }
};

} // namespace fformations
