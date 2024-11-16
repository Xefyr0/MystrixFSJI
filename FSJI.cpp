/**
 * C File for the Free-style Just Intonation app for MatrixOS.
 */
#include "FSJI.h"

#include "applications/BrightnessControl/BrightnessControl.h"

void FSJI::Setup() {
  MLOGD("FSJI", "FSJI Setup");
  UI noteView("Note view", Color(0xFFFFFF));

  FSJINotePad notePad(Dimension(8, 8));
  noteView.AddUIComponent(notePad, Point(0, 0));

  noteView.SetKeyEventHandler([&](KeyEvent* keyEvent) -> bool {
    if (keyEvent->id == FUNCTION_KEY)
    {
      if (keyEvent->info.state == HOLD)
      {
        Exit();
      }
      else if (keyEvent->info.state == RELEASED)
      {
        ActionMenu();
      }
      return true;  // Block UI from to do anything with FN, basically this function control the life cycle of the UI
    }
    return false;
  });
  noteView.Start();

  Exit();  // This should never be reached
}

void FSJI::ActionMenu() {
  MLOGD("FSJI", "FSJI Action Menu Method");

  // Set up the Action Menu UI ---------------------------------------------------------------------
  UI actionMenu("Action Menu", Color(0x00FFFF));

  UIButton brightnessBtn;
  brightnessBtn.SetName("Brightness");
  brightnessBtn.SetColor(Color(0xFFFFFF));
  brightnessBtn.SetSize(Dimension(2, 2));
  brightnessBtn.OnPress([&]() -> void { MatrixOS::LED::NextBrightness(); });
  brightnessBtn.OnHold([&]() -> void { BrightnessControl().Start(); });
  actionMenu.AddUIComponent(brightnessBtn, Point(3, 3));

  // Rotation control and canvas
  UIButton rotateUpBtn;
  rotateUpBtn.SetName("Rotate to this side (Default)");
  rotateUpBtn.SetColor(Color(0x00FF00));
  rotateUpBtn.SetSize(Dimension(2, 1));
  rotateUpBtn.OnPress([&]() -> void {});
  actionMenu.AddUIComponent(rotateUpBtn, Point(3, 2));

  UIButton rotateRightBtn;
  rotateRightBtn.SetName("Rotate to this side");
  rotateRightBtn.SetColor(Color(0x00FF00));
  rotateRightBtn.SetSize(Dimension(1, 2));
  rotateRightBtn.OnPress([&]() -> void { MatrixOS::SYS::Rotate(RIGHT); });
  actionMenu.AddUIComponent(rotateRightBtn, Point(5, 3));

  UIButton rotateDownBtn;
  rotateDownBtn.SetName("Rotate to this side");
  rotateDownBtn.SetColor(Color(0x00FF00));
  rotateDownBtn.SetSize(Dimension(2, 1));
  rotateDownBtn.OnPress([&]() -> void { MatrixOS::SYS::Rotate(DOWN); });
  actionMenu.AddUIComponent(rotateDownBtn, Point(3, 5));

  UIButton rotateLeftBtn;
  rotateLeftBtn.SetName("Rotate to this side");
  rotateLeftBtn.SetColor(Color(0x00FF00));
  rotateLeftBtn.SetSize(Dimension(1, 2));
  rotateLeftBtn.OnPress([&]() -> void { MatrixOS::SYS::Rotate(LEFT); });
  actionMenu.AddUIComponent(rotateLeftBtn, Point(2, 3));

  // System Settings button
  UIButton systemSettingBtn;
  systemSettingBtn.SetName("System Settings");
  systemSettingBtn.SetColor(Color(0xFFFFFF));
  systemSettingBtn.OnPress([&]() -> void { MatrixOS::SYS::OpenSetting(); });
  actionMenu.AddUIComponent(systemSettingBtn, Point(7, 7));

  actionMenu.SetKeyEventHandler([&](KeyEvent* keyEvent) -> bool {
    if (keyEvent->id == FUNCTION_KEY)
    {
      if (keyEvent->info.state == HOLD)
      {
        Exit();
      }
      else if (keyEvent->info.state == RELEASED)
      {
        actionMenu.Exit();
      }
      return true;  // Block UI from to do anything with FN, basically this function control the life cycle of the UI
    }
    return false;
  });
  actionMenu.Start();

  // Exit();  // This should never be reached
}