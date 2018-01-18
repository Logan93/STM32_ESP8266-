#include "stm32f10x.h"
#include"bsp_usart.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "bsp_led.h"
#include "test.h"

int main(void)
{
	/* ��ʼ�� */
  USART_Config(); //��ʼ������1	//�ض�����ʹ��printf
	SysTick_Init();	//���� SysTick Ϊ1ms�ж�һ��,ʹ���侫ȷ��ʱ����
	ESP8266_Init(); //��ʼ��WiFiģ��ʹ�õĽӿں�����
  LED_Init();
	
  ESP8266_StaTcpClient_UnvarnishTest();//ESP����ʵ������
	
  while(1)
  { 
  }
	return 0;
}

