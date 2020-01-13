#ifndef __BOARD_H__
#define __BOARD_H__

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
/* STM32 �̼���ͷ�ļ� */
#include "stm32h7xx.h"
#include "main.h"
/* ������Ӳ��bspͷ�ļ� */
#include "./led/bsp_led.h"
#include "./exti/bsp_exti.h"

#include "./delay/core_delay.h"   
#include "./usart/bsp_debug_usart.h"
/*
*************************************************************************
*                               ��������
*************************************************************************
*/
void rt_hw_board_init(void);
void SysTick_Handler(void);
	

#endif /* __BOARD_H__ */
