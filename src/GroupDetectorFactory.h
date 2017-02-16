/********************************************************************
**                                                                 **
** File   : src/GroupDetectorFactory.h                             **
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
#include "GroupDetector.h"
#include "Options.h"
#include <functional>
#include <memory>

namespace fformation {

class GroupDetectorFactory {
public:
  typedef std::function<GroupDetector::Ptr(const Options &)>
      ConstructorFunction;

  GroupDetector::Ptr create(const std::string &config);

  GroupDetector::Ptr create(const std::string &name, const Options &options);

  GroupDetector::Ptr create(const Options &options);

  void addDetector(const std::string &name,
                   const ConstructorFunction &constructor);

  std::vector<std::string> listDetectors() const;

  static GroupDetectorFactory &getDefaultInstance();

private:
  std::map<std::string, ConstructorFunction> _detectors;
};

} // namespace fformation
