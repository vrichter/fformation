/********************************************************************
**                                                                 **
** File   : src/FoV.h                                              **
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

// for now i have no idea what that is
class FoV : public JsonSerializable {
public:
  FoV(double a = 0., double b = 0., double c = 0., double d = 0.)
      : _a(a), _b(b), _c(c), _d(d) {}

  virtual void serializeJson(std::ostream &out) const override {
    out << "[ " << _a << ", " << _b << ", " << _c << ", " << _d << " ]";
  }

private:
  const double _a;
  const double _b;
  const double _c;
  const double _d;
};

} // namespace fformation
