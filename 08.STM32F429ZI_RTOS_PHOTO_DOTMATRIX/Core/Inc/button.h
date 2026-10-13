#include "main.h" // for HAL GPIO handling

#define BUTTON_PRESS 0   // 버튼을 누를 상태 : active low : pull-up
#define BUTTON_RELEASE 1 // 버튼을 뗀 상태 : 1
#define BUTTON_NUMBER 4  // 버튼 갯수 : 4

#define BUTTON0 0
#define BUTTON1 1
#define BUTTON2 2
#define BUTTON3 3
// gpio, pin, 버튼상태
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);
