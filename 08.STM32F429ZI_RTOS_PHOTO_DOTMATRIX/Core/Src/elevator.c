#include "elevator.h"
#include "i2c_lcd.h" // directly controlling the hardware) relative information

void elevate(void);
void elevator_default(void);

extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);
extern volatile int ext_int_no;
#define BUTTON_PRESS 0

void elevate(void)
{
	char lcd_buff[40];

	if((get_button(USER_Btn_GPIO_Port, USER_Btn_Pin, 13) == BUTTON_PRESS))
	{
		if(floor_status != FIRST)
		{
			elva_status = DOWN;
		}
		else
		{
			elva_status = STAY;
		}
	}
	if(floor1)
	{
		if(floor_status != FIRST)
		{
			elva_status = DOWN;
		}
		else
		{
			elva_status = STAY;
			floor1 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,0);  // column, row
			lcd_string(lcd_buff);
			HAL_Delay(2000);
		}
	}
	if(floor2)
	{
		if(floor_status != SECOND)
		{
			if(floor_status >= SECOND)
			{
				elva_status = DOWN;
			}
			if(floor_status <= SECOND)
			{
				elva_status = UP;
			}
		}
		else
		{
			elva_status = STAY;
			floor2 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,2);  // column, row
			lcd_string(lcd_buff);
			HAL_Delay(2000);
		}
	}
	if(floor3)
	{
		if(floor_status != THIRD)
		{
			if(floor_status <= THIRD)
			{
				elva_status = UP;
			}
			if(floor_status >= THIRD)
			{
				elva_status = DOWN;
			}
		}
		else
		{
			elva_status = STAY;
			floor3 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,4);  // column, row
			lcd_string(lcd_buff);
			HAL_Delay(2000);
		}
	}
	if(floor4)
	{

		if(floor_status != FOURTH)
		{
			elva_status = UP;
		}
		else
		{
			elva_status = STAY;
			floor4 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,6);  // column, row
			lcd_string(lcd_buff);
			HAL_Delay(2000);
		}
	}
}


void elevator_default(void)
{
	floor_L = 1;
	if(floor_status == FIRST)
	{
		floor_L = 0;
	}
}
