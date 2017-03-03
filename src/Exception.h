/********************************************************************
**                                                                 **
** File   : src/Exception.h                                        **
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
#include <exception>
#include <string>

namespace fformation {

class Exception : public std::exception {
public:
  Exception(const std::string &what) : _what(what) {}

  virtual const char *what() const noexcept { return _what.c_str(); }

  static void check(bool not_throw, const std::string &message) {
    if (!not_throw) {
      throw Exception(message);
    }
  }

private:
  const std::string _what;
};

class NotImplementedException : public Exception {
public:
  NotImplementedException(const std::string &what) : Exception(what + " not implemented.") {}
};

} // namespace fformation
