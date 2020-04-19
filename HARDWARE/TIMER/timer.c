#include "timer.h"
#include "l298n.h"
#include "usart.h"
#include "key.h"
#include "ds18b20.h" 
#include "oled.h"
/*********************************
ͨ�ö�ʱ��3�жϳ�ʼ��
����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
����ʹ�õ��Ƕ�ʱ
arr���Զ���װֵ��
psc��ʱ��Ԥ��Ƶ��
***********************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 		 //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; 					 //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 					 //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 				 //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 			 //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 					 //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;              //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;           //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                              //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);                                       //ʹ��TIM3����
							 
}


/*************************
//��ʱ��3�жϷ������
**************************/
extern  int temperature;
extern vu16 pwmval;
extern u16 flag;	
void TIM3_IRQHandler(void)   							   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 	   //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
			temperature=DS18B20_Get_Temp();				   //����¶�
			if(temperature<0)
			{
				OLED_ShowChar(40,0,'-',12,1);	           //��ʾ����
				temperature=-temperature;		           //תΪ����
			}
			else OLED_ShowChar(40,0,' ',12,1);             //ȥ������
			OLED_ShowNumber(40+8,0,temperature/10,2,12);   //��ʾ����
			OLED_ShowNumber(40+8+24,0,temperature%10,1,12);//��ʾС������	
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
			KEY_Scan(0);
		}
	if(KEY0==0)											   //KEY0���¿��л�����ģʽ
	{
		flag++;
		delay_ms(20);
	}
	if(flag%2==1)
	{
		OLED_ShowString(40,30,"Auto ");					   //�����Զ�ģʽ
		if((temperature/10)<27)							   //�¶�С��27��Ϊ1��
		{
			OLED_ShowNumber(40,14,1,1,12);
			pwmval=300;
		}
		else if((temperature/10)>=27&&(temperature/10)<=28)//�¶�Ϊ27��28��Ϊ2��
		{
			OLED_ShowNumber(40,14,2,1,12);
			pwmval=600;
		}
		else if ((temperature/10)>28)						//�¶ȴ���28��Ϊ3��
		{
			OLED_ShowNumber(40,14,3,1,12);
			pwmval=999;
		}
	}
}

/****************************
//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
*****************************/
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	//������ر���
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		 //ʹ�ܶ�ʱ��3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   	 //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

	//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;               	 //TIM2_CH3  L298N��PWM����ͨ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   			 //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	
	//����TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; 				      //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 				      //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;		          //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				  //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM4 Channe1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 			  //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	  //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  					  //����Tָ���Ĳ�����ʼ������TIM4 OC1
	
	//ͨ��ʹ��
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 			  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	//TIM3ʹ��
	TIM_Cmd(TIM4, ENABLE); 										  
	

}




