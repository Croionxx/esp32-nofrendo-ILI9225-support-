ESP32 Nofrendo with ILI9225 Display Driver
Welcome to ESP32 Nofrendo, a project that brings the Nofrendo NES emulator to the ESP32 microcontroller with a custom driver for the ILI9225 2.2" TFT display. This project adapts the open-source Nofrendo emulator for a portable, low-cost gaming platform, featuring a tailored display driver, button controls, and SPIFFS-based ROM loading. Key challenges, such as Task Watchdog Timer (WDT) errors and controller initialization issues, were resolved to ensure smooth operation on the ESP32.
Project Overview
This project builds on the Nofrendo emulator by integrating it with the ESP32, using the Arduino_GFX_Library to drive the ILI9225 display. The focus is on custom adaptations for the ESP32, including:

ILI9225 Display Driver: Optimized for 176x220 resolution, centered NES rendering, and PWM backlight control.
Button Controls: Eight GPIO buttons mapped to NES inputs (A, B, Select, Start, Up, Down, Left, Right).
SPIFFS ROM Loading: Loads .nes ROMs from the ESP32’s SPIFFS filesystem.
WDT Fix: Disables Task WDT to resolve task not found errors.
Controller Fix: Adds proper declarations to fix controller_init compilation errors.

Hardware Setup
Components

ESP32 Dev Module: Compatible with generic ESP32 boards (e.g., DOIT ESP32 DEVKIT V1). Boards with PSRAM are recommended for complex games.
ILI9225 2.2" TFT Display:
Resolution: 176x220 pixels (cropped from NES 256x240).
SPI Pins: RST=4, RS=19, CLK=18, SDI=23, CS=5.
Backlight: LED=0, brightness=200/255 (PWM-controlled).


Buttons: Eight active-low buttons with pull-up resistors:
Hard Drop (GPIO 12) → A
Hold (GPIO 13) → B
Pause (GPIO 14) → Select
Restart (GPIO 27) → Start
Rotate (GPIO 26) → Up
Down (GPIO 25) → Down
Left (GPIO 32) → Left
Right (GPIO 33) → Right



Wiring



Component
ESP32 Pin



ILI9225 RST
GPIO 4


ILI9225 RS (DC)
GPIO 19


ILI9225 CLK
GPIO 18


ILI9225 SDI
GPIO 23


ILI9225 CS
GPIO 5


ILI9225 LED
GPIO 0


Button Hard Drop
GPIO 12


Button Hold
GPIO 13


Button Pause
GPIO 14


Button Restart
GPIO 27


Button Rotate
GPIO 26


Button Down
GPIO 25


Button Left
GPIO 32


Button Right
GPIO 33


Software Setup
Dependencies

Arduino IDE: Version 1.8.x (recommended for ESP32FS plugin) or 2.x.
ESP32 Arduino Core: Version 3.0.2+, installed via Boards Manager (https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json).
Arduino_GFX_Library: Install via Tools > Manage Libraries (search “Arduino_GFX”).
Nofrendo: Use the open-source Nofrendo emulator, including noftypes.h, memguard.h, log.h, nofrendo.h, nes/nes.h.
ESP32FS Plugin (Arduino IDE 1.8.x): Download from github.com/me-no-dev/arduino-esp32fs-plugin for SPIFFS ROM uploads.

Project Files

esp32-nofrendo.ino: Main sketch for display, button, SPIFFS, and Nofrendo initialization.
hw_config.h: Defines SPIFFS (FSROOT "/fs") and hardware settings.
display.cpp: Custom ILI9225 driver using Arduino_GFX_Library.
controller.cpp: Button input handling for NES controls.
memguard.c: Memory management for Nofrendo, adapted for ESP32.

ILI9225 Driver and ESP32 Adaptations
This project extends the Nofrendo emulator with specific changes to support the ILI9225 display and ESP32 hardware. Below are the key modifications:
1. ILI9225 Display Driver (display.cpp)

