#include "FSJI.h"

#include "applications/BrightnessControl/BrightnessControl.h"

void FSJI::Setup() {
  // Load config from non-volatile storage if config versions match
  if (nvsVersion == (uint32_t)MYSTRIX_FSJI_VERSION)
  {
    MatrixOS::NVS::GetVariable(FSJI_CONFIGS_HASH, notePadConfig, sizeof(FSJINotePadConfig));
  }
  else
  {
    MatrixOS::NVS::SetVariable(FSJI_CONFIGS_HASH, notePadConfig, sizeof(FSJINotePadConfig));
  }

  // Start the app in the NoteView UI
  NoteView();

  Exit();  // This should never be reached
}

void FSJI::NoteView() {
  // Set up the Note View UI ---------------------------------------------------------------------
  UI noteView("Note View", Color(0xAAAAAA));

  FSJINotePad notePad(Dimension(8, 8), notePadConfig);
  noteView.AddUIComponent(notePad, Point(0, 0));

  // Enter the Action Menu if the fn key is pressed.
  // Exit the application if the fn key is held.
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

  // The NoteView UIComponent is now fully set up. Let the UI runtime take over from here.
  noteView.Start();
}

void FSJI::ActionMenu() {

  // Set up the Action Menu UI ---------------------------------------------------------------------
  UI actionMenu("Action Menu", Color(0x00FFFF));

  // Brightness control
  UIButton brightnessBtn;
  brightnessBtn.SetName("Brightness");
  brightnessBtn.SetColor(Color(0xFFFFFF));
  brightnessBtn.SetSize(Dimension(2, 2));
  brightnessBtn.OnPress([&]() -> void { MatrixOS::LED::NextBrightness(); });
  brightnessBtn.OnHold([&]() -> void { BrightnessControl().Start(); });
  actionMenu.AddUIComponent(brightnessBtn, Point(3, 3));

  // Rotation control
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

  // MIDI channel selector
  UIButton channelSelectorBtn;
  channelSelectorBtn.SetName("Channel Selector");
  channelSelectorBtn.SetColor(Color(0x60FF00));
  channelSelectorBtn.OnPress([&]() -> void { ChannelSelector(); });
  actionMenu.AddUIComponent(channelSelectorBtn, Point(7, 5));

  // MIDI velocity sensitivity toggle
  UIButton velocitySensitiveToggle;
  velocitySensitiveToggle.SetName("Velocity Sensitive");
  velocitySensitiveToggle.SetColorFunc([&]() -> Color { return Color(0x00FFB0).DimIfNot(notePadConfig->velocitySensitive); });
  velocitySensitiveToggle.OnPress([&]() -> void { notePadConfig->velocitySensitive = !notePadConfig->velocitySensitive; });
  velocitySensitiveToggle.OnHold([&]() -> void {
    MatrixOS::UIInterface::TextScroll(velocitySensitiveToggle.GetName() + " " + (notePadConfig->velocitySensitive ? "On" : "Off"), velocitySensitiveToggle.GetColor());
  });
  velocitySensitiveToggle.SetEnabled(Device::KeyPad::velocity_sensitivity);
  actionMenu.AddUIComponent(velocitySensitiveToggle, Point(6, 7));

  // System Settings button
  UIButton systemSettingBtn;
  systemSettingBtn.SetName("System Settings");
  systemSettingBtn.SetColor(Color(0xFFFFFF));
  systemSettingBtn.OnPress([&]() -> void { MatrixOS::SYS::OpenSetting(); });
  actionMenu.AddUIComponent(systemSettingBtn, Point(7, 7));

  // Exit the Action Menu if the fn key is pressed.
  // Exit the application if the fn key is held.
  actionMenu.SetKeyEventHandler([&](KeyEvent* keyEvent) -> bool {
    if (keyEvent->id == FUNCTION_KEY)
    {
      if (keyEvent->info.state == HOLD)
      {
        MatrixOS::NVS::SetVariable(FSJI_CONFIGS_HASH, notePadConfig, sizeof(FSJINotePadConfig));
        Exit();
      }
      else if (keyEvent->info.state == RELEASED)
      {
        MatrixOS::NVS::SetVariable(FSJI_CONFIGS_HASH, notePadConfig, sizeof(FSJINotePadConfig));
        actionMenu.Exit();
      }
      return true;  // Block UI from to do anything with FN, basically this function control the life cycle of the UI
    }
    return false;
  });

  // The Action Menu is now fully set up. Let the UI runtime take over from here.
  actionMenu.Start();
}

void FSJI::ChannelSelector() {
  // Set up the Channel Selector UI ---------------------------------------------------------------------
  UI channelSelector("Channel Selector", Color(0x60FF00), false);

  int32_t offsettedChannel = notePadConfig->channel + 1;
  UI4pxNumber numDisplay(Color(0x60FF00), 2, &offsettedChannel, Color(0xFFFF00), 1);
  channelSelector.AddUIComponent(numDisplay, Point(1, 0));

  UISelector channelInput(Dimension(8, 2), "Channel", Color(0x60FF00), 16, (uint16_t*)&notePadConfig->channel, [&](uint16_t val) -> void { offsettedChannel = val + 1; });

  channelSelector.AddUIComponent(channelInput, Point(0, 6));

  // The MIDI Channel Selector UIComponent is now fully set up. Let the UI runtime take over from here.
  channelSelector.Start();
}

FSJI::~FSJI() {
  free(notePadConfig);
}
