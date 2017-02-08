/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/io/Classification.cpp                             **
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

#include <stdlib.h>

#include "gtest/gtest.h"

namespace {
using fformation::Classification;
using fformation::Timestamp;
using fformation::IdGroup;
using fformation::PersonId;
using fformation::Exception;
typedef fformation::ConfusionMatrix<> ConfusionMatrix;

static IdGroup emptyGroup() { return IdGroup(std::set<PersonId>()); }
static IdGroup nonEmptyGroup() {
  return IdGroup({PersonId("one"), PersonId("two"), PersonId("three")});
}
static IdGroup nonEmptyGroup2() {
  return IdGroup({PersonId("four"), PersonId("fife"), PersonId("six")});
}

TEST(ClassificationTest, Classification) {
  EXPECT_NO_THROW(Classification());
  // default constructor
  Classification c;
  EXPECT_EQ(0, c.timestamp());
  EXPECT_TRUE(c.idGroups().empty());

  // constructor with empty group list
  Timestamp t = 1234;
  std::vector<IdGroup> groups;
  c = Classification(t, groups);
  EXPECT_EQ(t, c.timestamp());
  EXPECT_TRUE(c.idGroups().empty());

  // constructor with non-empty group list
  groups.push_back(nonEmptyGroup());
  c = Classification(t, groups);
  EXPECT_EQ(t, c.timestamp());
  EXPECT_EQ(groups.size(), c.idGroups().size());
  for (size_t i = 0; i < groups.size(); ++i) {
    auto first = groups.at(i).persons();
    auto second = c.idGroups().at(i).persons();
    EXPECT_EQ(first.size(), second.size());
    for (auto person : first) {
      EXPECT_NE(second.end(), second.find(person));
    }
  }
  // throw on empty group
  groups.push_back(emptyGroup());
  EXPECT_THROW(Classification(t, groups), Exception);
}

TEST(ClassificationTest, confusionMatrix) {
  Classification empty(0, {});
  Classification first(0, {nonEmptyGroup()});
  Classification second(0, {nonEmptyGroup2()});

  // edge case 1
  ConfusionMatrix cm = empty.createConfusionMatrix(empty);
  EXPECT_EQ(cm.true_positive(), 0);
  EXPECT_EQ(cm.true_negative(), 0);
  EXPECT_EQ(cm.false_positive(), 0);
  EXPECT_EQ(cm.false_negative(), 0);

  // edge case 2
  cm = empty.createConfusionMatrix(first);
  EXPECT_EQ(cm.true_positive(), 0);
  EXPECT_EQ(cm.true_negative(), 0);
  EXPECT_EQ(cm.false_positive(), 0);
  EXPECT_EQ(cm.false_negative(), (int)first.idGroups().size());

  // edge case 2
  cm = first.createConfusionMatrix(empty);
  EXPECT_EQ(cm.true_positive(), 0);
  EXPECT_EQ(cm.true_negative(), 0);
  EXPECT_EQ(cm.false_positive(), (int)first.idGroups().size());
  EXPECT_EQ(cm.false_negative(), 0);

  // equal
  cm = first.createConfusionMatrix(first);
  EXPECT_EQ(cm.true_positive(), (int)first.idGroups().size());
  EXPECT_EQ(cm.true_negative(), 0);
  EXPECT_EQ(cm.false_positive(), 0);
  EXPECT_EQ(cm.false_negative(), 0);

  // different
  cm = first.createConfusionMatrix(second);
  EXPECT_EQ(cm.true_positive(), 0);
  EXPECT_EQ(cm.true_negative(), 0);
  EXPECT_EQ(cm.false_positive(), (int)first.idGroups().size());
  EXPECT_EQ(cm.false_negative(), (int)second.idGroups().size());
}
}
