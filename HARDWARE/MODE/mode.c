#include "mode.h"
#include "key.h"
#include "oled.h"
#include "ds18b20.h"
extern vu16 pwmval;
void Human_Mode(void)
{
	OLED_ShowString(40,30,"Human");
	delay_ms(20);
		if(KEY1==0)
		{
			delay_ms(10);
			if(KEY1==0)
			{
				pwmval=300;
				OLED_ShowNumber(40,14,1,1,12);
			}
		
		}	
		else if(KEY2==0) 
		{
			delay_ms(10);
			if(KEY2==0)
			{
				pwmval=600;
				OLED_ShowNumber(40,14,2,1,12);
			}
		
		}	
 		else if(KEY3==0) 
		{
			delay_ms(10);
			if(KEY3==0)
			{
				pwmval=899;
				OLED_ShowNumber(40,14,3,1,12);
			
			}
		}	
		OLED_Refresh_Gram();
		delay_ms(30);		
		TIM_SetCompare3(TIM2,pwmval);	
}
extern u8 temperature;
void Auto_Mode(void)
{
	u8 temp;
	temp=temperature/10;
	OLED_ShowString(40,30,"Auto ");
	delay_ms(20);
	if(temp<26)
		{
			pwmval=600;
			OLED_ShowNumber(40,14,1,1,12);
			delay_ms(200);

		}
	else if(temp>=26&&temp<28)
		{
			pwmval=300;
			OLED_ShowNumber(40,14,2,1,12);
				delay_ms(200);
		}
	else if(temp>=28)
		{
			pwmval=999;
			OLED_ShowNumber(40,14,3,1,12);
				delay_ms(200);
		}
	OLED_Refresh_Gram();
	delay_ms(30);		
	TIM_SetCompare3(TIM2,pwmval);	

}

