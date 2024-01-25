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
* File Name    : r_cg_port.c
* Version      : Applilet4 for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NLG
* Tool-Chain   : CCRL
* Description  : This file implements device driver for Port module.
* Creation Date: 18/10/2023
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
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
* Function Name: R_PORT_Create
* Description  : This function initializes the Port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_Create(void)
{
    PFSEG0 = _80_PFSEG07_SEG | _00_PFSEG06_PORT | _00_PFSEG05_PORT | _00_PFSEG04_PORT;
    PFSEG1 = _00_PFSEG11_PORT | _00_PFSEG10_PORT | _00_PFSEG09_PORT | _01_PFSEG08_SEG | _F0_PFSEG1_DEFAULT_VALUE;
    PFSEG2 = _00_PFSEG20_PORT | _00_PFSEG19_PORT | _00_PFSEG18_PORT | _00_PFSEG17_PORT | _00_PFSEG16_PORT | 
             _E0_PFSEG2_DEFAULT_VALUE;
    PFSEG3 = _00_PFSEG25_PORT | _00_PFSEG24_PORT | _FC_PFSEG3_DEFAULT_VALUE;
    ISCLCD = _02_VL3_BUFFER_VALID | _01_CAPLH_BUFFER_VALID;
    P0 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1;
    P1 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1 | _20_Pn5_OUTPUT_1 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | 
         _01_Pn0_OUTPUT_1;
    P3 = _02_Pn1_OUTPUT_1 | _01_Pn0_OUTPUT_1;
    P6 = _04_Pn2_OUTPUT_1;
    P7 = _10_Pn4_OUTPUT_1 | _08_Pn3_OUTPUT_1 | _04_Pn2_OUTPUT_1 | _02_Pn1_OUTPUT_1 | _01_Pn0_OUTPUT_1;
    P12 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1 | _20_Pn5_OUTPUT_1;
    POM0 = _00_POMn7_NCH_OFF | _00_POMn6_NCH_OFF;
    POM1 = _00_POMn7_NCH_OFF | _00_POMn6_NCH_OFF | _00_POMn5_NCH_OFF | _00_POMn2_NCH_OFF;
    POM3 = _00_POMn1_NCH_OFF;
    PM0 = _1F_PM0_DEFAULT_VALUE | _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _20_PMn5_MODE_INPUT;
    PM1 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn5_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | 
          _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;
    PM3 = _FC_PM3_DEFAULT_VALUE | _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;
    PM6 = _F8_PM6_DEFAULT_VALUE | _00_PMn2_MODE_OUTPUT;
    PM7 = _E0_PM7_DEFAULT_VALUE | _00_PMn4_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | 
          _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;
    PM12 = _1F_PM12_DEFAULT_VALUE | _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn5_MODE_OUTPUT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
