# fformation

Detects social groups in sets of person percepts.

## How do I get set up? ###

    > git clone
    > mkdir -p fformation/build && cd fformation/build
    > cmake .. && make

## Applications

### fformation-evaluation

```bash
Allowed options:
  -h [ --help ]                         produce help message
  -c [ --classificator ] arg (=list)    Which classificator should be used for
                                        evaluation. Possible:  ( grow | none |
                                        one | )
  -e [ --evaluation ] arg (=threshold=0.6666)
                                        May be used to override evaluation
                                        settings and default settings from
                                        settings.json
  -d [ --dataset ] arg                  The root path of the evaluation
                                        dataset. The path is expected to
                                        contain features.json, groundtruth.json
                                        and settings.json
```

This application uses an evaluation dataset to evaluate a specific classificator
implementation. The dataset must be formatted as json and can be obtained from
[group-assignment-datasets](https://github.com/vrichter/group-assignment-datasets).

## Classificators

Different classificators can be implemented and chosen at runtime using the
generic `GroupDetector` interface and the corresponding `GroupDetectorFactory`.
Currently available classificators are:

#### none

A baseline classification that assigns every person to their own group.

#### one

A baseline classification that assigns all persons to a single group.

#### grow

An EM-Based classification that starts from a single group and then alternates
between optimizing the group center positions of the current assignment and
adding a new group for the person with the highest assignment cost until
convergence.

#### ...more

[fformation-gco](https://github.com/vrichter/fformation-gco)
implements the group assignment using a multi label graph-cuts optimization from
[1] as proposed in [2] and the corresponding matlab code
([GCFF](https://github.com/franzsetti/GCFF)) using the C++ implementation from [gco-v3.0](https://github.com/vrichter/gco-v3.0).

## Citations

> [1] Delong A, Osokin A, Isack H. N., Boykov Y (2010) "Fast Approximate Energy Minimization with Label Costs". In CVPR.

<p></p>

> [2] Setti F, Russell C, Bassetti C, Cristani M (2015) "F-Formation Detection:
Individuating Free-Standing Conversational Groups in Images". In PLoS ONE 10(5):
e0123783. [doi:10.1371/journal.pone.0123783](http://dx.doi.org/10.1371/journal.pone.0123783)

### 3rd party software used

* [RSB](https://code.cor-lab.de/projects/rsb "Robotics Service Bus") will be used
in future versions.

* [Boost](http://www.boost.org/ "Boost C++ Libraries") because it is boost.

## Copyright

GNU LESSER GENERAL PUBLIC LICENSE

This project may be used under the terms of the GNU Lesser General
Public License version 3.0 as published by the
Free Software Foundation and appearing in the file LICENSE.LGPL
included in the packaging of this project.  Please review the
following information to ensure the license requirements will
be met: http://www.gnu.org/licenses/lgpl-3.0.txt
