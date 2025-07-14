#include <Arduino.h>
#include "hw_config.h"

/* controller is GPIO */
#if defined(HW_CONTROLLER_GPIO)

extern "C" void controller_init()
{
#if defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK)
  pinMode(HW_CONTROLLER_GPIO_UP_DOWN, INPUT);
  pinMode(HW_CONTROLLER_GPIO_LEFT_RIGHT, INPUT);
#else  /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
  pinMode(HW_CONTROLLER_GPIO_UP, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_DOWN, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_LEFT, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_RIGHT, INPUT_PULLUP);
#endif /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
  pinMode(HW_CONTROLLER_GPIO_SELECT, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_START, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_A, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_B, INPUT_PULLUP);
}

extern "C" uint32_t controller_read_input()
{
  uint32_t u, d, l, r, s, t, a, b;

#if defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK)
#if defined(HW_CONTROLLER_GPIO_REVERSE_UD)
  int joyY = 4095 - analogRead(HW_CONTROLLER_GPIO_UP_DOWN);
#else
  int joyY = analogRead(HW_CONTROLLER_GPIO_UP_DOWN);
#endif
#if defined(HW_CONTROLLER_GPIO_REVERSE_LF)
  int joyX = 4095 - analogRead(HW_CONTROLLER_GPIO_LEFT_RIGHT);
#else
  int joyX = analogRead(HW_CONTROLLER_GPIO_LEFT_RIGHT);
#endif

  // Thresholds for non-Odroid boards
  if (joyY > 2048 + 1024)
  {
    u = 1;
    d = 0;
  }
  else if (joyY < 1024)
  {
    u = 0;
    d = 1;
  }
  else
  {
    u = 1;
    d = 1;
  }
  if (joyX > 2048 + 1024)
  {
    l = 1;
    r = 0;
  }
  else if (joyX < 1024)
  {
    l = 0;
    r = 1;
  }
  else
  {
    l = 1;
    r = 1;
  }
#else  /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
  u = digitalRead(HW_CONTROLLER_GPIO_UP);
  d = digitalRead(HW_CONTROLLER_GPIO_DOWN);
  l = digitalRead(HW_CONTROLLER_GPIO_LEFT);
  r = digitalRead(HW_CONTROLLER_GPIO_RIGHT);
#endif /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */

  s = digitalRead(HW_CONTROLLER_GPIO_SELECT);
  t = digitalRead(HW_CONTROLLER_GPIO_START);
  a = digitalRead(HW_CONTROLLER_GPIO_A);
  b = digitalRead(HW_CONTROLLER_GPIO_B);

  // NES controller bitmask: A, B, Select, Start, Up, Down, Left, Right (active low)
  return 0xFFFFFFFF ^ ((!a << 0) | (!b << 1) | (!s << 2) | (!t << 3) | (!u << 4) | (!d << 5) | (!l << 6) | (!r << 7));
}

/* controller is I2C M5Stack CardKB */
#elif defined(HW_CONTROLLER_I2C_M5CARDKB)

#include <Wire.h>

#define I2C_M5CARDKB_ADDR 0x5f
#define READ_BIT I2C_MASTER_READ
#define ACK_CHECK_EN 0x1
#define NACK_VAL 0x1

extern "C" void controller_init()
{
  Wire.begin();
}

extern "C" uint32_t controller_read_input()
{
  uint32_t value = 0xFFFFFFFF;

  Wire.requestFrom(I2C_M5CARDKB_ADDR, 1);
  while (Wire.available())
  {
    char c = Wire.read();
    if (c != 0)
    {
      switch (c)
      {
      case 181: // up
        value ^= (1 << 0);
        break;
      case 182: // down
        value ^= (1 << 1);
        break;
      case 180: // left
        value ^= (1 << 2);
        break;
      case 183: // right
        value ^= (1 << 3);
        break;
      case ' ': // select
        value ^= (1 << 4);
        break;
      case 13: // enter -> start
        value ^= (1 << 5);
        break;
      case 'k': // A
        value ^= (1 << 6);
        break;
      case 'l': // B
        value ^= (1 << 7);
        break;
      case 'o': // X
        value ^= (1 << 8);
        break;
      case 'p': // Y
        value ^= (1 << 9);
        break;
      }
    }
  }
  return value;
}

/* controller is I2C BBQ10Keyboard */
#elif defined(HW_CONTROLLER_I2C_BBQ10KB)

#include <Wire.h>
#include <BBQ10Keyboard.h>
BBQ10Keyboard keyboard;
static uint32_t value = 0xFFFFFFFF;

extern "C" void controller_init()
{
  Wire.begin();
  keyboard.begin();
  keyboard.setBacklight(0.2f);
}

extern "C" uint32_t controller_read_input()
{
  int keyCount = keyboard.keyCount();
  while (keyCount--)
  {
    const BBQ10Keyboard::KeyEvent key = keyboard.keyEvent();
    uint32_t bit = 0;
    if (key.key != 0)
    {
      switch (key.key)
      {
      case 'w': // up
        bit = (1 << 0);
        break;
      case 'z': // down
        bit = (1 << 1);
        break;
      case 'a': // left
        bit = (1 << 2);
        break;
      case 'd': // right
        bit = (1 << 3);
        break;
      case ' ': // select
        bit = (1 << 4);
        break;
      case 10: // enter -> start
        bit = (1 << 5);
        break;
      case 'k': // A
        bit = (1 << 6);
        break;
      case 'l': // B
        bit = (1 << 7);
        break;
      case 'o': // X
        bit = (1 << 8);
        break;
      case 'p': // Y
        bit = (1 << 9);
        break;
      }
      if (key.state == BBQ10Keyboard::StatePress)
      {
        value ^= bit;
      }
      else if (key.state == BBQ10Keyboard::StateRelease)
      {
        value |= bit;
      }
    }
  }
  return value;
}

#else /* no controller defined */

extern "C" void controller_init()
{
  Serial.printf("GPIO controller disabled in menuconfig; no input enabled.\n");
}

extern "C" uint32_t controller_read_input()
{
  return 0xFFFFFFFF;
}

#endif /* no controller defined */
