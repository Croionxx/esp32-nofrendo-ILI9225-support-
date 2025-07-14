extern "C"
{
#include <nes/nes.h>
}

#include "hw_config.h"
#include <Arduino_GFX_Library.h>
#include <Arduino.h> // Include for analogWrite

/* Custom hardware with ILI9225 */
#define TFT_BRIGHTNESS 200 /* 0 - 255 */
#define TFT_BL 0
Arduino_DataBus *bus = new Arduino_ESP32SPI(19 /* DC */, 5 /* CS */, 18 /* SCK */, 23 /* MOSI */, -1 /* MISO */);
Arduino_ILI9225 *gfx = new Arduino_ILI9225(bus, 4 /* RST */, 1 /* rotation */);

static int16_t w, h, frame_x, frame_y, frame_x_offset, frame_y_offset, frame_width, frame_height, frame_line_pixels;
extern int16_t bg_color;
extern uint16_t myPalette[];

extern void display_begin()
{
    gfx->begin();
    bg_color = gfx->color565(24, 28, 24); // DARK DARK GREY
    gfx->fillScreen(bg_color);

#ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    analogWrite(TFT_BL, TFT_BRIGHTNESS); // Use Arduino PWM for backlight
#endif
}

extern "C" void display_init()
{
    w = gfx->width();  // 176 for ILI9225
    h = gfx->height(); // 220 for ILI9225
    frame_width = min(w, (int16_t)NES_SCREEN_WIDTH);   // 176 < 256, so 176
    frame_height = min(h, (int16_t)NES_SCREEN_HEIGHT); // 220 < 240, so 220
    frame_x = (w - frame_width) / 2;                  // (176 - 176) / 2 = 0
    frame_y = (h - frame_height) / 2;                 // (220 - 220) / 2 = 0
    frame_x_offset = (NES_SCREEN_WIDTH - frame_width) / 2;  // (256 - 176) / 2 = 40
    frame_y_offset = (NES_SCREEN_HEIGHT - frame_height) / 2; // (240 - 220) / 2 = 10
    frame_line_pixels = frame_width;                  // 176
}

extern "C" void display_write_frame(const uint8_t *data[])
{
    gfx->startWrite();
    gfx->writeAddrWindow(frame_x, frame_y, frame_width, frame_height);
    for (int32_t i = 0; i < frame_height; i++)
    {
        gfx->writeIndexedPixels((uint8_t *)(data[i + frame_y_offset] + frame_x_offset), myPalette, frame_line_pixels);
    }
    gfx->endWrite();
}

extern "C" void display_clear()
{
    gfx->fillScreen(bg_color);
}
