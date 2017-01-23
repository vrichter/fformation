/********************************************************************
**                                                                 **
** File   : src/Settings.h                                         **
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
#include <array>

namespace fformations {

class Settings : public JsonSerializable {
private:
  const std::array<double, 9> _covariance_matrix;
  const double _empty;
  const double _radius;
  const size_t _nsamples;
  const double _quant;

public:
  Settings(std::array<double, 9> covariance_matrix = std::array<double, 9>(),
           double empty = 0., double radius = 0., size_t nsamples = 0,
           double quant = 0.)
      : _covariance_matrix(covariance_matrix), _empty(empty), _radius(radius),
        _nsamples(nsamples), _quant(quant) {}

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"covariance_matrix\": ";
    serializeIterable(out, _covariance_matrix);
    out << ", \"empty\": " << _empty;
    out << ", \"radius\": " << _radius;
    out << ", \"nsamples\": " << _nsamples;
    out << ", \"quant\": " << _quant;
    out << " }";
  }

  static Settings readMatlabJson(const std::string &filename);
};

class SettingsGC : public JsonSerializable {
private:
  const double _mdl;
  const double _stride;

public:
  SettingsGC(double mdl = 0., double stride = 0.)
      : _mdl(mdl), _stride(stride) {}

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"mdl\": " << _mdl;
    out << ", \"stride\": " << _stride;
    out << " }";
  }

  static SettingsGC readMatlabJson(const std::string &filename);
};

} // namespace fformations
