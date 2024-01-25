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
* File Name    : r_cg_dsadc.c
* Version      : Applilet4 for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NLG
* Tool-Chain   : CCRL
* Description  : This file implements device driver for DSADC module.
* Creation Date: 18/10/2023
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_dsadc.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_em_sw_config.h"
#include "wrp_app_mcu.h"
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
static uint16_t g_reg_phase0;
static uint16_t g_reg_phase1;
static uint16_t g_reg_phase2;
static uint16_t g_reg_phase3;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_DSADC_Create
* Description  : This function initializes the DSAD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Create(void)
{
    DSADCK = 0U;
    DSADRES = 1U;   /* reset DSAD converter */
    DSADRES = 0U;   /* reset release of DSAD converter */
    DSADCEN = 1U;   /* enables input clock supply */
    DSAMK = 1U;     /* disable INTDSAD interrupt */
    DSAIF = 0U;     /* clear INTDSAD interrupt flag */
    DSAZMK0 = 1U;   /* disable INTDSADZC0 interrupt */
    DSAZIF0 = 0U;   /* clear INTDSADZC0 interrupt flag */
    DSAZMK1 = 1U;   /* disable INTDSADZC1 interrupt */
    DSAZIF1 = 0U;   /* clear INTDSADZC1 interrupt flag */
    /* Set INTDSAD high priority */
    DSAPR1 = 0U;
    DSAPR0 = 0U;
    DSADMR = _0000_DSAD_SAMPLING_FREQUENCY_0 | _0000_DSAD_RESOLUTION_24BIT;
    DSADGCR0 = _00_DSAD_CH1_PGAGAIN_1 | _00_DSAD_CH0_PGAGAIN_1;
    DSADGCR1 = _00_DSAD_CH3_PGAGAIN_1;
    DSADHPFCR = _80_DSAD_CUTOFF_FREQUENCY_2 | _00_DSAD_CH3_HIGHPASS_FILTER_ENABLE | 
                _00_DSAD_CH1_HIGHPASS_FILTER_ENABLE | _00_DSAD_CH0_HIGHPASS_FILTER_ENABLE;
    DSADPHCR0 = _0000_DSAD_PHCR0_VALUE;
    DSADPHCR1 = _0000_DSAD_PHCR1_VALUE;
    DSADPHCR3 = _0000_DSAD_PHCR3_VALUE;
}
/***********************************************************************************************************************
* Function Name: R_DSADC_Start
* Description  : This function starts the DSAD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Start(void)
{
    DSAIF = 0U;     /* clear INTDSAD interrupt flag */
    DSAMK = 0U;     /* enable INTDSAD interrupt */
    DSADMR |= _0008_DSAD_CH3_OPERATION | _0002_DSAD_CH1_OPERATION | _0001_DSAD_CH0_OPERATION;
}
/***********************************************************************************************************************
* Function Name: R_DSADC_Stop
* Description  : This function stops the DSAD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Stop(void)
{
    DSADMR &= (uint16_t)~(_0008_DSAD_CH3_OPERATION | _0002_DSAD_CH1_OPERATION | _0001_DSAD_CH0_OPERATION);
    DSAMK = 1U;     /* disable INTDSAD interrupt */
    DSAIF = 0U;     /* clear INTDSAD interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_DSADC_Set_OperationOn
* Description  : This function power-on control.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Set_OperationOn(void)
{
    DSADMR |= _0800_DSAD_CH3_POWER_ON | _0200_DSAD_CH1_POWER_ON | _0100_DSAD_CH0_POWER_ON;
}
/***********************************************************************************************************************
* Function Name: R_DSADC_Set_OperationOff
* Description  : This function power-down control.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Set_OperationOff(void)
{
    DSADMR &= (uint16_t)~(_0800_DSAD_CH3_POWER_ON | _0200_DSAD_CH1_POWER_ON | _0100_DSAD_CH0_POWER_ON);
}
/***********************************************************************************************************************
* Function Name: R_DSADC_Channel0_Get_Result
* Description  : This function returns the conversion result in the buffer.
* Arguments    : buffer -
*                    the address where to write the conversion result
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Channel0_Get_Result(uint32_t * const buffer)
{
    *buffer = DSADCR0H;
    *buffer = (uint32_t)((*buffer << 16U) + DSADCR0);
}
/***********************************************************************************************************************
* Function Name: R_DSADC_Channel1_Get_Result
* Description  : This function returns the conversion result in the buffer.
* Arguments    : buffer -
*                    the address where to write the conversion result
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_Channel1_Get_Result(uint32_t * const buffer)
{
    *buffer = DSADCR1H;
    *buffer = (uint32_t)((*buffer << 16U) + DSADCR1);
}

/* Start user code for adding. Do not edit comment generated here */


