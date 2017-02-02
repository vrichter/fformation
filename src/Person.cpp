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
#include "Exception.h"
#include <cmath>

using fformation::Person;
using fformation::Position2D;
using fformation::Settings;
using fformation::RotationRadian;

Position2D Person::calculateTransactionalSegmentPosition(Stride stride) const {
  return Position2D(
      _pose.position().x() + (stride * std::cos(_pose.rotation())),
      _pose.position().y() + (stride * std::sin(_pose.rotation())));
}

double Person::calculateVisibilityCost(const Position2D &group_center,
                                       const Person &other) const {
  if (_id == other.id()) {
    return 0.; // same person
  }
  auto this_vector = group_center - pose().position();
  auto this_distance = this_vector.norm();
  auto other_vector = group_center - other.pose().position();
  auto other_distance = other_vector.norm();
  if (other_distance > this_distance) {
    return 0.; // other is farther away from group than this
  }
  auto cosinus_angle =
      this_vector.dot(other_vector) / (this_distance * other_distance);
  if (cosinus_angle > _acos_of_theta) {
    return 0.; // angle is too big
  }
  // exp(ln(k) * cos(\theta) * (d_i/d_j)) can be rearranged to k^(cos(\theta) *
  // (d_i/d_j))
  return pow(100, cosinus_angle * (this_distance / other_distance));
}
