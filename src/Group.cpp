/********************************************************************
**                                                                 **
** File   : src/Group.cpp                                          **
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

#include "Group.h"
#include "Exception.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>

using fformation::Group;
using fformation::Person;
using fformation::PersonId;
using fformation::Position2D;
using fformation::RotationRadian;
using fformation::Settings;
using fformation::Exception;

static std::map<PersonId, Person>
vector_to_map(const std::vector<Person> &persons) {
  std::map<PersonId, Person> result;
  for (auto p : persons) {
    result.insert(std::pair<PersonId, Person>(p.id(), p));
  }
  return result;
}

Group::Group(const std::vector<Person> &persons)
    : _persons(vector_to_map(persons)) {}

Group::Group(const std::map<PersonId, Person> &persons) : _persons(persons) {}

void Group::serializeJson(std::ostream &out) const {
  serializeMapAsVector(out, _persons);
}

static Position2D calculateCenter(const Position2D &a_pos,
                                  const RotationRadian &a_rot,
                                  const Position2D &b_pos,
                                  const Person::Stride &stride) {
  // we have one person with a rotation. assume the other one looks to the same
  // center
  auto a_center =
      Person::calculateTransactionalSegmentPosition(a_pos, a_rot, stride);
  // the center of the other person is in direction of the other center but on
  // the persons 'interaction cercle'
  auto b_center = b_pos + (a_center - b_pos).normalized() * stride;
  return (a_center + b_center) / 2; // the mean btw the two segments
}

static std::vector<Position2D>
calculatePairwiseCenters(const Person &a, const Person &b,
                         const Person::Stride stride) {
  std::vector<Position2D> result;
  if (a.pose().rotation() && b.pose().rotation()) {
    // both have rotation return mean transactional space position
    result.push_back(
        (Person::calculateTransactionalSegmentPosition(
             a.pose().position(), a.pose().rotation().get(), stride) +
         Person::calculateTransactionalSegmentPosition(
             b.pose().position(), b.pose().rotation().get(), stride)) /
        2.);
  } else if (a.pose().rotation()) {
    result.push_back(calculateCenter(a.pose().position(),
                                     a.pose().rotation().get(),
                                     b.pose().position(), stride));
  } else if (b.pose().rotation()) {
    result.push_back(calculateCenter(b.pose().position(),
                                     b.pose().rotation().get(),
                                     a.pose().position(), stride));
  } else {
    // no rotation information available
    auto aPos = a.pose().position();
    auto bPos = b.pose().position();
    auto dirAB = (bPos - aPos);
    auto dist = dirAB.norm();
    auto center_btw_persons = aPos + (dirAB.normalized() * (dist / 2));
    if (dist >= stride * 2) {
      // propose center btw them
      result.push_back(center_btw_persons);
    } else {
      // the height of the intersections of the circles relative to the line btw
      // the persons
      auto h = std::sqrt(std::pow(stride, 2) - std::pow(dist / 2, 2));
      // the vector pointing from a to the center, length = 1
      auto dist_dir = dirAB.normalized();
      // the vertical direction (90deg rotated) relative to the line btw the
      // persons
      auto h_dir = dist_dir.perpendicular();
      // the two intersection positions should be in h distance from the center
      // in h_dir direction
      result.push_back(center_btw_persons + (h_dir * h));
      result.push_back(center_btw_persons - (h_dir * h));
    }
  }
  return result;
}

static std::vector<std::vector<Position2D>>
calculatePairwiseCenters(const std::vector<Person> &persons,
                         const Person::Stride &stride) {
  assert(persons.size() > 1);
  std::vector<std::vector<Position2D>> result;
  for (size_t i = 0; i < persons.size(); i++) {
    for (size_t j = i + 1; j < persons.size(); ++j) {
      result.push_back(
          calculatePairwiseCenters(persons[i], persons[j], stride));
    }
  }
  return result;
}

static std::pair<Position2D, double> calculateNewBestMatchingCenter(
    const boost::optional<Position2D> &old,
    const std::vector<std::vector<Position2D>> &pairs) {
  std::vector<Position2D> best_centers;
  Position2D mean_center(0., 0.);
  for (auto proposals : pairs) {
    assert(!proposals.empty());
    Position2D best = proposals.front();
    if (old) {
      // with a known previous center use the proposals that are nearest to it
      double cost = (old.get() - best).norm();
      for (size_t i = 1; i < proposals.size(); ++i) {
        double new_cost = (old.get() - proposals[i]).norm();
        if (new_cost < cost) {
          best = proposals[i];
          cost = new_cost;
        }
      }
    } else {
      // without proposals just calculate the means
      for (size_t i = 1; i < proposals.size(); ++i) {
        best = best + proposals[i];
      }
      best = best / Position2D::Coordinate(proposals.size());
    }
    best_centers.push_back(best);
    mean_center = mean_center + best;
  }
  mean_center = mean_center / Position2D::Coordinate(pairs.size());
  double cost = 0.;
  for (auto center : best_centers) {
    cost += (center - mean_center).norm();
  }
  return std::make_pair(mean_center, cost);
}

Position2D Group::calculateCenter(const std::vector<Person> &persons,
                                  Person::Stride stride) {
  // edge cases
  if (persons.empty()) {
    throw Exception("Cannot calculate center of an empty group.");
  }
  if (persons.size() == 1) {
    const Person &p = persons.front();
    if (p.pose().rotation()) {
      return Person::calculateTransactionalSegmentPosition(
          p.pose().position(), p.pose().rotation().get(), stride);
    } else {
      // best guess...
      return p.pose().position();
    }
  }
  // default case
  auto pairwise_centers = calculatePairwiseCenters(persons, stride);
  auto center = boost::make_optional<Position2D>(false, Position2D(0., 0.));
  double costs = std::numeric_limits<double>::max();
  while (true) {
    auto new_center_costs =
        calculateNewBestMatchingCenter(center, pairwise_centers);
    if (new_center_costs.second < costs) {
      center = new_center_costs.first;
      costs = new_center_costs.second;
    } else {
      break;
    }
  }
  return center.get();
}

Position2D Group::calculateCenter(Person::Stride stride) const {
  return calculateCenter(generatePersonList(), stride);
}

std::map<PersonId, Person>
Group::find_persons(std::set<PersonId> person_ids) const {
  std::map<PersonId, Person> result;
  for (auto id : person_ids) {
    auto it = _persons.find(id);
    if (it == _persons.end()) {
      std::stringstream str;
      str << "Person with id " << id << " could not be found.";
      throw Exception(str.str());
    }
    result.insert({id, _persons.find(id)->second});
  }
  return result;
}

double Group::calculateDistanceCosts(Person::Stride stride) const {
  if (_persons.size() < 2)
    return 0.; // empty groups and groups of 1 person have zero costs.
  double cost = 0.;
  Position2D group_center = calculateCenter(stride);
  for (auto group_entry : _persons) {
    cost += group_entry.second.calculateDistanceCosts(group_center, stride);
  }
  return cost;
}

std::vector<Person> Group::generatePersonList() const {
  std::vector<Person> result;
  result.reserve(_persons.size());
  for (auto person : _persons) {
    result.push_back(person.second);
  }
  return result;
}
