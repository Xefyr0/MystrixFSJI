/**
 * Header for the Free-style Just Intonation app for MatrixOS.
 *
 */
#pragma once

#include "FSJINotePad.h"
#include "MatrixOS.h"
#include "applications/Application.h"
#include "ui/UI.h"

#define MYSTRIX_FSJI_VERSION 1
#define FSJI_CONFIGS_HASH StaticHash("NoahB-FreeStyleJI-NotePadConfig")

class FSJI : public Application {
 public:
  static Application_Info info;

  CreateSavedVar("FSJI", nvsVersion, uint32_t, MYSTRIX_FSJI_VERSION);  // In case FSJINoteConfig gets changed

  FSJINotePadConfig* notePadConfig = new FSJINotePadConfig();

  void Setup() override;

  void ActionMenu();
  void ChannelSelector();

  ~FSJI();
};

// Metadata about this application
inline Application_Info FSJI::info = {
    .name = "FSJI",
    .author = "Noah B.",
    .color = Color(0xAAAAAA),
    .version = MYSTRIX_FSJI_VERSION,
    .visibility = true,
};

// Register this Application to the OS (Use the class name of your application as the variable)
REGISTER_APPLICATION(FSJI);