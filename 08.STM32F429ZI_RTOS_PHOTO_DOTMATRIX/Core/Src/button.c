#include "button.h"
#include "i2c_lcd.h" // directly controlling the hardware) relative information

// 초기 버튼 상태
char button_status[BUTTON_NUMBER] =
{BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE};
int button0_status = 0;
void button0_status_check(void);
extern int func_num;
extern int floor1;
extern int floor2;
extern int floor3;
extern int floor4;
extern int elva_status;



// interruput hw -> sw  falling sw check
// gpio, pin, 버튼상태
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number)
{
	unsigned char curr_state;

	curr_state = HAL_GPIO_ReadPin(GPIO, GPIO_PIN); // press : 0 return : 1

	// 버튼이 눌려졌으나 처음 눌려진 상태
	if(curr_state == BUTTON_PRESS && button_status[button_number] == BUTTON_RELEASE)
	{
		HAL_Delay(100); // noise가 지나가기를 기다린다.

		button_status[button_number] = curr_state; // 현재의 버튼 저장
		return BUTTON_RELEASE; // 버튼이 눌려진 상태 이나 noise상태이다.
	}

	// 현재 버튼을 뗀 상태 앞전에 누른 상태
	else if (curr_state == BUTTON_RELEASE && button_status[button_number] == BUTTON_PRESS)
	{
		button_status[button_number] = BUTTON_RELEASE;
		return BUTTON_PRESS; // 버튼을 1번 눌렀다 뗀 상태로 인정한다.
	}
	return BUTTON_RELEASE;
}

// mode change
void button0_status_check(void)
{
	func_num = 5;

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
	{
		button0_status = ~button0_status;   // ( 0 <--> 1)
	}
}

void button_elva(void)
{
	#define STAY 0

	char lcd_buff[40];

	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
	{
		if(!floor1)
		{
			floor1 = 1;
			sprintf(lcd_buff, "1 ");
			move_cursor(1,0);  // column, row
			lcd_string(lcd_buff);
printf("do floor1\n");
		}
		else if(floor1)
		{
			floor1 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,0);  // column, row
			lcd_string(lcd_buff);
			if(floor2 || floor3 || floor4 == 0)
			{
				elva_status = STAY;
			}
printf("undo floor1\n");
		}
	}
	if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, BUTTON1) == BUTTON_PRESS)
	{
		if(!floor2)
		{
			floor2 = 1;
			sprintf(lcd_buff, "2 ");
			move_cursor(1,2);  // column, row
			lcd_string(lcd_buff);
printf("do floor2\n");
		}
		else if(floor2)
		{
			floor2 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,2);  // column, row
			lcd_string(lcd_buff);
			if(floor1 || floor3 || floor4 == 0)
			{
				elva_status = STAY;
			}
printf("undo floor2\n");
		}
	}
	if(get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, BUTTON2) == BUTTON_PRESS)
	{
		if(!floor3)
		{
			sprintf(lcd_buff, "3 ");
			move_cursor(1,4);  // column, row
			lcd_string(lcd_buff);
printf("do floor3\n");
			floor3 = 1;
		}
		else if(floor3)
		{
printf("undo floor3\n");
			floor3 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,4);  // column, row
			lcd_string(lcd_buff);
			if(floor1 || floor2 || floor4 == 0)
			{
				elva_status = STAY;
			}
		}
	}
	if(get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, BUTTON3) == BUTTON_PRESS)
	{
		if(!floor4)
		{
			floor4 = 1;
			sprintf(lcd_buff, "4 ");
			move_cursor(1,6);  // column, row
			lcd_string(lcd_buff);
printf("do floor4\n");
		}
		else if(floor4)
		{
printf("undo floor4\n");
			floor4 = 0;
			sprintf(lcd_buff, "  ");
			move_cursor(1,6);  // column, row
			lcd_string(lcd_buff);
			if(floor1 || floor2 || floor3 == 0)
			{
				elva_status = STAY;
			}
		}
	}
}

