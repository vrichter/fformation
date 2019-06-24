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
#include <assert.h>
#include <limits>

using fformation::Classification;
using fformation::Group;
using fformation::Observation;
using fformation::ConfusionMatrix;
using fformation::Timestamp;
using fformation::IdGroup;

Classification::Classification(Timestamp timestamp,
                               const std::vector<IdGroup> &groups)
    : _timestamp(timestamp), _groups(groups) {
  for (auto &group : _groups) {
    if (group.persons().empty()) {
      throw Exception("Empty IdGroup in Classifications are forbidden.");
    }
  }
}

std::vector<Group> Classification::createGroups(const Observation &observation,
                                                bool singular) const {
  std::vector<Group> result;
  result.reserve(_groups.size());
  std::vector<IdGroup> id_groups = _groups;
  if (singular) {
    for (auto person_it : observation.group().persons()) {
      auto id = person_it.first;
      bool found = false;
      for (auto id_group : id_groups) {
        if (id_group.has_person(id)) {
          found = true;
          break;
        }
      }
      if (!found) {
        id_groups.push_back({{id}});
      }
    }
  }
  for (auto group : id_groups) {
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

static std::vector<IdGroup> fillUpFrom(const std::vector<IdGroup> &fill,
                                       const std::vector<IdGroup> &from) {
  auto result = fill;
  for (auto gt_group : from) {
    for (auto gt_person : gt_group.persons()) {
      bool found = false;
      for (auto group : result) {
        if (group.persons().find(gt_person) != group.persons().end()) {
          found = true;
          break;
        }
      }
      if (!found) {
        result.push_back(IdGroup({gt_person}));
      }
    }
  }
  return result;
}

ConfusionMatrix
Classification::createConfusionMatrix(const Classification &ground_truth,
                                      double threshhold) const {
  assert(threshhold >= 0.);
  assert(threshhold <= 1.);
  // in case the algothithm does not add non-group persons as on-person groups
  // we need to collect the missing persons here for the correct results
  auto cl = fillUpFrom(_groups, ground_truth.idGroups());
  auto gt = fillUpFrom(ground_truth.idGroups(), _groups);

  // count the various cases
  ConfusionMatrix::IntType true_positive = 0;
  ConfusionMatrix::IntType true_negative = 0;
  ConfusionMatrix::IntType false_negative = 0;
  ConfusionMatrix::IntType false_positive = 0;
  for (auto classified_group : cl) {
    if (classified_group.persons().size() == 1) {
      // not added to a group. validate that person is not in a group in gt
      for (auto gt_group : gt) {
        if (gt_group.persons().find(*classified_group.persons().begin()) !=
            gt_group.persons().end()) {
          if (gt_group.persons().size() == 1) {
            true_negative += 1;
          } else {
            false_negative += 1;
          }
          break;
        }
      }
    } else {
      // find a grop in gt with a high enough intersection.
      bool found = false;
      for (auto gt_group : gt) {
        if (calculateGroupIntersection(classified_group, gt_group) >=
            (threshhold - std::numeric_limits<double>::epsilon())) {
          found = true;
          break;
        }
      }
      if (found) {
        true_positive += 1;
      } else {
        false_positive += 1;
      }
    } // else ignore empty groups
  }
  return ConfusionMatrix(true_positive, false_positive, true_negative,
                         false_negative);
}

double Classification::calculateGroupIntersection(const IdGroup &first,
                                                  const IdGroup &second) {
  if (first.persons().empty() || second.persons().empty())
    return 0.;
  size_t cardinality = (first.persons().size() > second.persons().size())
                           ? first.persons().size()
                           : second.persons().size();
  size_t intersection = 0;
  for (auto person : first.persons()) {
    if (second.persons().find(person) != second.persons().end()) {
      ++intersection;
    }
  }
  if (intersection == 0)
    return 0.;
  if (intersection == cardinality)
    return 1.;
  return double(intersection) / double(cardinality);
}
