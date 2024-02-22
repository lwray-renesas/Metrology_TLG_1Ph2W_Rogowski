/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : load_test.c
* Version      : 
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : 
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
#include "load_test.h"
#include "platform.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#ifdef __DEBUG
#ifdef METER_ENABLE_MEASURE_CPU_LOAD
/***********************************************************************************************************************
* Function Name: LOADTEST_TAU0_Channel0_Start
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void LOADTEST_TAU_Init(void)
{
	/* Initialised at start up - assumes TAU supplied with operating click undivided -
	 * not output and operating in interval mode with interrupts masked.*/
	TDR00 = 0xFFFFU;
}

/***********************************************************************************************************************
* Function Name: LOADTEST_TAU0_Channel0_Stop
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void LOADTEST_TAU_DeInit(void)
{
	/* Not necessary - start and stop function will work OK*/
}


/******************************************************************************
* Function Name   : LOADTEST_CGC_GetClock
* Interface       : uint16_t LOADTEST_CGC_GetClock(void)
* Description     : Get system clock
* Arguments       : uint8_t * arg_str: Arguments string
* Function Calls  : None
* Return Value    : None
******************************************************************************/
uint16_t LOADTEST_CGC_GetClock(void)
{
    if (HOCODIV == 0x00)
    {
        return 24000;
    }
    else if (HOCODIV == 0x01)
    {
        return 12000;
    }
    else if (HOCODIV == 0x02)
    {
        return 6000;
    }
    else if (HOCODIV == 0x02)
    {
        return 3000;
    }
    else if (HOCODIV == 0x02)
    {
        return 1500;
    }
    else
    {
        return 0xFFFF;
    }
}

#endif
#endif
/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
