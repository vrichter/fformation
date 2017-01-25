/********************************************************************
**                                                                 **
** File   : src/Group.h                                            **
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
#include "JsonSerializable.h"
#include "Person.h"
#include "Position.h"
#include "Settings.h"
#include <map>
#include <vector>

namespace fformation {

class Group : public JsonSerializable {
public:
  Group(const std::vector<Person> &person = std::vector<Person>());

  // Position2D calculateCenter(const std::vector<Person> known_persons,
  //                           const Settings &settings) {
  //  Position2D center(0., 0.);
  //  for (auto person : known_persons) {
  //    if (_person.find(person._id) != _person.end()) {
  //      Position2D ts = person.transactionalSegment(settings);
  //      center._x += ts;
  //    } else {
  //      throw std::exception()
  //    }
  //  }
  //}

  const std::map<PersonId, Person> &persons() const { return _person; }

  virtual void serializeJson(std::ostream &out) const override;

private:
  const std::map<PersonId, Person> _person;
};

} // namespace fformation
