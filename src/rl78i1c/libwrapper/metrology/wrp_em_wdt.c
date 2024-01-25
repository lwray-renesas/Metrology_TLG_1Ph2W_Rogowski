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
* File Name    : wrp_em_wdt.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper WDT APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "wrp_em_sw_config.h"   /* Wrapper Shared Configuration */
#include "wrp_em_wdt.h"             /* WDT Wrapper Layer */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
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
* Function Name    : void EM_WDT_Init(void)
* Description      : WDT Initialization
* Arguments        : None
* Functions Called : EM_WDT_DriverInit()
* Return Value     : None
******************************************************************************/
void EM_WDT_Init(void)
{
    EM_WDT_DriverInit();
}

/******************************************************************************
* Function Name    : void EM_WDT_Start(void)
* Description      : WDT Start
* Arguments        : None
* Functions Called : EM_WDT_DriverStart()
* Return Value     : None
******************************************************************************/
void EM_WDT_Start(void)
{
    EM_WDT_DriverStart();
}

/******************************************************************************
* Function Name    : void EM_WDT_Stop(void)
* Description      : WDT Stop
* Arguments        : None
* Functions Called : EM_WDT_DriverStop()
* Return Value     : None
******************************************************************************/
void EM_WDT_Stop(void)
{
    EM_WDT_DriverStop();
}

/******************************************************************************
* Function Name    : void EM_WDT_Restart(void)
* Description      : WDT Restart
* Arguments        : None
* Functions Called : EM_WDT_DriverRestart()
* Return Value     : None
******************************************************************************/
void EM_WDT_Restart(void)
{
    EM_WDT_DriverRestart();
}
