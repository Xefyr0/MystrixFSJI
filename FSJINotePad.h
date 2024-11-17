/**
 * NotePad UI for playing in Full-scale Just Intonation.
 */
#pragma once

#include <cmath>
#include <numeric>
#include <unordered_set>

#include "MatrixOS.h"
#include "os/framework/MidiSpecs.h"
#include "ui/UI.h"

#include "Ratio.h"

#define MIDI_NOTE_COUNT 128

#define TONIC_FREQ 440L

#define CONCERT_A_FREQ 440L
#define CONCERT_A_MIDI_NOTE 69

#define COLOR_ANGULAR_STRETCH_FACTOR 2  // Stretches the center of the spectrum
#define COLOR_PHASE_OFFSET -0.4         // Shifts the entire spectrum towards purple, away from red
#define COLOR_RANGE_FACTOR 1.8          // Increases the range of colors viewed

struct FSJINotePadConfig {
  bool velocitySensitive = true;
  uint8_t channel = 0;
  EMidiPortID port = MIDI_PORT_USB;
};

class FSJINotePad : public UIComponent {
 private:
  // Semitones per unit of the MSB in the single-note tuning syses
  const double MSBConversion = 1.0L / (1 << 7);
  // Semitones per unit of the LSB in the single-note tuning sysex
  const double LSBConversion = 1.0L / (1 << 14);

  FSJINotePadConfig* config;
  uint8_t midiNoteTable[8][8];
  uint8_t midiSysExTable[8][8][12];
  Color buttonColorTable[8][8];

  // Set of all active notes. Defined in local space.
  // Must be uint8_t due to Point not having the functions necessary to be used in an unordered set.
  std::unordered_set<uint8_t> activeNotes;

 public:
  Dimension dimension;

  virtual Color GetColor() { return Color(0xFFFFFF); }
  virtual Dimension GetSize() { return dimension; }

  virtual bool Render(Point origin);
  virtual bool KeyEvent(Point buttonPos, KeyInfo* keyInfo);

  /**
   * Generate an table of MIDI note numbers corresponding to each grid button.
   * Uses Stern-Brocot tree to help ordering the rational numbers represented by each button on the grid
   */
  void GenerateMIDINoteTable(uint8_t (&midiNoteTable)[8][8]);
  /**
   * Generate an table of MIDI SysEx messages corresponding to each grid button.
   */
  void GenerateMIDISysExTable(uint8_t (&midiSysExTable)[8][8][12]);
  /**
   * Generate a table of default colors corresponding to each grid button.
   */
  void GenerateButtonColors(Color (&buttonColorTable)[8][8]);

  FSJINotePad(Dimension dimension, FSJINotePadConfig* config);

  ~FSJINotePad();
};
