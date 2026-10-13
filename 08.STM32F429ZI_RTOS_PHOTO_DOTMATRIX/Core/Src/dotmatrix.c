#if 1
// 하트모양 고정으로 display 하기
#include "main.h"
// // Bit 추출 메크로 : data 에서 loc 번째 비트 부터 area 영역 만큼 추출
// ex) extract_bits( 0b00000010, 1, 2) 결과 1 -> 0b00000010 데이터의 2번째 비트 1크기 만큼 추출
#define extract_bits(data, area, loc)   (((data) >> (loc)) & (area))
#define COL_ON			0
#define COL_OFF		    1
#define ROW_ON			1
#define ROW_OFF		    0

int dotmatrix_main_test(void);
void init_dotmatrix();
int dotmatrix_main(void);


#define STAY 0
#define UP 1
#define DOWN 2

extern int elva_status;
extern int floor_status;
extern int stepmotor_status;

#define FIRST 1
#define SECOND 2
#define THIRD 3
#define FOURTH 4

extern int ext_int_no;
extern int floor1;
extern int floor2;
extern int floor3;
extern int floor4;

extern int stepmotor_status;
#define FORWARD 1
#define BACKWARD 2


const uint8_t blank[8] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};

const uint8_t heart8x8[8] = {
    0b00000000,
    0b01100110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,
    0b00011000
};

uint8_t first_floor[10][10] =	// 1층 0~2 : up, 3~5 : down
{
	{// UP 1step
		0b00100000,
		0b11110000,
		0b00100000,
		0b00000000,
		0b10000010,
		0b11111111,
		0b10000000,
		0b00000000
	},
	{// UP 2step
		0b00001000,
		0b11111100,
		0b00001000,
		0b00000000,
		0b10000010,
		0b11111111,
		0b10000000,
		0b00000000
	},
	{// UP 3step
		0b00000010,
		0b11111111,
		0b00000010,
		0b00000000,
		0b10000010,
		0b11111111,
		0b10000000,
		0b00000000
	},
	{// DOWN 1step
		0b00000100,
		0b00001111,
		0b00000100,
		0b00000000,
		0b10000010,
		0b11111111,
		0b10000000,
		0b00000000
	},
	{// DOWN 2step
		0b00010000,
		0b00111111,
		0b00010000,
		0b00000000,
		0b10000010,
		0b11111111,
		0b10000000,
		0b00000000
	},
	{// DOWN 3step
		0b01000000,
		0b11111111,
		0b01000000,
		0b00000000,
		0b10000010,
		0b11111111,
		0b10000000,
		0b00000000
	},
};

uint8_t second_floor[10][10] =	// 2층 0~2 : up, 3~5 : down
{
	{// UP 1step
		0b00100000,
		0b11110000,
		0b00100000,
		0b11000010,
		0b10100001,
		0b10010001,
		0b10001001,
		0b10000110
	},
	{// UP 2step
		0b00001000,
		0b11111100,
		0b00001000,
		0b11000010,
		0b10100001,
		0b10010001,
		0b10001001,
		0b10000110
	},
	{// UP 3step
		0b00000010,
		0b11111111,
		0b00000010,
		0b11000010,
		0b10100001,
		0b10010001,
		0b10001001,
		0b10000110
	},
	{// DOWN 1step
		0b00000100,
		0b00001111,
		0b00000100,
		0b11000010,
		0b10100001,
		0b10010001,
		0b10001001,
		0b10000110
	},
	{// DOWN 2step
		0b00010000,
		0b00111111,
		0b00010000,
		0b11000010,
		0b10100001,
		0b10010001,
		0b10001001,
		0b10000110
	},
	{// DOWN 3step
		0b01000000,
		0b11111111,
		0b01000000,
		0b11000010,
		0b10100001,
		0b10010001,
		0b10001001,
		0b10000110
	},
};

