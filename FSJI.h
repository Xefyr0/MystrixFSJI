/**
 * Header for the Free-style Just Intonation app for MatrixOS.
 * 
 */
#pragma once

#include "MatrixOS.h"
#include "applications/Application.h"

class FSJI : public Application {
 public:
  static Application_Info info;
};

// Metadata about this application
inline Application_Info FSJI::info = {
  .name = "FSJI",
  .author = "Noah B.",
  .color = Color(0xAAAAAA),
  .version = 1,
  .visibility = true,
};

// Register this Application to the OS (Use the class name of your application as the variable)
REGISTER_APPLICATION(FSJI);