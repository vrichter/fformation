/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/Group.cpp                                         **
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
#include "math.h"

#include "gtest/gtest.h"

namespace {
using fformation::Group;
using fformation::Person;
using fformation::PersonId;
using fformation::Pose2D;
using fformation::Position2D;
using fformation::Exception;

std::vector<Person> createPersons() {
  std::vector<Person> persons;
  for (auto i : {-1., 0., 1.}) {
    for (auto j : {-1., 0., 1.}) {
      std::stringstream s;
      s << i << j;
      persons.push_back({{s.str()}, {{i, j}}});
    }
  }
  return persons;
}
}

double pi() {
  return 3.14159; // more or less;
}

double precision() {
  return 10e-5; // for pi and calculations
}

TEST(Group, EmptyConstructor) {
  EXPECT_NO_THROW(Group());
  EXPECT_THROW(Group().calculateCenter(0.), Exception);
  EXPECT_EQ(0u, Group().generatePersonList().size());
  EXPECT_EQ(0u, Group().persons().size());
}

TEST(Group, MapConstructor) {
  std::map<PersonId, Person> map;
  for (auto person : createPersons()) {
    map.insert(std::make_pair(person.id(), person));
  }
  EXPECT_NO_THROW(Group(map));
  Group group(map);
  EXPECT_EQ(map.size(), group.generatePersonList().size());
  EXPECT_EQ(map.size(), group.persons().size());
}

TEST(Group, VectorConstructor) {
  auto vec = createPersons();
  EXPECT_NO_THROW(Group(vec));
  Group group(vec);
  EXPECT_EQ(vec.size(), group.generatePersonList().size());
  EXPECT_EQ(vec.size(), group.persons().size());
}

TEST(Group, CalculateCenterWithRotation) {
  Group group;
  Position2D center(0,0);

  group = {{{{"a"}, {{0, 0}, 0.}}, {{"b"}, {{1, 0}, -pi()}}}};
  center = group.calculateCenter(0.5);
  EXPECT_NEAR(0.5, center.x(), precision());
  EXPECT_NEAR(0., center.y(), precision());

  group = {{{{"a"}, {{-1, 0}, pi()/2.}}, {{"b"}, {{1, 0}, pi()/2}}}};
  center = group.calculateCenter(1.);
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(1., center.y(), precision());
}

TEST(Group, CalculateCenterWithoutRotation) {
  Group group;
  Position2D center(0,0);

  // simple: the center sould be exactly btw them
  group = {{{{"a"}, {{0, 0}}}, {{"b"}, {{1, 0}}}}};
  center = group.calculateCenter(0.5);
  EXPECT_NEAR(0.5, center.x(), precision());
  EXPECT_NEAR(0., center.y(), precision());

  // too far: each proposes a center on its circle in direction
  //          of the other. mean results in center btw them.
  group = {{{{"a"}, {{0, 0}}}, {{"b"}, {{5, 0}}}}};
  center = group.calculateCenter(0.5);
  EXPECT_NEAR(2.5, center.x(), precision());
  EXPECT_NEAR(0., center.y(), precision());

  // ts circles cut each other -> two possibilities (0,1) & (0,-1)
  // mean results in (0,0)
  group = {{{{"a"}, {{-1, 0}}}, {{"b"}, {{1, 0}}}}};
  center = group.calculateCenter(std::sqrt(2));
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(0., center.y(), precision()); // mean btw thw two centers

  // the third person rules out the center at (0,-1)
  group = {{{{"a"}, {{-1, 0}}}, {{"b"}, {{1, 0}}}, {{"c"},{{0,1.+std::sqrt(2)}}}}};
  center = group.calculateCenter(std::sqrt(2));
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(1., center.y(), precision()); // now the upper should be chosen

  // the third person rules out the center at (0,1)
  group = {{{{"a"}, {{-1, 0}}}, {{"b"}, {{1, 0}}}, {{"c"},{{0,-1-std::sqrt(2)}}}}};
  center = group.calculateCenter(std::sqrt(2));
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(-1., center.y(), precision()); // now the lower should be chosen

  // two match exactly, third person too far
  // centers (0,0),(-0.5,1)(0.5,1) -> (0.0.66)
  group = {{{{"a"}, {{-1, 0}}}, {{"b"}, {{1, 0}}}, {{"c"},{{0,2}}}}};
  center = group.calculateCenter(1.);
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(2./3., center.y(), precision()); // now the lower should be chosen

  // all ts cut each other
  // the pairwise points should be around the point on perfect
  // match and the mean too
  double dt = 0.1;
  group = {{{{"a"}, {{-1, 0}}}, {{"b"}, {{1, 0}}}, {{"c"},{{0,1.+std::sqrt(2)}}}}};
  center = group.calculateCenter(std::sqrt(2)+dt);
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(1., center.y(), dt);
}