uint8_t third_floor[10][10] =	// 3층 0~2 : up, 3~5 : down
{
	{// UP 1step
		0b00100000,
		0b11110000,
		0b00100000,
		0b01000010,
		0b10001001,
		0b10001001,
		0b10001001,
		0b01110110
	},
	{// UP 2step
		0b00001000,
		0b11111100,
		0b00001000,
		0b01000010,
		0b10001001,
		0b10001001,
		0b10001001,
		0b01110110
	},
	{// UP 3step
		0b00000010,
		0b11111111,
		0b00000010,
		0b01000010,
		0b10001001,
		0b10001001,
		0b10001001,
		0b01110110
	},
	{// DOWN 1step
		0b00000100,
		0b00001111,
		0b00000100,
		0b01000010,
		0b10001001,
		0b10001001,
		0b10001001,
		0b01110110
	},
	{// DOWN 2step
		0b00010000,
		0b00111111,
		0b00010000,
		0b01000010,
		0b10001001,
		0b10001001,
		0b10001001,
		0b01110110
	},
	{// DOWN 3step
		0b01000000,
		0b11111111,
		0b01000000,
		0b01000010,
		0b10001001,
		0b10001001,
		0b10001001,
		0b01110110
	},
};

uint8_t fourth_floor[10][10] =	// 3층 0~2 : up, 3~5 : down
{
	{// UP 1step
		0b00100000,
		0b11110000,
		0b00100000,
		0b00111110,
		0b00100000,
		0b00100000,
		0b11111111,
		0b00100000
	},
	{// UP 2step
		0b00001000,
		0b11111100,
		0b00001000,
		0b00111110,
		0b00100000,
		0b00100000,
		0b11111111,
		0b00100000
	},
	{// UP 3step
		0b00000010,
		0b11111111,
		0b00000010,
		0b00111110,
		0b00100000,
		0b00100000,
		0b11111111,
		0b00100000
	},
	{// DOWN 1step
		0b00000100,
		0b00001111,
		0b00000100,
		0b00111110,
		0b00100000,
		0b00100000,
		0b11111111,
		0b00100000
	},
	{// DOWN 2step
		0b00010000,
		0b00111111,
		0b00010000,
		0b00111110,
		0b00100000,
		0b00100000,
		0b11111111,
		0b00100000
	},
	{// DOWN 3step
		0b01000000,
		0b11111111,
		0b01000000,
		0b00111110,
		0b00100000,
		0b00100000,
		0b11111111,
		0b00100000
	},
};


uint8_t name_khk[20][10] =
{
	{					//김
		0b00000001,
		0b00010001,
		0b11101001,
		0b10100111,
		0b10100000,
		0b10100000,
		0b11111111,
		0b00000000
	},
	{					//학
		0b00001010,
		0b00110110,
		0b00110111,
		0b00110111,
		0b00101010,
		0b11111111,
		0b00001000,
		0b00000000
	},
	{					//겸
		0b00000001,
		0b00010001,
		0b11101001,
		0b10100111,
		0b10101010,
		0b10101010,
		0b11111111,
		0b00000000
	}
};

//uint8_t all_data[20][10] =
//{
//	{
//		0b01110000,	//0
//		0b10001000,
//		0b10011000,
//		0b10101000,
//		0b11001000,
//		0b10001000,
//		0b01110000,
//	    0b00000000
//	},
//	{
//		0b01000000,	//1
//		0b11000000,
//		0b01000000,
//		0b01000000,
//		0b01000000,
//		0b01000000,
//		0b11100000,
//	    6
//	},
//	{
//		0b01110000,	//2
//		0b10001000,
//		0b00001000,
//		0b00010000,
//		0b00100000,
//		0b01000000,
//		0b11111000,
//	    6
//	},
//	{
//		0b11111000,	//3
//	    0b00010000,
//		0b00100000,
//		0b00010000,
//		0b00001000,
//		0b10001000,
//		0b01110000,
//	    6
//	},
//	{
//		0b00010000,	//4
//		0b00110000,
//		0b01010000,
//		0b10010000,
//		0b11111000,
//		0b00010000,
//		0b00010000,
//	    6
//	},
//	{
//		0b11111000,	//5
//		0b10000000,
//		0b11110000,
//		0b00001000,
//		0b00001000,
//		0b10001000,
//		0b01110000,
//	    6
//	},
//	{
//		0b00110000,	//6
//		0b01000000,
//		0b10000000,
//		0b11110000,
//		0b10001000,
//		0b10001000,
//		0b01110000,
//	    6
//	},
//	{
//		0b11111000,	//7
//		0b10001000,
//		0b00001000,
//		0b00010000,
//		0b00100000,
//		0b00100000,
//		0b00100000,
//	    6
//	},
//	{
//		0b01110000,	//8
//		0b10001000,
//		0b10001000,
//		0b01110000,
//		0b10001000,
//		0b10001000,
//		0b01110000,
//	    6
//	},
//	{
//		0b01110000,	//9
//		0b10001000,
//		0b10001000,
//		0b01111000,
//		0b00001000,
//		0b00010000,
//		0b01100000,
//	    6
//	},
//	{
//		0b00000000,    // hart
//		0b01100110,
//		0b11111111,
//		0b11111111,
//		0b11111111,
//		0b01111110,
//		0b00111100,
//		0b00011000
//	}
//};

