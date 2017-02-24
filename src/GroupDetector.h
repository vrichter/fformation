/********************************************************************
**                                                                 **
** File   : src/GroupDetector.h                                    **
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
#include "Observation.h"
#include "Options.h"
#include <memory>

namespace fformation {

class GroupDetector {
public:
  typedef std::unique_ptr<GroupDetector> Ptr;

  GroupDetector(const Options &options) : _options(options) {}

  virtual ~GroupDetector() = default;

  virtual Classification detect(const Observation &observation) const = 0;

  const Options &options() const { return _options; }

private:
  Options _options;
};

class OneGroupDetector : public GroupDetector {
public:
  OneGroupDetector() : GroupDetector(Options()) {}

  virtual Classification detect(const Observation &observation) const final;
};

class NonGroupDetector : public GroupDetector {
public:
  NonGroupDetector() : GroupDetector(Options()) {}

  virtual Classification detect(const Observation &observation) const final;
};

class GrowingGroupDetector : public GroupDetector {
public:
  GrowingGroupDetector(const Options &options);

  virtual Classification detect(const Observation &observation) const final;

private:
  double _mdl;
  Person::Stride _stride;
};

} // namespace fformation
