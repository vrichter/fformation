/********************************************************************
**                                                                 **
** File   : src/Classification.cpp                                 **
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

#include "Classification.h"

using fformation::Classification;
using fformation::Group;
using fformation::Observation;

std::vector<Group>
Classification::createGroups(const Observation &observation) const {
  std::vector<Group> result;
  result.reserve(_groups.size());
  for (auto group : _groups) {
    result.push_back(Group(observation.group().find_persons(group.persons())));
  }
  return result;
}

double Classification::calculateDistanceCosts(const Observation &observation,
                                              Person::Stride stride) const {
  double cost = 0.;
  for (auto group : createGroups(observation)) {
    cost += group.calculateDistanceCosts(stride);
  }
  return cost;
}

double Classification::calculateMDLCosts(double mdl_prior) const {
  return mdl_prior * (double)_groups.size();
}

double Classification::calculateVisibilityCosts(const Observation &observation,
                                                Person::Stride stride) const {
  auto groups = createGroups(observation);
  auto all_persons = observation.group().persons();
  double cost = 0.;
  for (auto group : groups) {
    auto center = group.calculateCenter(stride);
    for (auto person_i : group.persons()) {
      for (auto person_j : all_persons) {
        cost +=
            person_i.second.calculateVisibilityCost(center, person_j.second);
      }
    }
  }
  return cost;
}
