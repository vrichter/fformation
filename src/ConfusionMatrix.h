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

#include <array>
#include <vector>
#include "JsonSerializable.h"

namespace fformation {

template <typename INTEGER = int, typename REAL = double>
class ConfusionMatrix : public JsonSerializable {
public:

  typedef INTEGER IntType;
  typedef REAL RealType;

  ConfusionMatrix() = default;

  ConfusionMatrix(IntType true_positive, IntType false_positive,
                  IntType true_negative, IntType false_negative)
      : _data({true_positive, false_positive, true_negative, false_negative}) {}

  ConfusionMatrix(const std::array<IntType,4> &data) : _data(data) {}

  const IntType &true_positive() const { return _data.at(0); }
  const IntType &false_positive() const { return _data.at(1); }
  const IntType &true_negative() const { return _data.at(2); }
  const IntType &false_negative() const { return _data.at(3); }
  const std::array<IntType,4>& data() const { return _data; }

  RealType calculatePrecision() const {
    return static_cast<RealType>(true_positive()) /
           (static_cast<RealType>(true_positive()) /
            static_cast<RealType>(false_positive()));
  }

  RealType calculateRecall() const {
    return static_cast<RealType>(true_positive()) /
           (static_cast<RealType>(true_positive()) /
            static_cast<RealType>(false_negative()));
  }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"true-positive\": " << true_positive()
        << ", \"false-positive\": " << false_positive()
        << ", \"true-negative\": " << true_negative()
        << ", \"false-negative\": " << false_negative()
        << " }";
  }

  template<typename Set, typename AccessFunction>
  static RealType calculateMean(Set set, AccessFunction function){
    RealType result = 0.;
    IntType elements = 0;
    for(auto elem : set){
      result += function(elem);
      elements += 1;
    }
    return result / RealType(elements);
  }

  template<typename Set>
  static RealType calculateMeanPrecision(const Set matrices){
    return calculateMean(matrices,[] (const ConfusionMatrix& m) { return m.calculatePrecision(); });
  }

  template<typename Set>
  static RealType calculateMeanRecall(const Set matrices){
    return calculateMean(matrices,[] (const ConfusionMatrix& m) { return m.calculateRecall(); });
  }


private:
  /**
   * @brief _data contains tp,fp,tn,fn
   */
  std::array<IntType, 4> _data;
};

} // namespace fformation
