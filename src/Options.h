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
#include <vector>

namespace fformation {

namespace validators {

template <typename T> class Validator {
public:
  virtual bool validate(T accept) const = 0;
};

template <typename T> class Accept : public Validator<T> {
public:
  virtual bool validate(T accept) const final { return true; }
};

template <typename T> class Min : public Validator<T> {
public:
  Min(const T &min, bool exclude = false) : _min(min), _exclude(exclude) {}
  virtual bool validate(T accept) const final {
    if (_exclude) {
      return _min < accept;
    } else {
      return _min <= accept;
    }
  }

private:
  T _min;
  bool _exclude;
};

template <typename T> class Max : public Validator<T> {
public:
  Max(const T &max, bool exclude = false) : _max(max), _exclude(exclude) {}

  virtual bool validate(T accept) const final {
    if (_exclude) {
      return accept < _max;
    } else {
      return accept <= _max;
    }
  }

private:
  T _max;
  bool _exclude;
};

template <typename T> class MinMax : public Validator<T> {
public:
  enum class Exclude { None, Min, Max, Both };

  MinMax(const T &min, const T &max, Exclude config = Exclude::None)
      : _min(min), _max(max), _exclude(config) {}
  virtual bool validate(T accept) const final {
    switch (_exclude) {
    case Exclude::None:
      return _min <= accept && accept <= _max;
    case Exclude::Min:
      return _min < accept && accept <= _max;
    case Exclude::Max:
      return _min <= accept && accept < _max;
    case Exclude::Both:
      return _min < accept && accept < _max;
    default:
      throw Exception("Unknown Exclude type.");
    }
  }

private:
  T _min;
  T _max;
  Exclude _exclude;
};

template <typename T> class OneOf : public Validator<T> {
public:
  OneOf(const std::vector<T> &list, bool result = true)
      : _list(list), _result(result) {}

  virtual bool validate(T accept) const final {
    for (auto i : _list) {
      if (i == accept) {
        return _result;
      }
    }
    return !_result;
  }

private:
  const std::vector<T> &_list;
  bool _result;
};

} // namespace vaidators

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
  Option(const NameType &name, const int &value)
      : _name(name), _value(fromValue(value)), _has_value(true) {}
  Option(const NameType &name, const double &value)
      : _name(name), _value(fromValue(value)), _has_value(true) {}

  const NameType &name() const { return _name; }
  const ValueType &value() const { return _value; }
  const bool &has_value() const { return _has_value; }

  template <typename T> T convertValue() const {
    T result;
    std::stringstream str(_value);
    str >> result;
    return result;
  }

  template <typename T>
  T validate(const validators::Validator<T> &validator) const {
    T result = convertValue<T>();
    if (!validator.validate(result)) {
      Exception("Cannot convert option '" + _name + "'='" + _value +
                "' to a valid value.");
    }
    return result;
  }

  template <typename T> static ValueType fromValue(const T value) {
    std::stringstream str;
    str << value;
    return str.str();
  }

private:
  NameType _name;
  ValueType _value;
  bool _has_value;
};

class Options : public std::set<Option, Option::Comp> {
public:
  Options() = default;

  void override(const Options &other);
  void override(const Option &other);

  const bool hasOption(const Option::NameType &name) const;
  const Option &getOption(const Option::NameType &name) const throw(Exception);
  const Option getOptionOr(const Option::NameType &name, const Option::ValueType &value) const;

  template<typename T>
  const T getValue(const Option::NameType &name, const validators::Validator<T> &validator = validators::Accept<T>()) const {
    return getOption(name).validate(validator);
  }

  template<typename T>
  const T getValueOr(const Option::NameType &name, T fallback, const validators::Validator<T> &validator=validators::Accept<T>()) const {
    if(hasOption(name)){
      return getOption(name).validate(validator);
    } else {
      validator.validate(fallback);
      return fallback;
    }
  }

  static Options
  parseFromString(const std::string &options,
                  const std::string &separator = "@") throw(Exception);
};

} // namespace fformation
