/********************************************************************
**                                                                 **
** File   : src/Fformation.cpp                                     **
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

#include "Fformation.h"

using fformations::Fformation;
using fformations::Classification;
using fformations::PersonId;

Classification Fformation::classify() {
  std::vector<Group> groups;
  for (auto person : _observation.group().persons()) {
    groups.push_back(Group({Person(person.second.id())}));
  }
  return Classification(_observation.timestamp(), std::vector<Group>({groups}));
}
