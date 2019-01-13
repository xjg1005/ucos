
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rak_config.h"
#include "wifi_module.h"
#include "rak_init_module.h"
#include "delay.h"
#include "app_wifi.h"
rak_api rak_strapi;
rak_CmdRsp	 uCmdRspFrame;
uint8_t msg_type;

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPIx, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPIx); //����ͨ��SPIx������յ�����	
 		    
}

void wifi_SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//ʹ��SPIxʱ��
	
 	GPIO_InitStructure.GPIO_Pin=SPIx_CS_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(SPIx_CS_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(SPIx_CS_GPIO_PORT,SPIx_CS_Pin); //GPIOA 

	GPIO_InitStructure.GPIO_Pin=Reset_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(Reset_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(Reset_GPIO_PORT,Reset_Pin); //GPIOA 

  //GPIOFB3,4,5��ʼ������
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_Pin|SPIx_MISO_Pin|SPIx_MOSI_Pin;//PB3~5���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,GPIO_PinSource10,GPIO_AF_SPI3); //PA5����Ϊ SPIx
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,GPIO_PinSource11,GPIO_AF_SPI3); //PA6����Ϊ SPIx
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,GPIO_PinSource12,GPIO_AF_SPI3); //PA7����Ϊ SPIx
 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);//��λSPIx
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//ֹͣ��λSPIx

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPIx, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPIx, ENABLE); //ʹ��SPI����

	SPI1_ReadWriteByte(0xff);//��������		 
}   

