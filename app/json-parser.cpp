/********************************************************************
**                                                                 **
** File   : app/json-parser.cpp                                    **
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

#include "Features.h"
#include "Fformation.h"
#include "GroundTruth.h"
#include "Settings.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

using fformation::Settings;
using fformation::Features;
using fformation::GroundTruth;
using fformation::Fformation;
using fformation::Classification;
using fformation::Position2D;
using fformation::Observation;
using fformation::Group;

void print_help(int exit_code) {
  std::cout << "Reads fformation-datasets as json.\n"
            << "  call: parser <path>" << std::endl;
  exit(exit_code);
}

void compareResult(const Observation &o, const Classification &c,
                   const GroundTruth &g, const Settings &s) {
  std::cout << "Classification resulted in:\n"
            << c
            << "\ndist-costs are: " << c.calculateDistanceCosts(o, s.stride())
            << "\n mdl-costs are: " << c.calculateMDLCosts(s.mdl())
            << "\n ocl-costs are: " << c.calculateVisibilityCosts(o, s.stride())
            << std::endl;
}

int main(const int argc, const char **args) {

  if (argc != 2) {
    std::cerr << "Not enough parameters passed." << std::endl;
    print_help(-1);
  } else {
    for (int i = 0; i < argc; ++i) {
      if (std::strcmp(args[i], "-h") == 0 ||
          std::strcmp(args[i], "--help") == 0) {
        print_help(0);
      }
    }
  }

  std::string path = args[1];
  std::string features_path = path + "/features.json";
  std::string groundtruth_path = path + "/groundtruth.json";
  std::string settings_path = path + "/settings.json";

  Features features = Features::readMatlabJson(features_path);
  GroundTruth groundtruth = GroundTruth::readMatlabJson(groundtruth_path);
  Settings settings = Settings::readMatlabJson(settings_path);

  for (auto observation : features.observations()) {
    Fformation ff(observation, settings);
    compareResult(observation, ff.classify(), groundtruth, settings);
  }
}
