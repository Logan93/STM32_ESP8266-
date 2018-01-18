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
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	//���������������
	uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;//LED�Ƶ�״̬

	char cStr[ 100 ] = { 0 }, cCh;

  char * pCh, * pCh1;
	
	uint8_t flag = 0;
	
	
	//���������������
	uint8_t ucStatus;//��ȡ����״̬
		
  printf ( "\r\n�������� ESP8266 ......\r\n" );

	macESP8266_CH_ENABLE();	//Ƭѡʹ��
	
	ESP8266_AT_Test ();	//��WF-ESP8266ģ�����AT��������
	
	ESP8266_Net_Mode_Choose ( STA );//ѡ��WF-ESP8266ģ��Ĺ���ģʽΪSTA---վ�����ӷ�����

  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	//����WiFi�������������ȵ㣺AT+CWJAP=������
	
	ESP8266_Enable_MultipleId ( DISABLE );//WF-ESP8266ģ������������
	
	//����IP��ַ�Ͷ˿����ӷ�������TCPЭ�飺sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);
	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
	
	while ( ! ESP8266_UnvarnishSend () );//����WF-ESP8266ģ�����͸������
	
	printf ( "\r\n���� ESP8266 ���\r\n" );
	
	
	while ( 1 )
	{		
		//��������
		if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
		{
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //���ô��ڽ����ж�
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
			
			//printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );	//��ӡ���յ�����Ϣ
			if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LED_" ) ) != 0 ) 
			{
				flag = 1;	//ʹ��ESP�������ݹ��ܣ������յ������ݷ�������������
				
				cCh = * ( pCh + 8 );//��ȡData_RX_BUF�еڰ�λ��ֵ��1,2��3�ŵ�
				
				//���ݽ��յ������ݿ���С��
				switch ( cCh )
				{
					case '1':
						cCh = * ( pCh + 10 );//��ȡData_RX_BUF�е�ʮλ��ֵ��1���ƣ�0Ϩ��
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
			
			//������Ϻ�esp8266�Ľ��ջ�����գ���־λҲ���
		  strEsp8266_Fram_Record .InfBit .FramLength = 0;
	    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
			
			USART_ITConfig( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�
		}
		
		//�������ݣ���esp8266���յ������ݷ�������������
		if(flag == 1)
		{
			flag = 0;
			
			sprintf( cStr,"%s\r\n",strEsp8266_Fram_Record .Data_RX_BUF);//��ʽ���ַ�����Data_RX_BUF��ֵ����CStr
		
			ESP8266_SendString ( ENABLE,cStr, 0, Single_ID_0 );	//����cStr����
		
			Delay_ms ( 100 );
		
			}
		
			//����Ƿ�ʧȥ����
			if ( ucTcpClosedFlag )                    	//����Ƿ�ʧȥ����
			{
				ESP8266_ExitUnvarnishSend ();             //�˳�͸��ģʽ
			
				do ucStatus = ESP8266_Get_LinkStatus ();  //��ȡ����״̬
				while ( ! ucStatus );
			
				if ( ucStatus == 4 )                      //ȷ��ʧȥ���Ӻ�����
				{
					printf ( "\r\n���������ȵ�ͷ����� ......\r\n" );
				
					while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
				
					while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
				
					printf ( "\r\n�����ȵ�ͷ������ɹ�\r\n" );

				}
				while ( ! ESP8266_UnvarnishSend () );
		
		}
	}
		
}


