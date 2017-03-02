/********************************************************************
**                                                                 **
** File   : src/Pose.h                                             **
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
#include "Position.h"
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

namespace fformation {

typedef double RotationRadian;
typedef boost::optional<RotationRadian> OptionalRotationRadian;

class Pose2D : public JsonSerializable {
public:
  Pose2D(const Position2D &position = Position2D(0., 0.),
         OptionalRotationRadian rotation_radian = 0.)
      : _position(position), _rotation_radian(rotation_radian) {}

  const Position2D &position() const { return _position; }
  const OptionalRotationRadian &rotation() const { return _rotation_radian; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"position\": ";
    _position.serializeJson(out);
    out << ", \"rotation_radian\": " << _rotation_radian << " }";
  }

private:
  Position2D _position;
  OptionalRotationRadian _rotation_radian;
};

} // namespace fformation
