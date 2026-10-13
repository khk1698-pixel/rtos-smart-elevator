/*
 * ultrasonic.c
 *
 *  Created on: Apr 6, 2023
 *      Author: kccistc
 */

#include "ultrasonic.h"
#include "cmsis_os.h"

extern volatile int t10ms_ultrasonic;
extern void delay_us(unsigned int us);
extern osMutexId_t myMutex01Handle;

volatile int distance;   // 거리 측정한 값을 저장하는 변수
volatile int ic_cpt_finish_flag=0;  // 초음파 거리 측정 완료 indicator변수

// 1. Driver/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
//  에 가서 잘라내기를 해서 이곳으로 가져 온다.
// 초음파센서의 ECHO핀의 상승에지 하강에지 발생시 이곳으로 들어 온다.
uint8_t is_first_capture=0;  // 상승에지:0 하강:1
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (is_first_capture == 0)   // 상승 에지
		{

			__HAL_TIM_SET_COUNTER(htim,0); // clear counter
			is_first_capture = 1; // 상승에지를 만났다는 변수 indicator를 set
		}
		else if (is_first_capture == 1)  // 하강에지를 만나면
		{

			is_first_capture=0;
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			ic_cpt_finish_flag=1;   // 초음파 측정 완료
		}

	}
}

//
void ultrasonic_main(void)
{
	int dis;		//거리 측정한 값을 저장하는 변수
	char lcd_buff[10];
//	while(1)
//	{
		if (t10ms_ultrasonic >= 100)  // 1초
		{
			t10ms_ultrasonic=0;
			make_trigger();

			if (ic_cpt_finish_flag)   // 초음파 측정이 완료 되었으면
			{
				ic_cpt_finish_flag=0;
				dis = distance;
				dis = dis * 0.034 / 2;   // 1us가 0.034cm를 이동
				// 왕복 거리가 리턴 되기 떄문에 / 2로 해주는 것임.
				if (osMutexWait(myMutex01Handle, 1000)==osOK)
				{
					sprintf(lcd_buff, "%d", dis);
					move_cursor(1,14);
					lcd_string(lcd_buff);
					osMutexRelease(myMutex01Handle);	//unlock
				}
				printf("dis : %dcm\n", dis);
			}
		}
//	}
}

void make_trigger()  //  make_trigger()
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, GPIO_PIN_RESET);
}