uint8_t allon[] = {			// allon 문자 정의
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
};

uint8_t display_data[8]; // 최종 8x8 출력 할 데이터
uint8_t scroll_buffer[60][8] = {0}; // 스크롤 할 데이터를 저장할 버퍼
int number_of_character = 3; // 출력할 문자의 갯수
int scroll_time = 100; // scroll_time 만큼 한 글자씩 시프트

GPIO_TypeDef *col_port[] =
{
	COL1_GPIO_Port,COL2_GPIO_Port,COL3_GPIO_Port,COL4_GPIO_Port,
	COL5_GPIO_Port,COL6_GPIO_Port,COL7_GPIO_Port,COL8_GPIO_Port
};

GPIO_TypeDef *row_port[] =
{
	ROW1_GPIO_Port,ROW2_GPIO_Port,ROW3_GPIO_Port,ROW4_GPIO_Port,
	ROW5_GPIO_Port,ROW6_GPIO_Port,ROW7_GPIO_Port,ROW8_GPIO_Port
};

uint16_t row_pin[] =
{
	ROW1_Pin,ROW2_Pin,ROW3_Pin,ROW4_Pin,
	ROW5_Pin,ROW6_Pin,ROW7_Pin,ROW8_Pin
};
uint16_t col_pin[] =
{
	COL1_Pin,COL2_Pin,COL3_Pin,COL4_Pin,
	COL5_Pin,COL6_Pin,COL7_Pin,COL8_Pin
};