Purpose: Drives the ILI9225 2.2" TFT display for NES game rendering.
Library: Uses Arduino_GFX_Library for efficient SPI communication.
Configuration:
Pins: RST=4, RS=19, CLK=18, SDI=23, CS=5.
Resolution: 176x220, centered to crop NES’s 256x240 frame (40-pixel offset per side).
Backlight: PWM control on GPIO 0 with analogWrite(0, 200) for adjustable brightness (200/255).


Changes:
Replaced digitalWrite with analogWrite for backlight to enable brightness control.
Implemented display_begin() to initialize the display with a dark background (color565(24, 28, 24)).
Added display_write_frame() to scale and center NES frames for the ILI9225’s resolution.


Validation: Tested with esp32-nofrendo_test.ino, displaying “Nofrendo Test” text to confirm functionality without a ROM.

2. Controller Integration (controller.cpp)

Purpose: Maps eight GPIO buttons to NES controls.
Configuration:
Buttons set as INPUT_PULLUP for active-low logic.
Mapping: Hard Drop (12)=A, Hold (13)=B, Pause (14)=Select, Restart (27)=Start, Rotate (26)=Up, Down (25)=Down, Left (32)=Left, Right (33)=Right.


Fixes:
Resolved controller_init was not declared compilation error by adding extern "C" void controller_init(); and extern "C" uint32_t controller_read_input(); in esp32-nofrendo.ino.
Verified button functionality with esp32-nofrendo_test.ino, printing active-low states (pressed=0) to Serial Monitor.


Implementation: controller_init() configures GPIOs, and controller_read_input() returns a bitmask for Nofrendo’s NES controller API.

3. Task Watchdog Timer Fix

Issue: Runtime errors (E (766) task_wdt: esp_task_wdt_reset(705): task not found) occurred due to Nofrendo tasks calling esp_task_wdt_reset() without WDT registration.
Solution: Disabled Task WDT with esp_task_wdt_deinit() in setup() to eliminate errors, as Nofrendo’s task creation (in nofrendo.c) wasn’t registering tasks.
Alternative: Task registration with esp_task_wdt_add() was considered but not implemented, as disabling WDT proved sufficient.
Validation: Confirmed no WDT errors in esp32-nofrendo_test.ino via Serial Monitor.

4. SPIFFS ROM Loading

Purpose: Loads .nes ROMs from SPIFFS for emulation.
Configuration: hw_config.h sets SPIFFS with FSROOT "/fs".
Implementation: esp32-nofrendo.ino scans /fs for .nes files and passes the first found ROM to nofrendo_main.
Upload Process:
Place .nes file (e.g., game.nes) in <sketch_folder>/data.
Upload to SPIFFS using Tools > ESP32 Sketch Data Upload (Arduino IDE 1.8.x) or mksfiffs/esptool.py (2.x).



5. Memory Management (memguard.c)

Purpose: Ensures Nofrendo’s memory allocation works on ESP32.
Change: Replaced custom memory management with malloc and free for compatibility.
Debugging: Added ASSERT_MSG for error logging, with optional NOFRENDO_DEBUG for leak detection via mem_checkleaks().

Issues
The following issues were identified during testing and may affect gameplay:

Screen Scaling Issues:

Description: Some NES games exhibit “iffy” scaling on the ILI9225’s 176x220 display when rendering the native 256x240 resolution. This can result in stretched or distorted graphics, as the display crops 40 pixels from each side to center the frame.
Impact: Visual quality may be suboptimal for certain games, particularly those with fine details or non-standard aspect ratios.
Workaround: Adjust display_write_frame() in display.cpp to fine-tune scaling or offset parameters. Alternatively, consider a display with a resolution closer to 256x240 (e.g., ILI9341) for better compatibility.
Future Fix: Implement dynamic scaling or aspect ratio correction in display.cpp to improve rendering consistency across games.


Game Complexity Limited by Flash Size:

