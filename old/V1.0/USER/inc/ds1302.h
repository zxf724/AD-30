#ifndef __DS1302_H
#define __DS1302_H

#include "sys.h"

void ds1302_GPIO_Configuration(void);
void write_1302byte(uint8_t dat);
uint8_t read_1302(uint8_t add);
void write_1302(uint8_t add,uint8_t dat);
void ds1302_init(uint8_t *write,uint8_t *time);
void ds1302_data(uint8_t *read);
void set_input(void);
void set_output(void);

#endif


