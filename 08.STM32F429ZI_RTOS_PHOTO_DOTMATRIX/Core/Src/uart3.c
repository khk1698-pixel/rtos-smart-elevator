#include "uart3.h"
#include "main.h"  // include interface about HAL

#define COMMAND_MAX 	40 // The maximum number of commands that can be stored
#define COMMAND_LENGTH		40 //Length 40 bytes per command
volatile int input_index2 = 0;   // 2D index value that stores characters received from UART
volatile int input_index1 = 0;     // 1D index value that stores characters received from UART
volatile int output_index2 = 0;   // Index value taken from while(1) loop

volatile int input_btindex1 = 0;     // 1D index value that stores characters received from BT UART
volatile int input_btindex2 = 0;
volatile int output_btindex2 = 0;

int printon = 1;   // printf default out
volatile unsigned char rx_buff[COMMAND_MAX][COMMAND_LENGTH]; // buffer to store strings
volatile unsigned char bt_buff[COMMAND_MAX][COMMAND_LENGTH]; // buffer to store BT strings

extern uint8_t rx_data; // PC data
extern uint8_t bt_data; // BT data
extern UART_HandleTypeDef huart3; // PC
extern UART_HandleTypeDef huart6; // BT
extern void set_rtc(char *date_time);
extern int func_num;
void pc_command_processing(void);
void bt_command_processing(void);

int led1toggle_time = 0;
int led2toggle_time = 0;
int led3toggle_time = 0;

volatile int bt_rx_receive_ok = 0;
extern volatile int TIM11_1ms_counter;
extern volatile int TIM10_10ms_counter;
extern volatile int t1ms_counter;

// move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
// Called when 1 byte is received from UART.
// When RX_INT is generated from UART2 / UART1, it enters here.
// The most important and precedent is input and output analysis.
// -- 입력 --
// ledallon\n
// ledalloff\n
// led_toggle\n
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) // *huart = save the type of UART_HandleTypeDef
{
  if (huart == &huart3)  // come in from PC
  {
	  unsigned char data;

	  data = rx_data;
//	  printf("%c", data);
	  if (output_index2 % COMMAND_MAX == (input_index2 + 1) % COMMAND_MAX)
	  {
		  printf("Queue Full!!!\n");
	  }
	  else
	  {
		  if(data == '\n' || data == '\r')
		  {
			  rx_buff[input_index2][input_index1] = '\0'; // End the sentence with \0.
			  input_index1 = 0;
			  input_index2++;
			  input_index2 %= COMMAND_MAX;
		  }
		  else  //   \n \r no recognize
		  {
			  rx_buff[input_index2][input_index1++] = data;
		  }
	  }
	  // warning : You must call HAL_UART2_Receive_IT to display the next RX_INT.
	  HAL_UART_Receive_IT(&huart3, &rx_data, 1); // assign to RX INTerrupt
  }
  if (huart == &huart6)  // come in from PC
   {
  	  unsigned char data;
  	  data = bt_data;
	  if (output_btindex2 % COMMAND_MAX == (input_btindex2 + 1) % COMMAND_MAX)
	  {
		  printf("Queue Full!!!\n");
	  }
	  else
	  {
		  if(data == '\n' || data == '\r')
  		  {
  			  bt_buff[input_btindex2][input_btindex1] = '\0'; // End the sentence with \0.
  			  input_btindex2++;
  			  bt_rx_receive_ok = 1;
  			  input_btindex1 = 0;
  			  input_btindex2 %= COMMAND_MAX;
  		  }
  		  else
  		  {
  			  bt_buff[input_btindex2][input_btindex1++] = data;
  		  }
		  HAL_UART_Receive_IT(&huart6, &bt_data, 1); // assign to RX INTerrupt
	  }
   }
}


void bt_command_processing()
{
	func_num = 4;

	// led1toggle 10ms
	if(bt_rx_receive_ok)
	{
		if(input_btindex2 != output_btindex2)
		{
			if (strncmp((const char *)bt_buff[output_btindex2],"led1toggle", strlen("led1toggle"))==NULL)
			{
				led1toggle_time = atoi(&bt_buff[output_btindex2][10]);  //ledtoggle300 ==> 300's ascii value change to int// [10]: 00110011 [11]: 00110000 [12]: 00110000
				TIM10_10ms_counter = 0;
			}
			else if (strncmp((const char *)bt_buff[output_btindex2],"led1off", strlen("led1off"))==NULL)
			{
				led1toggle_time = 0;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
			}
			bt_rx_receive_ok = 0;
			output_btindex2++;
			output_btindex2 %= COMMAND_MAX;
		}
	}
}

void pc_command_processing(void)
{
	func_num = 1;

	if(input_index2 != output_index2)  // If rx_buff contains data, it is processed.
	{
		printf("rx_buff[%d] : %s\n", output_index2, rx_buff[output_index2]);
		if (strncmp((const char *)rx_buff[output_index2],"led1toggle", strlen("led1toggle"))==NULL)
		{
			TIM10_10ms_counter = 0;
			led1toggle_time = atoi(&rx_buff[output_index2][10]);  //ledtoggle300 ==> 300's ascii value change to int
			                                                      // [10]: 00110011 [11]: 00110000 [12]: 00110000
			                                                      // ==> 000100101100 (300)
		}
		else if (strncmp((const char *)rx_buff[output_index2],"led2toggle", strlen("led2toggle"))==NULL)
		{
			t1ms_counter = 0;
			led2toggle_time = atoi(&rx_buff[output_index2][10]);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"led3toggle", strlen("led3toggle"))==NULL)
		{
			TIM11_1ms_counter = 0;
			led3toggle_time = atoi(&rx_buff[output_index2][10]);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"led1off", strlen("led1off"))==NULL)
		{
			led1toggle_time = 0;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"led2off", strlen("led2off"))==NULL)
		{
			led2toggle_time = 0;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"led3off", strlen("led3off"))==NULL)
		{
			led3toggle_time = 0;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"setrtc", strlen("setrtc"))==NULL)
		{	// setrtc230705160400
			set_rtc(&rx_buff[output_index2][6]);

		}
		else if (strncmp((const char *)rx_buff[output_index2],"printon", strlen("printon"))==NULL)
		{
			printon  = 1;
			printf(" printon:%d\n", printon);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"printoff", strlen("printoff"))==NULL)
		{
			printon  = 0;
			printf(" printoff:%d\n", printon);
		}
		else if (strncmp((const char *)rx_buff[output_index2],"printstatus", strlen("printstatus"))==NULL)
		{
			if(printon)
			{
				printf(" printon:%d\n", printon);
			}
			if(!printon)
			{
				printf(" printoff:%d\n", printon);
			}
		}
		output_index2++;
		output_index2 %= COMMAND_MAX;
	}
}
