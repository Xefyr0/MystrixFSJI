/**
 * NotePad UI for playing in Full-scale Just Intonation.
 */

#include <cmath>
#include <numeric>
#include <unordered_set>

#include "MatrixOS.h"
#include "os/framework/MidiSpecs.h"
#include "ui/UI.h"

#include "Ratio.h"

#define MIDI_NOTE_COUNT 128

class FSJINotePad : public UIComponent {
 private:
  uint8_t midiNoteTable[8][8];

 public:
  Dimension dimension;

  // Set of all active notes. Defined in local space.
  // Must be uint8_t due to Point not having the functions necessary to be used in an unordered set.
  std::unordered_set<uint8_t> activeNotes;

  virtual Color GetColor() { return Color(0xFFFFFF); }
  virtual Dimension GetSize() { return dimension; }

  virtual bool Render(Point origin) {
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
          MatrixOS::LED::SetColor(globalPos, Color(0x000000));
        }
      }
    }
    return true;
  }

  virtual bool KeyEvent(Point buttonPos, KeyInfo* keyInfo) {
    uint8_t buttonID = buttonPos.y * dimension.x + buttonPos.x;

    if (keyInfo->state == PRESSED)
    {
      MatrixOS::MIDI::Send(MidiPacket(MIDI_PORT_ALL, NoteOn, 0, midiNoteTable[buttonPos.x][buttonPos.y], keyInfo->velocity.to7bits()));
      activeNotes.emplace(buttonID);
    }
    else if (keyInfo->state == RELEASED)
    {
      MatrixOS::MIDI::Send(MidiPacket(MIDI_PORT_ALL, NoteOff, 0, midiNoteTable[buttonPos.x][buttonPos.y], keyInfo->velocity.to7bits()));
      activeNotes.erase(buttonID);
    }
    return true;
  }

  FSJINotePad(Dimension dimension) {
    MLOGD("FSJI", "FSJINotePad Constructor");
    this->dimension = dimension;

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
      if (x < 8 && y < 8)
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
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        Ratio buttonRatio = Ratio(i + 1, j + 1);

        // Copy over MIDI note number if the fraction is reducible
        if (i != buttonRatio.n - 1 || j != buttonRatio.d - 1)
        {
          midiNoteTable[i][j] = midiNoteTable[buttonRatio.n - 1][buttonRatio.d - 1];
        }
      }
    }

    activeNotes.reserve(8);
  }

  ~FSJINotePad() {
    // NO MEMORY LEAKS IN THIS HOUSE
    activeNotes.~unordered_set();
  }
};