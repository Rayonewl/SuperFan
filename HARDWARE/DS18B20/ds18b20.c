#include "ds18b20.h"
#include "delay.h"

/********************************************************
��������͵�ƽ�����ֵ͵�ƽʱ������480us���Բ�����λ���壬	  //DS18B20��λ����
���������ͷ����ߣ�����ʱ15~16us�����������ģʽ 
*********************************************************/

void DS18B20_Rst(void)
{
	DS18B20_IO_OUT();//������DS18B20_IO���ģʽ
	DS18B20_DQ_OUT=0;//����
	delay_us(750);	 //��ʱ750us��480us-960us��
	DS18B20_DQ_OUT=1;//�ͷţ�������
	delay_us(15);	 //��ʱ15us	
}
/*******************************
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���		   //���DS18B20�Ƿ���ں���
//����0:����
*********************************/

u8 DS18B20_Check(void)
{   
	u8 retry=0;
	DS18B20_IO_IN();				//����DS18B20_IOģʽ 
    while (DS18B20_DQ_IN&&retry<200)//DS18B20_DQ_IN������Ϊ�����ţ��۲����Ƿ�Ϊ�͵�ƽ
	{
		retry++;					//���Ϊ�ߵ�ƽ����retry++
		delay_us(1);
	};	 
	if(retry>=200)return 1;			//δ��⵽18B20
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)// ��Ӧ��DS18B20����һ��240us�ĵ͵�ƽ����Ӧ��
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;			//�����������ߵ�ʱ����60~240us������240��û��⵽18B20     
	return 0;
}
/*********************************************************
//д1ʱ����������͵�ƽ����ʱ2us��Ȼ���ͷ����ߣ���ʱ60us		//дʱ��
//д0ʱ����������͵�ƽ����ʱ60us��Ȼ���ͷ����ߣ���ʱ2us
**********************************************************/
//DS18B20 д ����
void DS18B20_Write_Byte(u8 dat)
{
	u8 i;
	u8 testb=0;
	DS18B20_IO_OUT();//����DS18B20_IO���
	for(i=0;i<8;i++)
	{
		testb=dat&0x01;
		dat=dat>>1;
		if(testb)	//����ߣ�Ҳ����д1
		{
			DS18B20_DQ_OUT=0;
			delay_us(2);
			DS18B20_DQ_OUT=1;
			delay_us(60);
		}
		else 		//����ͣ�Ҳ����д0
		{
			DS18B20_DQ_OUT=0;
			delay_us(60);
			DS18B20_DQ_OUT=1;
			delay_us(2);
		}
	}
}

//********************************************************************
//��������͵�ƽ��ʱ2us���ͷź�����ת������ģʽ����ʱ12us��			
//Ȼ���ȡ�����ߵ�ǰ�ĵ�ƽ��Ȼ����ʱ50us									//��ʱ��
//�����ڶ�ʱ���ڼ�����ͷ����ߣ�������ʱ����ʼ���15us֮�ڲ�������״̬
//********************************************************************/
//DS18B20 �� ���� 
//�ú���ֻ�ܶ���һλ����
u8 DS18B20_Read_Bit(void)
{
	u8 date;
	DS18B20_IO_OUT();//����DS18B20_IOΪ���ģʽ
	DS18B20_DQ_OUT=0;//����
	delay_us(2);	 //��ʱ2us
	DS18B20_DQ_OUT=1;//�ͷţ������
	DS18B20_IO_IN(); //����DS18B20_IOΪ����ģʽ
	delay_us(12);	 //��ʱ12us
	if(DS18B20_DQ_IN) date=1;//�ж�����ĵ�ƽ�����Ϊ�ߵ�ƽ���λ����Ϊ1
	else date=0;
	delay_us(50);
	return date;
}


//�˺����ɶ���һ���ֽ�����
u8 DS18B20_Read_Byte(void)//����һ���ֽ�
{
	u8 i,j,dat=0;
	for(i=0;i<8;i++)		  //һ���ֽ�8λ������Ҫ��8��
	{
		j=DS18B20_Read_Bit(); //��һλ
		dat=((j<<7)|(dat>>1));//����һλ���ݶ���������λȻ�����ƣ�8�κ�����ݼ�Ϊһ���ֽڵ�����
	}
	return dat;
}

//�¶�ת��
void DS18B20_Start(void)
{
	DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
}
//***************************************
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������								//DS8B20��ʼ��
//����0:����  
//***************************************
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;   //����ṹ�����
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTG��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PORTG.11
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 // �������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //�ٶ�50HZ
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_12);    //���1

	DS18B20_Rst();						//��λ
	return DS18B20_Check();				//���DS18B20�Ƿ����
}  

//****************************
//1����λ
//2����鴫�����Ƿ����
//3��дָ�0xcc��
//4��дָ�0x44��
//5����λ						//��ȡ�¶�
//6����鴫�����Ƿ����
//7��дָ�0xcc��
//8��дָ�0xbe��
//9����ȡTH
//10����ȡTL
//****************************
//��ȡ�¶Ⱥ���
short DS18B20_Get_Temp(void)
{	
	short tem;		//tem����װ���¶�ֵ
	u8 TL,TH,temp;	//temp��Ϊ�¶������ı�־λ
	DS18B20_Start();
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
    DS18B20_Write_Byte(0xbe);
	TL=DS18B20_Read_Byte(); //��LSB
	TH=DS18B20_Read_Byte(); // MSB
	if(TH>7)				//��TH>xxxxx111���¶�Ϊ��
	{
		TH=~TH;
		TL=~TH;
		temp=0;
	}
	else temp=1;			//�¶�Ϊ��
	tem=TH;					//�Ȼ�ø�8λ
	tem=tem<<8;
	tem=tem+TL;				//��õװ�λ
	tem=(float)tem*0.625;   //ת��
	if(temp) return tem;    //�����¶�ֵ
	else  return -tem;    

}




