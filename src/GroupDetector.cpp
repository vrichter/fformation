/********************************************************************
**                                                                 **
** File   : src/GroupDetector.cpp                                  **
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

#include "GroupDetector.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

using fformation::GroupDetector;
using fformation::Observation;
using fformation::Classification;
using fformation::GrowingGroupDetector;
namespace fv = fformation::validators;

Classification
fformation::OneGroupDetector::detect(const Observation &observation) const {
  std::set<PersonId> group;
  for (auto person : observation.group().persons()) {
    group.insert(person.first);
  }
  return Classification(observation.timestamp(), std::vector<IdGroup>({group}));
}

Classification
fformation::NonGroupDetector::detect(const Observation &observation) const {
  std::vector<IdGroup> groups;
  for (auto person : observation.group().persons()) {
    groups.push_back(IdGroup({person.first}));
  }
  return Classification(observation.timestamp(), groups);
}

fformation::GrowingGroupDetector::GrowingGroupDetector(const Options &options)
    : GroupDetector(options),
      _mdl(options.getOption("mdl").convertValue<double>(fv::Min<double>(0.))),
      _stride(options.getOption("stride").convertValue<double>(
          fv::Min<double>(0.))) {}

typedef size_t GroupNum;
typedef size_t PersonNum;
struct Assignment {
  PersonNum person_pos;
  GroupNum group_pos;
  double costs;

  Assignment() : person_pos(0), group_pos(0), costs(max()) {}
  Assignment(PersonNum person, GroupNum group, double cost)
      : person_pos(person), group_pos(group), costs(cost) {}

  static double max() { return std::numeric_limits<double>::max(); }
  static double min() { return 0.; }
};

typedef std::multimap<double, Assignment, std::less<double>> AssignmentCosts;

using fformation::Person;
using fformation::Position2D;
static AssignmentCosts
calculateAssignmentCosts(const std::vector<Person> &persons,
                         const std::vector<Position2D> &centers,
                         const Person::Stride stride) {
  AssignmentCosts result;
  for (PersonNum p = 0; p < persons.size(); ++p) {
    for (GroupNum g = 0; g < centers.size(); ++g) {
      const Person &person = persons[p];
      Assignment assign(p, g, Assignment::min());
      assign.costs += person.calculateDistanceCosts(centers[g], stride);
      for (PersonNum p2 = 0; p2 < persons.size(); ++p2) {
        assign.costs += person.calculateVisibilityCost(centers[g], persons[p2]);
      }
      result.insert(std::make_pair(assign.costs, assign));
    }
  }
  return result;
}

static AssignmentCosts findBestAssignment(const AssignmentCosts costs) {
  std::map<GroupNum, Assignment> best_assignment;
  for (auto it = costs.rbegin(); it != costs.rend(); ++it) {
    best_assignment[it->second.person_pos] = it->second;
  }
  AssignmentCosts result;
  for (auto it : best_assignment) {
    result.insert(std::make_pair(it.second.costs, it.second));
  }
  return result;
}

static double sumCosts(const AssignmentCosts &costs, const double &mdl) {
  std::set<GroupNum> groups;
  double sum = 0.;
  for (auto it : costs) {
    sum += it.first;
    groups.insert(it.second.group_pos);
  }
  sum += double(groups.size()) * mdl;
  return sum;
}

static std::vector<Position2D> proposeNewCenters(
    const AssignmentCosts &costs, const std::vector<Position2D> &groups,
    const std::vector<Person> &persons, const Person::Stride &stride) {
  if (groups.empty()) {
    // initially create a mean group center for a single group
    return {fformation::Group::calculateCenter(persons, stride)};
  } else {
    // just add a new group for the max-cost person if the cost is higher than
    // mdl
    auto result = groups;
    auto best_assignment = findBestAssignment(costs);
    const Person &max = persons[best_assignment.rbegin()->second.person_pos];
    result.push_back(max.calculateTransactionalSegmentPosition(stride));
    return result;
  }
}

static std::map<GroupNum, std::vector<Person>>
createGroups(const AssignmentCosts &assignment,
             const std::vector<Person> &persons) {
  std::map<GroupNum, std::vector<Person>> result;
  for (auto assign : assignment) {
    result[assign.second.group_pos].push_back(
        persons[assign.second.person_pos]);
  }
  return result;
}

static std::vector<Position2D> updateCenters(const std::vector<Position2D> old,
                                             const std::vector<Person> &persons,
                                             const AssignmentCosts &assignment,
                                             const Person::Stride &stride) {
  std::map<GroupNum, std::vector<Person>> groups =
      createGroups(assignment, persons);
  std::vector<Position2D> result;
  result.reserve(groups.size());
  for (auto group : groups) {
    result.push_back(fformation::Group::calculateCenter(group.second, stride));
  }
  return result;
}

static AssignmentCosts optimizeCenters(std::vector<Position2D> &centers,
                                       const std::vector<Person> &persons,
                                       const Person::Stride &stride) {
  auto assign = calculateAssignmentCosts(persons, centers, stride);
  auto best_assign = findBestAssignment(assign);
  double costs = std::numeric_limits<double>::max();
  while (true) {
    // E
    auto new_centers = updateCenters(centers, persons, best_assign, stride);
    // M
    auto new_assign = calculateAssignmentCosts(persons, centers, stride);
    auto new_best_assign = findBestAssignment(new_assign);
    double new_costs =
        sumCosts(new_best_assign, 0.); // mdl not important in this case
    if (new_costs < costs) {           // loop
      costs = new_costs;
      assign = new_assign;
      best_assign = new_best_assign;
      centers = new_centers; // update the centers for the caller
    } else {                 // EXIT
      break;
    }
  }
  return assign;
}

static Classification
createClassification(const fformation::Timestamp &timestamp,
                     const std::vector<Person> &persons,
                     const AssignmentCosts &costs) {
  using fformation::IdGroup;
  using fformation::PersonId;
  auto groups = createGroups(costs, persons);
  std::vector<IdGroup> id_groups;
  id_groups.reserve(groups.size());
  for (auto group : groups) {
    std::set<PersonId> person_ids;
    for (auto person : group.second) {
      person_ids.insert(person.id());
    }
    id_groups.push_back(IdGroup(person_ids));
  }
  return Classification(timestamp, id_groups);
}

Classification
fformation::GrowingGroupDetector::detect(const Observation &observation) const {
  // edge case
  if (observation.group().persons().size() < 2) {
    OneGroupDetector det;
    return det.detect(observation);
  }

  std::vector<Person> persons = observation.group().generatePersonList();
  std::vector<Position2D> centers;
  AssignmentCosts costs;
  double sum_costs = std::numeric_limits<double>::max();

  while (true) {
    // propose new center
    auto new_centers = proposeNewCenters(costs, centers, persons, _stride);
    // update centers through em
    // calculate assignment costs, sum costs
    auto new_costs = optimizeCenters(new_centers, persons, _stride);
    // if sum_costs < previous
    double new_sum_costs = sumCosts(findBestAssignment(new_costs), _mdl);
    if (new_sum_costs < sum_costs) {
      // // insert centers, assignment, sum into log
      centers = new_centers;
      costs = new_costs;
      sum_costs = new_sum_costs;
      std::cout << "costs dec " << sum_costs << std::endl;
    } else {
      std::cout << "costs inc " << sum_costs << std::endl;
      break;
    }
  }
  return createClassification(observation.timestamp(), persons,
                              findBestAssignment(costs));
}
