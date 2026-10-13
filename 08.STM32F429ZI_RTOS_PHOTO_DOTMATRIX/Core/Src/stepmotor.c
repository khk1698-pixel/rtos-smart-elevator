#include "main.h"	// GPIO PIN setting
#include "button.h"


void set_rpm(int rpm);
void stepmotor_main_test();
void stepmotor_driver(int direction);

#define STAY 0
#define UP 1
#define DOWN 2

int prev_elva_status = STAY;

extern void delay_us(unsigned long us);
extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);
extern void button0_status_check(void);
extern void button_elva(void);
extern void elevate(void);

extern int button0_status;
extern int floor_L;

/*
 ** RPM(Revolution Per Minutes)
 ** 1 Minute = 60 seconds : 1,000,000us(1sec) X 60 = 60,000,000us
 ** 1 second = 1000ms	 : 1ms(1000us) X 1000ms = 1,000,000us
 ** 60,000,000
 ** 4096 : 1분 4096 step 이동
 ** 시계방향 <---> 반시계방향
 ** 1바퀴 회전에 필요한 step : 4096
 ** 4096 / 8(0.7도) ==> 512 sequence : 360도
 ** 1 sequence 당 (8 step) ==> 0.70312도
 ** 0.70312 X 512 ==> 360도
 **
 ** -------set_rpm(13) 일 때의 동작-----
 ** 60,000,000 / 4096(1회전에 필요한 스텝)
 ** 			=4612096us
 ** 			=4612ms
 ** 			=4.6s
 ** 60(s) / 4.6(s) ==> 13(회전)
 ** 이를 이용해 동작시키는 C 코드 작성
 */

void set_rpm(int rpm)	// rpm 1 ~ 13 : MAX 13, min 1
{
	delay_us(60000000/4096/rpm);
	// 최대 speed 기준(13) : delay_us(1126)
}

// 시계방향 1회전, 반시계방향 1회전 반복하는 test program
void stepmotor_main_test()
{
	while(1)
	{
#if 1
		stepmotor_main();
#else
		for(int i = 0; i < 512 ; i++)	// 시계방향 1회전
		{
			for(int j = 0; j < 8 ; j++)	// 1 Sequence당 0.7도 회전
			{
				stepmotor_driver(j);
				set_rpm(11);
			}
		}
		for(int i = 0; i < 512 ; i++)	// 반시계방향 1회전
		{
			for(int j = 7; j >= 0 ; j--)	// 1 Sequence당 0.7도 회전
			{
				stepmotor_driver(j);
				set_rpm(13);
			}
		}
#endif
	}
}

//btn0 : forward
//btn1 : backward
#define IDLE 0
#define FORWARD 1
#define BACKWARD 2

int stepmotor_status = IDLE;
extern volatile int ext_int_no;
void stepmotor_main()
{
//	while(1)
//	{
		switch(stepmotor_status)
		{
			case IDLE :
				if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
				{
					stepmotor_status = FORWARD;   // ( 0 <--> 1)
				}
				break;
			case FORWARD :
				stepmotor_driver(stepmotor_status);
				set_rpm(13);	//rpm 속도 조절
				if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
				{
					stepmotor_status = BACKWARD;
				}
				if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, BUTTON1) == BUTTON_PRESS)
				{
					stepmotor_status = IDLE;
				}
				if(ext_int_no != -1)
				{
					ext_int_no = -1;
					stepmotor_status = IDLE;
				}
				break;
			case BACKWARD :
				stepmotor_driver(stepmotor_status);
				set_rpm(13);	//rpm 속도 조절
				if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, BUTTON0) == BUTTON_PRESS)
				{
					stepmotor_status = FORWARD;
				}
				if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, BUTTON1) == BUTTON_PRESS)
				{
					stepmotor_status = IDLE;
				}
				if(ext_int_no != -1)
				{
					ext_int_no = -1;
					stepmotor_status = IDLE;
				}
				break;
		}
//	}
}

void stepmotor_elva(void)
{

	extern int elva_status;
	button_elva();
	elevate();

	switch(stepmotor_status)
	{
		case IDLE :
			if (elva_status == UP)
			{
				stepmotor_status = FORWARD;   // ( 0 <--> 1)
				prev_elva_status = elva_status;
			}
			else if (elva_status == DOWN)
			{
				stepmotor_status = BACKWARD;
				prev_elva_status = elva_status;
			}
			break;
		case FORWARD :
			stepmotor_driver(stepmotor_status);
			set_rpm(13);	//rpm 속도 조절
			if(elva_status == STAY)
			{
				stepmotor_status = IDLE;
			}
			break;
		case BACKWARD :
			stepmotor_driver(stepmotor_status);
			set_rpm(13);	//rpm 속도 조절
			if(elva_status == STAY)
			{
				stepmotor_status = IDLE;
			}
			break;
	}
}

#if 1
void stepmotor_driver(int direction)
{
	static int step = 0;

	switch(step)
	{
	case 0 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 1);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 1 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 1);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 2 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 3 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 4 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 5 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);	// IN4
		break;
	case 6 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);	// IN4
		break;
	case 7 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 1);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);	// IN4
		break;
	}
	if (direction == FORWARD)
	{
		step++;
		step %= 8;	//다음애 진행할 step 준비
	}
	else if(direction == BACKWARD)
	{
		step--;
		if (step < 0)
			step = 7;	//다음애 진행할 step 준비
	}
}

#else
void stepmotor_driver(int step)
{
	static int step = 0;

	switch(step)
	{
	case 0 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 1);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 1 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 1);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 2 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 3 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 4 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);	// IN4
		break;
	case 5 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);	// IN4
		break;
	case 6 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 0);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);	// IN4
		break;
	case 7 :
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, 1);	// IN1
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);	// IN2
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);	// IN3
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);	// IN4
		break;
	}
}
#endif