static void R_DSADC_WaitInternalSetupTime(void)
{
    uint8_t wait;

    /* Wait for setup time */
    wait = _50_DSAD_WAIT_SETUP_TIME;
    while (1)
    {
        if (DSAIF == 1)
        {
            DSAIF = 0U;  /* Clear INTSAD interrupt flag */
            wait--;
        }
        if (wait == 0)
        {
            break;
        }
    }
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StartWithoutGainAndPhase
* Description  : This function starts 4 ADC channels without gain and phase setting
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartWithoutGainAndPhase(uint16_t channel_control)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH; /* Stop conversion */
    MCU_Delay(2);

    /* Gain setting */
    DSADGCR0 = 0x00;
    DSADGCR1 = 0x00;

    /* Phase setting */
    DSADPHCR0 = 0x0000;
    DSADPHCR1 = 0x0000;
    DSADPHCR2 = 0x0000;
    DSADPHCR3 = 0x0000;

    /* High pass filter */
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL2);      /* Turn on HPF on channel 2 */
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL1);      /* Turn on HPF on channel 1 */
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL0);      /* Turn on HPF on channel 0 */
    R_DSADC_Set_CutOffFrequency(HPF_2429_HZ);       /* Set cut off frequency for HPF */

    DSADMR |= channel_control;      /* Start conversion ADC channel 0-3 */

    R_DSADC_WaitInternalSetupTime();

    DSAMK = 0U;  /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StartChannel0Only
* Description  : This function starts the AD converter (CH0 only).
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartChannel0Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADPHCR0 = 0x0000;     /* Reset phase adjustment for channel 0 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    MCU_Delay(2);
    DSADMR |= _0101_DSAD_ENABLE_CH0;        /* Start conversion ADC channel 0 only */

    R_DSADC_WaitInternalSetupTime();
    DSAMK = 0U;  /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StartChannel1Only
* Description  : This function starts the AD converter (CH1 only).
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartChannel1Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADPHCR1 = 0x0000;     /* Reset phase adjustment for channel 0 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    MCU_Delay(2);
    DSADMR |= _0202_DSAD_ENABLE_CH1;        /* Start conversion ADC channel 0 only */

    R_DSADC_WaitInternalSetupTime();
    DSAMK = 0U;  /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StartChannel2Only
* Description  : This function starts the AD converter (CH2 only).
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartChannel2Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADPHCR1 = 0x0000;                     /* Reset phase adjustment for channel 2 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    MCU_Delay(2);
    DSADMR |= _0404_DSAD_ENABLE_CH2;        /* Start conversion ADC channel 2 only */
    R_DSADC_WaitInternalSetupTime();
    DSAMK = 0U;  /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_ADC_StartChannel3Only
* Description  : This function starts the AD converter (CH3 only).
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartChannel3Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADPHCR3 = 0x0000;                     /* Reset phase adjustment for channel 2 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    MCU_Delay(2);
    DSADMR |= _0808_DSAD_ENABLE_CH3;        /* Start conversion ADC channel 2 only */
    R_DSADC_WaitInternalSetupTime();
    DSAMK = 0U;  /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StartAllChannels
