#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <FFat.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>
#include <Arduino_GFX_Library.h>
#include "hw_config.h"

extern "C"
{
#include <nofrendo.h>
#include <nes/nes.h> // For NES controller defines
    void controller_init(); // Declare controller_init
    uint32_t controller_read_input(); // Declare controller_read_input
}

int16_t bg_color;
extern Arduino_TFT *gfx;
extern void display_begin();

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Nofrendo...");

    // Turn off WiFi to save resources
    esp_wifi_deinit();
    Serial.println("WiFi disabled");

    // Disable Task Watchdog Timer completely
    esp_task_wdt_deinit();
    Serial.println("Task WDT disabled");

    // Start display
    display_begin();
    Serial.println("Display initialized");

    // Initialize controller
    controller_init();
    Serial.println("Controller initialized");

    // Filesystem defined in hw_config.h (SPIFFS)
    FILESYSTEM_BEGIN
    Serial.println("Filesystem initialized");

    // Find first ROM file (*.nes)
    File root = filesystem.open("/");
    char *argv[1];
    if (!root)
    {
        Serial.println("Filesystem mount failed! Please check hw_config.h settings.");
        gfx->println("Filesystem mount failed! Please check hw_config.h settings.");
        while (1) delay(1000); // Halt on failure
    }

    bool foundRom = false;
    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            char *filename = (char *)file.name();
            int8_t len = strlen(filename);
            if (strstr(strlwr(filename + (len - 4)), ".nes"))
            {
                foundRom = true;
                char fullFilename[256];
                sprintf(fullFilename, "%s/%s", FSROOT, filename);
                Serial.printf("Found ROM: %s\n", fullFilename);
                argv[0] = fullFilename;
                break;
            }
        }
        file = root.openNextFile();
    }

    if (!foundRom)
    {
        Serial.println("Failed to find ROM file, please copy ROM file to data folder and upload with \"ESP32 Sketch Data Upload\"");
        gfx->println("Failed to find ROM file, please copy ROM file to data folder and upload with \"ESP32 Sketch Data Upload\"");
        while (1) delay(1000); // Halt on failure
    }

    Serial.println("Nofrendo start!");
    int result = nofrendo_main(1, argv);
    Serial.printf("Nofrendo ended with code: %d\n", result);
    while (1) delay(1000); // Halt after Nofrendo exits
}

void loop()
{
    // Nofrendo handles its own loop; nothing needed here
}
