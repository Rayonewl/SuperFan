#include "timer.h"
#include "l298n.h"
#include "usart.h"
#include "key.h"
#include "ds18b20.h" 
#include "oled.h"
/*********************************
通用定时器3中断初始化
这里时钟选择为APB1的2倍，而APB1为36M
这里使用的是定时
arr：自动重装值。
psc：时钟预分频数
***********************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 		 //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; 					 //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 					 //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 				 //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 			 //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 					 //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;              //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;           //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                              //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);                                       //使能TIM3外设
							 
}


/*************************
//定时器3中断服务程序
**************************/
extern  int temperature;
extern vu16 pwmval;
extern u16 flag;	
void TIM3_IRQHandler(void)   							   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 	   //检查指定的TIM中断发生与否:TIM 中断源 
		{
			temperature=DS18B20_Get_Temp();				   //获得温度
			if(temperature<0)
			{
				OLED_ShowChar(40,0,'-',12,1);	           //显示负号
				temperature=-temperature;		           //转为正数
			}
			else OLED_ShowChar(40,0,' ',12,1);             //去掉负号
			OLED_ShowNumber(40+8,0,temperature/10,2,12);   //显示整数
			OLED_ShowNumber(40+8+24,0,temperature%10,1,12);//显示小数部分	
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
			KEY_Scan(0);
		}
	if(KEY0==0)											   //KEY0按下可切换风扇模式
	{
		flag++;
		delay_ms(20);
	}
	if(flag%2==1)
	{
		OLED_ShowString(40,30,"Auto ");					   //进入自动模式
		if((temperature/10)<27)							   //温度小于27度为1档
		{
			OLED_ShowNumber(40,14,1,1,12);
			pwmval=300;
		}
		else if((temperature/10)>=27&&(temperature/10)<=28)//温度为27、28度为2档
		{
			OLED_ShowNumber(40,14,2,1,12);
			pwmval=600;
		}
		else if ((temperature/10)>28)						//温度大于28度为3档
		{
			OLED_ShowNumber(40,14,3,1,12);
			pwmval=999;
		}
	}
}

/****************************
//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
*****************************/
void TIM4_PWM_Init(u16 arr,u16 psc)
{  
	//定义相关变量
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		 //使能定时器3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   	 //使能GPIO外设和AFIO复用功能模块时钟

	//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;               	 //TIM2_CH3  L298N的PWM输入通道
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   			 //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	
	//配置TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; 				      //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 				      //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;		          //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				  //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channe1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 			  //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 	  //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  					  //根据T指定的参数初始化外设TIM4 OC1
	
	//通道使能
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 			  //使能TIM3在CCR2上的预装载寄存器
	
	//TIM3使能
	TIM_Cmd(TIM4, ENABLE); 										  
	

}




