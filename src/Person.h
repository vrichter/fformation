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
#include "Pose.h"
#include <boost/optional.hpp>

namespace fformations {

typedef std::string PersonId;

class Person : public JsonSerializable {
public:
  typedef boost::optional<Pose2D> OptionalPose2D;

  Person(PersonId id, OptionalPose2D pose = OptionalPose2D())
      : _id(id), _pose(pose) {}

  const PersonId &id() const { return _id; }
  const OptionalPose2D &pose() const { return _pose; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"id\": " << _id;
    if (_pose.is_initialized()) {
      out << ", \"pose\": " << _pose.get();
    }
    out << " }";
  }

private:
  const PersonId _id;
  const OptionalPose2D _pose;
};

} // namespace fformations
