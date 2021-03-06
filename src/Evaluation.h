/********************************************************************
**                                                                 **
** File   : src/Evaluation.h                                       **
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

#pragma once
#include "ConfusionMatrix.h"
#include "Features.h"
#include "GroundTruth.h"
#include "GroupDetector.h"
#include "Options.h"
#include "Settings.h"

namespace fformation {

class Evaluation {
public:
  Evaluation(const Features &features, const GroundTruth &ground_truth,
             const Settings &settings, const GroupDetector &detector,
             const Options &options = Options());

  const std::vector<Classification> classifications() const {
    return _classifications;
  }
  const std::vector<Classification> groundTruths() const {
    return _ground_truths;
  }
  const std::vector<ConfusionMatrix> confusionMatrices() const {
    return _confusion_matrices;
  }
  const std::ostream &printOutput(std::ostream &out) const;

private:
  double _threshold = 2. / 3.;
  Options _options;
  std::vector<Observation> _observations;
  std::vector<Classification> _classifications;
  std::vector<Classification> _ground_truths;
  std::vector<ConfusionMatrix> _confusion_matrices;
  std::map<std::string, std::function<const std::ostream &(std::ostream &)>>
      _printers;
};

} // namespace fformation
