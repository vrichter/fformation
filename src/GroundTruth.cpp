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
#include <set>

using fformation::GroundTruth;
using fformation::JsonReader;
using fformation::Timestamp;
using fformation::Json;
using fformation::Exception;
using fformation::Group;
using fformation::IdGroup;
using fformation::Person;
using fformation::PersonId;

static std::vector<IdGroup> readGroups(const Json &js) {
  std::vector<IdGroup> result;
  if (js.empty()) { // empty classifications may be null
    return result;
  }
  Exception::check(js.is_array(),
                   "Array of arrays expected. Got: " + js.dump());
  for (auto group : js) {
    Exception::check(group.is_array(), "Array expected. Got: " + group.dump());
    std::set<PersonId> persons;
    for (auto pid : group) {
      double val = pid.get<double>();
      std::stringstream str;
      str << val;
      persons.insert(PersonId(str.str()));
    }
    result.push_back(persons);
  }
  return result;
}

static std::vector<std::vector<IdGroup>> readClassifications(const Json &js) {
  std::vector<std::vector<IdGroup>> result;
  auto it = js.find("GTgroups");
  Exception::check(it != js.end(), "GTgroups not found. Got: " + js.dump());
  Exception::check(it.value().size(),
                   "GTgroups must not be empty. Got: " + js.dump());
  for (auto classification : it.value()) {
    result.push_back(readGroups(classification));
  }
  return result;
}

static std::vector<Timestamp> readTimestamps(const Json &js) {
  std::vector<Timestamp> result;
  auto it = js.find("GTtimestamp");
  if (it != js.end()) {
    Exception::check(it.value().is_array(),
                     "GTtimestamp must be an array. Got: " + js.dump());
    result.reserve(it.value().size());
    for (auto ts : it.value()) {
      result.push_back(Timestamp((Timestamp::TimestampType)ts));
    }
  }
  return result;
}

GroundTruth GroundTruth::readMatlabJson(const std::string &filename) {
  Json js = JsonReader::readFile(filename);
  std::vector<Timestamp> timestamps = readTimestamps(js);
  std::vector<std::vector<IdGroup>> groups = readClassifications(js);
  std::vector<Classification> classifications;
  Exception::check(groups.size() == timestamps.size(),
                   "Groups and Timestamps must have the same size.");
  classifications.reserve(timestamps.size());
  for (size_t i = 0; i < timestamps.size(); ++i) {
    classifications.push_back(Classification(timestamps[i], groups[i]));
  }
  return GroundTruth(classifications);
}

void GroundTruth::serializeJson(std::ostream &out) const {
  serializeIterable(out, _classifications);
}
