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
* File Name    : r_cg_pclbuz.h
* Version      : Applilet4 for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NLG
* Tool-Chain   : CCRL
* Description  : This file implements device driver for PCLBUZ module.
* Creation Date: 23/10/2023
***********************************************************************************************************************/
#ifndef PCLBUZ_H
#define PCLBUZ_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Clock output select registers n (CKSn) 
*/
/* PCLBUZn output enable/disable specification (PCLOEn) */
#define _00_PCLBUZ_OUTPUT_DISABLE          (0x00U) /* output disable */
#define _80_PCLBUZ_OUTPUT_ENABLE           (0x80U) /* output enable */
/* PCLBUZn output clock selection (CSELn,CCSn2,CCSn1,CCSn0) */
#define _00_PCLBUZ_OUTCLK_fMAIN0           (0x00U) /* fMAIN */
#define _01_PCLBUZ_OUTCLK_fMAIN1           (0x01U) /* fMAIN/2 */
#define _02_PCLBUZ_OUTCLK_fMAIN2           (0x02U) /* fMAIN/2^2 */
#define _03_PCLBUZ_OUTCLK_fMAIN3           (0x03U) /* fMAIN/2^3 */
#define _04_PCLBUZ_OUTCLK_fMAIN4           (0x04U) /* fMAIN/2^4 */
#define _05_PCLBUZ_OUTCLK_fMAIN5           (0x05U) /* fMAIN/2^11 */
#define _06_PCLBUZ_OUTCLK_fMAIN6           (0x06U) /* fMAIN/2^12 */
#define _07_PCLBUZ_OUTCLK_fMAIN7           (0x07U) /* fMAIN/2^13 */
#define _08_PCLBUZ_OUTCLK_fSX0             (0x08U) /* fSX */
#define _09_PCLBUZ_OUTCLK_fSX1             (0x09U) /* fSX/2 */
#define _0A_PCLBUZ_OUTCLK_fSX2             (0x0AU) /* fSX/2^2 */
#define _0B_PCLBUZ_OUTCLK_fSX3             (0x0BU) /* fSX/2^3 */
#define _0C_PCLBUZ_OUTCLK_fSX4             (0x0CU) /* fSX/2^4 */
#define _0D_PCLBUZ_OUTCLK_fSX5             (0x0DU) /* fSX/2^5 */
#define _0E_PCLBUZ_OUTCLK_fSX6             (0x0EU) /* fSX/2^6 */
#define _0F_PCLBUZ_OUTCLK_fSX7             (0x0FU) /* fSX/2^6 */


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_PCLBUZ0_Create(void);
void R_PCLBUZ0_Start(void);
void R_PCLBUZ0_Stop(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
