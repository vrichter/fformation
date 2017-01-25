/********************************************************************
**                                                                 **
** File   : src/GroundTruth.cpp                                    **
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

#include "GroundTruth.h"
#include "JsonReader.h"

using fformation::GroundTruth;
using fformation::JsonReader;

namespace fformation {

template <>
GroundTruth JsonReader::createFromTree<GroundTruth>(
    const boost::property_tree::ptree &tree) {
  assert(tree.count("GTgroups") == 1);
  assert(tree.count("GTtimestamp") == 1);
  std::vector<std::vector<Group>> classifications;
  std::vector<Timestamp::TimestampType> timestamps;
  for (auto node : tree) {
    if (node.first == "GTgroups") {
      assert(node.second.size() == 1);
      for (auto classification_node : node.second.front().second) {
        for (auto groups_node : classification_node.second) {
          std::vector<Group> classification;
          for (auto group_node : groups_node.second) {
            assert(group_node.second.size() == 1);
            std::vector<Person> group;
            for (auto person_node : group_node.second.front().second) {
              PersonId pid = person_node.second.get_value<PersonId>();
              group.push_back(Person(pid));
            }
            classification.push_back(group);
          }
          classifications.push_back(classification);
        }
      }
    } else if (node.first == "GTtimestamp") {
      timestamps =
          node_to_vector<Timestamp::TimestampType>(node.second.front().second);
    } else {
      assert(false);
    }
  }
  assert(classifications.size() == timestamps.size());
  std::vector<Classification> combined_classifications;
  combined_classifications.reserve(classifications.size());
  for (size_t i = 0; i < classifications.size(); ++i) {
    combined_classifications.push_back(
        Classification(timestamps[i], classifications[i]));
  }
  return GroundTruth(combined_classifications);
}
}

GroundTruth GroundTruth::readMatlabJson(const std::string &filename) {
  return JsonReader::createFromTree<GroundTruth>(
      JsonReader::readFile(filename));
}
