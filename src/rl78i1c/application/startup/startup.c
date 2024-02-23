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
 * File Name    : startup.c
 * Version      : 1.00
 * Device(s)    : RL78/I1C
 * Tool-Chain   :
 * H/W Platform : RL78/I1C Energy Meter Platform
 * Description  : Start-up source File
 ***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_userdefine.h"    /* CG User Define */
#include "r_cg_port.h"          /* Port Driver*/
#include "r_cg_sau.h"           /* Serial Driver */
#include "r_cg_rtc.h"           /* RTC Driver */
#include "r_cg_dsadc.h"         /* DSADC Driver */
#include "r_cg_tau.h"
#include "r_cg_lvd.h"

/* Wrapper/User */

/* MW/Core */
#include "em_core.h"            /* EM Core APIs */

/* Application */
#include "platform.h"           /* Default Platform Information Header */
#include "config_storage.h"
#include "startup.h"            /* Startup Header File */
#include "debug.h"
#include "stdio.h"
#include "storage_em.h"

/***********************************************************************************************************************
Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
 ***********************************************************************************************************************/
#define TRAP                0x80
#define WDTRF               0x10
#define RPERF               0x04
#define IAWRF               0x02
#define LVIRF               0x01

#define VERSION_LIB_TYPE_STR	"Library Type: "
#define VERSION_TGT_DEVS_STR	"Target Device(s): "
#define VERSION_GIT_HASH_STR	"Internal Git Hash: "
#define VERSION_LIB_COMP_STR	"Library Compiler Version: "
#define VERSION_BLD_DATE_STR	"Library Build Date: "

static const char app_build_date_str[] = "Build Date: "__DATE__;

/***********************************************************************************************************************
Imported global variables and functions (from other files)
 ***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/
uint8_t g_reset_flag;
st_em_startup_diag_t g_em_startup_diag;
extern volatile vdd_range_t vdd_range;

/***********************************************************************************************************************
Private global variables and functions
 ***********************************************************************************************************************/
#ifdef __DEBUG
static const uint8_t line_break[]   = "+------------------------------------------------------------------------+\n\r";
static const uint8_t param_intro[] = "| %-21s %26s %-21s |\n\r";
static const uint8_t param_text[] = "|     %-67s|\n\r";
static const uint8_t param_info_s[] = "|      . %-22s: %-40s|\n\r";
static const uint8_t param_info_i[] = "|      . %-22s: %-40d|\n\r";
static const uint8_t param_info_date[] = "|      . %-22s: %02x/%02x/20%02x %-29s|\n\r";


static const uint8_t param_itemgroup_start[] = "| %2d. %-54s             |\n\r";
static const uint8_t param_item_start[] = "| %2d. %-51s ";
static const uint8_t param_subitem_start[] = "|     . %-49s ";
static const uint8_t param_subitem_info_start[] = "|     . %-21s ";
static const uint8_t param_subitem_info_middle_rtc[] = "%02x/%02x/20%02x %02x:%02x:%02x %-7s ";
static const uint8_t param_item_end_normal[] = "\x1b[32m[%12s]\x1b[0m |\n\r";
static const uint8_t param_item_end_abnormal[] = "\x1b[31m[%12s]\x1b[0m |\n\r";
static const uint8_t param_item_end_warning[] = "\x1b[38;2;255;165;0m[%12s]\x1b[0m |\n\r";

#else
/* Must define 1 byte although not used in release mode
 * This is for compatibility with CA78K0R
 */
static const uint8_t line_break[1];
static const uint8_t param_intro[1];
static const uint8_t param_text[1];
static const uint8_t param_info_s[1];
static const uint8_t param_info_i[1] ;
static const uint8_t param_info_date[1] ;


static const uint8_t param_itemgroup_start[1] ;
static const uint8_t param_item_start[1];
static const uint8_t param_subitem_start[1] ;
static const uint8_t param_subitem_info_start[1] ;
static const uint8_t param_subitem_info_middle_rtc[1];
static const uint8_t param_item_end_normal[1];
static const uint8_t param_item_end_abnormal[1];
#endif

extern const uint8_t FAR_PTR g_em_lib_type[];
extern const uint8_t FAR_PTR g_em_lib_target_platform[];
extern const uint8_t FAR_PTR g_em_lib_git_revision[];
extern const uint8_t FAR_PTR g_em_lib_compiler[];
extern const uint8_t FAR_PTR g_em_lib_build_date[];
static char version_buffer[48];

static char code_buffer[128];

/***********************************************************************************************************************
 * Function Name    : static uint8_t config_data_load(EM_CALIBRATION * p_calib, st_em_setting_t * p_em_setting)
 * Description      : Load Configuration Page from MCU DataFlash memory
 * Arguments        : uint8_t init_status: initialization status
 * Return Value     : Execution Status
 *                  :    CONFIG_OK         Load data OK
 *                  :    CONFIG_ERROR         Load data error
 ***********************************************************************************************************************/
