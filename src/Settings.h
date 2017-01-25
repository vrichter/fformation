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

namespace fformation {

class Settings : public JsonSerializable {
public:
  Settings(std::array<double, 9> covariance_matrix = std::array<double, 9>(),
           double empty = 0., double radius = 0., size_t nsamples = 0,
           double quant = 0.)
      : _covariance_matrix(covariance_matrix), _empty(empty), _radius(radius),
        _nsamples(nsamples), _quant(quant) {}

  const std::array<double, 9> covariance_matrix() const {
    return _covariance_matrix;
  }
  const double empty() const { return _empty; }
  const double radius() const { return _radius; }
  const size_t nsamples() const { return _nsamples; }
  const double quant() const { return _quant; }

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

private:
  const std::array<double, 9> _covariance_matrix;
  const double _empty;
  const double _radius;
  const size_t _nsamples;
  const double _quant;
};

class SettingsGC : public JsonSerializable {
public:
  SettingsGC(double mdl = 0., double stride = 0.)
      : _mdl(mdl), _stride(stride) {}

  const double mdl() const { return _mdl; }
  const double stride() const { return _stride; }

  virtual void serializeJson(std::ostream &out) const override {
    out << "{ \"mdl\": " << _mdl;
    out << ", \"stride\": " << _stride;
    out << " }";
  }

  static SettingsGC readMatlabJson(const std::string &filename);

private:
  const double _mdl;
  const double _stride;
};

} // namespace fformation
