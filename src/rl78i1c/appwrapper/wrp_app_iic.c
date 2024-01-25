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
* File Name    : wrp_user_ext.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : IIC Wrapper Layer Source File
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* Macro Driver Definitions */
#include "r_cg_iica.h"

/* Standard library */
#include <stdlib.h>
/* Wrapper/User */
#include "wrp_app_iic.h"       /* Wrapper IIC header */

/* Application */
#include "eeprom.h"             /* EEPROM */

void WRP_IIC_Init(void)
{
    WRP_IIC_DRIVER_Init();
}

/******************************************************************************
* Function Name    : uint8_t WRP_IIC_SendStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
* Description      : IIC Send Start
* Arguments        : uint8_t device_addr: Device address
*                  : uint8_t *buffer: Buffer to send
*                  : uint16_t length: Buffer length
* Functions Called : WRP_IIC_DRIVER_SendStart()
* Return Value     : Execution Status
*                  :    WRP_IIC_OK      Normal end
*                  :    WRP_IIC_ERROR   Error occurred
******************************************************************************/
uint8_t WRP_IIC_SendStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
{
    if (WRP_IIC_DRIVER_SendStart(device_addr, buffer, length) != MD_OK)
    {
        return WRP_IIC_ERROR;
    }
    else
    {
        return WRP_IIC_OK;
    }
}

/******************************************************************************
* Function Name    : uint8_t WRP_IIC_ReceiveStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
* Description      : IIC Recieve Start
* Arguments        : uint8_t device_addr: Device address
*                  : uint8_t *buffer: Buffer to receive
*                  : uint16_t length: Buffer length
* Functions Called : WRP_IIC_DRIVER_ReceiveStart()
* Return Value     : Execution Status
*                  :    WRP_IIC_OK      Normal end
*                  :    WRP_IIC_ERROR   Error occurred
******************************************************************************/
uint8_t WRP_IIC_ReceiveStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
{
    if (WRP_IIC_DRIVER_ReceiveStart(device_addr, buffer, length) != MD_OK)
    {
        return WRP_IIC_ERROR;
    }
    else
    {
        return WRP_IIC_OK;
    }
}

/******************************************************************************
* Function Name    : void WRP_IIC_SendEndCallback(void)
* Description      : IIC Wrapper Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_IIC_SendEndCallback(void)
{
    /* Put application callback here */
    EPR_SendEndCallback();       /* EEPROM */
}

/******************************************************************************
* Function Name    : void WRP_IIC_ReceiveEndCallback(void)
* Description      : IIC Wraper Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_IIC_ReceiveEndCallback(void)
{
    /* Put application callback here */
    EPR_ReceiveEndCallback();    /* EEPROM */
}
