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

static std::map<PersonId, Person>
vector_to_map(const std::vector<Person> &person) {
  std::map<PersonId, Person> result;
  for (auto p : person) {
    result.insert(std::pair<PersonId, Person>(p.id(), p));
  }
  return result;
}

Group::Group(const std::vector<Person> &person)
    : _person(vector_to_map(person)) {}

void Group::serializeJson(std::ostream &out) const {
  serializeMapAsVector(out, _person);
}
