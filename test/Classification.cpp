/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/Classification.cpp                                **
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
using fformation::ConfusionMatrix;

static std::set<PersonId> persons(const std::vector<size_t> ids) {
  std::set<PersonId> result;
  for (auto id : ids) {
    std::stringstream str;
    str << id;
    result.insert(str.str());
  }
  return result;
}

static IdGroup group(const std::vector<size_t> &ids) {
  return IdGroup(persons(ids));
}

static Classification ccl(const std::vector<std::vector<size_t>> &data,
                          fformation::Timestamp timestamp = 0) {
  std::vector<IdGroup> result;
  for (auto d : data) {
    result.push_back(group(d));
  }
  return Classification(timestamp, result);
}

static ConfusionMatrix compare(const Classification &c,
                               const Classification &gt,
                               double threshold = 1.) {
  return c.createConfusionMatrix(gt, threshold);
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
  groups.push_back(group({1, 2, 3}));
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
  IdGroup empty = group({});
  groups.push_back(empty);
  EXPECT_THROW(Classification(t, groups), Exception);
}

TEST(ClassificationTest, confusionMatrix) {
  // auto empty = classification({});
  // auto first = classification({{1,2,3}});
  // auto first_single = classification({{1},{2},{3}});
  // auto mixed = classification({{1,2},{3}});
  // auto second = classification({{4,5,6}});
  ConfusionMatrix cm;

  // edge case 1: empty data
  cm = compare(ccl({}), ccl({}));
  EXPECT_EQ(0, cm.true_positive());
  EXPECT_EQ(0, cm.true_negative());
  EXPECT_EQ(0, cm.false_positive());
  EXPECT_EQ(0, cm.false_negative());

  // edge case 2: no groups found, single persons ignored by classification
  cm = compare(ccl({}), ccl({{1, 2, 3}}));
  EXPECT_EQ(0, cm.true_positive());
  EXPECT_EQ(0, cm.true_negative());
  EXPECT_EQ(0, cm.false_positive());
  EXPECT_EQ(3, cm.false_negative());

  // edge case 2: no groups found, single person not ignored by classification
  cm = compare(ccl({{1}, {2}, {3}}), ccl({{1, 2, 3}}));
  EXPECT_EQ(0, cm.true_positive());
  EXPECT_EQ(0, cm.true_negative());
  EXPECT_EQ(0, cm.false_positive());
  EXPECT_EQ(3, cm.false_negative());

  // edge case 3: no groups in ground truth, mixed classification
  cm = compare(ccl({{1, 2}, {3}}), ccl({}));
  EXPECT_EQ(0, cm.true_positive());
  EXPECT_EQ(1, cm.true_negative());
  EXPECT_EQ(1, cm.false_positive());
  EXPECT_EQ(0, cm.false_negative());

  // correct classification of mixed scene
  cm = compare(ccl({{1, 2}, {3}}), ccl({{1, 2}, {3}}));
  EXPECT_EQ(1, cm.true_positive());
  EXPECT_EQ(1, cm.true_negative());
  EXPECT_EQ(0, cm.false_positive());
  EXPECT_EQ(0, cm.false_negative());

  // classification completely different
  cm = compare(ccl({{1, 2, 3}}), ccl({{4, 5, 6}}));
  EXPECT_EQ(0, cm.true_positive());
  EXPECT_EQ(0, cm.true_negative());
  // one wrong group detected
  EXPECT_EQ(1, cm.false_positive());
  // three persons in gt, not in cl -> three negs. false because must be in
  // group.
  EXPECT_EQ(3, cm.false_negative());

  // more examples
  cm = compare(ccl({{1, 2}, {3, 4}, {5}, {6}, {7}}), ccl({{1, 2}, {3, 4}}));
  EXPECT_EQ(2, cm.true_positive());
  EXPECT_EQ(3, cm.true_negative());
  EXPECT_EQ(0, cm.false_positive());
  EXPECT_EQ(0, cm.false_negative());

  // a mixed example case with threshold 0.6
  cm = compare(ccl({{1, 2}, {3, 4, 5}, {6, 7}}), ccl({{1, 2}, {3, 4}, {8, 9}}),
               0.6);
  EXPECT_EQ(2, cm.true_positive());
  EXPECT_EQ(0, cm.true_negative());
  EXPECT_EQ(1, cm.false_positive());
  EXPECT_EQ(2, cm.false_negative());

  // another mixed case
  cm = compare(ccl({{1, 10, 2}, {3, 4, 5}, {6}, {7}, {8, 9}}),
               ccl({{1, 2}, {3, 4, 5}}));
  EXPECT_EQ(1, cm.true_positive());
  EXPECT_EQ(2, cm.true_negative());
  EXPECT_EQ(2, cm.false_positive());
  EXPECT_EQ(0, cm.false_negative());

  // the same but with different threshold
  cm = compare(ccl({{1, 10, 2}, {3, 4, 5}, {6}, {7}, {8, 9}}),
               ccl({{1, 2}, {3, 4, 5}}), 2. / 3.);
  EXPECT_EQ(2, cm.true_positive());
  EXPECT_EQ(2, cm.true_negative());
  EXPECT_EQ(1, cm.false_positive());
  EXPECT_EQ(0, cm.false_negative());
}
}
