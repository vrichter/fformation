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

static ConfusionMatrix random() {
  return ConfusionMatrix((std::rand() + 1), (std::rand() + 1),
                         (std::rand() + 1), (std::rand() + 1));
}

static size_t mcMatrices() { return 10; }

TEST(ConfusionMatrixTest, CreateInstance) {
  int tp = 1;
  int fp = 2;
  int tn = 3;
  int fn = 4;
  // general case
  ConfusionMatrix cm(tp, fp, tn, fn);
  EXPECT_EQ(cm.true_positive(), tp);
  EXPECT_EQ(cm.true_negative(), tn);
  EXPECT_EQ(cm.false_positive(), fp);
  EXPECT_EQ(cm.false_negative(), fn);
}

TEST(ConfusionMatrixTest, PrecisionRecallCalculation) {
  // general case
  ConfusionMatrix cm = random();
  EXPECT_FLOAT_EQ(double(cm.true_positive()) /
                      double(cm.true_positive() + cm.false_positive()),
                  cm.calculatePrecision());
  EXPECT_FLOAT_EQ(double(cm.true_positive()) /
                      double(cm.true_positive() + cm.false_negative()),
                  cm.calculateRecall());

  // edge case 1
  cm = ConfusionMatrix(0, 0, 0, 0);
  EXPECT_FLOAT_EQ(cm.calculatePrecision(), 1.);
  EXPECT_FLOAT_EQ(cm.calculateRecall(), 1.);

  // edge case 2
  cm = ConfusionMatrix(0, 0, 0, 1);
  EXPECT_FLOAT_EQ(cm.calculatePrecision(), 1.);
  EXPECT_FLOAT_EQ(cm.calculateRecall(), 0.);

  // edge case 2
  cm = ConfusionMatrix(0, 1, 0, 0);
  EXPECT_FLOAT_EQ(cm.calculatePrecision(), 0.);
  EXPECT_FLOAT_EQ(cm.calculateRecall(), 1.);
}

TEST(ConfusionMatrixTest, MeanCalculationsEquals) {
  std::vector<ConfusionMatrix> equals;
  ConfusionMatrix cm = random();
  for (size_t i = 0; i < mcMatrices(); ++i) {
    equals.push_back(cm);
  }
  ASSERT_DOUBLE_EQ(cm.calculatePrecision(),
                   ConfusionMatrix::calculateMeanPrecision(equals));
  ASSERT_DOUBLE_EQ(cm.calculateRecall(),
                   ConfusionMatrix::calculateMeanRecall(equals));
}

TEST(ConfusionMatrixTest, MeanCalculationsDifferent) {
  std::vector<ConfusionMatrix> matrices;
  for (size_t i = 0; i < mcMatrices(); ++i) {
    matrices.push_back(random());
  }
  double mean_precision = 0.;
  double mean_recall = 0.;
  for (auto mat : matrices) {
    mean_precision += mat.calculatePrecision();
    mean_recall += mat.calculateRecall();
  }
  ASSERT_DOUBLE_EQ(mean_precision / double(matrices.size()),
                   ConfusionMatrix::calculateMeanPrecision(matrices));
  ASSERT_DOUBLE_EQ(mean_recall / double(matrices.size()),
                   ConfusionMatrix::calculateMeanRecall(matrices));
}

TEST(ConfusionMatrixTest, F1ScoreCalculations) {
  double precision = 1. / (double)std::rand();
  double recall = 1. / (double)std::rand();
  ASSERT_DOUBLE_EQ((2 * precision * recall / (precision + recall)),
                   ConfusionMatrix::calculateF1Score(precision, recall));
  std::vector<ConfusionMatrix> matrices;
  for (size_t i = 0; i < mcMatrices(); ++i) {
    matrices.push_back(random());
  }
  double mean_precision = 0.;
  double mean_recall = 0.;
  for (auto mat : matrices) {
    precision = mat.calculatePrecision();
    recall = mat.calculateRecall();
    mean_precision += precision;
    mean_recall += recall;
    ASSERT_DOUBLE_EQ(ConfusionMatrix::calculateF1Score(precision, recall),
                     mat.calculateF1Score());
  }
  mean_precision /= double(matrices.size());
  mean_recall /= double(matrices.size());
  ASSERT_DOUBLE_EQ(
      ConfusionMatrix::calculateF1Score(mean_precision, mean_recall),
      ConfusionMatrix::calculateMeanF1Score(matrices));
}
}
