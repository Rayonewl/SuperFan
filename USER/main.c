#include "sys.h"
#include "l298n.h"
#include "exti.h"
#include "timer.h"
#include "ds18b20.h"

 int pwmval,flag=0;//pwmval用于存放设置占空比的比较值，flag为模式标志位
 int temperature;  //用于存放从DS18B20获取到的温度
 int main(void)
 {
	Stm32_Clock_Init(9);            					//系统时钟设置
	delay_init(72);                					    //延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);     					//关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           					//打开SWD接口 可以利用主板的SWD接口调试
	KEY_Init(); 	 									//按键初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	L298N_Init();			     						//L298N端口初始化
	OLED_Init();										
	delay_ms(30);										//给oled初始化一点缓冲时间
	TIM3_Int_Init(999,7199);	 						//不分频。PWM频率=72000000/900=80Khz
	TIM4_PWM_Init(999,7199);							//PWM初始化
	uart_init(72,128000);           					//初始化串口1
	OLED_ShowString(0,0,"Temp:   . C");
	OLED_ShowString(0,14,"Fan:");
	OLED_ShowString(0,30,"Mode:");
        OLED_ShowString(0,45,"DS18B20:"); 
	OLED_Refresh_Gram();
	delay_ms(200);
 
 while(1)
	{
		while(DS18B20_Init())							//检测温度传感器
		{
			OLED_ShowString(64,45,"Error"); 			//温度传感器不存在
		}
		OLED_ShowString(64,45,"OK"); 					//温度传感器存在
		OLED_Refresh_Gram();
		delay_ms(200);
		if(flag%2==0)
		{
			OLED_ShowString(40,30,"Human");				//进入按键模式
			if(KEY1==0)
			{
				OLED_ShowNumber(40,14,1,1,12);			//一档风速
				pwmval=300;
			}
			else if	(KEY2==0)
			{
				OLED_ShowNumber(40,14,2,1,12);			//二档风速
				pwmval=600;
			}
			else if	(KEY3==0)
			{
				OLED_ShowNumber(40,14,3,1,12);			//三档风速
				pwmval=999;
			}
			
		}
		TIM_SetCompare1(TIM4,pwmval);					//设置占空比，输出PWM
	}
 
 }
 
 
