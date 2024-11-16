/**
 * C File for the Free-style Just Intonation app for MatrixOS.
 */
#include "FSJI.h"

void FSJI::Setup() {
  MLOGD("FSJI", "FSJI App Setup");

  // Set up the Action Menu UI ---------------------------------------------------------------------
  UI actionMenu("Action Menu", Color(0x00FFFF));

  actionMenu.SetKeyEventHandler([&](KeyEvent* keyEvent) -> bool {
    if (keyEvent->id == FUNCTION_KEY)
    {
      if (keyEvent->info.state == HOLD)
      {
        Exit();
      }
      else if (keyEvent->info.state == RELEASED)
      {
        PlayView();
      }
      return true;  // Block UI from to do anything with FN
    }
    return false;
  });
  actionMenu.AllowExit(false);
  actionMenu.SetSetupFunc([&]() -> void { PlayView(); });
  actionMenu.Start();

  Exit();  // This should never be reached
}

// Enter the MIDI controller playing view
void FSJI::PlayView() {
  MLOGD("FSJI", "Enter PlayView");
  UI playView = UI("Note Play View", Color(0xFFFFFF), false);

  FSJINotePad notePad1(Dimension(8, 8));
  playView.AddUIComponent(notePad1, Point(0, 0));

  playView.Start();
}