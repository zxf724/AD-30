
#include "ds1302.h"
#include "stdio.h"
#include "delay.h"
 
uint8_t read[] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};//????????????????????
uint8_t write[] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};//????????????????????


void ds1302_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_4; 		//clk?rst?????
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//IO?????
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
 
}

void set_input()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;				//IO?????
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void set_output()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//IO?????
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void write_1302byte(uint8_t dat)											//????????sck??????
{
 	uint8_t i = 0;
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);										//GPIO_Pin_2=0
	
  delay_us(2);//????2us
	
 	for(i = 0;i < 8;i ++)
 	    {
			GPIO_ResetBits(GPIOE,GPIO_Pin_2);	 	    				//GPIO_Pin_2=0;
				set_output();
			if(dat&0x01)

				GPIO_SetBits(GPIOE,GPIO_Pin_3);
			else																						//GPIO_Pin_3=(dat&0x01)
				GPIO_ResetBits(GPIOE,GPIO_Pin_3);				
	 		delay_us(2);
			GPIO_SetBits(GPIOE,GPIO_Pin_2);									//??????,clk???,//GPIO_Pin_2=1
	 		dat >>= 1;
	 		delay_us(1);
 		}	
	}
	
uint8_t read_1302(uint8_t add)												//???
{
	uint8_t i=0;
	uint8_t Return_dat=0x00;
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);		  							//GPIO_Pin_4=0;
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);										//GPIO_Pin_2=0;
	delay_us(3);																				//????2us
	GPIO_SetBits(GPIOE,GPIO_Pin_4);	  									//GPIO_Pin_4=1;
	delay_us(3);																				//?????3us
	write_1302byte(add);																//????????
	set_input();
	for(i=0;i<8;i++)
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2);		 								//GPIO_Pin_2=1;
		Return_dat >>= 1;
		GPIO_ResetBits(GPIOE,GPIO_Pin_2);									//GPIO_Pin_2=0;//?????,????????
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==1)			//?????????
		{Return_dat = Return_dat|0x80;}
	}
	  delay_us(1);
	  GPIO_ResetBits(GPIOE,GPIO_Pin_4);	 							  
	  return Return_dat;
}
void write_1302(uint8_t add,uint8_t dat)							
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);										
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);										

	delay_us(1);																				
	GPIO_SetBits(GPIOE,GPIO_Pin_4);											 
	delay_us(2);																				
	write_1302byte(add);																
	write_1302byte(dat);																
	GPIO_ResetBits(GPIOE,GPIO_Pin_4);										
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);									

	delay_us(1); 
}

void ds1302_init(uint8_t *write,uint8_t *time)
{
	uint8_t i=0,j=0;
	write_1302(0x8e,0x00);															
	for(i=0;i<7;i++)																		
	{
		j=time[i]%10;																			
		time[i]=(time[i]/10)*16+j;	
	}
	for(i=0;i<7;i++)																		
	{
		write_1302(write[i],time[i]);										
		
	}
    write_1302(0x8e,0x80);													
}
void ds1302_data(uint8_t *read)											
{
	uint8_t i=0,g[7],time[7];
	for(i=0;i<7;i++)
	{
		time[i]=read_1302(read[i]);												
	}
	for(i=0;i<7;i++)
	{
		g[i]=time[i]%16;																
		time[i]=time[i]/16;																
	}

	printf("20%d%d年%d%d月%d%d日%d%d:%d%d:%d%d星期%d\r\n",time[6],g[6],time[4],g[4],time[3],g[3],time[2],g[2],time[1],g[1],time[0],g[0],g[5]);
}

