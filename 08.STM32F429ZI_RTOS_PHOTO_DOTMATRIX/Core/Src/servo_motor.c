#include "main.h"

void servo_motor_test_main();

extern TIM_HandleTypeDef htim2;
//  50kHz: 16000000/3360
//	T = 1/f = 1/50000 1펄스의 소요시간 0.00002 sec
//	2ms(180도) : 0.00002 x 100(개)
//	1.5ms(90도) : 0.00002 x 75(갸)
//	1ms(0도) : 0.00002 x 50(개)
void servo_motor_test_main()
{
	while(1)
	{
		// 180도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 62);
		HAL_Delay(1000);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 40);
		HAL_Delay(1000);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 18);
		HAL_Delay(1000);
	}
}
