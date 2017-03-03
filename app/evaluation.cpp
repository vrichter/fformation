/********************************************************************
**                                                                 **
** File   : app/evaluation.cpp                                     **
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

#include "Evaluation.h"
#include "Features.h"
#include "GroundTruth.h"
#include "GroupDetectorFactory.h"
#include "Settings.h"
#include <boost/program_options.hpp>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

using fformation::Settings;
using fformation::Features;
using fformation::GroundTruth;
using fformation::Classification;
using fformation::Position2D;
using fformation::Observation;
using fformation::Group;
using fformation::Evaluation;
using fformation::GroupDetector;
using fformation::GroupDetectorFactory;
using fformation::ConfusionMatrix;
using fformation::Option;
using fformation::Options;

auto &factory = GroupDetectorFactory::getDefaultInstance();

static std::string getClassificators(std::string prefix) {
  std::stringstream str;
  str << prefix;
  str << " ( ";
  for (auto cl : GroupDetectorFactory::getDefaultInstance().listDetectors()) {
    str << cl << " | ";
  }
  str << ")";
  return str.str();
}

int main(const int argc, const char **args) {
  boost::program_options::variables_map program_options;
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce help message");
  desc.add_options()(
      "classificator,c",
      boost::program_options::value<std::string>()->default_value("list"),
      getClassificators(
          "Which classificator should be used for evaluation. Possible: ")
          .c_str());
  desc.add_options()(
      "evaluation,e",
      boost::program_options::value<std::string>()->default_value(
          "threshold=0.6666@modify_rotations=keep@modify_proportion=0.75@seed=0"),
      "May be used to override evaluation settings and default settings "
      "from settings.json");
  desc.add_options()(
      "dataset,d", boost::program_options::value<std::string>()->required(),
      "The root path of the evaluation dataset. The path is expected "
      "to contain features.json, groundtruth.json and settings.json");
  try {
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, args, desc),
        program_options);
    boost::program_options::notify(program_options);
  } catch (const std::exception &e) {
    std::cerr << "Error while parsing command line parameters:\n\t" << e.what()
              << "\n";
    std::cerr << desc << std::endl;
    return 1;
  }

  if (program_options.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  std::string path = program_options["dataset"].as<std::string>();
  std::string features_path = path + "/features.json";
  std::string groundtruth_path = path + "/groundtruth.json";
  std::string settings_path = path + "/settings.json";
  Settings settings = Settings::readMatlabJson(settings_path);

  auto config = GroupDetectorFactory::parseConfig(
      program_options["classificator"].as<std::string>());
  config.second.insert(Option("stride", settings.stride()));
  config.second.insert(Option("mdl", settings.mdl()));

  GroupDetector::Ptr detector = factory.create(config.first, config.second);

  Features features = Features::readMatlabJson(features_path);
  GroundTruth groundtruth = GroundTruth::readMatlabJson(groundtruth_path);

  Evaluation evaluation(features, groundtruth, settings, *detector.get(),
                        Options::parseFromString(
                            program_options["evaluation"].as<std::string>()));
  evaluation.printMatlabOutput(std::cout, false);
}