* Description  : This function starts the AD converter (all channel).
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartAllChannels(void)
{
    /* Get channel DSADMR setting based on user channel setting */
    DSAMK = 1U;                                                     /* Disable INTAD interrupt */
    DSAIF = 0U;                                                     /* Clear INTSAD interrupt flag */

    DSADMR = 0x0707;
    R_DSADC_WaitInternalSetupTime();
    DSAMK = 0U;                                                     /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StartCurrentChannels
* Description  : This function starts the AD converter (2 current channel, CH0 & CH2).
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StartCurrentChannels(void)
{
    /* Get channel DSADMR setting based on user channel setting */
    uint16_t phase_setting = 0x0101;
    uint16_t neutral_setting = 0x0101;
    dsad_reg_setting_t dsad_reg;

    R_DSADC_GetGainAndPhase(&dsad_reg);
    R_DSADC_Create();
    R_DSADC_SetGainAndPhase(dsad_reg);
    /* Turn off High pass filter for all channels */
    DSADHPFCR = 0xCF;

    DSAMK = 1U;                                                     /* Disable INTAD interrupt */
    DSAIF = 0U;                                                     /* Clear INTSAD interrupt flag */

    /* Reset phase shift */
    (*((uint16_t *)&DSADPHCR0 + EM_ADC_DRIVER_CHANNEL_PHASE)) = 0x000;
    (*((uint16_t *)&DSADPHCR0 + EM_ADC_DRIVER_CHANNEL_NEUTRAL)) = 0x000;

    /* Turn on current channels */
    phase_setting <<= EM_ADC_DRIVER_CHANNEL_PHASE;                  /* Shift based on phase channel setting */
    neutral_setting <<=  EM_ADC_DRIVER_CHANNEL_NEUTRAL;             /* Shift based on phase channel setting */

    DSADMR &= ~(0x0F0F - (phase_setting | neutral_setting));        /* others off */
    MCU_Delay(2);
    DSADMR |= (phase_setting | neutral_setting);

    R_DSADC_WaitInternalSetupTime();

    DSAMK = 0U;                                                     /* Enable INTSAD interrupt */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_StopChannel
* Description  : This function stop specific channel
* Caution      : Only call this API on NM mode
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_StopChannel(uint8_t channel_number)
{
    uint16_t mask = 0x0101;
    mask = ~(mask << (channel_number));
    DSADMR &= mask;
    /* This API will not turn turn off DSAD interrupt
     * To turn off DSAD interrupt, use R_DSADC_Stop()
    */
}

/***********************************************************************************************************************
* Function Name: R_DSADC_BackupPhaseRegSetting
* Description  : This function backup register setting of DSADPHCR0 and DSADPHCR1
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_BackupPhaseRegSetting(void)
{
    g_reg_phase0 = DSADPHCR0;
    g_reg_phase1 = DSADPHCR1;
    g_reg_phase2 = DSADPHCR2;
    g_reg_phase3 = DSADPHCR3;
}

/***********************************************************************************************************************
* Function Name: R_DSADC_RestorePhaseRegSetting
* Description  : This function restore register setting of DSADPHCR0 and DSADPHCR1
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DSADC_RestorePhaseRegSetting(void)
{
    DSADPHCR0 = g_reg_phase0;
    DSADPHCR1 = g_reg_phase1;
    DSADPHCR2 = g_reg_phase2;
    DSADPHCR3 = g_reg_phase3;
}

/***********************************************************************************************************************
* Function Name: R_DSADC_SetHighPassFilterOff
* Description  : This function is used to turn off HPF function for specified channel
* Arguments    : channel -
*                    the channel use HPF
* Return Value : MD_OK
*                    set HPF successfully
*                MD_ARGERROR
                     Input argument is invalid
***********************************************************************************************************************/
MD_STATUS R_DSADC_SetHighPassFilterOff(dsad_channel_t channel)
{
    MD_STATUS status = MD_OK;
    switch(channel)
    {
        case DSADCHANNEL0:
            DSADHPFCR |= _01_DSAD_CH0_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL1:
            DSADHPFCR |= _02_DSAD_CH1_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL2:
            DSADHPFCR |= _04_DSAD_CH2_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL3:
            DSADHPFCR |= _08_DSAD_CH3_HIGHPASS_FILTER_DISABLE;
            break;
        default:
            /* Channel is invalid */
            status = MD_ARGERROR;
            break;
    }
    return status;
}

/***********************************************************************************************************************
* Function Name: R_DSADC_SetHighPassFilterOn
* Description  : This function is used to turn on HPF function for specified channel
* Arguments    : channel -
*                    the channel to turn on HPF
* Return Value : MD_OK
*                    turn on HPF successfully
*                MD_ARGERROR
                     Input argument is invalid
***********************************************************************************************************************/
MD_STATUS R_DSADC_SetHighPassFilterOn(dsad_channel_t channel)
{
    MD_STATUS status = MD_OK;
    switch(channel)
    {
        case DSADCHANNEL0:
            DSADHPFCR &= (uint8_t)~_01_DSAD_CH0_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL1:
            DSADHPFCR &= (uint8_t)~_02_DSAD_CH1_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL2:
            DSADHPFCR &= (uint8_t)~_04_DSAD_CH2_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL3:
            DSADHPFCR &= (uint8_t)~_08_DSAD_CH3_HIGHPASS_FILTER_DISABLE;
            break;
        default:
            /* Channel is invalid */
            status = MD_ARGERROR;
            break;
    }
    return status;
}

/***********************************************************************************************************************
* Function Name: R_DSADC_Set_CutOffFrequency
* Description  : This function is used to set the cut off frequency of HPF
* Arguments    : frequency -
*                    the cut off frequency to be set for HPF
* Return Value : MD_OK
*                    set cut off frequency successfully
*                MD_ARGERROR
                     Input argument is invalid
***********************************************************************************************************************/
MD_STATUS R_DSADC_Set_CutOffFrequency(dsad_cutoff_t frequency)
{
    MD_STATUS status = MD_OK;

    if(HPF_0067HZ > frequency || frequency > HPF_4857_HZ)
    {
        status = MD_ARGERROR;
    }
    else
    {
        DSADHPFCR &= 0x0F;
        DSADHPFCR |= (uint8_t)(frequency << 6);
    }
    return status;
}

/***********************************************************************************************************************
* Function Name: R_DSADC_AdjustPhaseDelay
* Description  : This function is used to set the cut off frequency of HPF
* Arguments    : channel -
*                    input channel to be selected
* Return Value : MD_OK
*                    phase adjust successfully
*                MD_ARGERROR
                     Input argument is invalid
***********************************************************************************************************************/
MD_STATUS R_DSADC_AdjustPhaseDelay(dsad_channel_t channel, uint16_t step)
{
    MD_STATUS status = MD_OK;

    if (step <= 1151)
    {   /* Step must be from 0 to 1151 */
        switch(channel)
        {
            case DSADCHANNEL0:
                DSADPHCR0 = step;    /* Set new step */
                break;
            case DSADCHANNEL1:
                DSADPHCR1 = step;    /* Set new step */
                break;
            case DSADCHANNEL2:
                DSADPHCR2 = step;    /* Set new step */
                break;
            case DSADCHANNEL3:
                DSADPHCR3 = step;    /* Set new step */
                break;
            default:
                /* Channel is invalid */
                status = MD_ARGERROR;
                break;
        }
    }
    else
    {
        /* Step is invalid */
        status = MD_ARGERROR;
    }

    return status;
}

/***********************************************************************************************************************
* Function Name:
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
void R_DSADC_GetGainAndPhase(dsad_reg_setting_t *adc_gain_phase)
{
    /* Get curretn gain setting */
    adc_gain_phase->gain0 = DSADGCR0;
    adc_gain_phase->gain1 = DSADGCR1;
    /* Get phase angle setting */
    adc_gain_phase->phase0 = DSADPHCR0;
    adc_gain_phase->phase1 = DSADPHCR1;
    adc_gain_phase->phase2 = DSADPHCR2;
    adc_gain_phase->phase3 = DSADPHCR3;
}

/***********************************************************************************************************************
* Function Name:
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
void R_DSADC_SetGainAndPhase(dsad_reg_setting_t adc_gain_phase)
{
    DSADGCR0    = adc_gain_phase.gain0;
    DSADGCR1    = adc_gain_phase.gain1;
    DSADPHCR0   = adc_gain_phase.phase0;
    DSADPHCR1   = adc_gain_phase.phase1;
    DSADPHCR2   = adc_gain_phase.phase2;
    DSADPHCR3   = adc_gain_phase.phase3;
}

/***********************************************************************************************************************
* Function Name:
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
void R_DSADC_SetGain(dsad_reg_setting_t adc_gain_phase)
{
    DSADGCR0    = adc_gain_phase.gain0;
    DSADGCR1    = adc_gain_phase.gain1;
}

/***********************************************************************************************************************
* Function Name:
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
MD_STATUS R_DSADC_SetChannelGain(dsad_channel_t channel, dsad_gain_t gain)
{
    MD_STATUS   status = MD_OK;

    /* Check input gain */
    if (gain < GAIN_X1 || gain > GAIN_X32)
    {
        status = MD_ARGERROR;
    }
    else
    {
        /* Set the gain */
        switch(channel)
        {
            case DSADCHANNEL0:  /* Used as Current channel */
                DSADGCR0 &= 0xF0;
                DSADGCR0 |= gain;
                break;
            case DSADCHANNEL1:  /* Used as Voltage channel */
                DSADGCR0 &= 0x0F;
                DSADGCR0 |= (uint8_t)(gain << 4);
                break;
            case DSADCHANNEL2:  /* Used as Current channel */
                DSADGCR1 &= 0xF0;
                DSADGCR1 |= gain;
                break;
            case DSADCHANNEL3:  /* Used as Current channel */
                DSADGCR1 &= 0x0F;
                DSADGCR1 |= (uint8_t)(gain << 4);
                break;
            default:
                /* Channel is invalid */
                status = MD_ARGERROR;
                break;
        }
    }

    return status;
}
/******************************************************************************
* Function Name    : dsad_gain_t R_DSADC_GetGainEnumValue(uint8_t gain)
* Description      : Get the enumuration gain value of ADC by a gain value
* Arguments        : None
* Functions Called : None
* Return Value     : dsad_gain_t, enum gain value
******************************************************************************/
dsad_gain_t R_DSADC_GetGainEnumValue(uint8_t gain)
{
    dsad_gain_t     gain_value;

    if (gain == 1)
    {
        gain_value = GAIN_X1;
    }
    else if (gain == 2)
    {
        gain_value = GAIN_X2;
    }
    else if (gain == 4)
    {
        gain_value = GAIN_X4;
    }
    else if (gain == 8)
    {
        gain_value = GAIN_X8;
    }
    else if (gain == 16)
    {
        gain_value = GAIN_X16;
    }
    else
    {
        gain_value = GAIN_X32;
    }

    return gain_value;
}

