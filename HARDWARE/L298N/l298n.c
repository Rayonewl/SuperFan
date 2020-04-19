#include "l298n.h"

/**************************
初始化PB0、PB1，使能时钟		    
L298N IN1，IN2 初始化
/**************************/
void L298N_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	 //PB0，1 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB0，1
 GPIO_SetBits(GPIOB,GPIO_Pin_0);						 //PB.0 输出高
 GPIO_ResetBits(GPIOB,GPIO_Pin_1); 						 //PE.1 输出高 
	
}
 
