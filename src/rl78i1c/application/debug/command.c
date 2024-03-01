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
 * File Name    : command.c
 * Version      : 1.00
 * Device(s)    : RL78/I1C
 * Tool-Chain   :
 * H/W Platform : RL78/I1C Energy Meter Platform
 * Description  : Command processing source File
 ***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_rtc.h"           /* RTC Driver */
#include "r_cg_dsadc.h"         /* DSAD Driver */
#include "r_cg_wdt.h"           /* WDT Driver */
#include "r_cg_lvd.h"

/* Wrapper */
#include "wrp_em_sw_config.h"   /* EM Software Config */
#include "wrp_em_sw_property.h"

/* MW/Core, Common */
#include "em_core.h"            /* EM Core APIs */

/* Application */
#include "config_format.h"      /* DATALFLASH Format */
#include "eeprom.h"             /* EEPROM Driver */
#include "dataflash.h"          /* DATALFLASH Driver */
#include "config_storage.h"     /* Storage Header File */
#include "platform.h"
#include "inst_read.h"
#include "calibration.h"
#include "storage_em.h"

/* Debug Only */
#ifdef __DEBUG
#include <stdio.h>          /* Standard IO */
#include <stdarg.h>         /* Variant argument */
#include <string.h>         /* String Standard Lib */
#include <stdlib.h>
#include <math.h>           /* Mathematic Standard Lib */
#include <limits.h>         /* Number limits definitions */
#include "cmd.h"            /* CMD Prompt Driver */

#ifdef METER_ENABLE_MEASURE_CPU_LOAD
#include "load_test.h"
#endif

#endif

#ifdef __DEBUG

/***********************************************************************************************************************
Typedef definitions
 ***********************************************************************************************************************/
/* Command Invoker Function */
typedef uint8_t (*FUNC)(uint8_t *arg_str);

/* Command Item */
typedef struct tagCommandTable
{
	const uint8_t   *cmd_name;
	const uint8_t   *params;
	const uint8_t   *description;
	const FUNC      func;
} COMMAND_ITEM;

/***********************************************************************************************************************
Macro definitions
 ***********************************************************************************************************************/
/* Common macro */
#define     _BCD2DEC(x)     {x = (x & 0xF) + (x >> 4) * 10;}                /* 1 byte BCD to DEC Conversion */
#define     _DEC2BCD(x)     {x = (x % 10 ) | (((x / 10) << 4) & 0xF0);}     /* 1 byte DEC to BCD Conversion */

/* Command Status */
#define COMMAND_OK          0
#define COMMAND_ERROR       1

/* Command Invoker Statement & Table Length */
#define INVOKE_COMMAND(index, arg_str)  {                                                   \
		if (cmd_table[index].func != NULL)              \
		{                                               \
			(*cmd_table[index].func)(arg_str);          \
		}                                               \
}
#define COMMAND_TABLE_LENGTH    (sizeof(cmd_table) / sizeof(COMMAND_ITEM))

/***********************************************************************************************************************
Imported global variables and functions (from other files)
 ******************************************************************************/
/* Import from dataflash */
extern uint8_t DATAFLASH_ReadHeaderStatus(uint8_t block, uint8_t *buf);
extern uint8_t DATAFLASH_ReadHeaderCRC(uint8_t bank, uint8_t *buf);
extern uint8_t DATAFLASH_InternalRead(uint32_t addr, uint8_t* buf, uint16_t size);

/* Import from Wrapper */
extern uint16_t g_sample_count;
extern int32_t  g_sample0[];
extern int32_t  g_sample1[];
extern uint8_t  g_sample1_direction;
extern const uint16_t g_sample_max_count;

static EM_CALIBRATION l_calib;

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
Private global variables and functions
 ******************************************************************************/
/* Sub-function (static for module) */
static uint8_t * COMMAND_GetScanOneParam(
		uint8_t *output,
		uint16_t size,
		uint8_t *input,
		uint8_t *ahead_str,
		uint8_t *break_str
);

/* Command Invoker */
static uint8_t COMMAND_InvokeUsage(uint8_t *arg_str);                           /* Usage */
static uint8_t COMMAND_InvokeStartEM(uint8_t *arg_str);                         /* Start EM */
static uint8_t COMMAND_InvokeStopEM(uint8_t *arg_str);                          /* Stop EM */
static uint8_t COMMAND_InvokeRestartEM(uint8_t *arg_str);                       /* Restart EM */
static uint8_t COMMAND_InvokeDisplay(uint8_t *arg_str);                         /* Display */
static uint8_t COMMAND_InvokeDisplayBin(uint8_t *arg_str);                      /* Display (Raw Binary)*/
static uint8_t COMMAND_InvokeEnergyReset(uint8_t *arg_str);                     /* Resets live energy data and energy log*/
static uint8_t COMMAND_InvokeRtc(uint8_t *arg_str);                             /* RTC */
static uint8_t COMMAND_InvokeSetRtc(uint8_t *arg_str);                          /* Set RTC */
static uint8_t COMMAND_InvokeGetEnergyLog(uint8_t *arg_str);                    /* Get energy log */
static uint8_t COMMAND_InvokeBackup(uint8_t *arg_str);                          /* Backup */
static uint8_t COMMAND_InvokeRestore(uint8_t *arg_str);                         /* Restore */
static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str);                      /* Read Memory */
static uint8_t COMMAND_InvokeWriteMemory(uint8_t *arg_str);                     /* Write Memory */
static uint8_t COMMAND_InvokeFormatMemory(uint8_t *arg_str);                    /* Format Memory */
static uint8_t COMMAND_InvokeReadDTFLHeader(uint8_t *arg_str);                  /* Read DTFL header */
static uint8_t COMMAND_InvokeEraseDTFL(uint8_t *arg_str);                       /* Erase DTFL content */
static uint8_t COMMAND_InvokeDumpSample(uint8_t *arg_str);                      /* Dump waveform samples */
static uint8_t COMMAND_InvokeCalibration(uint8_t *arg_str);                     /* Calibration */
static uint8_t COMMAND_InvokeGetCalibration(uint8_t *arg_str);                  /* Get Calibration Info */
static uint8_t COMMAND_InvokeSetConfig(uint8_t *arg_str);                       /* Set configuration */
static uint8_t COMMAND_InvokeCPULoad(uint8_t *arg_str);                         /* Measure CPU Load */
static uint8_t COMMAND_InvokeVddCheck(uint8_t *arg_str);                        /* Measure VDD Range using LVD */
static uint8_t COMMAND_InvokeInvertPolarity(uint8_t *arg_str);                  /* Inverts Polarity on current channel */

