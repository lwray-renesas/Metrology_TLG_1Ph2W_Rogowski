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
* File Name    : wrp_em_mcu.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Specified MCU APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "wrp_em_sw_config.h"   /* Wrapper Shared Configuration */
#include "wrp_em_mcu.h"         /* MCU Wrapper Layer definitions */
#include "wrp_app_mcu.h" 
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: void EM_MCU_Delay(uint16_t us)
* Note         : THIS FUNCTION STRICTLY DEPEND ON THE MCU
*              : BELOW IMPLEMENTATION IS USED FOR RL78I1B ONLY
*              : WHEN CHANGE TO OTHER MCU, PLEASE RE-IMPLEMENT THIS FUNCTION
* Description  : MCU Delay
* Arguments    : uint16_t us: Expected delay time, in micro second, us must > 0
* Return Value : None
***********************************************************************************************************************/
void EM_MCU_Delay(uint16_t us)
{
	MCU_Delay(us);
}

/******************************************************************************
* Function Name: void EM_MCU_MultipleInterruptEnable(uint8_t enable)
* Description  : MCU Multiple Interrupt Enable.
* Arguments    : uint8_t enable: Enable/Disable selection
*              :      0      Disable
*              :      Not 0  Enable
* Return Value : None
******************************************************************************/
void EM_MCU_MultipleInterruptEnable(uint8_t enable)
{
	MCU_MultipleInterruptEnable(enable);
}
