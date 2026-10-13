#include "keypad.h"

int keypad_rowpin[] = {GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_10,GPIO_PIN_11}; 		//R1~R4
int keypad_colpin[] = {GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4};		//C1~C4

volatile int front=0;
volatile int rear=0;

extern void lcd_data(uint8_t data);
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);
extern void i2c_lcd_init(void);

extern int func_num;

// 초기 버튼 상태
char key_status[4][4] =
{
	{KEY_IDLE,KEY_IDLE,KEY_IDLE,KEY_IDLE},
	{KEY_IDLE,KEY_IDLE,KEY_IDLE,KEY_IDLE},
	{KEY_IDLE,KEY_IDLE,KEY_IDLE,KEY_IDLE},
	{KEY_IDLE,KEY_IDLE,KEY_IDLE,KEY_IDLE}
};

void keypad_init(void)
{
	for (int i=0; i<4; i++)
	{
		HAL_GPIO_WritePin(GPIOB, keypad_colpin[i], 1);		// initialize set 1
	}
}

uint8_t get_keypad(int col, int row)
{
	static unsigned char key_map[4][4] = {
			{'1','4','7',' '},
			{'2','5','8','0'},
			{'3','6','9','='},
			{'/','*','-','+'}
	};
	unsigned char curr_state;

	HAL_GPIO_WritePin(GPIOB, keypad_colpin[col], 0);
	curr_state = HAL_GPIO_ReadPin(GPIOB, keypad_rowpin[row]); // press : 0 return : 1
	HAL_GPIO_WritePin(GPIOB, keypad_colpin[col], 1);

	// 버튼이 눌려졌으나 처음 눌려진 상태
	if(curr_state == KEY_PRESSED && key_status[col][row] == KEY_IDLE)
	{
		key_status[col][row] = curr_state; // 현재의 버튼 저장
		return 0; // 버튼이 눌려진 상태 이나 noise상태이다.
	}
	else if (curr_state == KEY_IDLE && key_status[col][row] == KEY_PRESSED)
	{
		key_status[col][row] = KEY_IDLE;
		return key_map[col][row]; // 버튼을 1번 눌렀다 뗀 상태로 인정한다.
	}
	return 0;
}

volatile char keypad_queue[cQ_SIZE];
// scan every 60ms
int keypad_scanning()
{
	unsigned char data_in;

	for(int col=0; col<4; col++)
	{
		for(int row=0; row<4; row++)
		{
			data_in = get_keypad(col, row);
			if (data_in != 0)
			{
				if (((rear+1) % cQ_SIZE) != front)
				{
					keypad_queue[rear] = data_in;   //enqueue
					rear++;
					rear %= cQ_SIZE;
				}
			}
		}
	}
	return 0;
}

int res = 0;
char num1[6];
char num2[6];
char op;

int cnt = 0;
int equal_sign = 0;
int reset_sign = 0;

void calculator(void)
{

	char msg_buff[40];

	func_num = 0;

	if ((front % cQ_SIZE) != rear)
	{
		while (!equal_sign && !reset_sign)
		{
			if ((front % cQ_SIZE) != rear)
			{
				msg_buff[cnt] = keypad_queue[front];	//de queue
				front++;
				front %= cQ_SIZE;

				if(msg_buff[cnt] == '=')
				{
					equal_sign = 1;
					msg_buff[cnt] = '\0';
					cnt=0;
				}
				if(msg_buff[cnt] == ' ')
				{
					reset_sign = 1;
					cnt=0;

					i2c_lcd_init();
					reset_sign = 0;
					equal_sign = 0;
					return;

				}
				printf("%c\n", msg_buff[cnt]);
				move_cursor(0, cnt);
				lcd_data(msg_buff[cnt]);
				cnt++;
			}
		}

		if(reset_sign)
		{
			i2c_lcd_init();
			reset_sign = 0;
			equal_sign = 0;
			cnt = 0;
			return;
		}
		else
		{
			int i = 0;
			int j;

			for (j=0; msg_buff[j] >= '0' && msg_buff[j] <= '9'; j++)
			{
				num1[i++] = msg_buff[j];
			}
			num1[i] = '\0';
			op = msg_buff[j];

			for (i=0; msg_buff[j+1] >= '0' && msg_buff[j+1] <= '9'; j++)
			{
				num2[i++] = msg_buff[j+1];
			}
			num2[i] = '\0';

			switch(op)
			{
				case '+':
					res = atoi(num1) + atoi(num2);
				break;
				case '-':
					res = atoi(num1) - atoi(num2);
				break;
				case '*':
					res = atoi(num1) * atoi(num2);
				break;
				case '/':
					res = atoi(num1) / atoi(num2);
				break;
			}
			printf("%d\n",res);
			char result[12];
			sprintf(result, "%d", res);
			move_cursor(1, 0);
			lcd_string(result);
			equal_sign = 0;
		}
	}
}
