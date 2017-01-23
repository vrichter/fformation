/********************************************************************
**                                                                 **
** File   : src/Features.cpp                                       **
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

#include "Features.h"
#include "JsonReader.h"

using fformations::Features;
using fformations::JsonReader;

namespace fformations {
template <>
Features JsonReader::createFromTree(const boost::property_tree::ptree &tree) {
  assert(tree.count("features") == 1);
  assert(tree.count("timestamp") == 1);
  std::vector<std::vector<Person>> observations;
  std::vector<double> timestamps;
  std::array<double, 4> fov{{0., 0., 0., 0.}};
  for (auto node : tree) {
    if (node.first == "features") {
      assert(node.second.size() == 1);
      for (auto observation : node.second.front().second) {
        std::vector<Person> persons;
        for (auto person : observation.second) {
          assert(person.second.size() == 4);
          std::stringstream idstream;
          auto data = node_to_vector<double>(person.second);
          idstream << data[0];
          Person p(PersonId(idstream.str()),
                   Pose2D({data[1], data[2]}, data[3]));
          persons.push_back(p);
        }
        observations.push_back(persons);
      }
    } else if (node.first == "timestamp") {
      timestamps = node_to_vector<double>(node.second.front().second);
    } else if (node.first == "FoV") {
      assert(node.second.size() == 1);
      assert(node.second.front().second.size() == 4);
      auto data = node_to_vector<double>(node.second.front().second);
      fov = {{data[0], data[1], data[2], data[3]}};
    } else {
      assert(false);
    }
  }
  assert(observations.size() == timestamps.size());
  std::vector<Observation> combined_observations;
  combined_observations.reserve(observations.size());
  for (size_t i = 0; i < observations.size(); ++i) {
    combined_observations.push_back(
        Observation(timestamps[i], observations[i]));
  }
  return Features(combined_observations, FoV{fov[0], fov[1], fov[2], fov[3]});
}
}

Features Features::readMatlabJson(const std::string &filename) {
  return JsonReader::createFromTree<Features>(JsonReader::readFile(filename));
}
