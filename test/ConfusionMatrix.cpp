/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/io/ConfusionMatrix.cpp                            **
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

#include "ConfusionMatrix.h"

#include <stdlib.h>

#include "gtest/gtest.h"

namespace {
typedef fformation::ConfusionMatrix<> ConfusionMatrix;

TEST(ConfusionMatrixTest, CreateInstance) {

  int tp = (std::rand() + 1) % 10000;
  int fp = (std::rand() + 1) % 10000;
  int tn = (std::rand() + 1) % 10000;
  int fn = (std::rand() + 1) % 10000;
  // general case
  ConfusionMatrix cm(tp, fp, tn, fn);
  EXPECT_EQ(cm.true_positive(), tp);
  EXPECT_EQ(cm.true_negative(), tn);
  EXPECT_EQ(cm.false_positive(), fp);
  EXPECT_EQ(cm.false_negative(), fn);
  EXPECT_FLOAT_EQ(double(tp) / double(tp + fp), cm.calculatePrecision());
  EXPECT_FLOAT_EQ(double(tp) / double(tp + fn), cm.calculateRecall());

  // edge case 1
  cm = ConfusionMatrix(0, 0, 0, 0);
  EXPECT_FLOAT_EQ(cm.calculatePrecision(), 1.);
  EXPECT_FLOAT_EQ(cm.calculateRecall(), 1.);

  // edge case 2
  cm = ConfusionMatrix(0, 0, 0, fn);
  EXPECT_FLOAT_EQ(cm.calculatePrecision(), 1.);
  EXPECT_FLOAT_EQ(cm.calculateRecall(), 0.);

  // edge case 2
  cm = ConfusionMatrix(0, fp, 0, 0);
  EXPECT_FLOAT_EQ(cm.calculatePrecision(), 0.);
  EXPECT_FLOAT_EQ(cm.calculateRecall(), 1.);
}
}
