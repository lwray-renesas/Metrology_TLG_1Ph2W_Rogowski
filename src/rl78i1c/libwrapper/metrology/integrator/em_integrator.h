/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : em_integrator.h
* Version      : 1.00
* Description  : Integrator Wrapper
*              : Implement the integrators (0 and 1) for 2 separated channels
*              : Using Runge-Kutta methods
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EM_INTEGRATOR_H
#define _EM_INTEGRATOR_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_cg_macrodriver.h"
#include "typedef.h"        /* GSCE Standard Typedef */

/******************************************************************************
Typedef definitions
******************************************************************************/
/* EM Config */
typedef struct tagEMADCIntegratorConfig
{
    uint8_t    mask_input          :1;     /* Enable/Disable Mask 8 bits (LSB) of input */
    uint8_t    hpf_input_enable    :1;     /* Enable/Disable HPF for input */
    uint8_t    integrator_enable   :1;     /* Enable/Disable Integrator */
    uint8_t    hpf_output_enable   :1;     /* Enable/Disable HPF for output */
    uint8_t    scale_down_output   :1;     /* Enable/Disable scale-down the output */
    uint8_t    reserved            :3;     /* Reserved */
} R_EM_INTEGRATOR_CONFIG;
/******************************************************************************
Macro definitions
******************************************************************************/

#define INTEGRATOR_MAX_CHANNEL      4
#define PH1_2W	1
#define PH2_3W	0
#define PH3_4W	0

/* Integrator 0 */
#define EM_ADC_ResetIntegrator0()       R_EM_Integrator_Reset(0)
#define EM_ADC_Integrator0(input)       R_EM_Integrator_DoIntegrate(input,0)

/* Integrator 1 */
#define EM_ADC_ResetIntegrator1()       R_EM_Integrator_Reset(1)
#define EM_ADC_Integrator1(input)       R_EM_Integrator_DoIntegrate(input,1)

/* Integrator 2 */
#define EM_ADC_ResetIntegrator2()       R_EM_Integrator_Reset(2)
#define EM_ADC_Integrator2(input)       R_EM_Integrator_DoIntegrate(input,2)

/* Integrator 2 */
#define EM_ADC_ResetIntegrator3()       R_EM_Integrator_Reset(2)
#define EM_ADC_Integrator3(input)       R_EM_Integrator_DoIntegrate(input,3)

/******************************************************************************
Variable Externs
******************************************************************************/
extern const char r_integrator_version[];

/******************************************************************************
Functions Prototypes
******************************************************************************/
void R_EM_Integrator_Reset(int16_t channel);

/******************************************************************************
* Function Name    : static int32_t R_EM_Integrator_DoIntegrate(
*                  :     integrator_data_t    *integrator,
*                  :     int32_t             input
*                  : );
* Description      : Processing of integration (common for all channels)
* Arguments        : integrator_data_t    *integrator: Pointer to integrator data
*                  : int32_t             input      : Input signal
* Functions Called : TBD
* Return Value     : Integated signal of selected integrator data
******************************************************************************/
int32_t R_EM_Integrator_DoIntegrate(
        int32_t input, 
        int16_t channel
);

#endif /* _EM_INTEGRATOR_H */


