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

using fformation::Evaluation;
using fformation::ConfusionMatrix;
using fformation::Timestamp;
using fformation::Classification;

Evaluation::Evaluation(const Features &features,
                       const GroundTruth &ground_truth,
                       const Settings &settings, const GroupDetector &detector,
                       const Options &options) {
  // apply options
  if (options.hasOption("threshold")) {
    _threshold = options.getOption("threshold")
                     .convertValue<double>(validators::MinMax<double>(0.,1.));
  }
  // do the evaluation
  for (auto obs : features.observations()) {
    const Classification *gt = ground_truth.findClassification(obs.timestamp());
    if (gt != nullptr) {
      _ground_truths.push_back(*gt);
      _classifications.push_back(detector.detect(obs));
      _confusion_matrices.push_back(
          _classifications.back().createConfusionMatrix(*gt, _threshold));
    }
  }
}

static std::ostream &printMatlab(const Classification &cl, std::ostream &out) {
  for (auto group : cl.idGroups()) {
    if (group.persons().size() <= 1)
      continue;
    for (auto person : group.persons()) {
      out << " " << person;
    }
    out << " |";
  }
  return out;
}

const std::ostream &Evaluation::printMatlabOutput(std::ostream &out) const {
  assert(_classifications.size() == _confusion_matrices.size());
  assert(_ground_truths.size() == _confusion_matrices.size());
  for (size_t frame = 0; frame < _classifications.size(); ++frame) {
    out << "Frame: " << frame + 1 << "/" << _classifications.size() << "\n";
    out << "   FOUND:-- ";
    printMatlab(_classifications[frame], out);
    out << "\n";
    out << "   GT   :-- ";
    printMatlab(_ground_truths[frame], out);
    out << "\n";
  }
  auto precision = ConfusionMatrix::calculateMeanPrecision(_confusion_matrices);
  auto recall = ConfusionMatrix::calculateMeanRecall(_confusion_matrices);
  out << "Average Precision: -- " << precision << "\n"
      << "Average Recall: -- " << recall << "\n"
      << "Average F1 score: -- "
      << ConfusionMatrix::calculateF1Score(precision, recall) << std::endl;
  return out;
}
