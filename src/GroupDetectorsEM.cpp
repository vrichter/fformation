/********************************************************************
**                                                                 **
** File   : src/GroupDetectorEM.cpp                                **
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

#include "GroupDetectorsEM.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

using fformation::Observation;
using fformation::Classification;
using fformation::GroupDetectorGrow;
namespace fv = fformation::validators;

fformation::GroupDetectorGrow::GroupDetectorGrow(const Options &options)
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
  std::map<PersonNum, Assignment> best_assignment;
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
  if(groups.empty()){
    return std::numeric_limits<double>::max();
  } else {
    return sum + double(groups.size()) * mdl;
  }
}

static Position2D calculateTransactionalSegmentPosition(const Person &person, const Person::Stride &stride){
  if(person.pose().rotation()){
    // use thepersons transactional space
    return Person::calculateTransactionalSegmentPosition(person.pose().position(),person.pose().rotation().get(),stride);
  } else {
    // the mean of the persons possible ts-centers is its position
    return person.pose().position();
  }
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
    result.push_back(calculateTransactionalSegmentPosition(max,stride));
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
    auto new_assign = calculateAssignmentCosts(persons, new_centers, stride);
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
fformation::GroupDetectorGrow::detect(const Observation &observation) const {
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

/// shrink detector

static GroupNum findLeastCostIncrease(const AssignmentCosts &costs,
                                      GroupNum groups_count) {
  if (groups_count == 1) {
    return 0;
  } // edge case
  auto best_costs = findBestAssignment(costs);
  std::vector<double> group_move_costs(groups_count, 0.);
  for (auto it : best_costs) {
    for (auto it2 = costs.begin(); it2 != costs.end(); ++it2) {
      if (it2->second.group_pos != it.second.group_pos &&
          it2->second.person_pos == it.second.person_pos) {
        // assignment of the same person to another group with second best costs
        // because of AssignmentCosts sorting.
        group_move_costs[it.second.group_pos] += (it2->second.costs - it.second.costs);
        break;
      }
    }
  }
  GroupNum least = 0;
  double least_cost = group_move_costs.front();
  for(GroupNum g = 0; g < group_move_costs.size(); ++g){
    if(group_move_costs[g] < least_cost){
      least = g;
      least_cost = group_move_costs[g];
    }
  }
  return least;
}

static std::vector<Position2D> proposeLessCenters(
    const AssignmentCosts &costs, const std::vector<Position2D> &groups,
    const std::vector<Person> &persons, const Person::Stride &stride) {
  std::vector<Position2D> centers;
  centers.reserve(persons.size());
  if (groups.empty()) {
    // initially create a group for each person in the observation
    for (auto person : persons) {
      centers.push_back(calculateTransactionalSegmentPosition(person,stride));
    }
    return centers;
  } else {
    GroupNum remove_group = findLeastCostIncrease(costs,groups.size());
    for (GroupNum i = 0; i < groups.size(); ++i) {
      if (i != remove_group) {
        centers.push_back(groups[i]);
      }
    }
    return centers;
  }
}

fformation::GroupDetectorShrink::GroupDetectorShrink(const Options &options)
    : GroupDetector(options),
      _mdl(options.getOption("mdl").convertValue<double>(fv::Min<double>(0.))),
      _stride(options.getOption("stride").convertValue<double>(
          fv::Min<double>(0.))) {}

Classification
fformation::GroupDetectorShrink::detect(const Observation &observation) const {
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
    // remove a group center
    auto new_centers = proposeLessCenters(costs, centers, persons, _stride);
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
