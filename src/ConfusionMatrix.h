/********************************************************************
**                                                                 **
** File   : src/ConfusionMatrix.h                                  **
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

#pragma once

#include "JsonSerializable.h"
#include <array>
#include <vector>

namespace fformation {

class ConfusionMatrix : public JsonSerializable {
public:
  typedef int IntType;
  typedef double RealType;

  ConfusionMatrix() = default;

  ConfusionMatrix(IntType true_positive, IntType false_positive,
                  IntType true_negative, IntType false_negative)
      : _data(
            {{true_positive, false_positive, true_negative, false_negative}}) {}

  ConfusionMatrix(const std::array<IntType, 4> &data) : _data(data) {}

  const IntType &true_positive() const { return _data.at(0); }
  const IntType &false_positive() const { return _data.at(1); }
  const IntType &true_negative() const { return _data.at(2); }
  const IntType &false_negative() const { return _data.at(3); }
  const std::array<IntType, 4> &data() const { return _data; }

  RealType calculatePrecision() const {
    if (true_positive() + false_positive() == 0) {
      return 1.;
    }
    return static_cast<RealType>(true_positive()) /
           static_cast<RealType>(true_positive() + false_positive());
  }

  RealType calculateRecall() const {
    if (true_positive() + false_negative() == 0) {
      return 1.;
    }
    return static_cast<RealType>(true_positive()) /
           static_cast<RealType>(true_positive() + false_negative());
  }

  static RealType calculateF1Score(const RealType &precision,
                                   const RealType &recall) {
    return (2. * precision * recall) / (precision + recall);
  }

  RealType calculateF1Score() const {
    return calculateF1Score(calculatePrecision(), calculateRecall());
  }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"true-positive\": " << true_positive()
        << ", \"false-positive\": " << false_positive()
        << ", \"true-negative\": " << true_negative()
        << ", \"false-negative\": " << false_negative() << " }";
  }

  template <typename Set, typename AccessFunction>
  static RealType calculateMean(Set set, AccessFunction function) {
    RealType result = 0.;
    IntType elements = 0;
    for (auto elem : set) {
      result += function(elem);
      elements += 1;
    }
    return result / RealType(elements);
  }

  template <typename Set>
  static RealType calculateMeanPrecision(const Set matrices) {
    return calculateMean(matrices, [](const ConfusionMatrix &m) {
      return m.calculatePrecision();
    });
  }

  template <typename Set>
  static RealType calculateMeanRecall(const Set matrices) {
    return calculateMean(
        matrices, [](const ConfusionMatrix &m) { return m.calculateRecall(); });
  }

  template <typename Set>
  static RealType calculateMeanF1Score(const Set matrices) {
    return calculateF1Score(calculateMeanPrecision(matrices),
                            calculateMeanRecall(matrices));
  }

  friend ConfusionMatrix operator+(const ConfusionMatrix &a,
                                   const ConfusionMatrix &b) {
    return ConfusionMatrix(std::array<IntType, 4>(
        {{a._data[0] + b._data[0], a._data[1] + b._data[1],
          a._data[2] + b._data[2], a._data[3] + b._data[3]}}));
  }

private:
  /**
   * @brief _data contains tp,fp,tn,fn
   */
  std::array<IntType, 4> _data;
};

} // namespace fformation
