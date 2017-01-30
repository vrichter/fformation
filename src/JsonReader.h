/********************************************************************
**                                                                 **
** File   : src/JsonReader.h                               **
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
#include "dep/json.hpp"
#include <map>
#include <string>
#include <vector>

// Forward declaration of the used json type.
namespace fformation {

typedef nlohmann::json Json;

class JsonReader {
public:
  static Json readFile(const std::string filename);

  template <typename T> static T createFromJson(const Json &json);

};

} // namespace fformation