static uint8_t config_data_load( EM_CALIBRATION * p_calib, st_em_setting_t * p_em_setting)
{
	uint8_t init_status;

	/* Assign pointer for phase degree in calib holder value */
	p_calib->sw_phase_correction.i1_phase_degrees = p_em_setting->degree_list_i1;
	p_calib->sw_phase_correction.i2_phase_degrees = p_em_setting->degree_list_i2;

	/* Assign pointer for gain in calib to holder value */
	p_calib->sw_gain.i1_gain_values = p_em_setting->gain_list_i1;
	p_calib->sw_gain.i2_gain_values = p_em_setting->gain_list_i2;

	/* Init config */
	init_status = CONFIG_Init(0);

	/* Check device format */
	if (init_status == CONFIG_OK)       /* Initial successful, already formatted */
	{

	}
	/* Initial successful, but not formatted */
	else if (init_status == CONFIG_NOT_FORMATTED)
	{
		/* Format device */
		if (CONFIG_Format() != CONFIG_OK)
		{
			/* When format fail,
			 * we need to beak the start-up process here */
			return CONFIG_ERROR;   /* Format fail */
		}
		else    /* format ok */
		{

		}
	}
	/* Restore data from storage in every load config */
	if (CONFIG_LoadEMCalib(p_calib, &p_em_setting->regs) != CONFIG_OK)
	{
		NOP();
	}
	else
	{
		NOP();
	}

	/* Load data sucessfully */
	return CONFIG_OK;
}

/***********************************************************************************************************************
 * Function Name    : uint8_t start_peripheral_and_app(void)
 * Description      : Start-up no checking, just diag code
 * Arguments        : None
 * Return Value     : None
 ***********************************************************************************************************************/