TEST(Group, CalculateCenterWithMixedRotation) {
  Group group;
  Position2D center(0,0);

  // simple: the center sould be exactly btw them
  group = {{{{"a"}, {{0, 0},0.}}, {{"b"}, {{1, 0}}}}};
  center = group.calculateCenter(0.5);
  EXPECT_NEAR(0.5, center.x(), precision());
  EXPECT_NEAR(0., center.y(), precision());

  // looking away. (0,-0.5) and (0,0.5)
  group = {{{{"a"}, {{0, 0},M_PI}}, {{"b"}, {{1, 0}}}}};
  center = group.calculateCenter(0.5);
  EXPECT_NEAR(0, center.x(), precision());
  EXPECT_NEAR(0., center.y(), precision());

  // the third person rules out the center at (0,-1)
  // c looks to -y
  group = {{{{"a"}, {{-1, 0}}}, {{"b"}, {{1, 0}}}, {{"c"},{{0,1.+std::sqrt(2)},-M_PI/2.}}}};
  center = group.calculateCenter(std::sqrt(2));
  EXPECT_NEAR(0., center.x(), precision());
  EXPECT_NEAR(1., center.y(), precision()); // now the upper should be chosen
}

TEST(Group, FindPersons) {
  std::vector<Person> persons = createPersons();
  EXPECT_TRUE(persons.size() > 1);
  std::set<PersonId> ids;
  for (size_t i = 0; i < persons.size() - 1; ++i) {
    ids.insert(persons[i].id());
  }
  Group group(persons);
  auto map = group.find_persons(ids);
  EXPECT_EQ(ids.size(), map.size());
  for (auto id : ids) {
    EXPECT_NE(map.end(), map.find(id));
  }
}

TEST(Group, GeneratePersonList) {
  auto persons = createPersons();
  auto generated = Group(persons).generatePersonList();
  EXPECT_EQ(persons.size(), generated.size());
  auto regenerated = Group(generated).generatePersonList();
  EXPECT_EQ(generated.size(), regenerated.size());
  for (size_t i = 0; i < generated.size(); ++i) {
    EXPECT_EQ(generated[i].id(), regenerated[i].id());
    EXPECT_EQ(generated[i].pose().position().x(),
              regenerated[i].pose().position().x());
    EXPECT_EQ(generated[i].pose().position().y(),
              regenerated[i].pose().position().y());
    EXPECT_EQ(generated[i].pose().rotation(), regenerated[i].pose().rotation());
  }
}

TEST(Group, CalculateSumDistanceCosts) {
  auto persons = createPersons();
  Group group(persons);
  Person::Stride stride = 0.5;
  auto center = group.calculateCenter(stride);
  double sumcosts = 0.;
  for (size_t p1 = 0; p1 < persons.size(); ++p1) {
    sumcosts += persons[p1].calculateDistanceCosts(center, stride);
  }
  EXPECT_EQ(sumcosts, group.calculateDistanceCosts(stride));
}
