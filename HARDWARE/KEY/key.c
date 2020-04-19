#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

/****************************/								    
/*������ʼ������*/
/*****************************/
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);							//ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;  //KEY0-KEY3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 									//���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);											//��ʼ��GPIOE9,10,11��12
}


/****************************/	
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
/*****************************/
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1; //�������ɿ���־
	if(mode)key_up=1;   //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==1))
	{
		delay_ms(10);	//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
	}
	else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==0)
		key_up=1; 	    
 	return 0;			// �ް�������
}
