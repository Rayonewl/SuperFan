#include "ds18b20.h"
#include "delay.h"

/********************************************************
主机输出低电平，保持低电平时间至少480us，以产生复位脉冲，	  //DS18B20复位函数
接着主机释放总线，，延时15~16us，并进入接收模式 
*********************************************************/

void DS18B20_Rst(void)
{
	DS18B20_IO_OUT();//配置与DS18B20_IO输出模式
	DS18B20_DQ_OUT=0;//拉低
	delay_us(750);	 //延时750us（480us-960us）
	DS18B20_DQ_OUT=1;//释放，即拉高
	delay_us(15);	 //延时15us	
}
/*******************************
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在		   //检测DS18B20是否存在函数
//返回0:存在
*********************************/

u8 DS18B20_Check(void)
{   
	u8 retry=0;
	DS18B20_IO_IN();				//配置DS18B20_IO模式 
    while (DS18B20_DQ_IN&&retry<200)//DS18B20_DQ_IN的作用为读引脚，观察其是否为低电平
	{
		retry++;					//如果为高电平，则retry++
		delay_us(1);
	};	 
	if(retry>=200)return 1;			//未检测到18B20
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)// 对应着DS18B20返回一个240us的低电平脉冲应答
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;			//由于拉低总线的时间在60~240us，大于240则没检测到18B20     
	return 0;
}
/*********************************************************
//写1时序：主机输出低电平，延时2us，然后释放总线，延时60us		//写时序
//写0时序：主机输出低电平，延时60us，然后释放总线，延时2us
**********************************************************/
//DS18B20 写 函数
void DS18B20_Write_Byte(u8 dat)
{
	u8 i;
	u8 testb=0;
	DS18B20_IO_OUT();//配置DS18B20_IO输出
	for(i=0;i<8;i++)
	{
		testb=dat&0x01;
		dat=dat>>1;
		if(testb)	//输出高，也就是写1
		{
			DS18B20_DQ_OUT=0;
			delay_us(2);
			DS18B20_DQ_OUT=1;
			delay_us(60);
		}
		else 		//输出低，也就是写0
		{
			DS18B20_DQ_OUT=0;
			delay_us(60);
			DS18B20_DQ_OUT=1;
			delay_us(2);
		}
	}
}

//********************************************************************
//主机输出低电平延时2us，释放后，主机转入输入模式，延时12us，			
//然后读取单总线当前的电平，然后延时50us									//读时序
//主机在读时序期间必须释放总线，并且在时序起始后的15us之内采样总线状态
//********************************************************************/
//DS18B20 读 函数 
//该函数只能读出一位数据
u8 DS18B20_Read_Bit(void)
{
	u8 date;
	DS18B20_IO_OUT();//配置DS18B20_IO为输出模式
	DS18B20_DQ_OUT=0;//拉低
	delay_us(2);	 //延时2us
	DS18B20_DQ_OUT=1;//释放，输出高
	DS18B20_IO_IN(); //配置DS18B20_IO为输入模式
	delay_us(12);	 //延时12us
	if(DS18B20_DQ_IN) date=1;//判断输入的电平，如果为高电平则该位数据为1
	else date=0;
	delay_us(50);
	return date;
}


//此函数可读出一个字节数据
u8 DS18B20_Read_Byte(void)//读出一个字节
{
	u8 i,j,dat=0;
	for(i=0;i<8;i++)		  //一个字节8位，所以要读8次
	{
		j=DS18B20_Read_Bit(); //读一位
		dat=((j<<7)|(dat>>1));//将第一位数据读出后放最高位然后左移，8次后的数据即为一个字节的数据
	}
	return dat;
}

//温度转换
void DS18B20_Start(void)
{
	DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
}
//***************************************
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在								//DS8B20初始化
//返回0:存在  
//***************************************
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;   //定义结构体变量
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PORTG口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PORTG.11
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 // 推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //速度50HZ
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_12);    //输出1

	DS18B20_Rst();						//复位
	return DS18B20_Check();				//检测DS18B20是否存在
}  

//****************************
//1、复位
//2、检查传感器是否存在
//3、写指令（0xcc）
//4、写指令（0x44）
//5、复位						//读取温度
//6、检查传感器是否存在
//7、写指令（0xcc）
//8、写指令（0xbe）
//9、读取TH
//10、读取TL
//****************************
//读取温度函数
short DS18B20_Get_Temp(void)
{	
	short tem;		//tem用于装载温度值
	u8 TL,TH,temp;	//temp作为温度正负的标志位
	DS18B20_Start();
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
    	DS18B20_Write_Byte(0xbe);
	TL=DS18B20_Read_Byte(); //读LSB
	TH=DS18B20_Read_Byte(); // MSB
	if(TH>7)				//（TH>xxxxx111）温度为负
	{
		TH=~TH;
		TL=~TH;
		temp=0;
	}
	else temp=1;			//温度为正
	tem=TH;					//先获得高8位
	tem=tem<<8;
	tem=tem+TL;				//获得底八位
	tem=(float)tem*0.625;   //转换
	if(temp) return tem;    //返回温度值
	else  return -tem;    

}




