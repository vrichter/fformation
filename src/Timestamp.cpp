/********************************************************************
**                                                                 **
** File   : src/Timestamp.cpp                                      **
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

#include "Timestamp.h"
#include <limits>
#include <sstream>

using fformation::Timestamp;

void Timestamp::serializeJson(std::ostream &out) const {
  std::stringstream s;
  s.precision(std::numeric_limits<double>::max_digits10);
  s << std::scientific << _timestamp;
  out << s.str();
}
