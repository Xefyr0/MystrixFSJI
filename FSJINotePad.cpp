#include "FSJINotePad.h"

bool FSJINotePad::Render(Point origin) {
  for (int8_t y = 0; y < dimension.y; y++)
  {
    for (int8_t x = 0; x < dimension.x; x++)
    {
      // Row-major index of each button
      // Runs 0 to 64, assuming the UI takes up the whole controller
      uint8_t buttonID = y * dimension.x + x;

      Point localPos = Point(x, y);
      Point globalPos = origin + localPos;

      if (activeNotes.find(buttonID) != activeNotes.end())
      {
        MatrixOS::LED::SetColor(globalPos, Color(0xFFFFFF));
      }
      else
      {
        MatrixOS::LED::SetColor(globalPos, buttonColorCache[x][y]);
      }
    }
  }
  return true;
}

bool FSJINotePad::KeyEvent(Point buttonPos, KeyInfo* keyInfo) {
  uint8_t buttonID = buttonPos.y * dimension.x + buttonPos.x;

  // char outMessage[100];
  // sprintf(outMessage, "Coordinates: (%d, %d) Color: %06X", 0, 0, (unsigned int)buttonColorCache[0][0].RGB());
  // MLOGD("FSJI", outMessage);

  if (keyInfo->state == PRESSED)
  {
    MatrixOS::MIDI::Send(MidiPacket(config->port, NoteOn, config->channel, midiNoteTable[buttonPos.x][buttonPos.y],
                                    config->velocitySensitive ? keyInfo->velocity.to7bits() : 0x7F));
    activeNotes.emplace(buttonID);
  }
  else if (keyInfo->state == RELEASED)
  {
    MatrixOS::MIDI::Send(MidiPacket(config->port, NoteOff, config->channel, midiNoteTable[buttonPos.x][buttonPos.y],
                                    config->velocitySensitive ? keyInfo->velocity.to7bits() : 0x7F));
    activeNotes.erase(buttonID);
  }
  return true;
}

void FSJINotePad::GenerateButtonColors(Color**& colorCache) {
  // Initialize cachedColors to an empty 2D array
  colorCache = new Color*[dimension.x];
  for (int i = 0; i < dimension.x; i++)
  {
    colorCache[i] = new Color[dimension.y];
  }

  // Fill out array of cached colors to avoid doing fun (difficult) computation in realtime
  for (int8_t x = 0; x < dimension.x; x++)
  {
    for (int8_t y = 0; y < dimension.y; y++)
    {
      Ratio buttonRatio = Ratio(x + 1, y + 1);
      float floatRatio = (float)buttonRatio.n / (float)buttonRatio.d;

      // Highlight doubled ratios
      if (buttonRatio.n - 1 < dimension.x / 2 && buttonRatio.d - 1 < dimension.y / 2)
      {
        // H, S, and V are 0...1 so we create hue with that range
        float hue = atan((log2(floatRatio) + COLOR_PHASE_OFFSET) / COLOR_ANGULAR_STRETCH_FACTOR) * COLOR_RANGE_FACTOR / M_PI + 0.5f;
        colorCache[x][y] = Color::HsvToRgb(hue, 1.0f, 0.5f);
      }
      // Other buttons are left dark
      else
        colorCache[x][y] = Color(0x000000);
    }
  }
}

void FSJINotePad::FreeButtonColors(Color**& colorCache) {
  for (int i = 0; i < dimension.x; i++)
  {
    delete[] colorCache[i];
  }
  delete[] colorCache;
}

FSJINotePad::FSJINotePad(Dimension dimension, FSJINotePadConfig* config) {
  MLOGD("FSJI", "FSJINotePad Constructor");
  this->dimension = dimension;
  this->config = config;

  GenerateButtonColors(buttonColorCache);

  // Zero-indexed root, so to store 1 MIDI note we need a tree of depth 0; to store 2-3 MIDI notes we need a tree of depth 1.
  // 6 when MIDI_NOTE_COUNT is 128.
  const uint8_t traverseDepth = floor(log(MIDI_NOTE_COUNT) / log(2));

  struct SternBrocotGeneratorInfo {
    Ratio L;
    Ratio C;
    Ratio R;
    uint8_t depth;
    uint8_t MIDINote;
  };

  // Generate MIDI notes using Stern-Brocot tree to help ordering the rational number representations of all the irreducible points on the grid
  queue<SternBrocotGeneratorInfo> SBTreeLeaves;

  SBTreeLeaves.push(SternBrocotGeneratorInfo(Ratio(0, 1), Ratio(1, 1), Ratio(1, 0), 0, 64));

  while (!SBTreeLeaves.empty())
  {
    // Get another node from the list of leaves
    SternBrocotGeneratorInfo curNode = SBTreeLeaves.front();

    // Add MIDI note to table if it fits (Array is zero-indexed, Points here are not!)
    uint8_t x = curNode.C.n - 1;
    uint8_t y = curNode.C.d - 1;
    if (x < dimension.x && y < dimension.y)
    {
      midiNoteTable[x][y] = curNode.MIDINote;
      // char outMessage[100];
      // sprintf(outMessage, "Coordinates: (%d, %d) MIDI note: %d", x, y, curNode.MIDINote);
      // MLOGD("FSJI", outMessage);
    }

    // Don't generate any more children if at or beyond maximum depth
    if (curNode.depth >= traverseDepth)
    {
      SBTreeLeaves.pop();
      continue;
    }

    // Some temp vars to help generate children
    uint8_t a = curNode.L.n;
    uint8_t b = curNode.L.d;
    uint8_t c = curNode.C.n;
    uint8_t d = curNode.C.d;
    uint8_t e = curNode.R.n;
    uint8_t f = curNode.R.d;

    // Generate and add children from stored information
    uint8_t newDepth = curNode.depth + 1;
    uint8_t LMIDINote = curNode.MIDINote - pow(2, (traverseDepth - newDepth) - 1);
    uint8_t RMIDINote = curNode.MIDINote + pow(2, (traverseDepth - newDepth) - 1);
    SBTreeLeaves.push(SternBrocotGeneratorInfo(Ratio(a, b), Ratio(a + c, b + d), Ratio(c, d), newDepth, LMIDINote));
    SBTreeLeaves.push(SternBrocotGeneratorInfo(Ratio(c, d), Ratio(c + e, d + f), Ratio(e, f), newDepth, RMIDINote));

    // Remove current node from leaves
    SBTreeLeaves.pop();
  }

  // Fill in the remaining values
  for (int x = 0; x < dimension.x; x++)
  {
    for (int y = 0; y < dimension.y; y++)
    {
      Ratio buttonRatio = Ratio(x + 1, y + 1);

      // Copy over MIDI note number if the fraction is reducible
      if (x != buttonRatio.n - 1 || y != buttonRatio.d - 1)
      {
        midiNoteTable[x][y] = midiNoteTable[buttonRatio.n - 1][buttonRatio.d - 1];
      }
    }
  }

  activeNotes.reserve(8);
}

FSJINotePad::~FSJINotePad() {
  // NO MEMORY LEAKS IN THIS HOUSE
  FreeButtonColors(buttonColorCache);
  activeNotes.~unordered_set();
}