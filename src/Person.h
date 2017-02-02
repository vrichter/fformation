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
  /**
   * @brief Stride the distance between a persons position and the center of its
   * transactional space.
   */
  typedef Position2D::Coordinate Stride;

  Person(PersonId id, Pose2D pose) : _id(id), _pose(pose) {}

  const PersonId &id() const { return _id; }
  const Pose2D &pose() const { return _pose; }

  /**
   * @brief calculateTransactionalSegmentPosition calculates the center of the
   * transactional segment (TS) of the person.
   *
   * The transactional segment center of a person is assumed to be at the
   * distance of stride along its viewing direction.
   *
   * @param stride the distance btw. a person and its TS. depends on the
   * actually used unit of scale and overall 'crowdedness' of the scene.
   * @return person-position + stride * viewing-direction
   */
  Position2D calculateTransactionalSegmentPosition(Stride stride) const;

  constexpr static const RotationRadian visibilityAngleThreshold() {
    return 0.75;
  }

  /**
   * @brief calculateVisibilityCost the visibility-constraint-costs caused by
   * another person.
   *
   * The visibility constraint can be calculated for an arbitrary group_center
   * and person combination.
   *
   * Given \f$\theta_{i,j}^g\f$ being the angle btw. the persons relative to the
   * passed group center, \f$d_i^g\f$ the distance of this and \f$d_j^g\f$ the
   * distance oth ther to the group center, \f$\hat\theta = 0.75\f$ and
   * \f$K=100\f$. This fuction calculates:
   *
   * \f$ R = 0 \f$ if \f$cos(\theta_{i,j}^g) < \hat\theta\f$ or \f$d_i^g <
   * d_j^g\f$ and in all other cases:
   *
   * We use the constants \f$\hat\theta=acos(0.75)\f$ and \f$K=ln(100)\f$ and
   * the Formula from the original matlab code:
   *
   * \f[ R = exp(K cos(\theta_{i,j}^g)) \frac{d_i^g}{d_j^g} \f]
   *
   * Instead of the original formula from the paper:
   * \f[ R = exp(K cos(\theta_{i,j}^g)) \frac{d_i^g-d_j^g}{d_j^g} \f]
   *
   * @param group_center the assumed center of a group this person may be in.
   * @param other the other person. not necessarily in the same group.
   * @return the cost of the occlusion produced by other btw. this and
   *         group_center.
   *         * 0. if other == this
   *         * 0. if other farther away from center than this
   *         * 0. if the cos of the angle btw. this and other rel. to the center
   *           is bigger than 0.75 radians.
   *         * a cost value depending on the angles btw. the persons and their
   *           distancces to the group center
   */
  double calculateVisibilityCost(const Position2D &group_center,
                                 const Person &other) const;

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"id\": " << _id;
    out << ", \"pose\": " << _pose;
    out << " }";
  }

private:
  const PersonId _id;
  const Pose2D _pose;
  const double _acos_of_theta = 0.75;
  const double _ln_of_k = 100;
};

} // namespace fformation
