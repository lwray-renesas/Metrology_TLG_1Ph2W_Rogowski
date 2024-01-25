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
 * File Name    : storage_em.c
 * Version      : 1.00
 * Device(s)    : RL78/I1C
 * Tool-Chain   : CCRL
 * H/W Platform : RL78/I1C Energy Meter Platform
 * Description  : Storage Source File
 ***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"       /* CG Macro Driver */

/* Library */
#include <string.h>                 /* Compiler standard library */

/* MW */
#include "eeprom.h"                 /* EEPROM MW (Init/Read/Write) */

/* Metrology */
#include "em_core.h"

/* WRP */
#include "wrp_em_adc.h"
#include "wrp_em_sw_config.h"

/* Application */
#include "format_em.h"                 /* EEPROM Format Header */
#include "storage_em.h"

/***********************************************************************************************************************
Typedef definitions
 ***********************************************************************************************************************/
/* Storage Module State */

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
static const uint32_t signature = 0xA5A5A5A5UL;

/***********************************************************************************************************************
 * Function Name    : uint8_t STORAGE_EM_Format(void)
 * Description      : Format: clear all energy counter value in memory
 *                  : Use with caution, all energy data may corrupt
 *                  : Please note this API only clear in storage memory, not in RAM counter
 * Arguments        : None
 * Functions Called : None
 * Return Value     : uint8_t
 ***********************************************************************************************************************/
uint8_t STORAGE_EM_Format(void)
{
	EM_EEPROM_ENERGY_LOG energy_log;

	/* Empty energy log*/
	memset(&energy_log, 0, sizeof(EM_EEPROM_ENERGY_LOG));

	return STORAGE_EM_SetEnergyData(&energy_log);
}

/***********************************************************************************************************************
 * Function Name    : uint8_t STORAGE_EM_Restore(void)
 * Description      : Restore metrology counter from eeprom to RAM
 * Arguments        : None
 * Functions Called : None
 * Return Value     : uint8_t
 ***********************************************************************************************************************/
uint8_t STORAGE_EM_Restore(void)
{
	uint8_t status;
	static EM_EEPROM_ENERGY_LOG restore_energy_log;

	/* Get from storage */
	status = STORAGE_EM_GetEnergyData(&restore_energy_log);
	if (EM_STORAGE_OK == status)
	{
		/* Set to metrology */
		status = EM_SetOperationData(&(restore_energy_log.energy_data));
	}

	return status;
}

/***********************************************************************************************************************
 * Function Name    : uint8_t STORAGE_EM_Backup(void)
 * Description      : Backup metrology counter from RAM to EEPROM
 * Arguments        : None
 * Functions Called : None
 * Return Value     : uint8_t
 ***********************************************************************************************************************/
uint8_t STORAGE_EM_Backup(void)
{
	uint8_t status;
	static EM_EEPROM_ENERGY_LOG backup_energy_log;

	/* Get timestamp*/
	R_RTC_Get_CalendarCounterValue(&(backup_energy_log.time_stamp));

	/* Get from metrology */
	status = EM_GetOperationData(&(backup_energy_log.energy_data));

	if (EM_OK == status)
	{
		/* Set to storage */
		status = STORAGE_EM_SetEnergyData(&backup_energy_log);
	}

	return status;
}
/***********************************************************************************************************************
 * Function Name    : uint8_t STORAGE_EM_GetEnergyData(EM_ENERGY_DATA * p_counter)
 * Description      : Read from storage eeprom to counter
 * Arguments        : EM_ENERGY_DATA * p_counter
 * Functions Called :
 * Return Value     : Execution Status
 *                  :    STORAGE_OK      Backup successfull
 *                  :    STORAGE_ERROR   EPR operation failed
 ***********************************************************************************************************************/
uint8_t STORAGE_EM_GetEnergyData(EM_EEPROM_ENERGY_LOG * p_log)
{
	uint8_t status;
	uint16_t sum_index = 0U;
	uint64_t local_sum = 0ULL;
	EM_EEPROM_HEADER local_header;

	/* Load header from eeprom */
	status = EPR_Read(
			STORAGE_EEPROM_EM_HEADER_ADDR,
			(uint8_t *)&local_header,
			STORAGE_EEPROM_EM_HEADER_SIZE
	);

	if (status != EPR_OK)
	{
		return EM_STORAGE_ERROR;
	}

	/* Load log from eeprom */
	status = EPR_Read(
			STORAGE_EEPROM_EM_ENERGY_DATA_ADDR,
			(uint8_t *)p_log,
			STORAGE_EEPROM_EM_ENERGY_DATA_SIZE
	);

	if (status != EPR_OK)
	{
		return EM_STORAGE_ERROR;
	}

	/* Compute simple checksum*/
	local_sum = local_header.signature;
	for(sum_index = 0U; sum_index < sizeof(EM_EEPROM_ENERGY_LOG); ++sum_index)
	{
		local_sum += *(uint8_t *)p_log;
	}

	return ((local_sum == local_header.sum) && (signature == local_header.signature)) ? EM_STORAGE_OK : EM_STORAGE_HEADER_INVALID;
}

/***********************************************************************************************************************
 * Function Name    : uint8_t STORAGE_EM_SetEnergyData(EM_ENERGY_DATA * p_counter)
 * Description      : Set to metrology counter in storage eeprom
 * Arguments        : None
 * Functions Called :
 * Return Value     : Execution Status
 ***********************************************************************************************************************/
uint8_t STORAGE_EM_SetEnergyData(EM_EEPROM_ENERGY_LOG * p_log)
{
	uint8_t status;
	uint16_t sum_index = 0U;
	uint64_t local_sum = 0ULL;
	EM_EEPROM_HEADER local_header;

	/* Compute simple checksum*/
	local_header.sum = signature;
	local_header.signature = signature;
	for(sum_index = 0U; sum_index < sizeof(EM_EEPROM_ENERGY_LOG); ++sum_index)
	{
		local_header.sum += *(uint8_t *)p_log;
	}

	/* Write log to eeprom */
	status = EPR_Write(
			STORAGE_EEPROM_EM_ENERGY_DATA_ADDR,
			(uint8_t *)p_log,
			STORAGE_EEPROM_EM_ENERGY_DATA_SIZE
	);

	if (status == EPR_OK)
	{
		/* Write header to eeprom */
		status = EPR_Write(
				STORAGE_EEPROM_EM_HEADER_ADDR,
				(uint8_t *)&local_header,
				STORAGE_EEPROM_EM_HEADER_SIZE
		);
	}

	return status;
}
