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
#include <iostream>

using fformation::Features;
using fformation::GroundTruth;
using fformation::Settings;
using fformation::SettingsGC;
using fformation::Classification;
using fformation::Fformation;

void print_help(int exit_code) {
  std::cout << "Reads fformation-datasets as json.\n"
            << "  call: parser <path_to_dataset>" << std::endl;
  exit(exit_code);
}

void compareResult(const Classification &c, const GroundTruth &g,
                   const Settings &s) {
  return;
}

int main(const int argc, const char **args) {

  if (argc < 2) {
    std::cerr << "Not enough parameters passed." << std::endl;
    print_help(-1);
  } else {
    for (int i = 1; i < argc; ++i) {
      if (args[i] == std::string("-h") || args[i] == std::string("--help")) {
        print_help(0);
      }
    }
  }

  std::string path = args[1];
  std::string features_path = path + "/features.json";
  std::string groundtruth_path = path + "/groundtruth.json";
  std::string settings_path = path + "/settings.json";
  std::string settingsgc_path = path + "/settings_gc.json";

  Features features = Features::readMatlabJson(features_path);
  GroundTruth groundtruth = GroundTruth::readMatlabJson(groundtruth_path);
  Settings settings = Settings::readMatlabJson(settings_path);
  SettingsGC settingsgc = SettingsGC::readMatlabJson(settingsgc_path);

  for (auto observation : features.observations()) {
    Fformation ff(observation, settings, settingsgc);
    compareResult(ff.classify(), groundtruth, settings);
  }
}
