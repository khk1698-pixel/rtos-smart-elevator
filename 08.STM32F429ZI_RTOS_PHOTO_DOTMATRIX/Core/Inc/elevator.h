#include "main.h" // for HAL GPIO handling

int floor_L = 0;
int floor1 = 0;
int floor2 = 0;
int floor3 = 0;
int floor4 = 0;

#define STAY 0
#define UP 1
#define DOWN 2

#define L 0
#define FIRST 1
#define SECOND 2
#define THIRD 3
#define FOURTH 4

int elva_status = STAY;
int floor_status = L;
