/********************************************************************
**                                                                 **
** File   : src/Group.cpp                                          **
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

#include "Group.h"
#include "Exception.h"
#include <sstream>
#include <cstring>
#include <cmath>

using fformation::Group;
using fformation::Person;
using fformation::PersonId;
using fformation::Position2D;
using fformation::Settings;
using fformation::Exception;

static std::map<PersonId, Person>
vector_to_map(const std::vector<Person> &persons) {
  std::map<PersonId, Person> result;
  for (auto p : persons) {
    result.insert(std::pair<PersonId, Person>(p.id(), p));
  }
  return result;
}

Group::Group(const std::vector<Person> &persons)
    : _persons(vector_to_map(persons)) {}

Group::Group(const std::map<PersonId,Person> &persons)
    : _persons(persons) {}

void Group::serializeJson(std::ostream &out) const {
  serializeMapAsVector(out, _persons);
}

Position2D Group::calculateCenter(Position2D::Coordinate stride) const {
  Position2D::Coordinate x = 0.;
  Position2D::Coordinate y = 0.;
  for (auto person : _persons) {
    Position2D ts = person.second.calculateTransactionalSegmentPosition(stride);
    x += ts.x();
    y += ts.y();
  }
  return Position2D(x / _persons.size(), y / _persons.size());
}

std::map<PersonId, Person> Group::find_persons(std::set<PersonId> person_ids) const {
  std::map<PersonId,Person> result;
  for(auto id : person_ids){
    auto it = _persons.find(id);
    if(it == _persons.end()){
      std::stringstream str;
      str << "Person with id" << id << " could not be found";
      throw Exception(str.str());
    }
    result.insert( {id, _persons.find(id)->second } );
  }
  return result;
}

double Group::calculateDistanceCosts(Position2D::Coordinate stride) const {
  if(_persons.size() < 2) return 0.; // empty groups and groups of 1 person have zero costs.
  double cost = 0.;
  Position2D group_center = calculateCenter(stride);
  for (auto group_entry : _persons) {
    Position2D person_ts = group_entry.second.calculateTransactionalSegmentPosition(stride);
    // square distance between the group center and the persons
    // transactional segments centre
    cost += std::pow(group_center.x() - person_ts.x(), 2) +
            std::pow(group_center.y() - person_ts.y(), 2);
  }
  return cost;
}