Description: The emulator’s performance depends on the ESP32 board’s flash size, with more complex NES games (e.g., those with large ROMs or advanced mappers) requiring more memory. Boards without PSRAM may struggle or fail to run these games.
Impact: Limits the range of playable games on boards with standard 4MB flash (e.g., ESP32 Dev Module). Crashes or slowdowns may occur with demanding titles.
Recommendation: Use an ESP32 board with PSRAM (e.g., ESP32-WROVER, ESP32-S3) to support larger ROMs and complex mappers. Ensure the partition scheme allocates sufficient space (e.g., Default 4MB with spiffs).
Future Fix: Optimize memory usage in memguard.c or add a ROM compatibility list to guide users.



Troubleshooting

ROM Not Found:
Ensure .nes file is in <sketch_folder>/data and uploaded to /fs.
Verify Tools > Partition Scheme > Default 4MB with spiffs.


Display Issues:
Check SPI pins (CLK=18, SDI=23, CS=5, RS=19, RST=4) and backlight (GPIO 0).
Test with a standalone Arduino_GFX_Library sketch.


Button Issues:
Confirm active-low logic (pressed=0) with a GPIO test sketch.


WDT Errors:
Verify esp_task_wdt_deinit() is called. Check nofrendo.c for task issues if needed.


Compilation Errors:
Ensure controller_init and controller_read_input are declared in extern "C".



Installation Instructions

Install Dependencies:

Arduino IDE (1.8.x recommended, or 2.x).
ESP32 Arduino Core (3.0.2+): Add URL in Preferences > Additional Boards Manager URLs and install via Boards Manager.
Arduino_GFX_Library: Install via Tools > Manage Libraries.
Nofrendo: Download from libretro/nofrendo.
ESP32FS Plugin (1.8.x): Install to <sketchbook>/tools from github.com/me-no-dev/arduino-esp32fs-plugin.


Set Up Sketch:

Place in sketch folder:
esp32-nofrendo.ino
hw_config.h
display.cpp
controller.cpp
memguard.c
Nofrendo files (noftypes.h, memguard.h, log.h, nofrendo.h, nes/nes.h).


Create <sketch_folder>/data with a .nes ROM (e.g., game.nes).


Upload ROM:

Arduino IDE 1.8.x: Use Tools > ESP32 Sketch Data Upload.
Arduino IDE 2.x:mksfiffs -c <sketch_folder>/data -p 256 -b 4096 spiffs.bin
esptool.py --chip esp32 --port <your_port> --baud 921600 write_flash 0x290000 spiffs.bin




Compile and Upload:

Select Tools > Board > ESP32 Arduino > ESP32 Dev Module and the correct port.
Compile and upload esp32-nofrendo.ino.
Hold BOOT button if upload times out.


Test:

Open Serial Monitor (115200 baud) to verify:Starting Nofrendo...
WiFi disabled
Task WDT disabled
Display initialized
Controller initialized
Filesystem initialized
Found ROM: /fs/game.nes
Nofrendo start!


Verify ILI9225 shows the NES game (176x220).
Test buttons: A (12), B (13), Select (14), Start (27), Up (26), Down (25), Left (32), Right (33).



Future Improvements

Add audio support via HW_AUDIO in hw_config.h.
Support multiple ROM selection with buttons.
Improve screen scaling in display.cpp for consistent rendering.
Optimize memory for non-PSRAM boards or add a ROM compatibility list.

Credits

Nofrendo: Built on the open-source Nofrendo emulator by the libretro team.
Arduino_GFX_Library: Thanks to moononournation for ILI9225 support.
ESP32 Arduino Core: Provided by Espressif Systems.

Contributing
Fork this project, submit pull requests, or report issues on GitHub. Share your ESP32 or ILI9225 enhancements to make retro gaming even better!
License
This project uses the Nofrendo emulator under its original license. Custom adaptations are licensed under MIT where applicable.

Enjoy classic NES gaming on your ESP32 with ILI9225! 🎮
