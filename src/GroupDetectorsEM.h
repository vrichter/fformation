/********************************************************************
**                                                                 **
** File   : src/GroupDetectorEM.h                                  **
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
#include "Classification.h"
#include "GroupDetector.h"
#include "Observation.h"
#include "Options.h"
#include <memory>

namespace fformation {

class GroupDetectorGrow : public GroupDetector {
public:
  GroupDetectorGrow(const Options &options);

  virtual Classification detect(const Observation &observation) const final;

private:
  double _mdl;
  Person::Stride _stride;
};

class GroupDetectorShrink : public GroupDetector {
public:
  GroupDetectorShrink(const Options &options);

  virtual Classification detect(const Observation &observation) const final;

private:
  double _mdl;
  Person::Stride _stride;
};

class GroupDetectorShrink2 : public GroupDetector {
public:
  GroupDetectorShrink2(const Options &options);

  virtual Classification detect(const Observation &observation) const final;

private:
  double _mdl;
  Person::Stride _stride;
};

} // namespace fformation
