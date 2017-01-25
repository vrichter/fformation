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
#include <boost/property_tree/json_parser.hpp>

using fformation::Settings;
using fformation::JsonReader;

namespace fformation {
template <>
Settings JsonReader::createFromTree(const boost::property_tree::ptree &tree) {
  auto node = tree.get_child("params...");
  assert(node.size() == 5);
  std::array<double, 9> cov;
  size_t cov_pos = 0;
  auto cov_row = node.begin()->second.begin();
  for (auto row : node.begin()->second) {
    for (auto element : row.second) {
      cov.at(cov_pos++) = element.second.get_value<double>();
    }
  }
  return Settings(
      cov,
      std::next(node.begin(), 1)->second.get_child("..").get_value<double>(),
      std::next(node.begin(), 2)->second.get_child("..").get_value<double>(),
      std::next(node.begin(), 3)->second.get_child("..").get_value<size_t>(),
      std::next(node.begin(), 4)->second.get_child("..").get_value<double>());
}
} // namespace fformation

Settings Settings::readMatlabJson(const std::string &filename_settings,
                                 const std::string &filename_settings_gc)
{
  auto settings_tree = JsonReader::readFile(filename_settings);
  auto settings_gc_tree = JsonReader::readFile(filename_settings_gc);
  Settings result = JsonReader::createFromTree<Settings>(settings_tree);
  assert(settings_gc_tree.size() == 2);
  result.mdl(settings_gc_tree.get_child("mdl...").get_value<double>());
  result.stride(settings_gc_tree.get_child("stride...").get_value<double>());
  return  result;
}
