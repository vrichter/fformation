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

namespace fformation {

template <typename INTEGER = int, typename REAL = double>
class ConfusionMatrix {
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

  RealType calculatePrecision() {
    return static_cast<RealType>(true_positive()) /
           (static_cast<RealType>(true_positive()) /
            static_cast<RealType>(false_positive()));
  }

  RealType calculateRecall() {
    return static_cast<RealType>(true_positive()) /
           (static_cast<RealType>(true_positive()) /
            static_cast<RealType>(false_negative()));
  }

private:
  /**
   * @brief _data contains tp,fp,tn,fn
   */
  std::array<IntType, 4> _data;
};

} // namespace fformation
