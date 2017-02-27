/********************************************************************
**                                                                 **
** File   : src/GroupDetector.cpp                                  **
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

#include "GroupDetector.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

using fformation::GroupDetector;
using fformation::Observation;
using fformation::Classification;
namespace fv = fformation::validators;

Classification
fformation::OneGroupDetector::detect(const Observation &observation) const {
  std::vector<IdGroup> groups;
  if(!observation.group().persons().empty()){
    std::set<PersonId> group;
    for (auto person : observation.group().persons()) {
      group.insert(person.first);
    }
    groups.push_back(group);
  }
  return Classification(observation.timestamp(), groups);
}

Classification
fformation::NonGroupDetector::detect(const Observation &observation) const {
  std::vector<IdGroup> groups;
  for (auto person : observation.group().persons()) {
    groups.push_back(IdGroup({person.first}));
  }
  return Classification(observation.timestamp(), groups);
}
