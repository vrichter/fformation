/********************************************************************
**                                                                 **
** File   : src/GroundTruth.h                                      **
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
#include "Group.h"
#include "JsonSerializable.h"
#include "Person.h"
#include <vector>

namespace fformation {

class GroundTruth : public JsonSerializable {
public:
  GroundTruth(const std::vector<Classification> &classifications =
                  std::vector<Classification>())
      : _classifications(classifications) {}

  const std::vector<Classification> &classifications() const {
    return _classifications;
  }

  virtual void serializeJson(std::ostream &out) const override;

  static GroundTruth readMatlabJson(const std::string &filename);

private:
  const std::vector<Classification> _classifications;
};

} // namespace fformation
