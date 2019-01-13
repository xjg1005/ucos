
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

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPIx, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPIx); //返回通过SPIx最近接收的数据	
 		    
}

void wifi_SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPIx时钟
	
 	GPIO_InitStructure.GPIO_Pin=SPIx_CS_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(SPIx_CS_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(SPIx_CS_GPIO_PORT,SPIx_CS_Pin); //GPIOA 

	GPIO_InitStructure.GPIO_Pin=Reset_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(Reset_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_SetBits(Reset_GPIO_PORT,Reset_Pin); //GPIOA 

  //GPIOFB3,4,5初始化设置
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_Pin|SPIx_MISO_Pin|SPIx_MOSI_Pin;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,GPIO_PinSource10,GPIO_AF_SPI3); //PA5复用为 SPIx
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,GPIO_PinSource11,GPIO_AF_SPI3); //PA6复用为 SPIx
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT,GPIO_PinSource12,GPIO_AF_SPI3); //PA7复用为 SPIx
 
	//这里只针对SPI口初始化
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);//复位SPIx
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//停止复位SPIx

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPIx, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPIx, ENABLE); //使能SPI外设

	SPI1_ReadWriteByte(0xff);//启动传输		 
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
	
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11);//PA11 连接到中断线11
	
  /* 配置EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;//LINE2
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE2
  EXTI_Init(&EXTI_InitStructure);//配置
	
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	   
}
//外部中断2服务程序
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

	 EXTI_ClearITPendingBit(EXTI_Line11);//清除LINE2上的中断标志位 
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
