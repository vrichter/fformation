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
  const Coordinate dot(const Position2D &other);

  double norm() const;

  Position2D normalized() const {
    return *this / norm();
  }

  Position2D perpendicular() const {
    return Position2D(-_y,_x);
  }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"x\": " << _x << ", \"y\": " << _y << " }";
  }

  friend Position2D operator+(const Position2D &a, const Position2D &b) {
    return Position2D(a.x() + b.x(), a.y() + b.y());
  }

  friend Position2D operator-(const Position2D &a, const Position2D &b) {
    return Position2D(a.x() - b.x(), a.y() - b.y());
  }

  friend Position2D operator/(const Position2D &a, const Coordinate &divisor) {
    return Position2D(a.x() / divisor, a.y() / divisor);
  }

  friend Position2D operator*(const Position2D &a, const Coordinate &scale) {
    return Position2D(a.x() * scale, a.y() * scale);
  }

private:
  Coordinate _x;
  Coordinate _y;
};

} // namespace fformation
