MystrixFSJI is an application for MatrixOS to turn the Mystrix into a controller for Free-style Just Intonation.
The key features to support this goal are use of System Exclusive (SysEx) messages under the MIDI Tuning Standard (MTS) to convey tuning information over MIDI, and a custom-made UI.
Future objectives include transposition of the grid-shaped UI, and easy modulation into other keys using another controller.

To add this project to your MatrixOS firmware build:
1. Add this repository as a submodule in `MatrixOS/applications/MystrixFSJI`
2. Add `#include "applications/MystrixFSJI/FSJI.h"` in the USER APPLICATION section of `Applications.h`