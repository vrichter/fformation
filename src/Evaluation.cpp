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
#include "JsonSerializable.h"
#include <assert.h>
#include <iomanip>
#include <iostream>

using fformation::Evaluation;
using fformation::ConfusionMatrix;
using fformation::Timestamp;
using fformation::Classification;
using fformation::Observation;
using fformation::PersonId;
using fformation::Person;
using fformation::Group;
using fformation::IdGroup;
using fformation::Options;

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

static std::ostream &printMatlab(const Classification &cl, std::ostream &out,
                                 bool all = false) {
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

static const std::ostream &
printMatlabOutput(std::ostream &out, Options &options,
                  std::vector<Classification> &classifications,
                  std::vector<Classification> &ground_truths,
                  std::vector<ConfusionMatrix> &confusion_matrices) {
  bool print_perfect_matches =
      options.getValueOr<bool>("print_perfect_matches", true);
  bool print_all_persons = options.getValueOr<bool>("print_all_persons", false);
  bool print_confusion_matrix =
      options.getValueOr<bool>("print_confusion_matrix", false);
  assert(classifications.size() == confusion_matrices.size());
  assert(ground_truths.size() == confusion_matrices.size());
  for (size_t frame = 0; frame < classifications.size(); ++frame) {
    if (print_perfect_matches || !perfectMatch(confusion_matrices[frame])) {
      out << "Frame: " << frame + 1 << "/" << classifications.size() << "\n";
      out << "   FOUND:-- ";
      printMatlab(classifications[frame], out, print_all_persons);
      out << "\n";
      out << "   GT   :-- ";
      printMatlab(ground_truths[frame], out, print_all_persons);
      out << "\n";
      if (print_confusion_matrix) {
        out << "     TP: " << confusion_matrices[frame].true_positive() << "\n"
            << "     FP: " << confusion_matrices[frame].false_positive() << "\n"
            << "     TN: " << confusion_matrices[frame].true_negative() << "\n"
            << "     FN: " << confusion_matrices[frame].false_negative() << "\n"
            << "\n";
      }
    }
  }
  auto precision = ConfusionMatrix::calculateMeanPrecision(confusion_matrices);
  auto recall = ConfusionMatrix::calculateMeanRecall(confusion_matrices);
  out << std::setprecision(8) << std::fixed << "Average Precision: -- "
      << precision << "\n"
      << "Average Recall: -- " << recall << "\n"
      << "Average F1 score: -- "
      << ConfusionMatrix::calculateF1Score(precision, recall) << std::endl;
  return out;
}

static std::ostream &printGoupLine(const Classification &cl,
                                   std::ostream &out) {
  fformation::JsonSerializable::serializeIterable(out, cl.idGroups());
  return out;
}

static const std::ostream &
printTsvOutput(std::ostream &out, std::vector<Classification> &classifications,
               std::vector<Classification> &ground_truths,
               std::vector<ConfusionMatrix> &confusion_matrices,
               std::string s = "\t") {
  // header
  out << "id" << s;
  out << "timestamp" << s;
  out << "annotation" << s;
  out << "classification" << s;
  out << "tp" << s;
  out << "fp" << s;
  out << "tn" << s;
  out << "fn" << s;
  out << "\n";
  // observations
  for (size_t frame = 0; frame < classifications.size(); ++frame) {
    out << frame + 1 << s;
    out << classifications[frame].timestamp() << s;
    printGoupLine(ground_truths[frame], out);
    out << s;
    printGoupLine(classifications[frame], out);
    out << s;
    out << confusion_matrices[frame].true_positive() << s
        << confusion_matrices[frame].false_positive() << s
        << confusion_matrices[frame].true_negative() << s
        << confusion_matrices[frame].false_negative() << s;
    out << "\n";
  }
  return out;
}

static const std::ostream &printTsvParticipantsOutput(
    std::ostream &out, const std::vector<Observation> &observations,
    const std::vector<Classification> &classifications,
    const std::vector<Classification> &ground_truths,
    const std::vector<ConfusionMatrix> &confusion_matrices,
    const Options &detector_options, const std::string s = "\t") {
  assert(observations.size() == classifications.size());
  assert(observations.size() == ground_truths.size());
  assert(observations.size() == confusion_matrices.size());
  // returns the IdGroup of the person
  auto persons_group = [](PersonId pid,
                          const std::vector<Group> &groups) -> Group {
    for (auto g : groups) {
      if (g.persons().find(pid) != g.persons().end()) {
        return g;
      }
    }
    throw fformation::Exception("Person is not in groups");
  };
  // creates a confusion matrix for one pid and its group participants
  auto persons_cm = [](const PersonId &pid,
                       const std::vector<Person> &persons_in_frame,
                       const Group &cl, const Group &an) -> ConfusionMatrix {
    ConfusionMatrix::IntType tp = 0;
    ConfusionMatrix::IntType fp = 0;
    ConfusionMatrix::IntType tn = 0;
    ConfusionMatrix::IntType fn = 0;
    for (auto person : persons_in_frame) {
      if (person.id() != pid) {
        bool in_cl = cl.persons().find(person.id()) != cl.persons().end();
        bool in_an = an.persons().find(person.id()) != an.persons().end();
        tp += (in_cl & in_an) ? 1 : 0;
        fp += (in_cl & !in_an) ? 1 : 0;
        tn += (!in_cl & !in_an) ? 1 : 0;
        fn += (!in_cl & in_an) ? 1 : 0;
      }
    }
    return ConfusionMatrix(tp, fp, tn, fn);
  };
  auto stride = detector_options.getValue<Person::Stride>("stride");
  auto mdl = detector_options.getValue<Person::Stride>("mdl");
  out << "timestamp" << s << "pid" << s << "x" << s << "y" << s << "rad" << s
      << "gt.group.size" << s << "cl.group.size" << s << "tp" << s << "fp" << s
      << "tn" << s << "fn" << s << "cl.group.distance.cost" << s
      << "cl.group.visibility.cost" << s << "mdl" << s << "stride"
      << "\n";
  for (size_t frame = 0; frame < classifications.size(); ++frame) {
    const Observation &obs = observations[frame];
    const Classification &gt = ground_truths[frame];
    const Classification &cl = classifications[frame];
    const auto person_list = obs.group().generatePersonList();
    const auto ts = classifications[frame].timestamp();
    for (auto person : person_list) {
      out << ts << s;
      out << person.id() << s;
      out << person.pose().position().x() << s;
      out << person.pose().position().y() << s;
      out << person.pose().rotation() << s;
      const Group &gt_group =
          persons_group(person.id(), gt.createGroups(obs, true));
      const Group &cl_group =
          persons_group(person.id(), cl.createGroups(obs, true));
      out << gt_group.persons().size() << s;
      out << cl_group.persons().size() << s;
      const ConfusionMatrix pcm =
          persons_cm(person.id(), person_list, cl_group, gt_group);
      out << pcm.true_positive() << s;
      out << pcm.false_positive() << s;
      out << pcm.true_negative() << s;
      out << pcm.false_negative() << s;
      auto gc = gt_group.calculateCenter(stride);
      out << person.calculateDistanceCosts(gc, stride) << s;
      auto cl_group_participants = cl_group.generatePersonList();
      double visibility_cost = 0.;
      for (auto group_participant : cl_group_participants) {
        visibility_cost +=
            person.calculateVisibilityCost(gc, group_participant);
      }
      out << visibility_cost << s << mdl << s << stride << s << "\n";
    }
  }
  return out;
}

Evaluation::Evaluation(const Features &features,
                       const GroundTruth &ground_truth,
                       const Settings &settings, const GroupDetector &detector,
                       const Options &options)
    : _options(options), _observations(features.observations()) {
  // apply options
  if (options.hasOption("threshold")) {
    _threshold =
        options.getOption("threshold").validate(validators::Min<double>(0.));
  }
  // add printers
  _printers["matlab"] = [this](std::ostream &out) -> const std::ostream & {
    return printMatlabOutput(out, this->_options, this->_classifications,
                             this->_ground_truths, this->_confusion_matrices);
  };
  _printers["tsv"] = [this](std::ostream &out) -> const std::ostream & {
    return printTsvOutput(out, this->_classifications, this->_ground_truths,
                          this->_confusion_matrices);
  };
  auto detector_options = detector.options();
  _printers["tsv_participants"] =
      [this, detector_options](std::ostream &out) -> const std::ostream & {
    return printTsvParticipantsOutput(
        out, this->_observations, this->_classifications, this->_ground_truths,
        this->_confusion_matrices, detector_options);
  };
  // do the evaluation
  size_t counter = 0;
  for (auto obs : features.observations()) {
    if ((++counter % 100) == 0) {
      std::cerr << "processing observation #" << counter << " of #"
                << features.observations().size() << " ("
                << counter * 100. / (double)features.observations().size()
                << "%)" << std::endl;
    }
    const Classification *gt = ground_truth.findClassification(obs.timestamp());
    if (gt != nullptr) {
      try {
        auto observation = modifyObservation(obs, *gt, options);
        auto cl = detector.detect(observation);
        auto cf = cl.createConfusionMatrix(*gt, _threshold);
        _ground_truths.push_back(*gt);
        _classifications.push_back(cl);
        _confusion_matrices.push_back(cf);
      } catch (const Exception &e) {
        std::cerr << "Classification failed: " << e.what() << std::endl;
      }
    }
  }
}
const std::ostream &Evaluation::printOutput(std::ostream &out) const {
  std::string printer_name =
      _options.getValueOr<std::string>("evaluation_printer", "matlab");
  auto callback = _printers.find(printer_name);
  if (callback != _printers.end()) {
    return callback->second(out);
  } else {
    std::stringstream err;
    err << "Could not find an output_printer with name = '" << printer_name
        << "'. Known printers: ";
    for (auto it : _printers) {
      err << it.first << ", ";
    }
    throw Exception(err.str());
  }
}
