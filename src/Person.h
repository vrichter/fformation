/********************************************************************
**                                                                 **
** File   : src/Person.h                                           **
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
#include "PersonId.h"
#include "Pose.h"
#include "Settings.h"

namespace fformation {

class Person : public JsonSerializable {
public:
  Person(PersonId id, Pose2D pose) : _id(id), _pose(pose) {}

  const PersonId &id() const { return _id; }
  const Pose2D &pose() const { return _pose; }

  Position2D
  calculateTransactionalSegmentPosition(Position2D::Coordinate stride) const;

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"id\": " << _id;
    out << ", \"pose\": " << _pose;
    out << " }";
  }

private:
  const PersonId _id;
  const Pose2D _pose;
};

} // namespace fformation
