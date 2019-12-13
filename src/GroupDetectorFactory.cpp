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
#include "GroupDetectorsEM.h"
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
  fac.addDetector("grow", [](const Options &opt) {
    return GroupDetector::Ptr(new fformation::GroupDetectorGrow(opt));
  });
  fac.addDetector("shrink", [](const Options &opt) {
    return GroupDetector::Ptr(new fformation::GroupDetectorShrink(opt));
  });
  fac.addDetector("shrink2", [](const Options &opt) {
    return GroupDetector::Ptr(new fformation::GroupDetectorShrink2(opt));
  });
  return fac;
}

GroupDetectorFactory &GroupDetectorFactory::getDefaultInstance() {
  static GroupDetectorFactory defaultInstance = fillDefault();
  return defaultInstance;
}

GroupDetector::Ptr GroupDetectorFactory::create(
    const std::pair<std::string, Options> &config) const {
  return create(config.first, config.second);
}

GroupDetector::Ptr
GroupDetectorFactory::create(const std::string &config) const {
  return create(parseConfig(config));
}

GroupDetector::Ptr GroupDetectorFactory::create(const std::string &name,
                                                const Options &options) const {
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

GroupDetectorFactory &
GroupDetectorFactory::addDetector(const std::string &name,
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

std::pair<std::string, Options>
GroupDetectorFactory::parseConfig(const std::string &config) {
  size_t pos = config.find('@');
  std::string name = config.substr(0, pos);
  if (name.empty()) {
    throw Exception("Factory cannot create an instance from an empty config.");
  }
  std::string options_str = (pos < config.size()) ? config.substr(pos) : "";
  return std::make_pair(name, Options::parseFromString(options_str));
}