/******************************************************************************
* Function Name    : uint8_t R_DSADC_GetChannelGain(dsad_channel_t channel)
* Description      : Get the gain value of channel
* Arguments        : dsad_channel_t channel: specific dsad channel
* Functions Called : None
* Return Value     : uint8_t: channel gain
******************************************************************************/
uint8_t R_DSADC_GetChannelGain(dsad_channel_t channel)
{
    uint8_t gain;
    switch(channel)
    {
        case DSADCHANNEL0:
            gain = (uint8_t)(1 << (DSADGCR0 & 0x0F));
            break;
        case DSADCHANNEL1:
            gain = (uint8_t)(1 << ((DSADGCR0 & 0xF0) >> 4));
            break;
        case DSADCHANNEL2:
            gain = (uint8_t)(1 << (DSADGCR1 & 0x0F));
            break;
        case DSADCHANNEL3:
            gain = (uint8_t)(1 << ((DSADGCR1 & 0xF0) >> 4));
            break;
        default:
            gain = 0;
            break;
    }

    return gain;
}

/******************************************************************************
* Function Name    : uint16_t R_DSADC_GetChannelPhase(dsad_channel_t channel)
* Description      : Get the gain value of channel
* Arguments        : dsad_channel_t channel: specific dsad channel
* Functions Called : None
* Return Value     : uint16_t: channel phase setting
******************************************************************************/
uint16_t R_DSADC_GetChannelPhase(dsad_channel_t channel)
{
    uint16_t phase_setting;

    if ( channel <= DSADCHANNEL3 )
    {
        phase_setting = (uint16_t)*(&DSADPHCR0 + channel);
    }

    return phase_setting;
}

/******************************************************************************
* Function Name    : void R_DSADC_ClearGainSetting(dsad_reg_setting_t * p_setting, dsad_channel_t channel)
* Description      : Clear gain setting in setting to 0
* Arguments        : dsad_reg_setting_t * p_setting: setting register
*                  : dsad_channel_t channel: specific dsad channel
* Functions Called : None
* Return Value     : None
******************************************************************************/
void R_DSADC_ClearGainSetting(dsad_reg_setting_t * p_setting, dsad_channel_t channel)
{
    if (p_setting != NULL)
    {
        switch (channel)
        {
            case DSADCHANNEL0:
                p_setting->gain0 = p_setting->gain0 & 0xF0;
                break;
            case DSADCHANNEL1:
                p_setting->gain0 = p_setting->gain0 & 0x0F;
                break;
            case DSADCHANNEL2:
                p_setting->gain1 = p_setting->gain1 & 0xF0;
                break;
            case DSADCHANNEL3:
                p_setting->gain1 = p_setting->gain1 & 0x0F;
                break;
            default:
                /* Keep the same */
                break;
        }
    }
}
/* End user code. Do not edit comment generated here */
