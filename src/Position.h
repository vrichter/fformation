/********************************************************************
**                                                                 **
** File   : src/Position.h                                         **
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
#include <ostream>

namespace fformation {

class Position2D : public JsonSerializable {
public:
  typedef double Coordinate;

  Position2D(const Coordinate x, const Coordinate y) : _x(x), _y(y) {}

  const Coordinate &x() const { return _x; }
  const Coordinate &y() const { return _y; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"x\": " << _x << ", \"y\": " << _y << " }";
  }

private:
  const Coordinate _x;
  const Coordinate _y;
};

} // namespace fformation
