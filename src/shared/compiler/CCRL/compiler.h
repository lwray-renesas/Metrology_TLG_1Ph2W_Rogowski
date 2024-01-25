/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : compiler.h
* Version      : 1.00
* Description  : Compiler specific declaration for CA78K0 compiler
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _COMPILER_H
#define _COMPILER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/* MCU IO & Register Define */
#include "iodefine.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/* MCU Supported functions */
#define DI      __DI
#define EI      __EI
#define HALT    __halt
#define NOP     __nop
#define STOP    __stop
#define BRK     __brk

/* Bit accessors */
#define SET_BIT(reg, bit_no, bit_value)                 (reg##_bit##.##no##bit_no = bit_value)
#define BIT_SELECT(reg, bit_no)                         (reg##_bit##.##no##bit_no)


/* Near & Far specifier */
#define NEAR_PTR    __near
#define FAR_PTR     __far
#define NEAR_FUNC   __near
#define FAR_FUNC    __far

/* Interuppt specifier */
#define __INTERRUPT

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif /* _COMPILER_H */
