#include "main.h"    // GPIO & HAL(Functions for
#include "i2c_lcd.h" // directly controlling the hardware) relative information
#include "cmsis_os.h"

void get_rtc_time(void);
void set_rtc(char *date_time);

extern osMutexId_t myMutex01Handle;
extern RTC_HandleTypeDef hrtc;
extern int button0_status;
extern int printon;

RTC_TimeTypeDef sTime = {0}; // Time information
RTC_DateTypeDef sDate = {0}; // date information

// ex) 23year saved to binary format
// 7654 3210
// ---- ----
// 0010 0011
// ---- ----
//    2    3
// ==> 23
//
unsigned char bin2dec(unsigned char byte)
{
	unsigned char high, low;

	low = byte & 0x0f; // ignore high 4bit and active low 4bit
	high = (byte >> 4) * 10;

	return high + low;
}
// decimal --> change to BCD     ex) 23 : 0001 0111
unsigned char dec2bin(unsigned char byte)
{
	unsigned char high, low;

	high = (byte / 10) << 4; // high 4bit 00000010 --> 00100000
	low = byte % 10;  // 00000011

	return high + low;  // 0010 0011
}
// Function to get date and time information from RTC
void get_rtc_time(void)   // return x receive x
{
	static RTC_TimeTypeDef oTime; // static : keep previous time value
	char lcd_buff[40];

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	if (oTime.Seconds != sTime.Seconds)
	{
//		if(printon)
//		{
//			// YYYY-MM-DD HH:mm:ss
//			printf("%04d-%02d-%02d %02d:%02d:%02d\n",
//					bin2dec(sDate.Year) + 2000, bin2dec(sDate.Month), bin2dec(sDate.Date),
//					bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
//		}
		if(!button0_status) // if (!button0_status) ==> if(button0_status == 0)
	    {
			if(osMutexWait(myMutex01Handle, 1000)==osOK)
			{
				sprintf(lcd_buff, "%04d-%02d-%02d %02d:%02d",
						bin2dec(sDate.Year) + 2000, bin2dec(sDate.Month), bin2dec(sDate.Date),
						bin2dec(sTime.Hours), bin2dec(sTime.Minutes));
				move_cursor(0,0);  // column, row
				lcd_string(lcd_buff);
				osMutexRelease(myMutex01Handle);	//unlock
			}
		}

	}
	oTime.Seconds = sTime.Seconds;
}
// Function to correct time and date
// ex) setrtc230705160400 -->The address of 230705160400 is transferred
// to a variable called data_time.
void set_rtc(char *date_time) // why use pointer? : variable to store memory address
{
	char yy[4], mm[4], dd[4];  // data
	char hh[4], min[4], ss[4];  // information of time

	strncpy(yy, &date_time[0], 2);
	strncpy(mm, date_time+2, 2);  // m[0]: '0', m[1]: '0', m[2]: '\0'
	// strncmp(mm, data_time+2, 2);
	strncpy(dd, date_time+4, 2);

	strncpy(hh, date_time+6, 2);
	strncpy(min, date_time+8, 2);
	strncpy(ss, date_time+10, 2);

	// ascii --> int --> bcd
	sDate.Year = dec2bin(atoi(yy));
	sDate.Month = dec2bin(atoi(mm));
	sDate.Date = dec2bin(atoi(dd));

	sTime.Hours = dec2bin(atoi(hh));
	sTime.Minutes = dec2bin(atoi(min));
	sTime.Seconds = dec2bin(atoi(ss));

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}
