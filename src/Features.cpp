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
#include "Exception.h"
#include "JsonReader.h"

using fformation::Features;
using fformation::JsonReader;
using fformation::FoV;
using fformation::Json;
using fformation::Observation;
using fformation::Group;
using fformation::Person;
using fformation::Timestamp;
using fformation::Exception;

static FoV readFov(const Json &js) {
  auto it = js.find("FoV");
  if (it != js.end()) {
    return FoV(it.value().at(0), it.value().at(1), it.value().at(2),
               it.value().at(3));
  } else {
    return FoV();
  }
}

static Person readPerson(const Json &js) {
  Exception::check(js.is_array(),
                   "Person data must be an array. Got: " + js.dump());
  Exception::check(js.size() >= 3,
                   "Person data must be of size >= 3. Got: " + js.dump());
  Exception::check(js.size() <= 4,
                   "Person data must be of size <= 4. Got: " + js.dump());
  std::string id = "";
  if (js[0].is_number()) {
    std::stringstream ids;
    ids << js[0];
    id = ids.str();
  } else if (js[0].is_string()) {
    id = js[0].get<std::string>();
  }
  auto x = fformation::Position2D::Coordinate(js[1]);
  auto y = fformation::Position2D::Coordinate(js[2]);
  auto r = fformation::OptionalRotationRadian();
  if (js.size() == 4) {
    r = fformation::OptionalRotationRadian(js[3]);
  }
  return Person(id, {{x, y}, r});
}

static Group readGroup(const Json &js) {
  if (!js.is_array() || js.empty()) {
    // no information about a group -> empty
    return Group();
  }
  std::vector<Person> persons;
  if (js.front().is_primitive()) {
    // only one person
    persons.push_back(readPerson(js));
  } else {
    // array of persons
    persons.reserve(js.size());
    for (auto it : js) {
      persons.push_back(readPerson(it));
    }
  }
  return Group(persons);
}

static std::vector<Group> readFeatures(const Json &js) {
  std::vector<Group> result;
  auto it = js.find("features");
  if (it != js.end()) {
    result.reserve(it.value().size());
    for (auto i : it.value()) {
      result.push_back(readGroup(i));
    }
  }
  return result;
}

static std::vector<Timestamp> readTimestamps(const Json &js) {
  std::vector<Timestamp> result;
  auto it = js.find("timestamp");
  if (it != js.end()) {
    Exception::check(it.value().is_array(),
                     "timestamp must be an array. Got: " + js.dump());
    result.reserve(it.value().size());
    for (auto ts : it.value()) {
      result.push_back(Timestamp((Timestamp::TimestampType)ts));
    }
  }
  return result;
}

Features Features::readMatlabJson(const std::string &filename) {
  Json js = JsonReader::readFile(filename);
  std::vector<Timestamp> timestamps = readTimestamps(js);
  std::vector<Group> features = readFeatures(js);
  std::vector<Observation> observations;
  observations.reserve(features.size());
  Exception::check(timestamps.size() == features.size(),
                   "Timestamps and Features must have the same size.");
  for (size_t i = 0; i < features.size(); ++i) {
    observations.push_back(Observation(timestamps[i], features[i]));
  }
  return Features(observations, readFov(js));
}

void Features::serializeJson(std::ostream &out) const {
  out << "{ \"fov\": " << _fov << ", \"observations\": ";
  serializeIterable(out, _observations);
  out << " }";
}
