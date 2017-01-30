/********************************************************************
**                                                                 **
** File   : src/PersonId.h                                         **
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

namespace fformation {

class PersonId : public JsonSerializable {
public:
  typedef std::string PersonIdType;

  PersonId(PersonIdType id) : _id(id) {}

  friend bool operator<(const PersonId &lhs, const PersonId &rhs) {
    return lhs._id < rhs._id;
  }

  virtual void serializeJson(std::ostream &out) const override { out << _id; }

private:
  const PersonIdType _id;
};

} // namespace fformation
