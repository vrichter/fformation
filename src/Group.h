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
#include <set>
#include <vector>

namespace fformation {

class Group : public JsonSerializable {
public:
  Group(
      const std::map<PersonId, Person> &persons = std::map<PersonId, Person>());
  Group(const std::vector<Person> &persons);

  Position2D calculateCenter(Person::Stride stride) const;
  static Position2D calculateCenter(const std::vector<Person> &persons,
                                    Person::Stride stride);

  const std::map<PersonId, Person> &persons() const { return _persons; }

  std::map<PersonId, Person> find_persons(std::set<PersonId> person_ids) const;

  std::vector<Person> generatePersonList() const;

  /**
   * @brief calculateDistanceCosts calculates the sum of the individual distance
   * costs of group members.
   *
   * @param stride the distance btw. a persons Position and its transactional
   * segment.
   * @return sum of persons costs relative to the probable center of this group,
   *         0 if group is empty or only one person in group.
   */
  double calculateDistanceCosts(Person::Stride stride) const;

  virtual void serializeJson(std::ostream &out) const override;

private:
  std::map<PersonId, Person> _persons;
};

class IdGroup : public JsonSerializable {
public:
  IdGroup(const std::set<PersonId> &persons) : _persons(persons){};

  const std::set<PersonId> &persons() const { return _persons; }

  virtual void serializeJson(std::ostream &out) const override {
    serializeIterable(out, _persons);
  }

private:
  std::set<PersonId> _persons;
};

} // namespace fformation
