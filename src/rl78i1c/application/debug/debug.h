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
* File Name    : debug.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Debug Header file
***********************************************************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#ifdef __DEBUG
#include "cmd.h"            /* Command Driver */
#include "command.h"        /* Command Interface */
#include <string.h>         /* Standard String */
#include <stdarg.h>         /* Variant Args */
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifdef __DEBUG
    #define DEBUG_Init                  COMMAND_Init
    #define DEBUG_Start                 CMD_Start
    #define DEBUG_Stop                  CMD_Stop
    #define DEBUG_Printf                CMD_Printf
    #define DEBUG_PollingProcessing     COMMAND_PollingProcessing
    #define DEBUG_AckNewLine            CMD_AckCmdReceived
#else
    //#define __DEBUG_HEAD              /
    #define DEBUG_Init()                {;}//__DEBUG_HEAD/
    #define DEBUG_Start()               {;}//__DEBUG_HEAD/
    #define DEBUG_Stop()                {;}//__DEBUG_HEAD/
    #define DEBUG_Printf                {;}//__DEBUG_HEAD/
    #define DEBUG_PollingProcessing()   {;}//__DEBUG_HEAD/
    #define DEBUG_AckNewLine()          {;}//__DEBUG_HEAD/
#endif

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
*****************************************************************************/


#endif /* _DEBUG_H */

