#include "stm32f10x.h"
#include"bsp_usart.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "bsp_led.h"
#include "test.h"

int main(void)
{
	/* 初始化 */
  USART_Config(); //初始化串口1	//重定向后课使用printf
	SysTick_Init();	//配置 SysTick 为1ms中断一次,使用其精确延时函数
	ESP8266_Init(); //初始化WiFi模块使用的接口和外设
  LED_Init();
	
  ESP8266_StaTcpClient_UnvarnishTest();//ESP测试实例函数
	
  while(1)
  { 
  }
	return 0;
}

