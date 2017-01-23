/********************************************************************
**                                                                 **
** File   : src/Features.h                                         **
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
#include "FoV.h"
#include "JsonSerializable.h"
#include "Observation.h"

namespace fformations {
class Features : public JsonSerializable {
private:
  const std::vector<Observation> _observations;
  const FoV _fov;

public:
  Features(
      const std::vector<Observation> &observations = std::vector<Observation>(),
      const FoV &fov = FoV())
      : _observations(observations), _fov(fov) {}

  const std::vector<Observation> &observations() const { return _observations; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"fov\": " << _fov << ", \"observation\": ";
    serializeIterable(out, _observations);
    out << " }";
  }

  static Features readMatlabJson(const std::string &filename);
};

} // namespace fformations
