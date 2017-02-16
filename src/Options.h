/********************************************************************
**                                                                 **
** File   : src/Options.h                                          **
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
#include "Exception.h"
#include <set>
#include <sstream>
#include <string>

namespace fformation {

class Option {
public:
  struct Comp {
    bool operator()(const Option &lhs, const Option &rhs) const {
      return lhs.name() < rhs.name();
    }
  };

  typedef std::string NameType;
  typedef std::string ValueType;

  Option(const NameType &name, const ValueType &value)
      : _name(name), _value(value), _has_value(true) {}
  Option(const NameType &name) : _name(name), _value(""), _has_value(false) {}

  const NameType &name() const { return _name; }
  const ValueType &value() const { return _value; }
  const bool &has_value() const { return _has_value; }

  template <typename T> struct Accept {
    static bool validate(T accept) { return true; }
  };

  template <typename T, typename Validator = Accept<T>> T convertValue() const {
    T result;
    std::stringstream str(_value);
    str >> result;
    if (!Validator::validate(result)) {
      Exception("Cannot convert option '" + _name + "'='" + _value +
                "' to a valid value.");
    }
    return result;
  }

private:
  NameType _name;
  ValueType _value;
  bool _has_value;
};

class Options : public std::set<Option, Option::Comp> {
public:
  Options() = default;
  Options(Options::iterator begin, Options::iterator end);

  const bool hasOption(const Option::NameType &name) const;
  const Option &getOption(const Option::NameType &name) const;

  static Options
  parseFromString(const std::string &options,
                  const std::string &separator = "@") throw(Exception);
};

} // namespace fformation
