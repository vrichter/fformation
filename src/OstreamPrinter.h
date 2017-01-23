/********************************************************************
**                                                                 **
** File   : src/OstreamPrinter.h                                   **
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
#include <ostream>

namespace fformations {

/**
 * Print-function to ostream printer template.
 */
template <class T>
auto operator<<(std::ostream &out, const T &data)
    -> decltype(data.print(out), out) {
  data.print(out);
  return out;
}

} // namespace fformations
