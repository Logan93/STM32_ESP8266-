#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_led.h"

volatile uint8_t ucTcpClosedFlag = 0;

char cStr [ 1500 ] = { 0 };

/**
  * @brief  ESP8266 （Sta Tcp Client）透传
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	//接收数据所需变量
	uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;//LED灯的状态

	char cStr[ 100 ] = { 0 }, cCh;

  char * pCh, * pCh1;
	
	uint8_t flag = 0;
	
	
	//发送数据所需变量
	uint8_t ucStatus;//获取连接状态
		
  printf ( "\r\n正在配置 ESP8266 ......\r\n" );

	macESP8266_CH_ENABLE();	//片选使能
	
	ESP8266_AT_Test ();	//对WF-ESP8266模块进行AT测试启动
	
	ESP8266_Net_Mode_Choose ( STA );//选择WF-ESP8266模块的工作模式为STA---站点连接服务器

  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	//根据WiFi名和密码连接热点：AT+CWJAP=。。。
	
	ESP8266_Enable_MultipleId ( DISABLE );//WF-ESP8266模块启动多连接
	
	//根据IP地址和端口连接服务器：TCP协议：sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);
	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
	
	while ( ! ESP8266_UnvarnishSend () );//配置WF-ESP8266模块进入透传发送
	
	printf ( "\r\n配置 ESP8266 完毕\r\n" );
	
	
	while ( 1 )
	{		
		//接收数据
		if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
		{
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //禁用串口接收中断
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
			
			//printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );	//打印接收到的信息
			if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LED_" ) ) != 0 ) 
			{
				flag = 1;	//使能ESP发送数据功能，将接收到的数据发送至服务器上
				
				cCh = * ( pCh + 8 );//获取Data_RX_BUF中第八位的值：1,2或3号灯
				
				//根据接收到的数据控制小灯
				switch ( cCh )
				{
					case '1':
						cCh = * ( pCh + 10 );//获取Data_RX_BUF中第十位的值：1开灯，0熄灯
					  switch ( cCh )
					  {
							case '0':
								macLED1_OFF ();
							  ucLed1Status = 0;
						    break;
							case '1':
								macLED1_ON ();
							  ucLed1Status = 1;
						    break;
							default :
								break;
						}
						break;
						
					case '2':
						cCh = * ( pCh + 10 );
					  switch ( cCh )
					  {
							case '0':
								macLED2_OFF ();
							  ucLed2Status = 0;
						    break;
							case '1':
								macLED2_ON ();
							  ucLed2Status = 1;
						    break;
							default :
								break;
						}
						break;

					case '3':
						cCh = * ( pCh + 10 );
					  switch ( cCh )
					  {
							case '0':
								macLED3_OFF ();
							  ucLed3Status = 0;
						    break;
							case '1':
								macLED3_ON ();
							  ucLed3Status = 1;
						    break;
							default :
								break;
						}
						break;
						
				  default :
            break;					
						
				}
			}
			
			//接收完毕后将esp8266的接收缓存清空，标志位也清空
		  strEsp8266_Fram_Record .InfBit .FramLength = 0;
	    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
			
			USART_ITConfig( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断
		}
		
		//发送数据：将esp8266接收到的数据发送至服务器上
		if(flag == 1)
		{
			flag = 0;
			
			sprintf( cStr,"%s\r\n",strEsp8266_Fram_Record .Data_RX_BUF);//格式化字符串：Data_RX_BUF的值赋给CStr
		
			ESP8266_SendString ( ENABLE,cStr, 0, Single_ID_0 );	//发送cStr数据
		
			Delay_ms ( 100 );
		
			}
		
			//检测是否失去连接
			if ( ucTcpClosedFlag )                    	//检测是否失去连接
			{
				ESP8266_ExitUnvarnishSend ();             //退出透传模式
			
				do ucStatus = ESP8266_Get_LinkStatus ();  //获取连接状态
				while ( ! ucStatus );
			
				if ( ucStatus == 4 )                      //确认失去连接后重连
				{
					printf ( "\r\n正在重连热点和服务器 ......\r\n" );
				
					while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
				
					while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
				
					printf ( "\r\n重连热点和服务器成功\r\n" );

				}
				while ( ! ESP8266_UnvarnishSend () );
		
		}
	}
		
}


