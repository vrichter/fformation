/********************************************************************
**                                                                 **
** File   : src/Classification.h                                   **
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
#include "Group.h"
#include "JsonSerializable.h"
#include "Timestamp.h"
#include "Observation.h"

namespace fformation {

class Classification : public JsonSerializable {
public:
  Classification(Timestamp timestamp = Timestamp(),
                 const std::vector<IdGroup> &groups = std::vector<IdGroup>())
      : _timestamp(timestamp), _groups(groups) {}

  const Timestamp &timestamp() const { return _timestamp; }

  const std::vector<IdGroup> &idGroups() const { return _groups; }

  std::vector<Group> createGroups(const Observation &observation) const;

  /**
   * @brief calculateDistanceCosts the sum of distance costs of the contained groups.
   * @param observation the observation this classification is derived from. Needed for person information.
   * @param stride the distance btw. a person and its transactional segment.
   * @return sum costs. 0 if empty. 0 if all groups of size 1.
   */
  double calculateDistanceCosts(const Observation &observation, Position2D::Coordinate stride) const;

  /**
   * @brief calculateMDLCosts calculates the minimum description length cost.
   *
   * These costs are needed to penalize groups of size 1 which otherwise would be
   * optimal (zero costs). MDL-Costs are mdl_prior * | groups |
   *
   * @param mdl_prior called sigma^2 in the paper. This is the sigma^2 of the normal
   *        probability distribution describing the position of a persons transactional
   *        segment.
   * @return mdl_prior * | groups |
   */
  double calculateMDLCosts(double mdl_prior) const;

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"timestamp\": " << _timestamp << ", \"groups\": ";
    serializeIterable(out, _groups);
    out << " }";
  }

private:
  Timestamp _timestamp;
  const std::vector<IdGroup> _groups;
};

} // namespace fformation
