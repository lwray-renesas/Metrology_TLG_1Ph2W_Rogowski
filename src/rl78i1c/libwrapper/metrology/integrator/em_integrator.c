/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.  */
/******************************************************************************	
* File Name    : em_integrator.c
* Version      : 1.01
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Integrator Wrapper for RL78/I1C Platform
*              : 
*              : Using Rungle-Kutta method.
*              : Also apply high pass filter for input, output
*              :
*              : High Pass Filter: 15Hz cut-off frequency
*              : Using RC circuit : fc = 1 / (2 * PI * RC)
*              : alpha = RC / (RC + dt)
*              : Alpha is defined by 63992/65536
*              : alpha = 0.976440987
*              :
*              : Signal flow:
*              : Rogoski Coil -> [HPF 0] -> Integration RK4 -> [HPF 1]
******************************************************************************
* History : DD.MM.YYYY Version Description
*           09.02.2017 using to RK4
*           20.02.2017 using 32-bit multiply-accumulator for calculation
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "em_integrator.h"

/* Application */
/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    /* High pass filter */
    int32_t hpf_input_old_signal;
    int32_t hpf_input_result;
    int32_t hpf_output_old_signal;
    int32_t hpf_output_result;
    
    /* Integrator */
    int32_t old_signal1;
    int32_t old_signal2;
    int32_t old_signal3;
    
    int32_t result;
    
} integrator_data_t;

/******************************************************************************
Macro definitions
******************************************************************************/
#define RK_16BIT                    16
#define RK_24BIT                    24
#define INTEGRATOR_RK_BIT_BASED     RK_24BIT
/* Input */
#define INPUT_MASK_USED             0             /* Enable/Disable mask low 8bits input */

/* Integrator */
#define RK4                         4
#define RK2                         2

#define INTEGRATION_RK_USED         RK4           /* Enable rungle-kutta or just adding deriviation value */
#define INTEGRATION_RK_DIV_6        10923         /* ..... 10922/65536 ~ 1/6 ..... */
/* High pass filter */
#define HPF0_USED                   0             /* Enable/Disable HPF 0 */
#define HPF1_USED                   1             /* Enable/Disable HPF 1 */

/* High pass filter */
//#define HPF_COFF                  (0.97644f)      /* 63992/2^16, high-pass filter coeff */
#define HPF_COEF_NUM                63493         /* 63493/2^16 ~ 20Hz HPF cutoff Freq  */
#define HPF_COEF_NUM_NEGATIVE       (0x07FB)        /* 0xFFFF07FB = -63493 */
/* Output */
#define SCALE_DOWN_OUTPUT_USED      0             /* Enable/Disable Scale Down Output  */
#define SCALE_DOWN_OUTPUT           4096          /* Scale down output (4096/65536 ~ 1/16) */
/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern const R_EM_INTEGRATOR_CONFIG r_em_integrator_config[INTEGRATOR_MAX_CHANNEL];


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Variable for integrator[n] */
static integrator_data_t g_integrator[INTEGRATOR_MAX_CHANNEL];
static integrator_data_t * integrator;
static const R_EM_INTEGRATOR_CONFIG * integrator_config;

