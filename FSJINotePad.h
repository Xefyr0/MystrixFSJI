/**
 * NotePad UI for playing in Full-scale Just Intonation.
 */

#include "MatrixOS.h"
#include "ui/UI.h"

#include <unordered_set>

class FSJINotePad : public UIComponent {
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
      activeNotes.emplace(buttonID);
    }
    else if (keyInfo->state == RELEASED)
    {
      activeNotes.erase(buttonID);
    }
    return true;
  }

  FSJINotePad(Dimension dimension) {
    MLOGD("FSJI", "FSJINotePad Constructor");
    this->dimension = dimension;
    activeNotes.reserve(8);
  }

  ~FSJINotePad() {
    // NO MEMORY LEAKS IN THIS HOUSE
    activeNotes.~unordered_set();
  }
};