/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.  */
/****************************************************************************** 
* File Name    : wrp_user_uart.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : UART Wrapper Layer Source File
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"       /* Macro Driver Definitions */
#include "r_cg_sau.h"               /* UART Driver Serial definitions */

/* Wrapper/User */
#include "wrp_user_uart.h"          /* UART Wrapper header */

/* Application */
#include "cmd.h"                    /* CMD Module (of Debug) */
/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Macro Definitions Checking
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint8_t g_received_byte;

/******************************************************************************
* Function Name    : void WRP_UART_Init(void)
* Description      : WRAPPER UART Initialization
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Init()
* Return Value     : None
******************************************************************************/
void WRP_UART_Init(void)
{
    WRP_UART_DRIVER_Init();
}

/******************************************************************************
* Function Name    : void WRP_UART_Start(void)
* Description      : WRAPPER UART Start
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Start()
* Return Value     : None
******************************************************************************/
void WRP_UART_Start(void)
{
    WRP_UART_DRIVER_Start();
    
    /* Start receive data byte */
    WRP_UART_ReceiveData(&g_received_byte, 1);
}

/******************************************************************************
* Function Name    : void WRP_UART_Stop(void)
* Description      : WRAPPER UART Stop
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Stop()
* Return Value     : None
******************************************************************************/
void WRP_UART_Stop(void)
{
    WRP_UART_DRIVER_Stop();
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART_SendData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Send Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART_DRIVER_SendData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART_SendData(uint8_t *data, uint16_t length)
{
    if (WRP_UART_DRIVER_SendData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART_ReceiveData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Receive Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART_DRIVER_ReceiveData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART_ReceiveData(uint8_t *data, uint16_t length)
{
    if (WRP_UART_DRIVER_ReceiveData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

/******************************************************************************
* Function Name    : void WRP_UART_SendEndCallback(void)
* Description      : WRAPPER UART Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART_SendEndCallback(void)
{
    /* Put application callback here */
#ifdef __DEBUG
    CMD_SendEndCallback();
#endif
}

/******************************************************************************
* Function Name    : void WRP_UART_ReceiveEndCallback(void)
* Description      : WRAPPER UART Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART_ReceiveEndCallback(void)
{
    /* Put application callback here */
#ifdef __DEBUG
    CMD_ReceiveEndCallback(g_received_byte);
#endif

    WRP_UART_ReceiveData(&g_received_byte, 1);
}
