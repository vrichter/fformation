/********************************************************************
**                                                                 **
** File   : src/Person.cpp                                         **
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

#include "Person.h"
#include <cmath>

using fformation::Person;
using fformation::Position2D;
using fformation::Settings;

Position2D Person::calculateTransactionalSegmentPosition(
    Position2D::Coordinate stride) const {
  return Position2D(
      _pose.position().x() + (stride * std::cos(_pose.rotation())),
      _pose.position().y() + (stride * std::sin(_pose.rotation())));
}
