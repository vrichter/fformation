/********************************************************************
**                                                                 **
** File   : src/Classification.h                                   **
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

namespace fformation {

class Classification : public JsonSerializable {
public:
  Classification(Timestamp timestamp = Timestamp(),
                 const std::vector<Group> &groups = std::vector<Group>())
      : _timestamp(timestamp), _groups(groups) {}

  const Timestamp &timestamp() const { return _timestamp; }

  const std::vector<Group> &groups() const { return _groups; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"timestamp\": " << _timestamp << ", \"groups\": ";
    serializeIterable(out, _groups);
    out << " }";
  }

private:
  Timestamp _timestamp;
  const std::vector<Group> _groups;
};

} // namespace fformation
