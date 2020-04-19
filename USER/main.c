#include "sys.h"
#include "l298n.h"
#include "exti.h"
#include "timer.h"
#include "ds18b20.h"

 int pwmval,flag=0;//pwmval���ڴ������ռ�ձȵıȽ�ֵ��flagΪģʽ��־λ
 int temperature;  //���ڴ�Ŵ�DS18B20��ȡ�����¶�
 int main(void)
 {
	Stm32_Clock_Init(9);            					//ϵͳʱ������
	delay_init(72);                					    //��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);     					//�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           					//��SWD�ӿ� �������������SWD�ӿڵ���
	KEY_Init(); 	 									//������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	L298N_Init();			     						//L298N�˿ڳ�ʼ��
	OLED_Init();										
	delay_ms(30);										//��oled��ʼ��һ�㻺��ʱ��
	TIM3_Int_Init(999,7199);	 						//����Ƶ��PWMƵ��=72000000/900=80Khz
	TIM4_PWM_Init(999,7199);							//PWM��ʼ��
	uart_init(72,128000);           					//��ʼ������1
	OLED_ShowString(0,0,"Temp:   . C");
	OLED_ShowString(0,14,"Fan:");
	OLED_ShowString(0,30,"Mode:");
    OLED_ShowString(0,45,"DS18B20:"); 
	OLED_Refresh_Gram();
	delay_ms(200);
 
 while(1)
	{
		while(DS18B20_Init())							//����¶ȴ�����
		{
			OLED_ShowString(64,45,"Error"); 			//�¶ȴ�����������
		}
		OLED_ShowString(64,45,"OK"); 					//�¶ȴ���������
		OLED_Refresh_Gram();
		delay_ms(200);
		if(flag%2==0)
		{
			OLED_ShowString(40,30,"Human");				//���밴��ģʽ
			if(KEY1==0)
			{
				OLED_ShowNumber(40,14,1,1,12);			//һ������
				pwmval=300;
			}
			else if	(KEY2==0)
			{
				OLED_ShowNumber(40,14,2,1,12);			//��������
				pwmval=600;
			}
			else if	(KEY3==0)
			{
				OLED_ShowNumber(40,14,3,1,12);			//��������
				pwmval=999;
			}
			
		}
		TIM_SetCompare1(TIM4,pwmval);					//����ռ�ձȣ����PWM
	}
 
 }
 
 