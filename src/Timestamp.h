/********************************************************************
**                                                                 **
** File   : src/Timestamp.h                                        **
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
#include <cmath>
#include <limits>

namespace fformation {

class Timestamp : public JsonSerializable {
public:
  typedef double TimestampType;

  Timestamp(const TimestampType &timestamp = 0.) : _timestamp(timestamp) {}

  const TimestampType &time() const { return _timestamp; }

  friend bool operator==(const Timestamp &lhs, const Timestamp &rhs) {
    return lhs.equals(rhs);
  }

  friend bool operator<(const Timestamp &lhs, const Timestamp &rhs) {
    return lhs._timestamp < rhs._timestamp;
  }

  bool equals(const Timestamp &other) const {
    return std::fabs(time() - other.time()) <
           std::numeric_limits<TimestampType>::epsilon();
  }

  virtual void serializeJson(std::ostream &out) const override;

private:
  double _timestamp;
};

} // namespace fformation
