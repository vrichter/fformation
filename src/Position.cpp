/********************************************************************
**                                                                 **
** File   : src/Position.cpp                                       **
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

#include "Position.h"
#include "cmath"
#include <algorithm>

using fformation::Position2D;

const Position2D::Coordinate Position2D::dot(const Position2D &other) {
  return (_x * other._x) + (_y * other._y);
}

double Position2D::norm() const {
  return std::sqrt(std::pow(_x, 2) + std::pow(_y, 2));
}