uint8_t start_peripheral_and_app(void)
{
	EM_CALIBRATION           calib;
	st_em_setting_t          em_hold_setting_value;
	volatile uint32_t clock_count = 0UL;
	uint16_t cpu_speed = 24;

	if(HOCODIV == 0x00)
    {
		cpu_speed = 24;
    }
    else if(HOCODIV == 0x01)
    {
    	cpu_speed = 12;
    }
    else
    {
    	cpu_speed = 6;
    }

	DEBUG_Printf((uint8_t *)"\n\r");
	DEBUG_Printf((uint8_t *)line_break);
	DEBUG_Printf((uint8_t *)param_intro, " ", "Energy Meter Start-up", " ");
	DEBUG_Printf((uint8_t *)line_break);
#if defined(SHUNT)
	DEBUG_Printf((uint8_t *)param_intro, " ", "RL78/I1C Shunt PMOD", " ");
#elif defined(ROGOWSKI)
	DEBUG_Printf((uint8_t *)param_intro, " ", "RL78/I1C Rogowski PMOD", " ");
#else
#error
#endif
	DEBUG_Printf((uint8_t *)line_break);
	DEBUG_Printf((uint8_t *)param_intro, " ", app_build_date_str, " ");
	DEBUG_Printf((uint8_t *)line_break);
	DEBUG_Printf((uint8_t *)param_text, "Metrology library version info");

	_COM_strcpy_ff(version_buffer,(const char FAR_PTR *)VERSION_LIB_TYPE_STR);
	_COM_strcpy_ff(&version_buffer[_COM_strlen_f(VERSION_LIB_TYPE_STR)],(const char FAR_PTR *)g_em_lib_type);
	DEBUG_Printf((uint8_t *)param_text, version_buffer);

	_COM_strcpy_ff(version_buffer,(const char FAR_PTR *)VERSION_TGT_DEVS_STR);
	_COM_strcpy_ff(&version_buffer[_COM_strlen_f(VERSION_TGT_DEVS_STR)],(const char FAR_PTR *)g_em_lib_target_platform);
	DEBUG_Printf((uint8_t *)param_text, version_buffer);

	_COM_strcpy_ff(version_buffer,(const char FAR_PTR *)VERSION_GIT_HASH_STR);
	_COM_strcpy_ff(&version_buffer[_COM_strlen_f(VERSION_GIT_HASH_STR)],(const char FAR_PTR *)g_em_lib_git_revision);
	DEBUG_Printf((uint8_t *)param_text, version_buffer);

	_COM_strcpy_ff(version_buffer,(const char FAR_PTR *)VERSION_LIB_COMP_STR);
	_COM_strcpy_ff(&version_buffer[_COM_strlen_f(VERSION_LIB_COMP_STR)],(const char FAR_PTR *)g_em_lib_compiler);
	DEBUG_Printf((uint8_t *)param_text, version_buffer);

	_COM_strcpy_ff(version_buffer,(const char FAR_PTR *)VERSION_BLD_DATE_STR);
	_COM_strcpy_ff(&version_buffer[_COM_strlen_f(VERSION_BLD_DATE_STR)],(const char FAR_PTR *)g_em_lib_build_date);
	DEBUG_Printf((uint8_t *)param_text, version_buffer);

	sprintf(code_buffer, "CPU Speed: %u MHz", cpu_speed);
	DEBUG_Printf((uint8_t *)param_text, code_buffer);

	DEBUG_Printf((uint8_t *)param_text, " ");
	DEBUG_Printf((uint8_t *)param_item_start, 1, "Checking Crystal Operation");
	clock_count = R_TAU0_7_CountPCLBUZ0();
	if((clock_count > 3194UL) && (clock_count < 3358UL)) /* check for crystal oscillator clocks within 100ms +/- 2.5% (allowing for HOCO variation)*/
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_abnormal, "ABNORMAL");
		return 1U;
	}
	sprintf(code_buffer, " . Crystal Clock Cycles in 100ms: %lu", clock_count);
	DEBUG_Printf((uint8_t *)param_text, code_buffer);

	DEBUG_Printf((uint8_t *)param_item_start, 2, "Checking RESF");
	if (g_reset_flag == 0x00)
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_abnormal, "ABNORMAL");

		if (g_reset_flag & TRAP)
		{
			DEBUG_Printf((uint8_t *)param_text, " . Illegal instruction Reset");
		}
		if (g_reset_flag & WDTRF)
		{
			DEBUG_Printf((uint8_t *)param_text, " . WDT Reset");
		}
		if (g_reset_flag & RPERF)
		{
			DEBUG_Printf((uint8_t *)param_text, " . RAM Parity Error Reset");
		}
		if (g_reset_flag & IAWRF)
		{
			DEBUG_Printf((uint8_t *)param_text, " . Illegal-memory access reset");
		}
		if (g_reset_flag & LVIRF)
		{
			DEBUG_Printf((uint8_t *)param_text, " . LVD Reset");
		}
	}

	/** Initialise Config Storage*/
	DEBUG_Printf((uint8_t *)param_item_start, 3, "Retrieve Meter Config (calibration)");
	g_em_startup_diag.config_load_status = config_data_load(&calib, &em_hold_setting_value);
	if(EM_OK != g_em_startup_diag.config_load_status)
	{
		sprintf(code_buffer, " . Error: %u", g_em_startup_diag.config_load_status);
		DEBUG_Printf((uint8_t *)param_text, code_buffer);
		DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
		return 1U;
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
	}

	/** Initialise EM Library*/
	DEBUG_Printf((uint8_t *)param_item_start, 4, "Initialise EM Core");
	R_DSADC_SetGain(em_hold_setting_value.regs); /* Init load data from storage for ADC driver */
	g_em_startup_diag.em_init_status = EM_Init((EM_PLATFORM_PROPERTY FAR_PTR *)&g_EM_DefaultProperty, &calib);
	if(EM_OK != g_em_startup_diag.em_init_status)
	{
		sprintf(code_buffer, " . Error: %u", g_em_startup_diag.em_init_status);
		DEBUG_Printf((uint8_t *)param_text, code_buffer);
		DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
		return 1U;
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
	}

	/** Initialise Meter Storage*/
	DEBUG_Printf((uint8_t *)param_item_start, 5, "Retrieve & Restore Meter Storage (energy)");
	g_em_startup_diag.energy_load_status = STORAGE_EM_Restore();
	if(EM_STORAGE_HEADER_INVALID == g_em_startup_diag.energy_load_status)
	{
		g_em_startup_diag.energy_load_status = STORAGE_EM_Format();
	}

	if(EM_STORAGE_OK == g_em_startup_diag.energy_load_status)
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
	}


	/** Start the EM Library*/
	DEBUG_Printf((uint8_t *)param_item_start, 6, "Start EM Core");
	g_em_startup_diag.em_start_status = EM_Start();
	if (EM_OK != g_em_startup_diag.em_start_status)
	{
		sprintf(code_buffer, " . Error: %u", g_em_startup_diag.em_start_status);
		DEBUG_Printf((uint8_t *)param_text, code_buffer);
		DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
		return 1U;
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
	}

	/** Check VDD*/
	DEBUG_Printf((uint8_t *)param_item_start, 7, "Checking VDD");
	vdd_range = R_LVD_Check();
	if(vdd_less_than_2_46 == vdd_range || vdd_2_46_to_2_67 == vdd_range)
	{
		DEBUG_Printf((uint8_t *)param_item_end_warning, "WARNING");
		sprintf(code_buffer, " . VDD LOW: %s", R_LVD_range_to_str(vdd_range));
		DEBUG_Printf((uint8_t *)param_text, code_buffer);
	}
	else
	{
		DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
		sprintf(code_buffer, " . VDD OK: %s", R_LVD_range_to_str(vdd_range));
		DEBUG_Printf((uint8_t *)param_text, code_buffer);
	}

	/** Start peripherals */
	R_RTC_Start();
	R_RTC_Set_ConstPeriodInterruptOn(SEC1_2);

	/** End Start-up*/
	DEBUG_Printf((uint8_t *)line_break);
	DEBUG_AckNewLine();

	return 0U;
}

/***********************************************************************************************************************
 * Function Name    : void startup(void)
 * Description      : Start-up energy meter
 * Arguments        : None
 * Return Value     : None
 ***********************************************************************************************************************/
uint8_t startup(void)
{
	g_reset_flag = RESF;

	return start_peripheral_and_app();
}

