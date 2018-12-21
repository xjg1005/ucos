#ifndef __WIFI_MODULE_H
#define __WIFI_MODULE_H
/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32f4xx_conf.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */



/* Includes ------------------------------------------------------------------*/
//#include "stm32f411e_discovery.h"
#define MASTER_BOARD 
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor SPIx instance used and associated 
   resources */
/* Definition for SPIx clock resources */
#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE)//Ê¹ÄÜUSART1Ê±ÖÓ
#define SPIx_SCK_GPIO_CLK_ENABLE()       RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE)
#define SPIx_MISO_GPIO_CLK_ENABLE()      RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE) 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE) 

#define SPIx_FORCE_RESET()               RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE)
#define SPIx_RELEASE_RESET()             RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISENABLE)

/* Definition for SPIx Pins */
#define SPIx_SCK_Pin                     GPIO_Pin_5
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_SCK_AF                      GPIO_AF5_SPI1
#define SPIx_MISO_Pin                    GPIO_Pin_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MISO_AF                     GPIO_AF5_SPI1
#define SPIx_MOSI_Pin                    GPIO_Pin_7
#define SPIx_MOSI_GPIO_PORT              GPIOA
#define SPIx_MOSI_AF                     GPIO_AF5_SPI1

#define SPIx_CS_Pin                      GPIO_Pin_4 
#define SPIx_CS_GPIO_PORT                GPIOA
#define SPIx_INT_Pin                     GPIO_Pin_2
#define SPIx_INT_GPIO_PORT               GPIOA

#define Reset_Pin                        GPIO_Pin_3
#define Reset_GPIO_PORT                  GPIOA
/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
u8 SPI1_ReadWriteByte(u8 TxData);
int init_wifi_module_STA(void);
int  init_wifi_module_AP(void);
#endif
