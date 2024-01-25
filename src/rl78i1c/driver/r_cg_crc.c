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
* Copyright (C) 2015, 2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_crc.c
* Version      : Code Generator for RL78/I1C V1.01.00.02 [15 May 2017]
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for CRC module.
* Creation Date: 3/22/2019
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_crc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_CRC_Clear
* Description  : This function clear the CRC result register.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CRC_Clear(void)
{
    CRCD = 0x0000;
}

/***********************************************************************************************************************
* Function Name: R_CRC_Clear
* Description  : This function set the CRC result register.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CRC_Set(uint16_t value)
{
    CRCD = value;
}

/***********************************************************************************************************************
* Function Name: R_CRC_GetResult
* Description  : This function get the CRC result register.
* Arguments    : None
* Return Value : CRC-16-CCITT result (2 bytes)
***********************************************************************************************************************/
uint16_t R_CRC_GetResult(void)
{
    return (CRCD);
}

/***********************************************************************************************************************
* Function Name: R_CRC_Calculate
* Description  : This function calculate the CRC value for a specified buffer
* Arguments    : buffer -
*              :     memory buffer to calculate CRC-16-CCITT
*              : length -
*              :     memory buffer length
* Return Value : None
***********************************************************************************************************************/
void R_CRC_Calculate(uint8_t *buffer, uint16_t length)
{
    while (length > 0)
    {
        CRCIN = *buffer;
        
        buffer++;
        length--;
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
