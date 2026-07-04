#include "key_actions.h"
#include "config.h"

void SingleClickAction(void) {
    VELIX_LED_OFF(VELIX_LED_R_GPIO_Port, VELIX_LED_R_Pin);
    VELIX_LED_OFF(VELIX_LED_G_GPIO_Port, VELIX_LED_G_Pin);
    VELIX_LED_OFF(VELIX_LED_B_GPIO_Port, VELIX_LED_B_Pin);
}

void DoubleClickAction(void) {
    VELIX_LED_ON(VELIX_LED_R_GPIO_Port, VELIX_LED_R_Pin);
}

void LongPressAction(void) {
    VELIX_LED_ON(VELIX_LED_R_GPIO_Port, VELIX_LED_R_Pin);
    VELIX_LED_ON(VELIX_LED_G_GPIO_Port, VELIX_LED_G_Pin);
    VELIX_LED_ON(VELIX_LED_B_GPIO_Port, VELIX_LED_B_Pin);
}
