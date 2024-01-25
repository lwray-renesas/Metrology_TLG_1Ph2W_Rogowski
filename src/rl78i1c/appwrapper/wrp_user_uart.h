/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : wrp_user_uart.h
* Version      : 1.00
* Description  : Wrapper User UART
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _WRAPPER_USER_UART_H
#define _WRAPPER_USER_UART_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define WRP_UART_OK     0   /* OK */
#define WRP_UART_ERROR  1   /* Error */

/* Interface mapping, for each driver,
 * we must re-map again about the UART_DRIVER interface */
 
/* Initialization */
#define     WRP_UART_DRIVER_Init()                      {;}

#ifdef __DEBUG

/* Control */
#define     WRP_UART_DRIVER_Start()                     R_UART1_Start()
#define     WRP_UART_DRIVER_Stop()                      R_UART1_Stop()

/* Send/Receive data */
#define     WRP_UART_DRIVER_SendData(data, length)      R_UART1_Send(data, length)
#define     WRP_UART_DRIVER_ReceiveData(data, length)   R_UART1_Receive(data, length)

#else

/* Control */
#define     WRP_UART_DRIVER_Start()                     {;}
#define     WRP_UART_DRIVER_Stop()                      {;}

/* Send/Receive data */
#define     WRP_UART_DRIVER_SendData(data, length)      {;}
#define     WRP_UART_DRIVER_ReceiveData(data, length)   {;}

#endif

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* UART Wrapper Controller */
void    WRP_UART_Init(void);
void    WRP_UART_Start(void);
void    WRP_UART_Stop(void);
uint8_t WRP_UART_SendData(uint8_t *data, uint16_t length);
uint8_t WRP_UART_ReceiveData(uint8_t *data, uint16_t length);

/*
 * Callback Registration
 * Callback, put 2 below interfaces to the serial user callback
 *      Send End Callback   : WRP_UART_SendEndCallback
 *      Receive End Callback: WRP_UART_ReceiveEndCallback
 */
void    WRP_UART_SendEndCallback(void);
void    WRP_UART_ReceiveEndCallback(void);

#endif /* _WRAPPER_USER_UART_H */
