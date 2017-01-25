/********************************************************************
**                                                                 **
** File   : src/Observation.h                                      **
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
#include "Group.h"
#include "JsonSerializable.h"
#include "Timestamp.h"
#include <vector>

namespace fformation {

class Observation : public JsonSerializable {
private:
  const Timestamp _timestamp;
  const Group _group;

public:
  Observation(Timestamp timestamp = 0.,
              const Group &group = std::vector<Person>())
      : _timestamp(timestamp), _group(group) {}

  const Timestamp &timestamp() const { return _timestamp; }
  const Group &group() const { return _group; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"timestamp\": " << _timestamp << ", \"person\": ";
    serializeMapAsVector(out, _group.persons());
    out << " }";
  }
};

} // namespace fformation
