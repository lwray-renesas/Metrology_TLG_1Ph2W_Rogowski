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
* File Name    : wrp_em_pulse.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper EM PULSE APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "wrp_em_sw_config.h"   /* Wrapper Shared Configuration */
#include "wrp_em_pulse.h"       /* EM PULSE Wrapper Layer */

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

/******************************************************************************
* Function Name    : void EM_PULSE_Init(void)
* Description      : EM Pulse Initialization
* Arguments        : None
* Functions Called : EM_PULSE_DriverInit()
* Return Value     : None
******************************************************************************/
void EM_PULSE_Init(void)
{
    EM_PULSE_DriverInit();
}

/******************************************************************************
* Function Name    : void EM_PULSE_ACTIVE_On(void)
* Description      : EM Pulse 0 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE_ACTIVE_ON_STATEMENT
* Return Value     : None
******************************************************************************/
void EM_PULSE_ACTIVE_On(void)
{
    EM_PULSE_ACTIVE_ON_STATEMENT;
}

/******************************************************************************
* Function Name    : void EM_PULSE_ACTIVE_Off(void)
* Description      : EM Pulse 0 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE_ACTIVE_OFF_STATEMENT
* Return Value     : None
******************************************************************************/
void EM_PULSE_ACTIVE_Off(void)
{
    EM_PULSE_ACTIVE_OFF_STATEMENT;
}

/******************************************************************************
* Function Name    : void EM_PULSE_REACTIVE_On(void)
* Description      : EM Pulse 1 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE_REACTIVE_ON_STATEMENT
* Return Value     : None
******************************************************************************/
void EM_PULSE_REACTIVE_On(void)
{
    EM_PULSE_REACTIVE_ON_STATEMENT;
}

/******************************************************************************
* Function Name    : void EM_PULSE_REACTIVE_Off(void)
* Description      : EM Pulse 1 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE_REACTIVE_OFF_STATEMENT
* Return Value     : None
******************************************************************************/
void EM_PULSE_REACTIVE_Off(void)
{
    EM_PULSE_REACTIVE_OFF_STATEMENT;
}

/******************************************************************************
* Function Name    : void EM_PULSE_APPARENT_On(void)
* Description      : EM Pulse 2 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE_APPARENT_ON_STATEMENT
* Return Value     : None
******************************************************************************/
void EM_PULSE_APPARENT_On(void)
{
    EM_PULSE_APPARENT_ON_STATEMENT;
}

/******************************************************************************
* Function Name    : void EM_PULSE_APPARENT_Off(void)
* Description      : EM Pulse 2 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE_APPARENT_OFF_STATEMENT
* Return Value     : None
******************************************************************************/
void EM_PULSE_APPARENT_Off(void)
{
    EM_PULSE_APPARENT_OFF_STATEMENT;
}