/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ʹ�ô���1���ض���c��printf������usart�˿ڣ��жϽ���ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 H750 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"
#include "rtthread.h"

/* �ⲿ�����ź������ƿ� */
extern rt_sem_t test_sem;

UART_HandleTypeDef UartHandle;
DMA_HandleTypeDef DMA_Handle;

__attribute__ ((at(0x30000000))) uint8_t RX_BUFF[USART_RBUFF_SIZE]={0};
 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */  
void DEBUG_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
        
    DEBUG_USART_RX_GPIO_CLK_ENABLE();
    DEBUG_USART_TX_GPIO_CLK_ENABLE();
    
    /* ���ô���1ʱ��Դ*/
		RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
		RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* ʹ�ܴ���1ʱ�� */
    DEBUG_USART_CLK_ENABLE();

    /* ����Tx����Ϊ���ù���  */
    GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
    HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
    
    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
    GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
    HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
    
    /* ���ô�DEBUG_USART ģʽ */
    UartHandle.Instance = DEBUG_USART;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle);

    /*����1�жϳ�ʼ�� */
    HAL_NVIC_SetPriority(DEBUG_USART_IRQ, 4, 0);
    HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ);
		
}

void USART_DMA_Config(void)
{
  /*����DMAʱ��*/
  DEBUG_USART_DMA_CLK_ENABLE();

  DMA_Handle.Instance = DEBUG_USART_DMA_STREAM;
  /*usart1 tx��Ӧdma2��ͨ��4��������7*/	
  DMA_Handle.Init.Request = DMA_REQUEST_USART1_RX; 
  /*���򣺴��ڴ浽����*/		
  DMA_Handle.Init.Direction= DMA_PERIPH_TO_MEMORY;	
  /*�����ַ����*/	    
  DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE; 
  /*�ڴ��ַ����*/
  DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;	
  /*�������ݵ�λ*/	
  DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  /*�ڴ����ݵ�λ 8bit*/
  DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;	
  /*DMAģʽ������ѭ��*/
  DMA_Handle.Init.Mode = DMA_CIRCULAR;	 
  /*���ȼ�����*/	
  DMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;      
  /*����FIFO*/
  DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;        
  DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;    
  /*�洢��ͻ������ 1������*/
  DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;    
  /*����ͻ������ 1������*/
  DMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;    
  /*����DMA2��������7*/		   
//  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&DMA_Handle);
  /* Configure the DMA stream */
  HAL_DMA_Init(&DMA_Handle); 
  
	__HAL_DMA_ENABLE(&UartHandle);  
	
   /* Associate the DMA handle */
  __HAL_LINKDMA(&UartHandle, hdmarx, DMA_Handle);
	
	HAL_UART_Receive_DMA(&UartHandle, RX_BUFF, USART_RBUFF_SIZE);
  
	/*���ô��ڽ����жϣ��ڴ��ڳ�ʼ�����õĻ������һ���жϵ��´������ⲿʹ�ܼ��� */
	__HAL_UART_CLEAR_IT(&UartHandle, UART_CLEAR_IDLEF);
	__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_IDLE);  
  
}


void Uart_DMA_Rx_Data(void)
{
   // �ر�DMA ����ֹ����
   __HAL_DMA_DISABLE(&DMA_Handle);      
   // ��DMA��־λ
   __HAL_DMA_CLEAR_FLAG(&DMA_Handle,DMA_FLAG_TCIF3_7);     
	
   //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ   
	 WRITE_REG(((DMA_Stream_TypeDef   *)DMA_Handle.Instance)->NDTR , USART_RBUFF_SIZE);
   
	__HAL_DMA_ENABLE(&DMA_Handle);  

//   //������ֵ�ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
   rt_sem_release(test_sem);  
	
  /* 
    DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ�
    �ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����

    1. �����¿�������DMAͨ��֮ǰ����LumMod_Rx_Buf��������������ݸ��Ƶ�����һ�������У�
    Ȼ���ٿ���DMA��Ȼ�����ϴ����Ƴ��������ݡ�

    2. ����˫���壬��LumMod_Uart_DMA_Rx_Data�����У���������DMA_MemoryBaseAddr �Ļ�������ַ��
    ��ô�´ν��յ������ݾͻᱣ�浽�µĻ������У������ڱ����ǡ�
  */
}


/*****************  �����ַ��� **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit( &UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}
///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}


void UART_IdelCallback(void)
{
  Uart_DMA_Rx_Data();       /* �ͷ�һ���ź�������ʾ�����ѽ��� */
}

/*********************************************END OF FILE**********************/