/* Command Table */
static const COMMAND_ITEM   cmd_table[] = 
{
		/* ---------------------------------------------------------------------------------------------------------------------*/
		/* Command Name     Parameter                   Description                                     Func Pointer */
		/* ---------------------------------------------------------------------------------------------------------------------*/
		{(const uint8_t *)"?"               ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Help"                                                         ,   COMMAND_InvokeUsage                     },
		{(const uint8_t *)"cls"             ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Clear screen"                                                 ,   NULL                                    },      /* CMD module processed it internally */
		{(const uint8_t *)"start"           ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Start EM"                                                     ,   COMMAND_InvokeStartEM                   },
		{(const uint8_t *)"stop"            ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Stop EM"                                                      ,   COMMAND_InvokeStopEM                    },
		{(const uint8_t *)"restart"         ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Restart EM"                                                   ,   COMMAND_InvokeRestartEM                 },
		{(const uint8_t *)"display"         ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Display current measured data"                                ,   COMMAND_InvokeDisplay                   },
		{(const uint8_t *)"displaybin"      ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Display current measured data (Raw Binary)"                   ,   COMMAND_InvokeDisplayBin                },
		{(const uint8_t *)"energyreset"     ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Stops EM - resets live and logged energy - starts EM"         ,   COMMAND_InvokeEnergyReset               },
		{(const uint8_t *)"rtc"             ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Display current RTC time"                                     ,   COMMAND_InvokeRtc                       },
		{(const uint8_t *)"setrtc"          ,   (const uint8_t *)"dd/mm/yy hh:mm:ss ww"                     ,   (const uint8_t *)"Set RTC time"                                                 ,   COMMAND_InvokeSetRtc                    },
		{(const uint8_t *)"getenergylog"    ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Prints last energy log"                                       ,   COMMAND_InvokeGetEnergyLog              },
		{(const uint8_t *)"backup"          ,   (const uint8_t *)"selection"                                ,   (const uint8_t *)"Backup to Storage Memory"                                     ,   COMMAND_InvokeBackup                    },
		{(const uint8_t *)"restore"         ,   (const uint8_t *)"selection"                                ,   (const uint8_t *)"Restore from Storage Memory"                                  ,   COMMAND_InvokeRestore                   },
		{(const uint8_t *)"readmem"         ,   (const uint8_t *)"type(0:EEP,1:DTFL) addr size [cast]"      ,   (const uint8_t *)"Read memory type at addr, size, display value"                ,   COMMAND_InvokeReadMemory                },
		{(const uint8_t *)"writemem"        ,   (const uint8_t *)"type(0:EEP,1:DTFL) addr size value"       ,   (const uint8_t *)"Write memory type at addr, size with value"                   ,   COMMAND_InvokeWriteMemory               },
		{(const uint8_t *)"readdtflhead"    ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Read DataFlash status header"                                 ,   COMMAND_InvokeReadDTFLHeader            },
		{(const uint8_t *)"erasedtfl"       ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Erase content of dataflash"                                   ,   COMMAND_InvokeEraseDTFL                 },
		{(const uint8_t *)"formatmem"       ,   (const uint8_t *)"type(0:EEP,1:DTFL)"                       ,   (const uint8_t *)"Format memory type (followed format.h)"                       ,   COMMAND_InvokeFormatMemory              },
		{(const uint8_t *)"dump"            ,   (const uint8_t *)"current"                                  ,   (const uint8_t *)"Dump waveform from EM Core"                                   ,   COMMAND_InvokeDumpSample          		},
		{(const uint8_t *)"calib"           ,   (const uint8_t *)"c cp imax v i w "                         ,   (const uint8_t *)"Calibrate (cycle,cycle_phase,imax,V,I,wire)"                  ,   COMMAND_InvokeCalibration               },
		{(const uint8_t *)"getcalib"        ,   (const uint8_t *)""                         				,   (const uint8_t *)"Prints Calibration Coefficients"                  			,   COMMAND_InvokeGetCalibration               },
		{(const uint8_t *)"setconfig"       ,   (const uint8_t *)"get:leave empty; set:1 later follow guide",   (const uint8_t *)"Manually set configuration"                                   ,   COMMAND_InvokeSetConfig                 },
		{(const uint8_t *)"cpuload"         ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Measure the CPU Load"                                         ,   COMMAND_InvokeCPULoad                   },
		{(const uint8_t *)"vddcheck"        ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Return VDD Range, According to LVD"                           ,   COMMAND_InvokeVddCheck                   },
		{(const uint8_t *)"invertpolarity"  ,   (const uint8_t *)"channel (0 or 1)"                         ,   (const uint8_t *)"Inverts ADC readings (multiply by -1) on given channel"       ,   COMMAND_InvokeInvertPolarity                   },
};

static const uint8_t * g_mem_epr = (const uint8_t *)"EEPROM";
static const uint8_t * g_mem_dtfl = (const uint8_t *)"DATAFLASH";


/******************************************************************************
 * Function Name : COMMAND_GetUnsigned16
 * Interface     : static uint16_t COMMAND_GetUnsigned16(uint8_t *buffer)
 * Description   : Get unsigned int 16 bit from buffer
 * Arguments     : uint8_t * buffer: Buffer to cast to uint16_t
 * Function Calls: None
 * Return Value  : static uint16_t
 ******************************************************************************/
static uint16_t COMMAND_GetUnsigned16(uint8_t *buffer)
{
	uint16_t u16;
	u16  = (uint16_t)buffer[0];
	u16 |= (uint16_t)buffer[1] << 8;

	return (u16);
}

/******************************************************************************
 * Function Name : COMMAND_GetUnsigned32
 * Interface     : static uint32_t COMMAND_GetUnsigned32(uint8_t *buffer)
 * Description   : Get unsigned long 32 bit from buffer
 * Arguments     : uint8_t * buffer: Buffer to cast to uint32_t
 * Function Calls: None
 * Return Value  : static uint32_t
 ******************************************************************************/
static uint32_t COMMAND_GetUnsigned32(uint8_t *buffer)
{
	uint32_t u32;
	u32  = (uint32_t)buffer[0];
	u32 |= (uint32_t)buffer[1] << 8;
	u32 |= (uint32_t)buffer[2] << 16;
	u32 |= (uint32_t)buffer[3] << 24;

	return (u32);
}

/***********************************************************************************************************************
 * Function Name : COMMAND_IsFloatNumber
 * Interface     : static uint8_t COMMAND_IsFloatNumber(uint8_t *buffer)
 * Description   : Check whether the input buffer is number or not
 * Arguments     : uint8_t * buffer: Buffer to check
 * Function Calls: None
 * Return Value  : static uin8t_t: 1 if buffer is number or 0 if buffer contain characters
 ***********************************************************************************************************************/
static uint8_t COMMAND_IsFloatNumber(uint8_t *buffer)
{
	/* Note this function not check the NULL buffer */
	uint8_t dot_count = 0;
	uint8_t plus_count = 0;
	uint8_t minus_count = 0;

	while(*buffer != 0)
	{
		if ((*buffer >= '0' && *buffer <='9') || (*buffer == '.')
				|| (*buffer == '+') || (*buffer == '-'))
		{
			/* Check number of '.' sign */
			if (*buffer == '.')
			{
				dot_count++;
				if (dot_count > 1)
				{
					return 0;
				}
			}

			/* Check number of '+' sign */
			if (*buffer == '+')
			{
				plus_count++;
				if (plus_count > 1)
				{
					return 0;
				}
			}

			/* Check number of '-' sign */
			if (*buffer == '-')
			{
				minus_count++;
				if (minus_count > 1)
				{
					return 0;
				}
			}
			buffer++;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

/******************************************************************************
 * Function Name    : static uint8_t *COMMAND_GetScanOneParam(
 *                  :     uint8_t *output,
 *                  :     uint16_t size,
 *                  :     uint8_t *input,
 *                  :     uint8_t *ahead_str,
 *                  :     uint8_t *break_str
 *                  : );
 * Description      : Command Get One Parameter
 * Arguments        : uint8_t *output: Output buffer
 *                  : uint16_t size: Size of output buffer
 *                  : uint8_t *input: Input buffer
 *                  : uint8_t *ahead_str: Char array to skip the ahead the arg_str
 *                  : uint8_t *break_str: Char array to break the scan processing
 * Functions Called : None
 * Return Value     : Next location of parameter
 ******************************************************************************/
static uint8_t *COMMAND_GetScanOneParam(uint8_t *output,
		uint16_t size,
		uint8_t *input,
		uint8_t *ahead_str,
		uint8_t *break_str)
{
	uint8_t i, j, break_len, ahead_len;

	/* Check parameter */
	if (output == NULL || size == 0 ||
			input  == NULL || break_str == NULL)
	{
		return NULL;
	}

	if (ahead_str != NULL && *ahead_str != 0)
	{
		ahead_len = (uint8_t)strlen((char *)ahead_str);

		/* skip all ahead char that found on arg_str */
		while (*input != 0)
		{
			for (j = 0; j < ahead_len; j++)
			{
				if (ahead_str[j] == *input)
					break;
			}

			/* is found in break_str? */
			if (j != ahead_len)
			{
				input++;
			}
			else    /* Not found */
			{
				break;  /* Break the outsize loop */
			}
		}
	}

	/* Is have remain char? */
	if (*input != 0)
	{
		/* clear output buffer */
		memset(output, 0, size);

		/* Get the lenght of break_str */
		break_len = (uint8_t)strlen((char *)break_str);

		/* Scan the parameter */
		i = 0;
		while (i < size && *input != 0)
		{
			for (j = 0; j < break_len; j++)
			{
				if (break_str[j] == *input)
					break;
			}

			/* is found in break_str?
			 * --> end of scanning */
			if (j != break_len)
			{
				break;  /* end of scanning */
			}
			else    /* Not found --> continue to scan */
			{
				/* Update to buffer */
				output[i] = *input;

				/* scan next byte */
				input++;
				i++;
			}
		}

		return input;
	}
	else
	{
		return NULL;
	}
}

/******************************************************************************
 * Function Name    : static uint16_t COMMAND_HexStringToUnsigned16(uint8_t *str)
 * Description      : Command Convert Hex String to number
 * Arguments        : uint8_t *str: Hex string
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static uint16_t COMMAND_HexStringToUnsigned16(uint8_t *str, uint8_t *b_found)
{
	uint16_t    rlt = 0;
	uint8_t     hexnum;

	*b_found = 0; /* Not found */

	/* Check parameter */
	if (str == NULL)
	{
		return 0;
	}

	/* point to 'x' char */
	while (*str != 0 && *str != 'x')
	{
		str++;
	}

	/* end string? */
	if (*str == 0 || (*(str+1)) == 0)
	{
		return 0;
	}

	str++;  /* skip 'x' */
	while ( *str != 0  &&       /* not end string?, and */
			((*str >= '0' &&
					*str <= '9') ||       /* is numberic char?, or */
					(*str >= 'a' &&
							*str <= 'f') ||       /* is in 'a-f'?, or */
							(*str >= 'A' &&
									*str <= 'F')) )       /* is in 'A-F'? */
	{
		*b_found = 1;   /* found */

		if (*str >= 'a')
		{
			hexnum = 10 + (*str - 'a');     /* Get number */
		}
		else if (*str >= 'A')
		{
			hexnum = 10 + (*str - 'A');
		}
		else
		{
			hexnum = *str - '0';
		}

		rlt  = rlt << 4;
		rlt += hexnum;

		/* next char */
		str++;
	}

	return rlt;
}

/******************************************************************************
 * Function Name    : static uint16_t COMMAND_DecStringToNum16_Logic(uint8_t *str)
 * Description      : Command Logic part of converting decimal string to 16bit number
 * Arguments        : uint8_t *str: Hex string
 *                  : uint8_t *b_found: is found number?
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static uint16_t COMMAND_DecStringToNum16_Logic(uint8_t *str, uint8_t *b_found)
{
	uint16_t    rlt = 0;
	uint8_t     decnum;

	/* No pointer checking here, this serve as logic implementation only */
	while ( *str != 0  &&       /* not end string?, and */
			(*str >= '0' &&
					*str <= '9'))     /* is numberic char?*/
	{
		*b_found = 1;   /* found */

		decnum = *str - '0';

		rlt  = rlt * 10;
		rlt += decnum;

		/* next char */
		str++;
	}

	return rlt;
}

/******************************************************************************
 * Function Name    : static uint16_t COMMAND_DecStringToUnsigned16(uint8_t *str)
 * Description      : Command Convert Dec String to number
 * Arguments        : uint8_t *str: Hex string
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static uint16_t COMMAND_DecStringToUnsigned16(uint8_t *str, uint8_t *b_found)
{
	uint16_t    rlt;

	*b_found = 0; /* Not found */

	/* Check parameter */
	if (str == NULL)
	{
		return 0;
	}

	/* end string? */
	if (*str == 0)
	{
		return 0;
	}

	rlt = COMMAND_DecStringToNum16_Logic(str, b_found);

	return rlt;
}


/******************************************************************************
 * Function Name    : static uint16_t COMMAND_DecStringToSigned16(uint8_t *str)
 * Description      : Command Convert Dec String to number
 * Arguments        : uint8_t *str: Hex string
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static int16_t COMMAND_DecStringToSigned16(uint8_t *str, uint8_t *b_found)
{
	int16_t     rlt;
	uint16_t    rlt_u;
	int16_t     sign;

	*b_found = 0; /* Not found */

	/* Check parameter */
	if (str == NULL)
	{
		return 0;
	}

	/* end string? */
	if (*str == 0)
	{
		return 0;
	}

	sign = (*str == '-') ? -1 : 1;
	if (sign == -1)
	{
		str++;
	}

	rlt_u = COMMAND_DecStringToNum16_Logic(str, b_found);

	/* Is input number in range of signed 16bit number? */
	if (sign == -1)
	{
		if (rlt_u > ((uint16_t)INT_MAX + 1))
		{
			*b_found = 0;
		}
	}
	else
	{
		if (rlt_u > INT_MAX)
		{
			*b_found = 0;
		}
	}

	/* Number checking OK, get sign to result */
	rlt = ((int16_t)rlt_u * sign);

	return rlt;
}


/******************************************************************************
 * Function Name    : static uint16_t COMMAND_HexBufferToUnsigned16(uint8_t *str, uint16_t len, uint8_t *b_found)
 * Description      : Command Convert Hex Buffer to number
 * Arguments        : uint8_t *str: Hex string
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static uint16_t COMMAND_HexBufferToUnsigned16(uint8_t *buf, uint16_t len, uint8_t *b_found)
{
	uint16_t    pos = 0;
	uint16_t    rlt = 0;
	uint8_t     hexnum;

	*b_found = 0; /* Not found */

	/* Check parameter */
	if (buf == NULL || len == 0)
	{
		return 0;
	}

	/* point to 'x' char */
	while (pos < len && buf[pos] != 'x')
	{
		pos++;
	}

	/* end of buffer? */
	if (pos == len)
	{
		return 0;
	}

	pos++;  /* skip 'x' */
	while ( pos < len  &&       /* not end string?, and */
			((buf[pos] >= '0' &&
					buf[pos] <= '9') ||       /* is numberic char?, or */
					(buf[pos] >= 'a' &&
							buf[pos] <= 'f') ||       /* is in 'a-f'?, or */
							(buf[pos] >= 'A' &&
									buf[pos] <= 'F')) )       /* is in 'A-F'? */
	{
		*b_found = 1;   /* found */

		if (buf[pos] >= 'a')
		{
			hexnum = 10 + (buf[pos] - 'a');     /* Get number */
		}
		else if (buf[pos] >= 'A')
		{
			hexnum = 10 + (buf[pos] - 'A');
		}
		else
		{
			hexnum = buf[pos] - '0';
		}

		rlt  = rlt << 4;
		rlt += hexnum;

		/* next char */
		pos++;
	}

	return rlt;
}

/******************************************************************************
 * Function Name : COMMAND_GetPowerFactorSignName
 * Interface     : static const uint8_t* COMMAND_GetPowerFactorSignName(EM_PF_SIGN sign)
 * Description   : Get the PF sign name
 * Arguments     : EM_PF_SIGN sign: The sign of power factor
 * Function Calls: None
 * Return Value  : static const uint8_t*
 ******************************************************************************/
static const uint8_t* COMMAND_GetPowerFactorSignName(EM_PF_SIGN sign)
{
	switch (sign)
	{
	case PF_SIGN_LEAD_C:
		return (const uint8_t*)"PF_SIGN_LEAD_C";
	case PF_SIGN_UNITY:
		return (const uint8_t*)"PF_SIGN_UNITY";
	case PF_SIGN_LAG_L:
		return (const uint8_t*)"PF_SIGN_LAG_L";
	default:
		return (const uint8_t*)"";
	}
}

/******************************************************************************
 * Function Name : COMMAND_GetPowerFactorSignName
 * Interface     : static const uint8_t* COMMAND_GetPowerFactorSignName(EM_PF_SIGN sign)
 * Description   : Get the DSAD gain name
 * Arguments     : dsad_gain_t gain: The gain of DSAD channel
 * Function Calls: None
 * Return Value  : static const uint8_t*
 ******************************************************************************/
static const uint8_t* COMMAND_GetDsadGainName(dsad_gain_t gain)
{
	switch (gain)
	{
	case GAIN_X1:
		return (const uint8_t*)"GAIN_X1";
	case GAIN_X2:
		return (const uint8_t*)"GAIN_X2";
	case GAIN_X4:
		return (const uint8_t*)"GAIN_X4";
	case GAIN_X8:
		return (const uint8_t*)"GAIN_X8";
	case GAIN_X16:
		return (const uint8_t*)"GAIN_X16";
	case GAIN_X32:
		return (const uint8_t*)"GAIN_X32";
	default:
		return (const uint8_t*)"UNKNOWN";
	}
}

/******************************************************************************
 * Function Name    : static uint16_t COMMAND_DecBufferToUnsigned16(uint8_t *str, uint16_t len, uint8_t *b_found)
 * Description      : Command Convert Dec Buffer to number
 * Arguments        : uint8_t *buf: Dec buffer
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static uint16_t COMMAND_DecBufferToUnsigned16(uint8_t *buf, uint16_t len, uint8_t *b_found)
{
	uint16_t    pos = 0;
	uint16_t    rlt = 0;
	uint8_t     decnum;

	*b_found = 0; /* Not found */

	/* Check parameter */
	if (buf == NULL)
	{
		return 0;
	}

	/* end string? */
	if (len == 0)
	{
		return 0;
	}

	while ( pos < len &&        /* not end string?, and */
			(buf[pos] >= '0' &&
					buf[pos] <= '9'))     /* is numberic char?*/
	{
		*b_found = 1;   /* found */

		decnum = buf[pos] - '0';

		rlt  = rlt * 10;
		rlt += decnum;

		/* next char */
		pos++;
	}

	return rlt;
}

/********************************************************************************************
 * Function Name    : static uint8_t COMMAND_FastInputScan(uint8_t *output, uint8_t * input)
 * Description      : Command Convert Fast input parameter
 *                  : Ex1: Input "0-3" ==> Output: {0,1,2,3}
 *                  : Ex2: Input "0-1,2,3-4" ==> Output: {0,1,2,3,4}
 * Arguments        : uint8_t *input: Input argument string
 *                  : uint8_t *output: Output decoded argument in byte array
 * Functions Called : None
 * Return Value     : None
 ********************************************************************************************/
#define DELIMITER_SIGN              (',')
#define UPTO_SIGN                   ('-')

static uint8_t COMMAND_FastInputScan(uint8_t *output, uint8_t * input, uint8_t len_limit)
{
	uint8_t i;

	uint8_t pos_deli_start = 0, pos_deli_stop = 0;
	uint8_t pos_upto;

	uint8_t range_start, range_stop;
	uint8_t decoded_num;

	uint8_t input_len;
	uint8_t is_found;
	uint8_t upto_cnt = 0;

	/* Check for NULL argument */
	if (output == NULL || input == NULL)
	{
		return 1;                           //NULL params
	}

	/* Check first character */
	if (*input == DELIMITER_SIGN || *input == UPTO_SIGN)
	{
		return 2;                           //Wrong format
	}

	/* Get length of input string */
	input_len = (uint8_t)strlen((const char *)input);

	/* Check last character */
	if (input[input_len] == DELIMITER_SIGN || input[input_len] == UPTO_SIGN)
	{
		return 2;                           //Wrong format
	}

	/* Clear the buffer */
	memset(output, 0, len_limit);

	while (pos_deli_stop < input_len)
	{
		/* Find next DELIMITER_SIGN */
		for (i=pos_deli_start; i < input_len; i++)
		{
			pos_deli_stop = i;
			if (input[i] == DELIMITER_SIGN)
			{
				break;              //Found the delimiter
			}

			if (i == (input_len - 1))
			{
				pos_deli_stop++;    /* In case reach the end, need to increase one more time*/
			}
		}

		/* Now, got the range of  input*/
		/* Find the UPTO_SIGN */
		for (i=pos_deli_start; i < pos_deli_stop; i++)
		{
			if (input[i] == UPTO_SIGN)
			{
				upto_cnt++;
				pos_upto = i;
			}
		}

		if (upto_cnt > 1)
		{
			return 2;                       //Wrong format, duplicate UPTO_SIGN
		}
		else
		{
			if (upto_cnt == 1)
			{
				//Have upto_sign ==> decode
				/* Range start */
				range_start = (uint8_t)COMMAND_DecBufferToUnsigned16(&input[pos_deli_start],
						pos_upto - pos_deli_start,
						&is_found);

				/* Range stop */
				range_stop = (uint8_t)COMMAND_DecBufferToUnsigned16(&input[pos_upto+1],
						pos_deli_stop - 1 - pos_upto,
						&is_found);
				if (is_found &&
						(range_start < len_limit) && (range_stop < len_limit))
				{
					while(range_start <= range_stop)
					{
						*output++ = range_start;
						range_start++;
					}
				}
				else
				{
					return 2;               //Wrong data
				}
			}
			else
			{
				//No upto_sign ==> decode normally
				decoded_num = (uint8_t)COMMAND_DecBufferToUnsigned16(&input[pos_deli_start],
						pos_deli_stop - pos_deli_start,
						&is_found);
				if(is_found &&
						(decoded_num < len_limit))
				{
					*output++ = decoded_num;
				}
				else
				{
					return 2;                   //Wrong data
				}
			}
		}
		upto_cnt = 0;                           /* Reset upto_cnt */
		pos_deli_start = pos_deli_stop + 1;     /* Jump to pos after delimiter */
	}

	return 0;
}


/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeUsage(uint8_t *arg_str)
 * Description      : Command Invoke Usage
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeUsage(uint8_t *arg_str)
{
	uint8_t i;

	CMD_SendString((uint8_t *)"\n\r");
	CMD_SendString((uint8_t *)"----------------------------------------------------------------------------------------------------------\n\r");
	CMD_Printf((uint8_t *)" %- 15s %- 42s %s\n\r", "Command Name", "Parameter", "Description");
	CMD_SendString((uint8_t *)"----------------------------------------------------------------------------------------------------------\n\r");
	for (i = 0; i < COMMAND_TABLE_LENGTH; i++)
	{
		R_WDT_Restart();
		CMD_Printf( (uint8_t *)" %- 15s %- 42s %s\n\r",
				cmd_table[i].cmd_name,
				cmd_table[i].params,
				cmd_table[i].description);
	}

	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeStartEM(uint8_t *arg_str)
 * Description      : Command Invoke Start EM
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeStartEM(uint8_t *arg_str)
{
	CMD_SendString((uint8_t *)"\n\rStarting EM...");
	if (EM_Start() == EM_OK)
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}
	else
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
	}

	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeStopEM(uint8_t *arg_str)
 * Description      : Command Invoke Stop EM
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeStopEM(uint8_t *arg_str)
{
	CMD_SendString((uint8_t *)"\n\rStopping EM...");
	if (EM_Stop() == EM_OK)
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}
	else
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
	}

	return 0;
}

/******************************************************************************
 * Function Name    : static void COMMAND_ResetByIllegalMemory(void)
 * Description      : Command Intentionally Reset by using illegal memory access
 * Arguments        : None
 * Functions Called : None
 * Return Value     : None
 ******************************************************************************/
static void COMMAND_ResetByIllegalMemory(void)
{
#ifdef __IAR__
	asm("MOV ES, #1H");
	asm("MOV ES:1800H, #00H");
#else
	FAR_PTR uint16_t * ptr;

	ptr = (FAR_PTR uint16_t *)0x00011800;

	/* Illegal address access */
	*ptr = 0xFFFF;
#endif /* __IAR__ */
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeRestartEM(uint8_t *arg_str)
 * Description      : Command Invoke Restart EM
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeRestartEM(uint8_t *arg_str)
{
	CMD_SendString((uint8_t *)"\n\r Prepare illegal memory access...reset immediately...");

	COMMAND_ResetByIllegalMemory();

	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeDisplay(uint8_t *arg_str)
 * Description      : Command Invoke Display
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeDisplay(uint8_t *arg_str)
{
	uint32_t    timeout;
	float32_t   temps_degree = 25.0f;
	static const uint8_t line_break[]   = "+------------------------------------------------------------------------+\n\r";
	static const uint8_t param_format[] = "| %- 30s | %# 15.3f | %- 20s|\n\r";
	static const uint8_t param_format_s[] = "| %- 30s | %# 15s | %- 20s|\n\r";

	CMD_Printf((uint8_t *)"\n\rWaiting for signal stable...");
	CMD_Printf((uint8_t *)"\n\r");

	timeout = 30000;
	while (timeout > 0)
	{
		timeout--;
	}

	CMD_SendString((uint8_t *)line_break);
	CMD_Printf((uint8_t *)"| %- 30s | %- 15s | %- 20s|\n\r", "Parameter", "Total", "Unit");
	CMD_SendString((uint8_t *)line_break);

	CMD_Printf(
			(uint8_t *)param_format,
			"Voltage RMS",
			g_inst_read_params.vrms,
			"Volt"
	);

	CMD_Printf(
			(uint8_t *)param_format,
			"Current RMS Phase",
			g_inst_read_params.irms,
			"Ampere"
	);

	CMD_Printf(
			(uint8_t *)param_format,
			"Current RMS Neutral",
			g_inst_read_params.irms2,
			"Ampere"
	);

	CMD_Printf(
			(uint8_t *)param_format,
			"Active Power",
			g_inst_read_params.active_power,
			"Watt"
	);

	CMD_Printf(
			(uint8_t *)param_format,
			"Fundamental Power",
			g_inst_read_params.fundamental_power,
			"Watt"
	);

	CMD_Printf(
			(uint8_t *)param_format,
			"Reactive Power",
			g_inst_read_params.reactive_power,
			"VAr"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Apparent Power",
			g_inst_read_params.apparent_power,
			"VA"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Power Factor",
			g_inst_read_params.power_factor,
			" "
	);
	CMD_Printf(
			(uint8_t *)param_format_s,
			"Power Factor Sign",
			COMMAND_GetPowerFactorSignName(g_inst_read_params.power_factor_sign),
			" "
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Line Frequency",
			g_inst_read_params.freq,
			"Hz"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Total Active Energy",
			g_inst_read_params.active_energy_total_import,
			"kWh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Total Reactive Energy Lag (L)",
			g_inst_read_params.reactive_energy_lag_total_import,
			"kVArh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Total Reactive Energy Lead (C)",
			g_inst_read_params.reactive_energy_lead_total_import,
			"kVArh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Total Apparent Energy",
			g_inst_read_params.apparent_energy_total_import,
			"kVAh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Active Max Demand",
			g_inst_read_params.active_energy_total_import,
			"kWh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Reactive Max Demand Lag (L)",
			g_inst_read_params.reactive_energy_lag_total_import,
			"kVArh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Reactive Max Demand Lead (C)",
			g_inst_read_params.reactive_energy_lead_total_import,
			"kVArh"
	);
	CMD_Printf(
			(uint8_t *)param_format,
			"Apparent Max Demand",
			g_inst_read_params.apparent_energy_total_import,
			"kVAh"
	);

	CMD_SendString((uint8_t *)line_break);

	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeDisplayBin(uint8_t *arg_str)
 * Description      : Command Invoke Display (Raw Binary) - outputs the inst read struct in raw binary format.
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeDisplayBin(uint8_t *arg_str)
{
	static const uint16_t num_bytes = sizeof(EM_INST_READ_PARAMS);
	CMD_SendBuffer((uint8_t*)&num_bytes, 2); /* Send number of bytes first*/
	CMD_SendBuffer((uint8_t*)&g_inst_read_params, num_bytes);
	CMD_Printf((uint8_t *)"\n\r\n\r");

	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeEnergyReset(uint8_t *arg_str)
 * Description      : Resets the energy log and live energy value, stops and starts EM to do so.
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeEnergyReset(uint8_t *arg_str)
{
	CMD_Printf((uint8_t*) "\n\rStopping EM...");
	if(EM_Stop() == EM_OK)
	{
		CMD_Printf((uint8_t*) "Success!\n\r");
		CMD_Printf((uint8_t*) "Formatting Energy Log...");
		if(STORAGE_EM_Format() == EM_STORAGE_OK)
		{
			CMD_Printf((uint8_t*) "Success!\n\r");
			CMD_Printf((uint8_t*) "Resetting Energy Counters...");
			if(STORAGE_EM_Restore() == EM_STORAGE_OK)
			{
				CMD_Printf((uint8_t*) "Success!\n\r");
				CMD_Printf((uint8_t*) "Starting EM...");
				if(EM_Start() == EM_OK)
				{
					CMD_Printf((uint8_t*) "Success!\n\r");
				}
				else
				{
					CMD_Printf((uint8_t*) "Failed!\n\r");
				}
			}
			else
			{
				CMD_Printf((uint8_t*) "Failed!\n\r");
			}
		}
		else
		{
			CMD_Printf((uint8_t*) "Failed!\n\r");
		}
	}
	else
	{
		CMD_Printf((uint8_t*) "Failed!\n\r");
	}
	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeRtc(uint8_t *arg_str)
 * Description      : Command Invoke RTC
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeRtc(uint8_t *arg_str)
{
	rtc_counter_value_t rtctime;

	R_RTC_Get_CalendarCounterValue(&rtctime);

	CMD_Printf((uint8_t *)"\n\rRTC Time: %02x/%02x/20%x %02x:%02x:%02x Week: %02x\n\r",
			rtctime.day,
			rtctime.month,
			rtctime.year,
			rtctime.hour,
			rtctime.min,
			rtctime.sec,
			rtctime.week);

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeSetRtc(uint8_t *arg_str)
 * Description      : Command Invoke Set RTC
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeSetRtc(uint8_t *arg_str)
{   
	//uint16_t timeout;
	uint8_t status;
	rtc_counter_value_t rtctime;
	uint8_t buffer[20];

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get Day parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.day = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Month parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" /", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.month = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Year parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" /", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.year = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Hour parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" :");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.hour = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Min parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" :", (uint8_t *)" :");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.min = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Sec parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" :", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.sec = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Week parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		rtctime.week = (uint8_t)atoi((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	_DEC2BCD(rtctime.day);
	_DEC2BCD(rtctime.month);
	_DEC2BCD(rtctime.year);
	_DEC2BCD(rtctime.hour);
	_DEC2BCD(rtctime.min);
	_DEC2BCD(rtctime.sec);

	CMD_SendString((uint8_t *)"Set RTC time...");

	R_RTC_Set_CalendarCounterValue(rtctime);
	status = MD_OK;

	if (status == MD_OK)
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}
	else
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
	}

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeGetEnergyLog(uint8_t *arg_str)
 * Description      : Command Invoke get energy log
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeGetEnergyLog(uint8_t *arg_str)
{
	EM_EEPROM_ENERGY_LOG l_debug_energy_log;
	EM_ENERGY_VALUE em_energy_value;

	(void)STORAGE_EM_GetEnergyData(&l_debug_energy_log);
	EM_EnergyDataToEnergyValue(&(l_debug_energy_log.energy_data), &em_energy_value);

	CMD_Printf((uint8_t *)"\n\rRTC Time: %02x/%02x/20%x %02x:%02x:%02x Week: %02x\n\r",
			l_debug_energy_log.time_stamp.day,
			l_debug_energy_log.time_stamp.month,
			l_debug_energy_log.time_stamp.year,
			l_debug_energy_log.time_stamp.hour,
			l_debug_energy_log.time_stamp.min,
			l_debug_energy_log.time_stamp.sec,
			l_debug_energy_log.time_stamp.week);

	CMD_Printf((uint8_t *)"Total Active Energy (Import): %.03f [kWh]\n\r",
			(em_energy_value.integer.active_imp        + em_energy_value.decimal.active_imp));
	CMD_Printf((uint8_t *)"Total Active Energy (Export): %.03f [kWh]\n\r",
			(em_energy_value.integer.active_exp        + em_energy_value.decimal.active_exp));
	CMD_Printf((uint8_t *)"Total Reactive Energy Lag (L - Import): %.03f [kVArh]\n\r",
			(em_energy_value.integer.reactive_ind_imp  + em_energy_value.decimal.reactive_ind_imp));
	CMD_Printf((uint8_t *)"Total Reactive Energy Lag (L - Export): %.03f [kVArh]\n\r",
			(em_energy_value.integer.reactive_ind_exp  + em_energy_value.decimal.reactive_ind_exp));
	CMD_Printf((uint8_t *)"Total Reactive Energy Lead (C - Import): %.03f [kVArh]\n\r",
			(em_energy_value.integer.reactive_cap_imp  + em_energy_value.decimal.reactive_cap_imp));
	CMD_Printf((uint8_t *)"Total Reactive Energy Lead (C - Export): %.03f [kVArh]\n\r",
			(em_energy_value.integer.reactive_cap_exp  + em_energy_value.decimal.reactive_cap_exp));
	CMD_Printf((uint8_t *)"Total Apparent Energy (Import): %.03f [kVAh]\n\r",
			(em_energy_value.integer.apparent_imp      + em_energy_value.decimal.apparent_imp));
	CMD_Printf((uint8_t *)"Total Apparent Energy (Export): %.03f [kVAh]\n\r",
			(em_energy_value.integer.apparent_exp      + em_energy_value.decimal.apparent_exp));

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeBackup(uint8_t *arg_str)
 * Description      : Command Invoke Backup
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeBackup(uint8_t *arg_str)
{
	uint8_t selection;
	uint8_t buffer[20];
	uint8_t b_found;

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get Selection parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* Get hex first */
		selection = (uint8_t)COMMAND_HexStringToUnsigned16(buffer, &b_found);

		/* next for get number */
		if (b_found == 0)
		{
			selection = (uint8_t)atoi((char * __near)buffer);
		}

		if (selection == CONFIG_ITEM_NONE)
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}

		/* Backup Item */
		CMD_SendString((uint8_t *)"Backup Item: ");
		if (selection & CONFIG_ITEM_CALIB)
		{
			CMD_SendString((uint8_t *)"Calibration ");
		}

		CMD_SendString((uint8_t *)"...");

		if (CONFIG_Backup(selection) != CONFIG_OK)
		{
			CMD_SendString((uint8_t *)"FAILED\n\r");
		}
		else
		{
			CMD_SendString((uint8_t *)"OK\n\r");
		}

		if (STORAGE_EM_Backup() != EM_STORAGE_OK)
		{
			CMD_SendString((uint8_t *)"FAILED\n\r");
		}
		else
		{
			CMD_SendString((uint8_t *)"OK\n\r");
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeRestore(uint8_t *arg_str)
 * Description      : Command Invoke Restore
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeRestore(uint8_t *arg_str)
{
	uint8_t selection;
	uint8_t buffer[20];
	uint8_t b_found;

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get Selection parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* Get hex first */
		selection = (uint8_t)COMMAND_HexStringToUnsigned16(buffer, &b_found);

		/* next for get number */
		if (b_found == 0)
		{
			selection = (uint8_t)atoi((char * __near)buffer);
		}

		if (selection == CONFIG_ITEM_NONE)
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}

		/* Backup Item */
		CMD_SendString((uint8_t *)"Backup Item: ");
		if (selection & CONFIG_ITEM_CALIB)
		{
			CMD_SendString((uint8_t *)"Calibration ");
		}

		CMD_SendString((uint8_t *)"...");

		EM_Stop();

		if (CONFIG_Restore(selection) != CONFIG_OK)
		{
			CMD_SendString((uint8_t *)"FAILED\n\r");
		}
		else
		{
			CMD_SendString((uint8_t *)"OK\n\r");
		}

		if (STORAGE_EM_Restore() != EM_STORAGE_OK)
		{
			CMD_SendString((uint8_t *)"FAILED\n\r");
		}
		else
		{
			CMD_SendString((uint8_t *)"OK\n\r");
		}

		EM_Start();
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str)
 * Description      : Command Invoke Read Memory
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str)
{
	static const uint16_t type_cast_size[7] =
	{
			1,  /* uint8_t */
			1,  /* int8_t */
			2,  /* uint16_t */
			2,  /* int16_t */
			4,  /* uint32_t */
			4,  /* int32_t */
			4,  /* float32_t */
	};

	uint32_t    addr;
	uint16_t    size, i, pagesize, count;
	uint8_t     b_found = 0;
	uint8_t     buffer[32];
	int8_t      cast_id = -1;
	uint16_t    u16;
	uint32_t    u32;
	float32_t   f32;
	uint8_t     is_dtfl;
	uint8_t const * p_mem_name;

	uint8_t (*FUNC_MemRead)(uint32_t addr, uint8_t *buf, uint16_t size);

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get memory type parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		is_dtfl = buffer[0] - '0';

		if (is_dtfl)
		{
			FUNC_MemRead = DATAFLASH_Read;
			p_mem_name = g_mem_dtfl;
		}
		else
		{
			FUNC_MemRead = EPR_Read;
			p_mem_name = g_mem_epr;
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Addr parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* Get hex first */
		addr = COMMAND_HexStringToUnsigned16(buffer, &b_found);

		/* next for get number */
		if (b_found == 0)
		{
			addr = COMMAND_DecStringToUnsigned16(buffer, &b_found);
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Size parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* Get hex first */
		size = COMMAND_HexStringToUnsigned16(buffer, &b_found);

		/* next for get number */
		if (b_found == 0)
		{
			size = atoi((char * __near)buffer);
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Cast parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL)
	{
		/* found argument */
		if (strcmp("uint8_t", (const char*)buffer) == 0)
		{
			cast_id = 0;    /* uint8_t */
		}
		else if (strcmp("int8_t", (const char*)buffer) == 0)
		{
			cast_id = 1;    /* int8_t */
		}
		else if (strcmp("uint16_t", (const char*)buffer) == 0)
		{
			cast_id = 2;    /* uint16_t */
		}
		else if (strcmp("int16_t", (const char*)buffer) == 0)
		{
			cast_id = 3;    /* int16_t */
		}
		else if (strcmp("uint32_t", (const char*)buffer) == 0)
		{
			cast_id = 4;    /* uint32_t */
		}
		else if (strcmp("int32_t", (const char*)buffer) == 0)
		{
			cast_id = 5;    /* int32_t */
		}
		else if (strcmp("float32_t", (const char*)buffer) == 0)
		{
			cast_id = 6;    /* float32_t */
		}
		else
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}
	}

	if (cast_id != -1)
	{
		CMD_Printf((uint8_t *)"\n\r--- %s Data (cast by %s) ---\n\r",p_mem_name, buffer);
	}
	else
	{
		CMD_Printf((uint8_t *)"\n\r--- %s Data (HEX, Max 1line = 32 bytes) ---\n\r", p_mem_name);
	}

	count = 0;
	while (size > 0)
	{
		if (size >= 32)
		{
			pagesize = 32;
		}
		else
		{
			pagesize = size;
		}

		if (FUNC_MemRead(addr, buffer, pagesize) == EPR_OK)
		{
			if (cast_id != -1)
			{
				for (i = 0; i < pagesize; i += type_cast_size[cast_id])
				{
					switch (cast_id)
					{
					case 0:/* uint8_t */
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%02x) %i\n\r", count, addr+i, (uint8_t)buffer[i], (uint8_t)buffer[i]);
						break;

					case 1:/* int8_t */
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%02x) %i\n\r", count, addr+i, (uint8_t)buffer[i], (int8_t)buffer[i]);
						break;

					case 2:/* uint16_t */
						u16 = COMMAND_GetUnsigned16(&buffer[i]);
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%04x) %d\n\r", count, addr+i, u16, u16);
						break;

					case 3:/* int16_t */
						u16 = COMMAND_GetUnsigned16(&buffer[i]);
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%04x) %d\n\r", count, addr+i, u16, (int16_t)u16);
						break;

					case 4:/* uint32_t */
						u32 = COMMAND_GetUnsigned32(&buffer[i]);
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %ld\n\r", count, addr+i, u32, u32);
						break;

					case 5:/* int32_t */
						u32 = COMMAND_GetUnsigned32(&buffer[i]);
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %ld\n\r", count, addr+i, u32, (int32_t)u32);
						break;

					case 6:/* float32_t */
						u32 = COMMAND_GetUnsigned32(&buffer[i]);
						f32 = *((float32_t *)&u32);
						CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %.06f\n\r", count, addr+i, u32, f32);
						break;

					default:
						CMD_SendString((uint8_t *)"Internal error.\n\r");
						return 2;
					}

					count++;
				}
			}
			else
			{
				for (i = 0; i < pagesize; i++)
				{
					CMD_Printf((uint8_t *)"%02x ", buffer[i]);
				}
				CMD_SendString((uint8_t *)"\n\r");
			}

			/* New page */
			addr += pagesize;
			size -= pagesize;

			/* Break if CTRL+C is pressed */
			if (CMD_IsCtrlKeyReceived())
			{
				CMD_Printf((uint8_t *)"\n\rCTRL + C is pressed! Break!\n\r");
				CMD_AckCtrlKeyReceived();

				/* Break printout here */
				break;
			}

			R_WDT_Restart();
		}
		else
		{
			CMD_Printf((uint8_t *)"\n\rRead %s failed at page [addr:0x%04x, size:0x%04x]!\n\r", p_mem_name, addr, pagesize);
			break;
		}
	}

	CMD_Printf((uint8_t *)"--- %s Data -----------------\n\r", p_mem_name);

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeWriteMemory(uint8_t *arg_str)
 * Description      : Command Invoke Write Memory
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeWriteMemory(uint8_t *arg_str)
{
	uint32_t    addr;
	uint16_t    size, value, pagesize;
	uint8_t     b_found = 0;
	uint8_t     b_string = 0;
	uint8_t *   p_str_old;
	uint8_t     buffer[32];
	uint8_t     is_dtfl;
	uint8_t const * p_mem_name;

	uint8_t (*FUNC_MemWrite)(uint32_t addr, uint8_t *buf, uint16_t size);

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get memory type parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		is_dtfl = buffer[0] - '0';

		if (is_dtfl)
		{
			FUNC_MemWrite = DATAFLASH_Write;
			p_mem_name = g_mem_dtfl;
		}
		else
		{
			FUNC_MemWrite = EPR_Write;
			p_mem_name = g_mem_epr;
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Addr parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* Get hex first */
		addr = COMMAND_HexStringToUnsigned16(buffer, &b_found);

		/* next for get number */
		if (b_found == 0)
		{
			addr = COMMAND_DecStringToUnsigned16(buffer, &b_found);
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get Size parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* Get hex first */
		size = COMMAND_HexStringToUnsigned16(buffer, &b_found);

		/* next for get number */
		if (b_found == 0)
		{
			size = atoi((char * __near)buffer);
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get string parameter */
	p_str_old = arg_str;
	arg_str   = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" (", (uint8_t *)")");

	/* is NOT have string parameter? */
	if (arg_str == NULL || *arg_str != ')')
	{
		/* scan number */
		arg_str = COMMAND_GetScanOneParam(buffer, 20, p_str_old, (uint8_t *)" ", (uint8_t *)" ");

		if (arg_str != NULL &&
				(buffer[0] >= '0' && buffer[0] <= '9'))
		{
			/* Get hex first */
			value = COMMAND_HexStringToUnsigned16(buffer, &b_found);

			/* next for get number */
			if (b_found == 0)
			{
				value = atoi((char * __near)buffer);
			}
		}
		else
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}
	}
	else
	{
		b_string = 1;
	}

	if (arg_str == NULL)
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	CMD_Printf((uint8_t *)"Start write to %s...", p_mem_name);

	while (size > 0)
	{
		if (size >= 32)
		{
			pagesize = 32;
		}
		else
		{
			pagesize = size;
		}

		if (b_string != 1)
		{
			memset(buffer, value, pagesize);
		}

		/* write buffer */
		if (FUNC_MemWrite(addr, buffer, pagesize) == EPR_OK)
		{
			/* New page */
			addr += pagesize;
			size -= pagesize;
		}
		else
		{
			CMD_Printf((uint8_t *)"\n\rWrite %s failed at page [addr:0x%04x, size:%i]!\n\r", p_mem_name, addr, pagesize);
			return 1;
		}

		R_WDT_Restart();
	}

	CMD_SendString((uint8_t *)"OK\n\r");

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeReadDTFLHeader(uint8_t *arg_str)
 * Description      : Command Invoke Read the DataFlash header byte
 * Arguments        : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeReadDTFLHeader(uint8_t *arg_str)
{
	uint8_t status0, status1;
	uint16_t crc0, crc1;
	CMD_SendString((uint8_t *)"\n\r");
	CMD_SendString((uint8_t *)"DataFlash Bank 0 Header Status Read...");
	if (DATAFLASH_ReadHeaderStatus(0, &status0))
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
		return 1;
	}
	else
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}

	CMD_SendString((uint8_t *)"DataFlash Bank 0 Header CRC Read...");
	if (DATAFLASH_ReadHeaderCRC(0, (uint8_t *)&crc0))
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
		return 1;
	}
	else
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}

	CMD_SendString((uint8_t *)"DataFlash Bank 1 Header Status Read...");
	if (DATAFLASH_ReadHeaderStatus(1, &status1))
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
		return 1;
	}
	else
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}

	CMD_SendString((uint8_t *)"DataFlash Bank 1 Header CRC Read...");
	if (DATAFLASH_ReadHeaderCRC(1, (uint8_t *)&crc1))
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
		return 1;
	}
	else
	{
		CMD_SendString((uint8_t *)"OK\n\r");
	}



	/* Reading OK, print result */
	CMD_Printf((uint8_t *)"\n\r Bank 0, status: %s, CRC: 0x%x", (status0 == 1) ? "valid" : "invalid", crc0);
	CMD_Printf((uint8_t *)"\n\r Bank 1, status: %s, CRC: 0x%x", (status1 == 1) ? "valid" : "invalid", crc1);

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeEraseDTFL(uint8_t *arg_str)
 * Description      : Command Invoke Erase DataFlash content
 * Arguments        : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeEraseDTFL(uint8_t *arg_str)
{
	CMD_SendString((uint8_t *)"\n\r");

	CMD_SendString((uint8_t *)"DataFlash Bank0 Clear...");
	if (DATAFLASH_BankErase(0) != DATAFLASH_OK)
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
		return 1;
	}
	else
	{
		/* OK */
		CMD_SendString((uint8_t *)"OK\n\r");
	}

	CMD_SendString((uint8_t *)"DataFlash Bank1 Clear...");
	if (DATAFLASH_BankErase(1) != DATAFLASH_OK)
	{
		CMD_SendString((uint8_t *)"FAILED\n\r");
		return 1;
	}
	else
	{
		/* OK */
		CMD_SendString((uint8_t *)"OK\n\r");
	}

	/* Restart the meter right after erase
	 * Purpose: the calibration information is cleared, will cause math error exception in CA
	 */
	CMD_SendString((uint8_t *)"Restart meter now...");
	COMMAND_ResetByIllegalMemory();

	return 0;
}

/***********************************************************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeFormatMemory(uint8_t *arg_str)
 * Description      : Command Invoke Format Memory
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ***********************************************************************************************************************/
static uint8_t COMMAND_InvokeFormatMemory(uint8_t *arg_str)
{
	uint8_t buffer[20];
	uint8_t is_dtfl;

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get memory type parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		is_dtfl = buffer[0] - '0';

	}

	if (is_dtfl)
	{
		CMD_SendString((uint8_t *)"\n\rFormatting Configuration Page...");

		R_WDT_Restart();

		if (CONFIG_Format() == CONFIG_OK)
		{
			CMD_SendString((uint8_t *)"OK\n\r");
		}
		else
		{
			CMD_SendString((uint8_t *)"FAILED\n\r");
		}

	}
	else
	{
		CMD_SendString((uint8_t *)"\n\rFormatting MeterData...");

		R_WDT_Restart();

		if (STORAGE_EM_Format() == EM_STORAGE_OK)
		{
			CMD_SendString((uint8_t *)"OK\n\r");
		}
		else
		{
			CMD_SendString((uint8_t *)"FAILED\n\r");
		}

	}
	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeDumpSample(uint8_t *arg_str)
 * Description      : Command Invoke Dump ADC samples
 * Arguments        : uint8_t index: Command index
 *                  : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeDumpSample(uint8_t *arg_str)
{
	uint16_t i;
	uint8_t buffer[20];

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get Day parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		g_sample1_direction = (uint8_t)atoi((char * __near)buffer);

		if (g_sample1_direction != 0 &&
				g_sample1_direction != 1)
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Send request to EM Core to dump the waveform */
	g_sample_count = 0;
	while(g_sample_count != g_sample_max_count)
	{
		NOP();
	}

	/* Print out */
	CMD_Printf(
			(uint8_t *)"Waveform of voltage & current %i, %i sample\n\r",
			g_sample1_direction + 1,
			g_sample_max_count
	);

	for (i = 0; i < g_sample_count; i++)
	{
		CMD_Printf((uint8_t *)"%ld, %ld\n\r", g_sample0[i], g_sample1[i]);
	}

	return 0;
}

/******************************************************************************
 * Function Name   : COMMAND_InvokeCalibration
 * Interface       : static void COMMAND_InvokeCalibration(uint8_t *arg_str)
 * Description     : Command Invoke Calibration
 * Arguments       : uint8_t * arg_str: Arguments string
 * Function Calls  : None
 * Return Value    : None
 ******************************************************************************/
static uint8_t COMMAND_InvokeCalibration(uint8_t *arg_str)
{
	/* Command parameters */
	float32_t   calib_imax, calib_v, calib_i;                     /* Voltage and Current for calibrating */
	uint16_t    calib_c, calib_cp;                          /* Number of line cycle for calibrating & phase error loop */
	uint8_t     w;                              /* Choose calibrated channel */
	uint8_t buffer[20];                         /* Data buffer to store input arguments */

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get c parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		calib_c = (uint16_t)atol((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get cp parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		calib_cp = (uint16_t)atol((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get imax parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		calib_imax = atof((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get v parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		calib_v = atof((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get i parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		calib_i = atof((char * __near)buffer);
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	/* Get w parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		w = (uint8_t)atoi((char * __near)buffer);
		if (w > 1)
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	CALIBRATION_Invoke(calib_c, calib_cp, calib_imax, calib_v, calib_i, w);

	l_calib = EM_GetCalibInfo();

	CMD_Printf((uint8_t *)"\n\r");
	CMD_Printf((uint8_t *)"Calibration info: \n\r");
	CMD_Printf((uint8_t *)"\n\r");
	CMD_Printf((uint8_t *)"FS: %.04f [Hz]\n\r" , l_calib.sampling_frequency);
	CMD_Printf((uint8_t *)"V_COEFF: %.04f\n\r" , l_calib.coeff.vrms);
	CMD_Printf((uint8_t *)"I1_COEFF: %.04f\n\r" , l_calib.coeff.i1rms);
	CMD_Printf((uint8_t *)"I2_COEFF: %.04f\n\r" , l_calib.coeff.i2rms);
	CMD_Printf((uint8_t *)"ACT1_COEFF: %.04f\n\r" , l_calib.coeff.active_power);
	CMD_Printf((uint8_t *)"ACT2_COEFF: %.04f\n\r" , l_calib.coeff.active_power2);
	CMD_Printf((uint8_t *)"REA1_COEFF: %.04f\n\r" , l_calib.coeff.reactive_power);
	CMD_Printf((uint8_t *)"REA2_COEFF: %.04f\n\r" , l_calib.coeff.reactive_power2);
	CMD_Printf((uint8_t *)"APP1_COEFF: %.04f\n\r" , l_calib.coeff.apparent_power);
	CMD_Printf((uint8_t *)"APP2_COEFF: %.04f\n\r" , l_calib.coeff.apparent_power2);
	CMD_Printf((uint8_t *)"ANGLE0_1: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i1_phase_degrees[0]);
	CMD_Printf((uint8_t *)"ANGLE0_2: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i2_phase_degrees[0]);
	CMD_Printf((uint8_t *)"ANGLE1_1: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i1_phase_degrees[1]);
	CMD_Printf((uint8_t *)"ANGLE1_2: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i2_phase_degrees[1]);
	CMD_Printf((uint8_t *)"GAIN0_1: %.04f\n\r" , l_calib.sw_gain.i1_gain_values[0]);
	CMD_Printf((uint8_t *)"GAIN0_2: %.04f\n\r" , l_calib.sw_gain.i2_gain_values[0]);
	CMD_Printf((uint8_t *)"GAIN1_1: %.04f\n\r" , l_calib.sw_gain.i1_gain_values[1]);
	CMD_Printf((uint8_t *)"GAIN1_2: %.04f\n\r" , l_calib.sw_gain.i2_gain_values[1]);
	CMD_Printf((uint8_t *)"Voltage Polarity: %ld\n\r" , VOLTAGE_INVERSION_FLAG);
	CMD_Printf((uint8_t *)"Phase Polarity: %ld\n\r" , PHASE_INVERSION_FLAG);
	CMD_Printf((uint8_t *)"Neutral Polarity: %ld\n\r" , NEUTRAL_INVERSION_FLAG);

	return 0;
}

static uint8_t COMMAND_InvokeGetCalibration(uint8_t *arg_str)
{
	(void)arg_str;

	l_calib = EM_GetCalibInfo();

	CMD_Printf((uint8_t *)"\n\r");
	CMD_Printf((uint8_t *)"Calibration info: \n\r");
	CMD_Printf((uint8_t *)"\n\r");
	CMD_Printf((uint8_t *)"FS: %.04f [Hz]\n\r" , l_calib.sampling_frequency);
	CMD_Printf((uint8_t *)"V_COEFF: %.04f\n\r" , l_calib.coeff.vrms);
	CMD_Printf((uint8_t *)"I1_COEFF: %.04f\n\r" , l_calib.coeff.i1rms);
	CMD_Printf((uint8_t *)"I2_COEFF: %.04f\n\r" , l_calib.coeff.i2rms);
	CMD_Printf((uint8_t *)"ACT1_COEFF: %.04f\n\r" , l_calib.coeff.active_power);
	CMD_Printf((uint8_t *)"ACT2_COEFF: %.04f\n\r" , l_calib.coeff.active_power2);
	CMD_Printf((uint8_t *)"REA1_COEFF: %.04f\n\r" , l_calib.coeff.reactive_power);
	CMD_Printf((uint8_t *)"REA2_COEFF: %.04f\n\r" , l_calib.coeff.reactive_power2);
	CMD_Printf((uint8_t *)"APP1_COEFF: %.04f\n\r" , l_calib.coeff.apparent_power);
	CMD_Printf((uint8_t *)"APP2_COEFF: %.04f\n\r" , l_calib.coeff.apparent_power2);
	CMD_Printf((uint8_t *)"ANGLE0_1: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i1_phase_degrees[0]);
	CMD_Printf((uint8_t *)"ANGLE0_2: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i2_phase_degrees[0]);
	CMD_Printf((uint8_t *)"ANGLE1_1: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i1_phase_degrees[1]);
	CMD_Printf((uint8_t *)"ANGLE1_2: %.04f [deg]\n\r" , l_calib.sw_phase_correction.i2_phase_degrees[1]);
	CMD_Printf((uint8_t *)"GAIN0_1: %.04f\n\r" , l_calib.sw_gain.i1_gain_values[0]);
	CMD_Printf((uint8_t *)"GAIN0_2: %.04f\n\r" , l_calib.sw_gain.i2_gain_values[0]);
	CMD_Printf((uint8_t *)"GAIN1_1: %.04f\n\r" , l_calib.sw_gain.i1_gain_values[1]);
	CMD_Printf((uint8_t *)"GAIN1_2: %.04f\n\r" , l_calib.sw_gain.i2_gain_values[1]);
	CMD_Printf((uint8_t *)"Voltage Polarity: %ld\n\r" , VOLTAGE_INVERSION_FLAG);
	CMD_Printf((uint8_t *)"Phase Polarity: %ld\n\r" , PHASE_INVERSION_FLAG);
	CMD_Printf((uint8_t *)"Neutral Polarity: %ld\n\r" , NEUTRAL_INVERSION_FLAG);

	return 0;
}

/******************************************************************************
 * Function Name    : static uint8_t COMMAND_InvokeSetConfig(uint8_t *arg_str)
 * Description      : Command Invoke Set configuration
 * Arguments        : uint8_t *arg_str: Arguments string
 * Functions Called : None
 * Return Value     : uint8_t, execution code, 0 is success
 ******************************************************************************/
static uint8_t COMMAND_InvokeSetConfig(uint8_t *arg_str)
{
	EM_SW_PROPERTY FAR_PTR * config;         /* Configuration */

	/* Ack the lask received CMD */
	CMD_AckCmdReceivedNoHeader();

	/* Set current configuration */
	config = EM_SW_GetProperty();

	/* Display current information */
	CMD_SendString((uint8_t *)"\n\r Current configuration info:");
	CMD_Printf((uint8_t *)"\n\r - Voltage Sag Fall Threshold (V): %.01f", config->sag_swell.sag_rms_fall_threshold);
	CMD_Printf((uint8_t *)"\n\r - Voltage Sag Rise Threshold (V): %.01f", config->sag_swell.sag_rms_rise_threshold);
	CMD_Printf((uint8_t *)"\n\r - Voltage Swell Fall Threshold (V): %.01f", config->sag_swell.swell_rms_fall_threshold);
	CMD_Printf((uint8_t *)"\n\r - Voltage Swell Rise Threshold (V): %.01f", config->sag_swell.swell_rms_rise_threshold);

	CMD_Printf((uint8_t *)"\n\r - Rounding Digits Power: %d", config->rounding.power);
	CMD_Printf((uint8_t *)"\n\r - Rounding Digits RMS: %d", config->rounding.rms);
	CMD_Printf((uint8_t *)"\n\r - Rounding Digits Frequency: %d", config->rounding.freq);
	CMD_Printf((uint8_t *)"\n\r - Rounding Digits Power Factor: %d", config->rounding.pf);

	CMD_Printf((uint8_t *)"\n\r - I-rms No Load Threshold (A): %.01f", config->operation.irms_noload_threshold);
	CMD_Printf((uint8_t *)"\n\r - Power No Load Threshold (W): %.01f", config->operation.power_noload_threshold);
	CMD_Printf((uint8_t *)"\n\r - No Voltage Threshold (V): %.01f", config->operation.no_voltage_threshold);
	CMD_Printf((uint8_t *)"\n\r - Minimum Frequency (Hz): %.01f", config->operation.freq_low_threshold);
	CMD_Printf((uint8_t *)"\n\r - Maximum Frequency (Hz): %.01f", config->operation.freq_high_threshold);
	CMD_Printf((uint8_t *)"\n\r - Earth Diff Threshold (%): %.04f", config->operation.earth_diff_threshold);
	CMD_Printf((uint8_t *)"\n\r - Meter Constant (imp/KWh): %lu", config->operation.meter_constant);
	CMD_Printf((uint8_t *)"\n\r - Pulse On Time (ms): %.01f", config->operation.pulse_on_time);

	CMD_SendString((uint8_t *)"\n\r");

	return 0;
}

/******************************************************************************
 * Function Name   : COMMAND_InvokeCPULoad
 * Interface       : static void COMMAND_InvokeCPULoad(uint8_t *arg_str)
 * Description     : Command Invoke Watch DSAD
 * Arguments       : uint8_t * arg_str: Arguments string
 * Function Calls  : None
 * Return Value    : None
 ******************************************************************************/
static uint8_t COMMAND_InvokeCPULoad(uint8_t *arg_str)
{
#ifdef METER_ENABLE_MEASURE_CPU_LOAD

	float max_dsad_time     = 0.0f;
	float min_dsad_time     = 0.0f;
	float avg_dsad_time     = 0.0f;
	EM_CALIBRATION calib    = EM_GetCalibInfo();
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

	LOADTEST_TAU_Init();

	g_timer0_dsad_sum_counter = 0;
	g_timer0_dsad_maxcounter = 0;
	g_timer0_dsad_mincounter = 0xFFFF;
	g_dsad_count = 0;
	g_timer0_diff = 0;

	CMD_Printf((uint8_t *)"\n\r");
	CMD_Printf((uint8_t *)"\n\rStarted measuring DSAD functions performance");
	CMD_Printf((uint8_t *)"\n\rCPU Speed: %u MHz", cpu_speed);
	CMD_Printf((uint8_t *)"\n\rPlease wait for about %0.3f seconds while system measuring",(float)g_dsad_max_count * 0.000256f);
	while(g_dsad_count < g_dsad_max_count)
	{
		R_WDT_Restart();
	}

	/*
        Substract cycles not related to EM processing (from Timer start to Timer stop
            No need now
	 */
	g_timer0_dsad_maxcounter -= (0);

	max_dsad_time   = ((float)g_timer0_dsad_maxcounter      / (float)(LOADTEST_CGC_GetClock())) / 1000.0f;
	min_dsad_time   = ((float)g_timer0_dsad_mincounter      / (float)(LOADTEST_CGC_GetClock())) / 1000.0f;
	avg_dsad_time   = ((float)g_timer0_dsad_sum_counter     / ((float)(LOADTEST_CGC_GetClock()) * g_dsad_max_count)) / 1000.0f;

	CMD_Printf((uint8_t *)"\n\r-------------------------------------------");

	CMD_Printf((uint8_t *)"\n\r% -35s %00.3fus",
			"Max CPU time spent in DSAD:",
			max_dsad_time * 1e6f
	);

	CMD_Printf((uint8_t *)"\n\r% -35s %00.3f%%",
			"Max CPU Load in percentage:",
			((max_dsad_time * calib.sampling_frequency)*100.0f)
	);

	CMD_Printf((uint8_t *)"\n\r% -35s %00.3fus",
			"Min CPU time spent in DSAD:",
			min_dsad_time * 1e6f
	);

	CMD_Printf((uint8_t *)"\n\r% -35s %00.3f%%",
			"Min CPU Load in percentage:",
			((min_dsad_time * calib.sampling_frequency)*100.0f)
	);

	CMD_Printf((uint8_t *)"\n\r% -35s %00.3fus",
			"Average CPU time spent in DSAD:",
			avg_dsad_time * 1e6f
	);

	CMD_Printf((uint8_t *)"\n\r% -35s %00.3f%%",
			"Average CPU Load in percentage:",
			((avg_dsad_time * calib.sampling_frequency)*100.0f)
	);

	CMD_Printf((uint8_t *)"\n\r-------------------------------------------");

	LOADTEST_TAU_DeInit();

#else
	CMD_Printf((uint8_t *)"\n\r No support. please turn on the macro METER_ENABLE_MEASURE_CPU_LOAD! \n\r ");
#endif


	return 0;
}

/******************************************************************************
 * Function Name   : COMMAND_InvokeVddCheck
 * Interface       : static void COMMAND_InvokeVddCheck(uint8_t *arg_str)
 * Description     : Command Invoke VDD check on LVD
 * Arguments       : uint8_t * arg_str: Arguments string
 * Function Calls  : None
 * Return Value    : None
 ******************************************************************************/
static uint8_t COMMAND_InvokeVddCheck(uint8_t *arg_str)
{
	CMD_Printf((uint8_t*)"\n\rVDD Status: ");
	CMD_Printf((uint8_t*)R_LVD_range_to_str(R_LVD_Check()));
	CMD_SendString((uint8_t *)"\n\r");

	return 0;
}

/******************************************************************************
 * Function Name   : COMMAND_InvokeInvertPolarity
 * Interface       : static void COMMAND_InvokeInvertPolarity(uint8_t *arg_str)
 * Description     : Command Invoke Inver Polarity
 * Arguments       : uint8_t * arg_str: Arguments string
 * Function Calls  : None
 * Return Value    : None
 ******************************************************************************/
static uint8_t COMMAND_InvokeInvertPolarity(uint8_t *arg_str)
{
	uint8_t channel_to_invert;
	uint8_t buffer[20];

	CMD_SendString((uint8_t *)"\n\rParameter(s): ");
	CMD_SendString((uint8_t *)arg_str);
	CMD_SendString((uint8_t *)"\n\r");

	/* Get Day parameter */
	arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");

	if (arg_str != NULL &&
			(buffer[0] >= '0' && buffer[0] <= '9'))
	{
		/* get number */
		channel_to_invert = (uint8_t)atoi((char * __near)buffer);

		if(0 == channel_to_invert)
		{
			CMD_SendString((uint8_t *)"Phase inverted!\n\r");
			PHASE_INVERSION_FLAG *= -1;
		}
		else if(1 == channel_to_invert)
		{
			CMD_SendString((uint8_t *)"Neutral inverted!\n\r");
			NEUTRAL_INVERSION_FLAG *= -1;
		}
		else
		{
			CMD_SendString((uint8_t *)"Parameter error\n\r");
			return 1;
		}
	}
	else
	{
		CMD_SendString((uint8_t *)"Parameter error\n\r");
		return 1;
	}

	return 0;
}


/******************************************************************************
 * Function Name : COMMAND_Init
 * Interface     : void COMMAND_Init(void)
 * Description   : Init the COMMAND application
 * Arguments     : None
 * Return Value  : None
 ******************************************************************************/
void COMMAND_Init(void)
{
	CMD_Init();
	CMD_RegisterSuggestion((void *)cmd_table, COMMAND_TABLE_LENGTH);
}

/***********************************************************************************************************************
 * Function Name    : void COMMAND_PollingProcessing(void)
 * Description      : COMMAND Polling Processing
 * Arguments        : None
 * Functions Called : None
 * Return Value     : None
 ***********************************************************************************************************************/
void COMMAND_PollingProcessing(void)
{
	uint8_t i, is_found_cmd;
	uint8_t *pstr, *p_strarg;
	uint8_t cmdstr[64];

	/* Processing command line */
	if (CMD_IsCmdReceived())
	{
		/* Get command */
		pstr = CMD_Get();
		memset(cmdstr, 0, 64);

		/* Skip all ahead space */
		while (pstr[0] == ' ' && pstr[0] != 0)
		{
			pstr++;
		}

		/* Get command name */
		i = 0;
		while (pstr[0] != ' ' && pstr[0] != 0)
		{
			cmdstr[i++] = *pstr;
			pstr++;
		}

		/* Skip all ahead space on argument list */
		while (pstr[0] == ' ' && pstr[0] != 0)
		{
			pstr++;
		}
		p_strarg = pstr;

		/* is end of string */
		if (p_strarg[0] == 0)
		{
			p_strarg = NULL;    /* No parameter */
		}

		/* Find to invoke the command on the cmd_table */
		if (cmdstr[0] != 0)
		{
			is_found_cmd = 0;       /* Not found */
			for (i = 0; i < COMMAND_TABLE_LENGTH; i++)
			{
				/* is matched? */
				if (strcmp((const char *)cmdstr, (const char *)cmd_table[i].cmd_name) == 0)
				{
					INVOKE_COMMAND(i, p_strarg);        /* Invoke to function */
					is_found_cmd = 1;                   /* Found command */
				}
			}

			/* is NOT found command? */
			if (!is_found_cmd)
			{
				CMD_SendString((uint8_t *)"\n\rUnknown command: [");
				CMD_SendString(CMD_Get());
				CMD_SendString((uint8_t *)"]\n\rType ? for command list\n\r");
			}
		}

		CMD_AckCmdReceived();
	}
}


#endif /* __DEBUG */
