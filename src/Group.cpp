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

using fformation::Group;
using fformation::Person;
using fformation::PersonId;
using fformation::Position2D;
using fformation::Settings;

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
