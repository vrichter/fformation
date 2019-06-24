/********************************************************************
**                                                                 **
** File   : src/JsonSerializable.h                                 **
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
#include "OstreamPrinter.h"
#include <ostream>

namespace fformation {

class JsonSerializable {
public:
  virtual void serializeJson(std::ostream &out) const = 0;

  virtual void print(std::ostream &out) const final { serializeJson(out); }

  template <typename T>
  static void serializeIterable(std::ostream &out, const T &iterable) {
    if (iterable.begin() == iterable.end()) {
      out << "[ ]";
    } else {
      auto it = iterable.begin();
      out << "[ ";
      for (;;) {
        out << *it;
        if (++it != iterable.end()) {
          out << ", ";
        } else {
          break;
        }
      }
      out << " ]";
    }
  }
  template <typename T>
  static void serializeMapAsVector(std::ostream &out, const T &map) {
    if (map.begin() == map.end()) {
      out << "[ ]";
    } else {
      auto it = map.begin();
      out << "[ ";
      for (;;) {
        out << it->second;
        if (++it != map.end()) {
          out << ", ";
        } else {
          break;
        }
      }
      out << " ]";
    }
  }
};

} // namespace fformation