static void Reset_config(void)
{
	GPIO_ResetBits(Reset_GPIO_PORT, Reset_Pin);
	delay_ms(50);
	GPIO_SetBits(Reset_GPIO_PORT, Reset_Pin);
	delay_ms(200);

}
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11);//PA11 ���ӵ��ж���11
	
  /* ����EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;//LINE2
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE2
  EXTI_Init(&EXTI_InitStructure);//����
	
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	   
}
//�ⲿ�ж�2�������
void EXTI15_10_IRQHandler(void)
{
	OS_ERR err;
	if((GPIO_ReadInputDataBit(SPIx_INT_GPIO_PORT,SPIx_INT_Pin))){
		msg_type = MSG_NOTIFY_GET_DATA;
		printf("report get data\r\n");
		OSQPost((OS_Q*		)&WIFI_Msg,		
			(void*		)&msg_type,
			(OS_MSG_SIZE)1,
			(OS_OPT		)OS_OPT_POST_FIFO,
			(OS_ERR*	)&err);
	}

	 EXTI_ClearITPendingBit(EXTI_Line11);//���LINE2�ϵ��жϱ�־λ 
}

int init_wifi_module_STA(void)
{
	int i=0;
	int retval=0;

	wifi_SPI1_Init();
	Reset_config();
//	printf("This is a Demo to printf\n");
	rak_init_struct(&rak_strapi);
	strcpy((char *)rak_strapi.uPskFrame.psk, RAK_STA_PSK);
	strcpy((char *)rak_strapi.uConnFrame.ssid, RAK_STA_SSID);
	rak_strapi.uConnFrame.mode=	NET_STATION;

	while(!(GPIO_ReadInputDataBit(SPIx_INT_GPIO_PORT,SPIx_INT_Pin)));
	
	rak_sys_init(&uCmdRspFrame);
	for(i = 0; i < 18; i++)
	{
        printf("%c",uCmdRspFrame.initResponse.strdata[i]);	
	} 
	     
  delay_ms(200);
	retval=  rak_get_version();
	if(retval!=RUN_OK)
  	{
    	return retval;
  	}
  	else
  	{               
	    RAK_RESPONSE_TIMEOUT(RAK_GETVERSION_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  printf(" version=%s\r\n",uCmdRspFrame.versionFrame.host_fw);
		  rak_clearPktIrq();
  	}
		
//		delay_ms(500);
		rak_scan_t ScanFrame;
		do {
			ScanFrame.channel=0;
			ScanFrame.ssid[0]=0;
			retval=  rak_uscan(&ScanFrame);
			if(retval!=RUN_OK)
			{
				return retval;
			}
			else
			{                    
				RAK_RESPONSE_TIMEOUT(RAK_SCAN_IMEOUT);
				rak_read_packet(&uCmdRspFrame);
//				printf("scan retval =%d\r\n",uCmdRspFrame.scanResponse.status);
				rak_clearPktIrq();
				if (uCmdRspFrame.scanResponse.status==0)
				{ 
					printf("Scan num =%d\r\n",uCmdRspFrame.scanResponse.ap_num);
					break;
				}else {
					delay_ms(100);
				}
  	  } 
	 }while(1);
		
	 rak_getscan_t GetScanFrame;
	 GetScanFrame.scan_num=uCmdRspFrame.scanResponse.ap_num;
	 retval=  rak_getscan(&GetScanFrame);
	 if(retval!=RUN_OK)
		{
			return retval;
		}else{                    
			RAK_RESPONSE_TIMEOUT(RAK_GETSCAN_IMEOUT);
			rak_read_packet(&uCmdRspFrame);
			for (int i=0;i<GetScanFrame.scan_num;i++)
			{
			printf("%s  ch=%d   rssi=-%d   security=0x%X\r\n",uCmdRspFrame.getscanResponse.strScanInfo[i].ssid,
			                                           uCmdRspFrame.getscanResponse.strScanInfo[i].rfChannel,
			                                           255-uCmdRspFrame.getscanResponse.strScanInfo[i].rssiVal,
			                                           uCmdRspFrame.getscanResponse.strScanInfo[i].securityMode);
			}
			rak_clearPktIrq();	
  	} 

	retval=rak_set_psk(&rak_strapi.uPskFrame);
	if(retval!=RUN_OK)
  	{
		  printf ("psk error!\r\n");
    	return retval;
  	}
  	else
  	{
                       
	    RAK_RESPONSE_TIMEOUT(RAK_SETPSK_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  printf("PSK=%s,0X%X\r\n",rak_strapi.uPskFrame.psk,uCmdRspFrame.rspCode[0]) ;
		  rak_clearPktIrq();
  	}  
	printf ("ssid=%s\r\n",rak_strapi.uConnFrame.ssid);
	retval= rak_connect(&rak_strapi.uConnFrame);
	if(retval!=RUN_OK)
  	{	
			printf ("connect error !\r\n");
    	return retval;
  	}
  	else
  	{
        
	    RAK_RESPONSE_TIMEOUT(RAK_CONNECT_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  rak_clearPktIrq();
  	}  

//	 retval = rak_set_ipstatic(&rak_strapi.uIpstaticFrame);
//	 if (retval!=RUN_OK)
//	 {
//	 	  return retval;
//	 }
//	 else 
//	 {
//	    RAK_RESPONSE_TIMEOUT(RAK_CONNECT_TIMEOUT);
//	    rak_read_packet(&uCmdRspFrame);
//		  if (uCmdRspFrame.mgmtResponse.status==0)
//		  {		
//			    printf ("Set static ip ok!\r\n");
//			}
//		  rak_clearPktIrq();
//	 }
		
 do
 {
	retval = rak_ipconfig_dhcp(RAK_DHCP_CLIENT_ENABLE) ;
	if (retval!=RUN_OK)
	{
		  printf ("DHCP error!\r\n");
	    return retval;
		
	}
	else 
	{
	    RAK_RESPONSE_TIMEOUT(RAK_CONNECT_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		if (uCmdRspFrame.ipparamFrameRcv.status==0){	
			printf ("IP=");
			for (i=3;i>=0;i--)
			{
			printf ("%d",uCmdRspFrame.ipparamFrameRcv.ipaddr[i]);
			if (i<=3&&i>0)
			   {  printf (".");  }
			}
			  printf ("\r\n")	;
        printf ("DHCP OK!\r\n");
			rak_clearPktIrq();
			break;
		}else{
			delay_ms(100);
		}
	}
 }while(1);
 	EXTIX_Init();
	return 0;
}


int init_wifi_module_AP(void)
{
	int i=0;
	int retval=0;

	wifi_SPI1_Init();
	Reset_config();
//	printf("This is a Demo to printf\n");
	rak_init_struct(&rak_strapi);
	strcpy((char *)rak_strapi.uPskFrame.psk, RAK_AP_PSK);
	strcpy((char *)rak_strapi.uConnFrame.ssid, RAK_AP_SSID);
	rak_strapi.uConnFrame.mode=	NET_AP;

	while((GPIO_ReadOutputDataBit(SPIx_INT_GPIO_PORT,SPIx_INT_Pin)));
	rak_sys_init(&uCmdRspFrame);
	for(i = 0; i < 18; i++)
	{
        printf("%c",uCmdRspFrame.initResponse.strdata[i]);	
	} 
	     
  delay_ms(200);
	retval=  rak_get_version();
	if(retval!=RUN_OK)
  	{
    	return retval;
  	}
  	else
  	{               
	    RAK_RESPONSE_TIMEOUT(RAK_GETVERSION_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  printf(" version=%s\r\n",uCmdRspFrame.versionFrame.host_fw);
		  rak_clearPktIrq();
  	}
		
//		delay_ms(500);
		rak_scan_t ScanFrame;
		do {
			ScanFrame.channel=0;
			ScanFrame.ssid[0]=0;
			retval=  rak_uscan(&ScanFrame);
			if(retval!=RUN_OK)
			{
				return retval;
			}
			else
			{                    
				RAK_RESPONSE_TIMEOUT(RAK_SCAN_IMEOUT);
				rak_read_packet(&uCmdRspFrame);
//				printf("scan retval =%d\r\n",uCmdRspFrame.scanResponse.status);
				rak_clearPktIrq();
				if (uCmdRspFrame.scanResponse.status==0)
				{ 
					printf("Scan num =%d\r\n",uCmdRspFrame.scanResponse.ap_num);
					break;
				}else {
					delay_ms(100);
				}
  	  } 
	 }while(1);
		
	 rak_getscan_t GetScanFrame;
	 GetScanFrame.scan_num=uCmdRspFrame.scanResponse.ap_num;
	 retval=  rak_getscan(&GetScanFrame);
	 if(retval!=RUN_OK)
		{
			return retval;
		}else{                    
			RAK_RESPONSE_TIMEOUT(RAK_GETSCAN_IMEOUT);
			rak_read_packet(&uCmdRspFrame);
			for (int i=0;i<GetScanFrame.scan_num;i++)
			{
			printf("%s  ch=%d   rssi=-%d   security=0x%X\r\n",uCmdRspFrame.getscanResponse.strScanInfo[i].ssid,
			                                           uCmdRspFrame.getscanResponse.strScanInfo[i].rfChannel,
			                                           255-uCmdRspFrame.getscanResponse.strScanInfo[i].rssiVal,
			                                           uCmdRspFrame.getscanResponse.strScanInfo[i].securityMode);
			}
			rak_clearPktIrq();	
  	} 

	retval=rak_set_psk(&rak_strapi.uPskFrame);
	if(retval!=RUN_OK)
  	{
		  printf ("psk error!\r\n");
    	return retval;
  	}
  	else
  	{
                       
	    RAK_RESPONSE_TIMEOUT(RAK_SETPSK_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  printf("PSK=%s,0X%X\r\n",rak_strapi.uPskFrame.psk,uCmdRspFrame.rspCode[0]) ;
		  rak_clearPktIrq();
  	}  
	printf ("ssid=%s\r\n",rak_strapi.uConnFrame.ssid);
	retval= rak_connect(&rak_strapi.uConnFrame);
	if(retval!=RUN_OK)
  	{	
			printf ("connect error !\r\n");
    	return retval;
  	}
  	else
  	{
        
	    RAK_RESPONSE_TIMEOUT(RAK_CONNECT_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  rak_clearPktIrq();
  	}  

	 retval = rak_set_ipstatic(&rak_strapi.uIpstaticFrame);
	 if (retval!=RUN_OK)
	 {
	 	  return retval;
	 }
	 else 
	 {
	    RAK_RESPONSE_TIMEOUT(RAK_CONNECT_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		  if (uCmdRspFrame.mgmtResponse.status==0)
		  {		
			    printf ("Set static ip ok!\r\n");
			}
		  rak_clearPktIrq();
	 }
		
 do
 {
	retval = rak_ipconfig_dhcp(RAK_DHCP_SERVER_ENABLE) ;
	if (retval!=RUN_OK)
	{
		  printf ("DHCP error!\r\n");
	    return retval;
		
	}
	else 
	{
	    RAK_RESPONSE_TIMEOUT(RAK_CONNECT_TIMEOUT);
	    rak_read_packet(&uCmdRspFrame);
		if (uCmdRspFrame.ipparamFrameRcv.status==0){	
			printf ("IP=");
			for (i=3;i>=0;i--)
			{
			printf ("%d",uCmdRspFrame.ipparamFrameRcv.ipaddr[i]);
			if (i<=3&&i>0)
			   {  printf (".");  }
			}
			  printf ("\r\n")	;
        printf ("DHCP OK!\r\n");
			rak_clearPktIrq();
			break;
		}else{
			delay_ms(100);
		}
	}
 }while(1);
 return 0;
}
