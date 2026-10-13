#include "main.h"
#define KEY_PRESSED 0
#define KEY_IDLE 	1
#define cQ_SIZE		80

void keypad_init(void);
uint8_t get_keypad(int col, int row);
int keypad_scanning();
