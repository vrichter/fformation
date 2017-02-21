/********************************************************************
**                                                                 **
** File   : src/GroupDetectorFactory.cpp                           **
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

#include "GroupDetectorFactory.h"
#include "Exception.h"
#include "GroupDetector.h"
#include <boost/tokenizer.hpp>

using fformation::GroupDetectorFactory;
using fformation::GroupDetector;
using fformation::Exception;
using fformation::Options;

GroupDetectorFactory fillDefault() {
  GroupDetectorFactory fac;
  fac.addDetector("one", [](const Options &opt) {
    return GroupDetector::Ptr(new fformation::OneGroupDetector());
  });
  fac.addDetector("none", [](const Options &opt) {
    return GroupDetector::Ptr(new fformation::NonGroupDetector());
  });
  return fac;
}

GroupDetectorFactory &GroupDetectorFactory::getDefaultInstance() {
  static GroupDetectorFactory defaultInstance = fillDefault();
  return defaultInstance;
}

GroupDetector::Ptr GroupDetectorFactory::create(const std::string &config) {
  auto name_and_options = Options::parseFromString(config);
  if (name_and_options.empty()) {
    throw Exception("Factory cannot create an instance from an empty config.");
  }
  auto name = *name_and_options.begin();
  auto options = Options(name_and_options.begin(), name_and_options.end());
  return create(name.name(), options);
}

GroupDetector::Ptr GroupDetectorFactory::create(const std::string &name,
                                                const Options &options) {
  auto it = _detectors.find(name);
  if (it == _detectors.end()) {
    std::stringstream error;
    error << "Unknown detector " << name
          << " requested. Please use one of the following: ( ";
    for (auto det : _detectors) {
      error << "'" << det.first << "' ";
    }
    error << ").";
    throw Exception(error.str());
  }
  return it->second(options);
}

GroupDetectorFactory &GroupDetectorFactory::addDetector(const std::string &name,
                                       const ConstructorFunction &constructor) {
  _detectors[name] = constructor;
  return *this;
}

std::vector<std::string> GroupDetectorFactory::listDetectors() const {
  std::vector<std::string> result;
  result.reserve(_detectors.size());
  for (auto det : _detectors) {
    result.push_back(det.first);
  }
  return result;
}
