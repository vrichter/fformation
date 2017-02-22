/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/io/Options.cpp                                    **
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

#include "gtest/gtest.h"

namespace {
using fformation::Option;
using fformation::Options;

Option one("name1", "value1");
Option two("name2", "value2");
Option three("name3", "value3");

TEST(OptionsTest, Explicit) {
  EXPECT_NO_THROW(Options());
  Options o1;
  o1.insert(one);
  o1.insert(two);

  EXPECT_TRUE(o1.hasOption(one.name()));
  EXPECT_EQ(one.value(), o1.getOption(one.name()).value());

  EXPECT_TRUE(o1.hasOption(two.name()));
  EXPECT_EQ(two.value(), o1.getOption(two.name()).value());

  EXPECT_FALSE(o1.hasOption(three.name()));
  EXPECT_THROW(o1.getOption(three.name()), fformation::Exception);
}

TEST(OptionsTest, ParseFromString) {
  EXPECT_NO_THROW(Options::parseFromString(""));
  EXPECT_THROW(Options::parseFromString("name=value=value"),
               fformation::Exception);
  EXPECT_THROW(Options::parseFromString("@="), fformation::Exception);

  Options o1 = Options::parseFromString("name1=value1@name2=value2");
  EXPECT_TRUE(o1.hasOption("name1"));
  EXPECT_EQ("value1", o1.getOption("name1").value());

  EXPECT_TRUE(o1.hasOption("name2"));
  EXPECT_EQ("value2", o1.getOption("name2").value());

  EXPECT_FALSE(o1.hasOption("name3"));
  EXPECT_THROW(o1.getOption("name3"), fformation::Exception);
}

TEST(OptionsTest, Override) {
  Options o;
  o.insert(one);
  o.insert(two);

  o.override(Option(one.name(), two.value()));
  EXPECT_TRUE(o.hasOption(one.name()));
  EXPECT_EQ(two.value(), o.getOption(one.name()).value());

  Options ov;
  ov.insert(three);
  o.override(ov);
  EXPECT_TRUE(o.hasOption(three.name()));
  EXPECT_EQ(three.value(), o.getOption(three.name()).value());
}
}
