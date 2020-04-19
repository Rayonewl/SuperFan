#include "l298n.h"

/**************************
��ʼ��PB0��PB1��ʹ��ʱ��		    
L298N IN1��IN2 ��ʼ��
/**************************/
void L298N_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	 //PB0��1 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB0��1
 GPIO_SetBits(GPIOB,GPIO_Pin_0);						 //PB.0 �����
 GPIO_ResetBits(GPIOB,GPIO_Pin_1); 						 //PE.1 ����� 
	
}
 
