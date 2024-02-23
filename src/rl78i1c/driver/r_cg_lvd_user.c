/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
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
* Copyright (C) 2015, 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_lvd_user.c
* Version      : Applilet4 for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NLG
* Tool-Chain   : CCRL
* Description  : This file implements device driver for LVD module.
* Creation Date: 23/02/2024
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_lvd.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_app_mcu.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_lvd_vddinterrupt(vect=INTLVDVDD)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


/***********************************************************************************************************************
* Function Name: r_lvd_vddinterrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_lvd_vddinterrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}




/* Start user code for adding. Do not edit comment generated here */

vdd_range_t R_LVD_Check(void)
{
	vdd_range_t l_vdd_range = vdd_less_than_2_46;

	LVDVDMK = 1U;
	LVDVDDEN = 0U;

	/* Check less than 2.46V*/
	LVDVDD = (LVDVDD & 0xC0U) | 0x0U;
	LVDVDDEN = 1U;
    MCU_Delay(300);
    LVDVDIF = 0U;

    if(1U == LVDVDDF)
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_less_than_2_46;
    }

	/* Check less than 2.67V*/
	LVDVDD = (LVDVDD & 0xC0U) | 0x1U;
	LVDVDDEN = 1U;
    MCU_Delay(300);
    LVDVDIF = 0U;

    if(1U == LVDVDDF)
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_2_46_to_2_67;
    }

	/* Check less than 2.87V*/
	LVDVDD = (LVDVDD & 0xC0U) | 0x2U;
	LVDVDDEN = 1U;
    MCU_Delay(300);
    LVDVDIF = 0U;

    if(1U == LVDVDDF)
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_2_67_to_2_87;
    }

	/* Check less than 3.08V*/
	LVDVDD = (LVDVDD & 0xC0U) | 0x3U;
	LVDVDDEN = 1U;
    MCU_Delay(300);
    LVDVDIF = 0U;

    if(1U == LVDVDDF)
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_2_87_to_3_08;
    }

	/* Check less than 3.39V*/
	LVDVDD = (LVDVDD & 0xC0U) | 0x4U;
	LVDVDDEN = 1U;
    MCU_Delay(300);
    LVDVDIF = 0U;

    if(1U == LVDVDDF)
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_3_08_to_3_39;
    }

	/* Check less than 3.7V or more than 3.77*/
	LVDVDD = (LVDVDD & 0xC0U) | 0x5U;
	LVDVDDEN = 1U;
    MCU_Delay(300);
    LVDVDIF = 0U;

    if(1U == LVDVDDF)
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_3_39_to_3_70;
    }
    else
    {
    	LVDVDDEN = 0U;
        LVDVDIF = 0U;
    	return vdd_more_than_3_77;
    }
}
/* END OF FUNCTION*/

char const * R_LVD_range_to_str(const vdd_range_t range)
{
	static const char * lvd_str_arr[] = {
			"Vdd < 2.46V",
			"2.46V < Vdd < 2.67V",
			"2.67V < Vdd < 2.87V",
			"2.87V < Vdd < 3.08V",
			"3.08V < Vdd < 3.39V",
			"3.39V < Vdd < 3.70V",
			"3.77V < Vdd"
	};

	return lvd_str_arr[range];
}
/* END OF FUNCTION*/

/* End user code. Do not edit comment generated here */
