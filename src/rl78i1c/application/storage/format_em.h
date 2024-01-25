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
* File Name    : metrology_format.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Storage Format Header file
***********************************************************************************************************************/

#ifndef _STORAGE_EEPROM_METROLOGY_FORMAT_H
#define _STORAGE_EEPROM_METROLOGY_FORMAT_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */
#include "r_cg_rtc.h"

/* Middleware */
#include "em_type.h"        /* EM Core Type Definitions */

/* Application */
#include "eeprom.h"         /* EEPROM MW */

/***********************************************************************************************************************
Macro definitions for Typedef
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
	uint64_t sum;
	uint32_t signature;
}EM_EEPROM_HEADER;

typedef struct
{
	EM_OPERATION_DATA energy_data;
	rtc_counter_value_t time_stamp;
}EM_EEPROM_ENERGY_LOG;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* EEPROM Information */
#define STORAGE_EEPROM_START_ADDR                       EPR_DEVICE_START_ADDR   /* EEPROM Start address */
#define STORAGE_EEPROM_SIZE                             EPR_DEVICE_SIZE         /* EEPROM Size */
#define STORAGE_EEPROM_PAGESIZE                         EPR_DEVICE_PAGESIZE     /* EEPROM Page Size */

/* Header Code */
#define STORAGE_EEPROM_EM_HEADER_ADDR                      (STORAGE_EEPROM_START_ADDR)
#define STORAGE_EEPROM_EM_HEADER_SIZE                      (sizeof(EM_EEPROM_HEADER))

/* Data Code */
#define STORAGE_EEPROM_EM_ENERGY_DATA_ADDR                      (STORAGE_EEPROM_START_ADDR+STORAGE_EEPROM_EM_HEADER_SIZE)
#define STORAGE_EEPROM_EM_ENERGY_DATA_SIZE                      (sizeof(EM_EEPROM_ENERGY_LOG))

#define STORAGE_EEPROM_EM_LAST_ADDR                             (STORAGE_EEPROM_EM_ENERGY_DATA_ADDR + STORAGE_EEPROM_EM_ENERGY_DATA_SIZE - 1)
STATIC_ASSERT((STORAGE_EEPROM_EM_LAST_ADDR) < 0x0100);

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

#endif /* _STORAGE_EEPROM_FORMAT_H */

