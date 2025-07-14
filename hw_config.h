#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#define FSROOT "/fs"

/* Custom hardware with ILI9225 */
#define FILESYSTEM_BEGIN SPIFFS.begin(false, FSROOT); FS filesystem = SPIFFS;

/* Controller is GPIO with digital buttons */
#define HW_CONTROLLER_GPIO
#define HW_CONTROLLER_GPIO_UP 26     // BUTTON_ROTATE (mapped to Up)
#define HW_CONTROLLER_GPIO_DOWN 25   // BUTTON_DOWN
#define HW_CONTROLLER_GPIO_LEFT 32   // BUTTON_LEFT
#define HW_CONTROLLER_GPIO_RIGHT 33  // BUTTON_RIGHT
#define HW_CONTROLLER_GPIO_SELECT 14 // BUTTON_PAUSE
#define HW_CONTROLLER_GPIO_START 27  // BUTTON_RESTART
#define HW_CONTROLLER_GPIO_A 12      // BUTTON_HARD_DROP
#define HW_CONTROLLER_GPIO_B 13      // BUTTON_HOLD

#endif /* _HW_CONFIG_H_ */