/******************************************************************************
* Function Name    : static void EM_ADC_ResetIntegrator0(void)
* Description      : Reset data storage for integrator0
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void R_EM_Integrator_Reset(int16_t channel)
{
    if(channel > INTEGRATOR_MAX_CHANNEL)
    {
        return;
    }
    g_integrator[channel].hpf_input_old_signal = 0;
    g_integrator[channel].hpf_input_result = 0;
    g_integrator[channel].hpf_output_old_signal = 0;
    g_integrator[channel].hpf_output_result = 0;
    g_integrator[channel].old_signal1 = 0;
    g_integrator[channel].old_signal2 = 0;
    g_integrator[channel].old_signal3 = 0;    
    g_integrator[channel].result = 0;
}



/******************************************************************************
* Function Name    : static int32_t EM_ADC_Integrate(
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
)
{
    /* init */
    if(channel > INTEGRATOR_MAX_CHANNEL)
        {
        return input;
    }
    integrator_config = &(r_em_integrator_config[channel]);
    integrator        = &(g_integrator[channel]);
        
    /* Mask 4 low bits in input signal */
    #if (INPUT_MASK_USED == 1)
        if(integrator_config->mask_input)
        {
        	if(input < 0L)
        	{
        		(*((uint8_t *)&(input) + 0)) |= 0x0FU;
        	}
        	else
        	{
        		(*((uint8_t *)&(input) + 0)) &= 0xF0U;
        	}
        }
        else
        {
            /* input = input */
        }
    #endif  /* INPUT_MASK_USED */
    
    /* High pass filter for input */
    #if (HPF0_USED == 1)
        if(integrator_config->hpf_input_enable)
        {
        /* HPF algorithm */
        /*
            temp = integrator1->hpf_output_old_signal;
            temp = input - temp;
            temp = integrator1->hpf_output_result + temp;
            
            // temp *= HPF_COFF;
            
            integrator1->hpf_output_old_signal = input;
            integrator1->hpf_output_result = temp;
            return temp;
        */
        
        
        /* Implement HPF by using 32-BIT MULTIPLY-ACCUMULATOR */

        /* TODO: MURL = input * HPF_COEF */
        
        /* Load input signal */
        MUL32SL = (*((uint16_t *)&(input)));
        MUL32SH = (*((uint16_t *)&(input) + 1));
        
        /* Load HPF coef */
        MULBL = HPF_COEF_NUM;     // HPF_COEF_NUM
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        
        /*----------------------------------------------------*/
        /* TODO: MURL += output_result * HPF_COEF             */
        /* Load signal0 */
        MAC32SL = (*((uint16_t *)&(integrator->hpf_input_result)));
        MAC32SH = (*((uint16_t *)&(integrator->hpf_input_result) + 1));
        /* Load signal1 */
        //MULBL = HPF_COEF_NUM;     /* HPF_COEF_NUM */
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        
        /*----------------------------------------------------*/
        // TODO: MURL += old_signal * (-HPF_COEF)
        /* Load old signal */
        MAC32SL = (*((uint16_t *)&(integrator->hpf_input_old_signal)));
        MAC32SH = (*((uint16_t *)&(integrator->hpf_input_old_signal) + 1));
        /* Load HPF coef */
        MULBL = HPF_COEF_NUM_NEGATIVE;     // (-HPF_COEF_NUM)
        MULBH = 0xFFFF;
         
        /*  Wait 5 cycles or more elapse */
        //integrator->hpf_input_old_signal = input;
        (*((uint16_t *)&(integrator->hpf_input_old_signal)))   = (*((uint16_t *)&(input)));
        (*((uint16_t *)&(integrator->hpf_input_old_signal)+1)) = (*((uint16_t *)&(input)+1));
        
        /*----------------------------------------------------*/
        /* result = result / 65536 */
            (*((uint16_t *)&(integrator->hpf_input_result)))     = MULR1;
            (*((uint16_t *)&(integrator->hpf_input_result) + 1)) = MULR2;
        }
        else
        {
            (*((uint16_t *)&(integrator->hpf_input_result)))     = (*((uint16_t *)&(input)));
            (*((uint16_t *)&(integrator->hpf_input_result) + 1)) = (*((uint16_t *)&(input)+1));
        }
    #else /* HPF0 used */
        (*((uint16_t *)&(integrator->hpf_input_result)))     = (*((uint16_t *)&(input)));
        (*((uint16_t *)&(integrator->hpf_input_result) + 1)) = (*((uint16_t *)&(input)+1));
    #endif /* HPF0 not used */
    
    /* Integrate signal: Rungle-Kutta 4 */
    #if (INTEGRATION_RK_USED == RK4)
        if(integrator_config->integrator_enable)
        {
        /* RK4 algorithm */
        /*
                temp = integrator->hpf_input_result;
            temp = (integrator->old_signal1+(integrator->old_signal2*2)+(integrator->old_signal3*2)+temp);
            temp = temp/6;
            
            integrator->old_signal1 = integrator->old_signal2;
            integrator->old_signal2 = integrator->old_signal3;
                integrator->old_signal3 = integrator->hpf_input_result;
                    
                temp += integrator->result;
                    
                integrator->result = temp;
        */

        /*----------------------------------------------------*/
        /* MULR = old[1] * 1/6; */
        /* Load old_signal1 */
        MUL32SL = (*((uint16_t *)&(integrator->old_signal1)));
        MUL32SH = (*((uint16_t *)&(integrator->old_signal1) + 1));
        /* Load DIV_6_NUM */
        MULBL = INTEGRATION_RK_DIV_6;         // = 110923/65536 ~ 1/6
        MULBH = 0;
          
        /* Wait 5 cycles or more elapse */
        // integrator->old_signal1 = integrator->old_signal2;
        (*((uint16_t *)&(integrator->old_signal1)))   = (*((uint16_t *)&(integrator->old_signal2)));
        (*((uint16_t *)&(integrator->old_signal1)+1)) = (*((uint16_t *)&(integrator->old_signal2)+1));
        
        /*----------------------------------------------------*/
        /* TODO: MULR += old[2] * 2/6; */
        /* Load old_signal2 */
        MAC32SL = (*((uint16_t *)&(integrator->old_signal2)));
        MAC32SH = (*((uint16_t *)&(integrator->old_signal2) + 1));
        /* Load DIV_6_NUM */
        MULBL = INTEGRATION_RK_DIV_6 * 2;    // = 2*10923/65536 ~ 2/6
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        //      integrator->old_signal2 = integrator->old_signal3;
        (*((uint16_t *)&(integrator->old_signal2)))   = (*((uint16_t *)&(integrator->old_signal3)));
        (*((uint16_t *)&(integrator->old_signal2)+1)) = (*((uint16_t *)&(integrator->old_signal3)+1));
        
        /*----------------------------------------------------*/
        /* TODO: MULR += old[3] * 2/6; */
        /* Load old_signal3 */
        MAC32SL = (*((uint16_t *)&(integrator->old_signal3)));
        MAC32SH = (*((uint16_t *)&(integrator->old_signal3) + 1));
        /* Load DIV_6_NUM */
        // MULBL = INTEGRATION_RK_DIV_6 * 2;    // = 2*10923/65536 ~ 2/6
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        //    integrator->old_signal2 = integrator->old_signal3;
            (*((uint16_t *)&(integrator->old_signal3)))   = (*((uint16_t *)&(integrator->hpf_input_result)));
            (*((uint16_t *)&(integrator->old_signal3)+1)) = (*((uint16_t *)&(integrator->hpf_input_result)+1));
        
        /*----------------------------------------------------*/
        /* TODO: MULR += INPUT * 1/6;             */
        /* Load input */
            MAC32SL = (*((uint16_t *)&(integrator->hpf_input_result)));
            MAC32SH = (*((uint16_t *)&(integrator->hpf_input_result) + 1));
        /* Load 1/6 const */
        MULBL = INTEGRATION_RK_DIV_6;         // = 110923/65536 ~ 1/6
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        
        /*----------------------------------------------------*/
        /* TODO: MULR += result; */
        MAC32SL = (*((uint16_t *)&(integrator->result)));
        MAC32SH = (*((uint16_t *)&(integrator->result) + 1));
        /* Load signal1 */
        MULBL = 0;      // = 65536/65536 = 1
        MULBH = 1;
        
        /* Wait 5 cycles or more elapse */
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        
        /*----------------------------------------------------*/
        // result = result / 65536
        (*((uint16_t *)&(integrator->result)))     = MULR1;
        (*((uint16_t *)&(integrator->result) + 1)) = MULR2;
        }
        else
        {
            (*((uint16_t *)&(integrator->result)))   = (*((uint16_t *)&(integrator->hpf_input_result)));
            (*((uint16_t *)&(integrator->result)+1)) = (*((uint16_t *)&(integrator->hpf_input_result)+1));
        }
        
    #elif (INTEGRATION_RK_USED == RK2)
         if(integrator_config->integrator_enable)
         {
            /* RK2 algorithm */
            /*
                temp = integrator->hpf_input_result;
                temp = (integrator->old_signal3+temp);
                temp = temp/2;

                integrator->old_signal3 = integrator->hpf_input_result;
                
                temp += integrator->result;
                integrator->result = temp;
            */

            /*----------------------------------------------------*/
            /* MULR = old[1] * 1/6; */
            /* Load old_signal1 */
            MUL32SL = (*((uint16_t *)&(integrator->old_signal3)));
            MUL32SH = (*((uint16_t *)&(integrator->old_signal3) + 1));
            /* Load DIV_6_NUM */
            MULBL = 0x8000;         // = 110923/65536 ~ 1/6
            MULBH = 0;
              
            /* Wait 5 cycles or more elapse */
            //    integrator->old_signal3 = integrator->hpf_input_result;
            (*((uint16_t *)&(integrator->old_signal3)))   = (*((uint16_t *)&(integrator->hpf_input_result)));
            (*((uint16_t *)&(integrator->old_signal3)+1)) = (*((uint16_t *)&(integrator->hpf_input_result)+1));
            
            /*----------------------------------------------------*/
            /* TODO: MULR += INPUT * 1/6;             */
            /* Load input */
            MAC32SL = (*((uint16_t *)&(integrator->hpf_input_result)));
            MAC32SH = (*((uint16_t *)&(integrator->hpf_input_result) + 1));
            /* Load 1/6 const */
            MULBL = 0x8000;         // = 110923/65536 ~ 1/6
            MULBH = 0;
            
            /* Wait 5 cycles or more elapse */
            NOP();
            NOP();
            NOP();
            NOP();
            NOP();
            
            /*----------------------------------------------------*/
            /* TODO: MULR += result; */
            MAC32SL = (*((uint16_t *)&(integrator->result)));
            MAC32SH = (*((uint16_t *)&(integrator->result) + 1));
            /* Load signal1 */
            MULBL = 0;      // = 65536/65536 = 1
            MULBH = 1;
            
            /* Wait 5 cycles or more elapse */
            NOP();
            NOP();
            NOP();
            NOP();
            NOP();
            
            /*----------------------------------------------------*/
            // result = result / 65536
            (*((uint16_t *)&(integrator->result)))     = MULR1;
            (*((uint16_t *)&(integrator->result) + 1)) = MULR2;
         }
         else /* RK2 not used? */
         {
            (*((uint16_t *)&(integrator->result)))   = (*((uint16_t *)&(integrator->hpf_input_result)));
            (*((uint16_t *)&(integrator->result)+1)) = (*((uint16_t *)&(integrator->hpf_input_result)+1));
         }
            
    #else    /* (INTEGRATION_RUNGLE_KUTTA == 0) */
        (*((uint16_t *)&(integrator->result)))   = (*((uint16_t *)&(integrator->hpf_input_result)));
        (*((uint16_t *)&(integrator->result)+1)) = (*((uint16_t *)&(integrator->hpf_input_result)+1));
    #endif    /* (INTEGRATION_RUNGLE_KUTTA == 1) */

    /* High pass filter for output */
    #if (HPF1_USED == 1)
        if(integrator_config->hpf_output_enable)
        {
        /* HPF algorithm */    
        /*
            temp = integrator1->hpf_output_old_signal;
            temp = input - temp;
            temp = integrator1->hpf_output_result + temp;
            
            // temp *= HPF_COFF;
            
            integrator1->hpf_output_old_signal = input;
            integrator1->hpf_output_result = temp;
            return temp;
        */
        
        /* Implement HPF by using 32-BIT MULTIPLY-ACCUMULATOR */
    
        /* TODO: MURL = input * HPF_COEF */
        /* Load input signal */
        MUL32SL = (*((uint16_t *)&(integrator->result)));
        MUL32SH = (*((uint16_t *)&(integrator->result) + 1));
        
        /* Load HPF coef */
        MULBL = HPF_COEF_NUM;     // HPF_COEF_NUM
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        
        /*----------------------------------------------------*/
        // TODO: MURL += output_result * HPF_COEF
        /* Load signal0 */
        MAC32SL = (*((uint16_t *)&(integrator->hpf_output_result)));
        MAC32SH = (*((uint16_t *)&(integrator->hpf_output_result) + 1));
        
        /* Load signal1 */
        //MULBL = HPF_COEF_NUM;     // HPF_COEF_NUM
        MULBH = 0;
        
        /* Wait 5 cycles or more elapse */
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        
        /*----------------------------------------------------*/
        // TODO: MURL += old_signal * (-HPF_COEF)
        /* Load old signal */
        MAC32SL = (*((uint16_t *)&(integrator->hpf_output_old_signal)));
        MAC32SH = (*((uint16_t *)&(integrator->hpf_output_old_signal) + 1));
        /* Load HPF coef */
        MULBL = HPF_COEF_NUM_NEGATIVE;     // (-HPF_COEF_NUM)
        MULBH = 0xFFFF;
        
        /*----------------------------------------------------*/
        /* Wait 5 cycles or more elapse */
        // TODO: Save old signal;
        //integrator->hpf_output_old_signal = integrator->result;
        (*((uint16_t *)&(integrator->hpf_output_old_signal)))   = (*((uint16_t *)&(integrator->result)));
        (*((uint16_t *)&(integrator->hpf_output_old_signal)+1)) = (*((uint16_t *)&(integrator->result)+1));
        
        /*----------------------------------------------------*/
        /* result = result / 65536 */
        (*((uint16_t *)&(integrator->hpf_output_result)))     = MULR1;
        (*((uint16_t *)&(integrator->hpf_output_result) + 1)) = MULR2;
        
         //return (integrator->hpf_output_result);
        }
        else
        {
            (*((uint16_t *)&(integrator->hpf_output_result)))     = (*((uint16_t *)&(integrator->result)));
            (*((uint16_t *)&(integrator->hpf_output_result) + 1)) = (*((uint16_t *)&(integrator->result)+1));
        }

    #else    /* (HPF1_USED == 0) */
        (*((uint16_t *)&(integrator->hpf_output_result)))     = (*((uint16_t *)&(integrator->result)));
        (*((uint16_t *)&(integrator->hpf_output_result) + 1)) = (*((uint16_t *)&(integrator->result)+1));
    
        //return (integrator->result);
    #endif    /* (HPF1_USED == 1) */

    /* Scale down output signal */
    #if ((SCALE_DOWN_OUTPUT_USED == 1) && (INPUT_MASK_USED == 0))
        if(integrator_config.scale_down_output)
        {
            /* output = ( output / 2^4 ) */
    
            /* Load input signal */
            MUL32SL = (*((uint16_t *)&(integrator->hpf_output_result)));
            MUL32SH = (*((uint16_t *)&(integrator->hpf_output_result) + 1));
            
            /* Load mask bit num */
            MULBL = SCALE_DOWN_OUTPUT;     // HPF_COEF_NUM
            MULBH = 0;
            
            /* Wait 5 cycles or more elapse */
            NOP();
            NOP();
            NOP();
            NOP();
            NOP();
            
            /* return output; */
            (*((uint16_t *)&(input)))     =  MULR1;
            (*((uint16_t *)&(input) + 1)) =  MULR2;
        
            return (input);
        }
        else
        {
            return (integrator->hpf_output_result);
        }
    #else
        return (integrator->hpf_output_result);
    #endif /* SCALE_DOWN_OUTPUT_USED */
}