void write_column_data(uint8_t col)
{
	for (int i=0; i < 8; i++)
	{
		if (i == col)
			HAL_GPIO_WritePin(col_port[i], col_pin[i], GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(col_port[i], col_pin[i], GPIO_PIN_SET);
	}
}

void write_row_data(uint8_t data)
{
	uint8_t d;

	d = data;

	for (int i=0; i < 8; i++)
	{
		if ( d & ( 1 << i) )
		{
			HAL_GPIO_WritePin(row_port[i], row_pin[i], GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(row_port[i], row_pin[i], GPIO_PIN_RESET);
		}
	}
}

void init_dotmatrix()
{
	for(int i = 0; i < 8; i++)
	{
		display_data[i] = name_khk[0][i];
	}

	for(int i = 1; i < number_of_character + 1 ; i++)
	{
		for(int j = 0 ; j < 8 ; j++)
		{
			scroll_buffer[i][j] = name_khk[i-1][j];
		}
	}

	for(int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(col_port[i], col_pin[i], GPIO_PIN_SET);  // LED를 all off
	}

}
/*

=============
scroll_buffer[0][0] >> 0: 0
scroll_buffer[1][0] << 8: 7000
display_data[0]: 0
scroll_buffer[0][1] >> 0: 0
scroll_buffer[1][1] << 8: 8800
display_data[1]: 0
scroll_buffer[0][2] >> 0: 0
scroll_buffer[1][2] << 8: 9800
display_data[2]: 0
scroll_buffer[0][3] >> 0: 0
scroll_buffer[1][3] << 8: a800
display_data[3]: 0
scroll_buffer[0][4] >> 0: 0
scroll_buffer[1][4] << 8: c800
display_data[4]: 0
scroll_buffer[0][5] >> 0: 0
scroll_buffer[1][5] << 8: 8800
display_data[5]: 0
scroll_buffer[0][6] >> 0: 0
scroll_buffer[1][6] << 8: 7000
display_data[6]: 0
scroll_buffer[0][7] >> 0: 0
scroll_buffer[1][7] << 8: 0
display_data[7]: 0
scroll_buffer[0][0] >> 1: 0
scroll_buffer[1][0] << 7: 3800
display_data[0]: 0
scroll_buffer[0][1] >> 1: 0
scroll_buffer[1][1] << 7: 4400
display_data[1]: 0
scroll_buffer[0][2] >> 1: 0
scroll_buffer[1][2] << 7: 4c00
display_data[2]: 0
scroll_buffer[0][3] >> 1: 0
scroll_buffer[1][3] << 7: 5400
display_data[3]: 0
scroll_buffer[0][4] >> 1: 0
scroll_buffer[1][4] << 7: 6400
display_data[4]: 0
scroll_buffer[0][5] >> 1: 0
scroll_buffer[1][5] << 7: 4400
display_data[5]: 0
scroll_buffer[0][6] >> 1: 0
scroll_buffer[1][6] << 7: 3800
display_data[6]: 0
scroll_buffer[0][7] >> 1: 0
scroll_buffer[1][7] << 7: 0
display_data[7]: 0
scroll_buffer[0][0] >> 2: 0
scroll_buffer[1][0] << 6: 1c00
display_data[0]: 0
scroll_buffer[0][1] >> 2: 0
scroll_buffer[1][1] << 6: 2200
display_data[1]: 0
scroll_buffer[0][2] >> 2: 0
scroll_buffer[1][2] << 6: 2600
display_data[2]: 0
scroll_buffer[0][3] >> 2: 0
scroll_buffer[1][3] << 6: 2a00
display_data[3]: 0
scroll_buffer[0][4]
 */
//int dotmatrix_main_test(void)
//{
//	static int count=0;
//	static int index=0;
//	static uint32_t past_time=0;
//
//	init_dotmatrix();
//
//printf("\n=============\n");
//	while(1)
//	{
//		uint32_t now = HAL_GetTick();	// 1ms마다 증가
//
//		/*
//			scroll_time 시간 마다 실행되는 if문
//			scroll_time 마다 최종 출력 데이터 display_data 를 한칸씩 시프트 한다.
//		*/
//		// ex) past_time : 0, now : 500
//		if (now - past_time >= 500) {  // 100ms마다 scroll
//			past_time = now;
//
//			for (int i=0; i < 8; i++)
//			{
//				display_data[i] = (scroll_buffer[index][i] >> count) | (scroll_buffer[index+1][i] << 8 - count);
//			}
//
//			if(++count == 8)
//			{
//				count = 0;
//				index++;
//				if (index == number_of_character + 1) index = 0;
//
////				while(1)
////					;
//			}
//	   }
//
//		for (int i = 0; i < 8; i++)
//		{
//			// 공통 양극 방식
//			// column에는 0을, ROW에는 1을 출력해야 해당 LED가 켜짐
//			write_column_data(i);
//			write_row_data(display_data[i]);
//
//			HAL_Delay(1);
//		}
//	}
//}

int dotmatrix_main(void)
{
	static int count=0;
	static int index=0;
	static uint32_t past_time=0;

//	init();
//	init_dotmatrix();

	uint32_t now = HAL_GetTick();	// 1ms마다 증가

	/*
		scroll_time 시간 마다 실행되는 if문
		scroll_time 마다 최종 출력 데이터 display_data 를 한칸씩 시프트 한다.
	*/
	// ex) past_time : 0, now : 500
	if (now - past_time >= 100) {  // 100ms마다 scroll
		past_time = now;

//		curr_floor;	// 현재 층(직전 층)
//				curr_up_down

		if(stepmotor_status == FORWARD)	// 엘리베이터 운행중, 화살표와 숫자 표시 (UP)
		{
			if(floor_status == FIRST)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor1)
					{
						display_data[i] = first_floor[index][i];
					}
					display_data[i] = first_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index == 2 + 1) index = 0;
				}
			}
			else if(floor_status == SECOND)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor2)
					{
						display_data[i] = second_floor[index][i];
					}
					display_data[i] = second_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index == 2 + 1) index = 0;
				}
			}
			else if(floor_status == THIRD)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor3)
					{
						display_data[i] = third_floor[index][i];
					}
					display_data[i] = third_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index == 2 + 1) index = 0;
				}
			}
			else if(floor_status == FOURTH)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor4)
					{
						display_data[i] = fourth_floor[index][i];
					}
					display_data[i] = fourth_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index >= 3) index = 0;
				}
			}

		}
		else if(stepmotor_status == BACKWARD)	// 엘리베이터 운행중, 화살표와 숫자 표시 (DOWN)
		{
			if(floor_status == FIRST)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor1)
					{
						display_data[i] = first_floor[index][i];
					}
					display_data[i] = first_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index >= 6) index = 3;
				}
			}
			else if(floor_status == SECOND)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor2)
					{
						display_data[i] = second_floor[index][i];
					}
					display_data[i] = second_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index >= 6) index = 3;
				}
			}
			else if(floor_status == THIRD)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor3)
					{
						display_data[i] = third_floor[index][i];
					}
					display_data[i] = third_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index >= 6) index = 3;
				}
			}
			else if(floor_status == FOURTH)
			{
				for (int i = 0; i < 8; i++)
				{
					if(floor4)
					{
						display_data[i] = fourth_floor[index][i];
					}
					display_data[i] = fourth_floor[index][i];
				}

				if(++count == 8)
				{
					count = 0;
					index++;
					if (index >=6) index = 3;
				}
			}
		}
		else	// 엘리베이터 운행 X, 이름 표시
		{
			for (int i=0; i < 8; i++)
			{
				display_data[i] = (scroll_buffer[index][i] >> count) | (scroll_buffer[index+1][i] << 8 - count);
			}

			if(++count == 8)
			{
				count = 0;
				index++;
				if (index == number_of_character + 1) index = 0;
			}
		}
   }

	for (int i = 0; i < 8; i++)
	{
		// 공통 양극 방식
		// column에는 0을, ROW에는 1을 출력해야 해당 LED가 켜짐
		write_column_data(i);
		write_row_data(display_data[i]);

		HAL_Delay(1);
	}
}
#endif

