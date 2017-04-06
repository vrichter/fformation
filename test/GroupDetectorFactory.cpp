/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/GroupDetectorFactory.cpp                          **
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

#include "gtest/gtest.h"

namespace {
using fformation::GroupDetectorFactory;
using fformation::GroupDetector;
using fformation::OneGroupDetector;
using fformation::NonGroupDetector;
using fformation::Options;
using fformation::Option;
using fformation::Classification;
using fformation::Observation;

class OptionsKeeper : public GroupDetector {
public:
  OptionsKeeper(const Options &options) : GroupDetector(options) {}

  virtual Classification detect(const Observation &observation) const final {
    return Classification(observation.timestamp(), {});
  }
};

TEST(GroupDetectorFactory, DefaultInstance) {
  auto &inst = GroupDetectorFactory::getDefaultInstance();
  EXPECT_EQ(&inst, &GroupDetectorFactory::getDefaultInstance());

  GroupDetector::Ptr det;
  EXPECT_NO_THROW(det = inst.create("one"));
  EXPECT_NE(nullptr, det.get());
  EXPECT_NO_THROW(det = inst.create("none"));
  EXPECT_NE(nullptr, det.get());
}

TEST(GroupDetectorFactory, AddConstructors) {
  GroupDetectorFactory inst;
  EXPECT_THROW(inst.create("one"), fformation::Exception);

  inst.addDetector("test", [](const Options &opt) {
    return GroupDetector::Ptr(new OptionsKeeper(opt));
  });

  GroupDetector::Ptr detector;
  EXPECT_NO_THROW(detector = inst.create("test"));
  EXPECT_EQ(0u, detector->options().size());

  Option option("name", "value");
  Options options;
  options.insert(option);
  EXPECT_NO_THROW(detector = inst.create("test", options));
  EXPECT_EQ(options.size(), detector->options().size());
  EXPECT_EQ("value", detector->options().getOption("name").value());

  EXPECT_NO_THROW(detector = inst.create("test@name"));
  EXPECT_EQ(1u, detector->options().size());
  EXPECT_EQ("", detector->options().getOption("name").value());

  EXPECT_NO_THROW(detector = inst.create("test@name=value"));
  EXPECT_EQ(1u, detector->options().size());
  EXPECT_EQ("value", detector->options().getOption("name").value());
}
}
