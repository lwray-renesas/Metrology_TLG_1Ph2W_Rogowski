/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : startup.h
* Version      : 1.00
* Description  : Start-up Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _START_UP
#define _START_UP

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct st_em_startup_diag {
    uint8_t em_init_status;             /* EM_Init status */
    uint8_t config_load_status;         /* Config storage reload from dataflash status (other than 0 means error) */
    uint8_t energy_load_status;         /* Energy storage reload from EEPROM status (other than 0 means error) */
    uint8_t em_start_status;            /* EM_Start status */
} st_em_startup_diag_t;
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/
extern uint8_t g_reset_flag;
extern st_em_startup_diag_t g_em_startup_diag;

/******************************************************************************
Functions Prototypes
******************************************************************************/
uint8_t startup(void);

#endif /* _START_UP */