//==================================================================
#if 0
// 스마일 모양 고정으로 display 하기
#include "main.h"

#define COL_ON			0
#define COL_OFF		    1
#define ROW_ON			1
#define ROW_OFF		    0

int dotmatrix_main_test(void);

uint8_t smile[] = {			// 스마일 문자 정의
	0b00111100,
	0b01000010,
	0b10010101,
	0b10100001,
	0b10100001,
	0b10010101,
	0b01000010,
	0b00111100
};

uint8_t allon[] = {			// allon 문자 정의
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
};


// 행, 열의 PORT, PIN 설정
GPIO_TypeDef* col_port[] =
{
	COL1_GPIO_Port,COL2_GPIO_Port,COL3_GPIO_Port,COL4_GPIO_Port,
	COL5_GPIO_Port,COL6_GPIO_Port,COL7_GPIO_Port,COL8_GPIO_Port
};

GPIO_TypeDef* row_port[] =
{
	ROW1_GPIO_Port,ROW2_GPIO_Port,ROW3_GPIO_Port,ROW4_GPIO_Port,
	ROW5_GPIO_Port,ROW6_GPIO_Port,ROW7_GPIO_Port,ROW8_GPIO_Port
};

uint16_t row_pin[] =
{
	ROW1_Pin,ROW2_Pin,ROW3_Pin,ROW4_Pin,
	ROW5_Pin,ROW6_Pin,ROW7_Pin,ROW8_Pin
};
uint16_t col_pin[] =
{
	COL1_Pin,COL2_Pin,COL3_Pin,COL4_Pin,
	COL5_Pin,COL6_Pin,COL7_Pin,COL8_Pin
};

// 열마다 0으로 set
void write_column_data(uint8_t col)
{
	for (int i=0; i < 8; i++)
	{
		if (i==col)
			HAL_GPIO_WritePin(col_port[i], col_pin[i], GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(col_port[i], col_pin[i], GPIO_PIN_SET);
	}
}
// 0으로 set 된 열에서 1인 행만 High -> led on
void write_row_data(uint8_t data)
{
	uint8_t d;

	d = data;

	for (int i=0; i < 8; i++)
	{
		if ( d & ( 1 << i) )	// 1
		{
			HAL_GPIO_WritePin(row_port[i], row_pin[i], GPIO_PIN_SET);
		}
		else	// 0
		{
			HAL_GPIO_WritePin(row_port[i], row_pin[i], GPIO_PIN_RESET);
		}
	}
}


int dotmatrix_main_test(void)
{

	while(1)
	{
		for(int i = 0; i < 8; i++)
		{
			// 공통 양극 방식
			// column에는 0을, ROW에는 1을 출력해야 해당 LED가 켜짐
			write_column_data(i);
			write_row_data(smile[i]);

			HAL_Delay(1);	// 잔상효과 이용. col 계속 바꿔줌
		}
	}
}
#endif
