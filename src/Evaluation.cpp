/********************************************************************
**                                                                 **
** File   : src/Evaluation.cpp                                     **
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
#include <assert.h>
#include <iostream>

using fformation::Evaluation;
using fformation::ConfusionMatrix;
using fformation::Timestamp;
using fformation::Classification;
using fformation::Observation;
using fformation::PersonId;
using fformation::Person;
using fformation::Group;

static Person withoutRotation(const Person &person) {
  return Person(person.id(), {person.pose().position()});
}

std::map<PersonId, Person>
removeAllRotations(const std::map<PersonId, Person> &group) {
  std::map<PersonId, Person> result;
  for (auto it : group) {
    result.insert(std::make_pair(it.first, withoutRotation(it.second)));
  }
  return result;
}

template <bool ROTATION>
static std::vector<Person>
filterPersons(const std::map<PersonId, Person> &group) {
  std::vector<Person> result;
  for (auto it : group) {
    if (it.second.pose().rotation() && ROTATION) {
      result.push_back(it.second);
    } else if (!it.second.pose().rotation() && !ROTATION) {
      result.push_back(it.second);
    }
  }
  return result;
}

static size_t howManyToRemove(size_t with_size, size_t without_size,
                              double proportion) {
  if (proportion >= 1.) {
    return size_t(proportion);
  } else {
    // find out how many need to be removed to acchieve required proportion.
    double remove =
        double(with_size) - proportion * double(with_size + without_size);
    if (remove <= 0.) {
      return 0;
    } else {
      return std::round<size_t>(remove + 0.5); // always round up
    }
  }
}

static std::vector<Person> removeRotations(std::vector<Person> persons,
                                           size_t num, size_t seed) {
  std::mt19937 generator(seed);
  std::shuffle(persons.begin(), persons.end(), generator);
  if (num > persons.size()) {
    num = persons.size();
  }
  for (size_t i = 0; i < num; ++i) {
    persons[i] = withoutRotation(persons[i]);
  }
  return persons;
}

std::map<PersonId, Person>
removeRandomRotations(const std::map<PersonId, Person> &group, double portion,
                      size_t seed) {
  auto with_rotations = filterPersons<true>(group);
  auto without_rotations = filterPersons<false>(group);
  auto filtered = removeRotations(
      with_rotations,
      howManyToRemove(with_rotations.size(), without_rotations.size(), portion),
      seed);
  std::map<PersonId, Person> result;
  for (auto it : without_rotations) {
    result.insert(std::make_pair(it.id(), it));
  }
  for (auto it : filtered) {
    result.insert(std::make_pair(it.id(), it));
  }
  return result;
}

std::map<PersonId, Person>
removeRandomRotationsGrouped(const std::vector<Group> &groups, double portion,
                             size_t seed) {
  std::map<PersonId, Person> result;
  for (auto group : groups) {
    auto filtered = removeRandomRotations(group.persons(), portion, seed);
    result.insert(filtered.begin(), filtered.end());
  }
  return result;
}

static Observation modifyObservation(const Observation &o,
                                     const Classification &gt,
                                     const fformation::Options &options) {
  std::string mod_by = options.getValueOr<std::string>(
      "modify_rotations", "keep", fformation::validators::OneOf<std::string>(
                                      {"keep", "remove", "group", "random"}));
  if (mod_by == "keep") {
    return o;
  }
  if (mod_by == "remove") {
    return Observation(o.timestamp(), removeAllRotations(o.group().persons()));
  }
  double mod_part = options.getValue<double>(
      "modify_proportion", fformation::validators::MinMax<double>(0., 1.));
  size_t seed = options.getValueOr<size_t>("seed", 0);
  if (mod_by == "group") {
    return Observation(o.timestamp(), removeRandomRotationsGrouped(
                                          gt.createGroups(o), mod_part, seed));
  }
  if (mod_by == "random") {
    return Observation(o.timestamp(), removeRandomRotations(o.group().persons(),
                                                            mod_part, seed));
  }
  throw fformation::Exception("Unknown config 'modify_rotations'='" + mod_by +
                              "'");
}

Evaluation::Evaluation(const Features &features,
                       const GroundTruth &ground_truth,
                       const Settings &settings, const GroupDetector &detector,
                       const Options &options)
    : _options(options) {
  // apply options
  if (options.hasOption("threshold")) {
    _threshold =
        options.getOption("threshold").validate(validators::Min<double>(0.));
  }
  // do the evaluation
  for (auto obs : features.observations()) {
    const Classification *gt = ground_truth.findClassification(obs.timestamp());
    if (gt != nullptr) {
      try {
        auto observation = modifyObservation(obs, *gt, options);
        auto cl = detector.detect(observation);
        auto cf = cl.createConfusionMatrix(*gt,_threshold);
        _ground_truths.push_back(*gt);
        _classifications.push_back(cl);
        _confusion_matrices.push_back(cf);
      } catch (const Exception &e) {
        std::cerr << "Classification failed: " << e.what() << std::endl;
      }
    }
  }
}

static std::ostream &printMatlab(const Classification &cl, std::ostream &out, bool all=false) {
  for (auto group : cl.idGroups()) {
    if (group.persons().size() <= 1 && !all)
      continue;
    for (auto person : group.persons()) {
      out << " " << person;
    }
    out << " |";
  }
  return out;
}

static bool perfectMatch(const ConfusionMatrix &m) {
  return m.false_negative() == 0 && m.false_positive() == 0;
}

const std::ostream &Evaluation::printMatlabOutput(std::ostream &out) const {
  bool print_perfect_matches = _options.getValueOr<bool>("print_perfect_matches",true);
  bool print_all_persons = _options.getValueOr<bool>("print_all_persons",false);
  bool print_confusion_matrix = _options.getValueOr<bool>("print_confusion_matrix",false);
  assert(_classifications.size() == _confusion_matrices.size());
  assert(_ground_truths.size() == _confusion_matrices.size());
  for (size_t frame = 0; frame < _classifications.size(); ++frame) {
    if (print_perfect_matches || !perfectMatch(_confusion_matrices[frame])) {
      out << "Frame: " << frame + 1 << "/" << _classifications.size() << "\n";
      out << "   FOUND:-- ";
      printMatlab(_classifications[frame], out, print_all_persons);
      out << "\n";
      out << "   GT   :-- ";
      printMatlab(_ground_truths[frame], out, print_all_persons);
      out << "\n";
      if(print_confusion_matrix){
        out << "     TP: " << _confusion_matrices[frame].true_positive() << "\n"
            << "     FP: " << _confusion_matrices[frame].false_positive() << "\n"
            << "     TN: " << _confusion_matrices[frame].true_negative() << "\n"
            << "     FN: " << _confusion_matrices[frame].false_negative() << "\n"
            << "\n";
      }
    }
  }
  auto precision = ConfusionMatrix::calculateMeanPrecision(_confusion_matrices);
  auto recall = ConfusionMatrix::calculateMeanRecall(_confusion_matrices);
  out << "Average Precision: -- " << precision << "\n"
      << "Average Recall: -- " << recall << "\n"
      << "Average F1 score: -- "
      << ConfusionMatrix::calculateF1Score(precision, recall) << std::endl;
  return out;
}
