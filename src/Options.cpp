/********************************************************************
**                                                                 **
** File   : src/Options.cpp                                        **
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

#include "Options.h"
#include "Exception.h"
#include <boost/tokenizer.hpp>

using fformation::Option;
using fformation::Options;
using fformation::Exception;

namespace fformation {
template <> std::string Option::convertValue() const { return _value; }
}

class OptionParserException : public Exception {
public:
  OptionParserException(const std::string &message)
      : Exception("Options parser exception: " + message) {}
};

static Option parseOption(const std::string &option) {
  boost::char_separator<char> option_separator("=");
  boost::tokenizer<boost::char_separator<char>> tokens(option,
                                                       option_separator);
  if (tokens.begin() == tokens.end()) {
    throw OptionParserException("Option needs at least a name. Option: '" +
                                option + "'");
  }
  Option::NameType name = *tokens.begin();
  if (name.empty()) {
    throw OptionParserException("Option needs a non-empty name. Option: '" +
                                option + "'");
  }
  auto value_it = tokens.begin();
  if (++value_it != tokens.end()) {
    Option::ValueType value = *value_it;
    if (value.empty()) {
      throw OptionParserException("Option a non-empty value. Option: '" +
                                  option + "'");
    }
    if (++value_it != tokens.end()) {
      throw OptionParserException(
          "Option cannot have multiple values. Option: '" + option + "'");
    }
    return Option(name, value);
  }
  return Option(name);
}

void Options::override(const Options &other) {
  for (Option o : other) {
    override(o);
  }
}

void Options::override(const Option &other) {
  auto it = this->find(other.name());
  if (it == this->end()) {
    this->insert(other);
  } else if (it->value() != other.value()) {
    this->erase(it);
    this->insert(other);
  }
}

const bool Options::hasOption(const Option::NameType &name) const {
  return this->find(name) != this->end();
}

const Option &Options::getOption(const Option::NameType &name) const
    throw(Exception) {
  auto it = this->find(name);
  if (it != this->end()) {
    return *it;
  } else {
    throw Exception("Option with name '" + name + "' not found.");
  }
}

const Option Options::getOptionOr(const Option::NameType &name, const Option::ValueType &value) const {
  if(hasOption(name)){
    return getOption(name);
  } else {
    return Option(name,value);
  }
}

Options
Options::parseFromString(const std::string &options,
                         const std::string &separator) throw(Exception) {
  Options result;
  boost::char_separator<char> option_separator(separator.c_str());
  boost::tokenizer<boost::char_separator<char>> tokens(options,
                                                       option_separator);
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    auto option = parseOption(*it);
    if (result.hasOption(option.name())) {
      throw OptionParserException("Opiton " + option.name() +
                                  " provided multiple times.");
    }
    result.insert(option);
  }
  return result;
}
