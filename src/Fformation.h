/********************************************************************
**                                                                 **
** File   : src/Fformation.h                                       **
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
#include "Classification.h"
#include "Observation.h"
#include "Settings.h"

namespace fformation {

class Fformation {

public:
  Fformation(const Observation &observation, const Settings &settings,
             const SettingsGC &settingsgc)
      : _settings(settings), _settingsgc(settingsgc),
        _observation(observation) {}

  Classification classify();

private:
  Settings _settings;
  SettingsGC _settingsgc;
  Observation _observation;
};

} // namespace fformation
