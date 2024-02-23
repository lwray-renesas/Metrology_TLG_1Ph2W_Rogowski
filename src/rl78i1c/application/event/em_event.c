/*
 * em_event.c
 *
 *  Created on: 24 Oct 2023
 *      Author: a5126135
 */

#include "em_core.h"
#include "em_event.h"
#include "debug.h"
#include "r_cg_lvd.h"

/** @brief check for events every 3 seconds*/
#define EVENT_CHECK_COUNT_VALUE	(6U)

static EM_STATUS current_status = {0U, 0U, 0U, 0U, 0U, 0U};
static volatile uint8_t event_check_count = 0U;
static volatile uint8_t event_check_flag = 1U;
volatile vdd_range_t vdd_range = vdd_less_than_2_46;

void EVENT_PollingProcessing(void)
{
	if(1U == event_check_flag)
	{
		EM_STATUS new_status;
		volatile uint8_t ack_new_line_flag = 0U;
		volatile vdd_range_t current_vdd_range = vdd_less_than_2_46;
		event_check_flag = 0U;

		new_status = EM_GetStatus();

		if((1U == new_status.is_voltage_sag) && (0U == current_status.is_voltage_sag))
		{
			DEBUG_Printf((uint8_t*)"\n\rVoltage Sag Event!");
			ack_new_line_flag = 1U;
		}
		else if((0U == new_status.is_voltage_sag) && (1U == current_status.is_voltage_sag))
		{
			DEBUG_Printf((uint8_t*)"\n\rVoltage Sag Recovered!");
			ack_new_line_flag = 1U;
		}
		else
		{
			/* Do Nothing*/
		}

		if((1U == new_status.is_voltage_swell) && (0U == current_status.is_voltage_swell))
		{
			DEBUG_Printf((uint8_t*)"\n\rVoltage Swell Event!");
			ack_new_line_flag = 1U;
		}
		else if((0U == new_status.is_voltage_swell) && (1U == current_status.is_voltage_swell))
		{
			DEBUG_Printf((uint8_t*)"\n\rVoltage Swell Recovered!");
			ack_new_line_flag = 1U;
		}
		else
		{
			/* Do Nothing*/
		}

		if((1U == new_status.noload_active) && (0U == current_status.noload_active))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Active Load Event (Phase)!");
			ack_new_line_flag = 1U;
		}
		else if((0U == new_status.noload_active) && (1U == current_status.noload_active))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Active Load Recovered (Phase)!");
			ack_new_line_flag = 1U;
		}
		else
		{
			/* Do Nothing*/
		}

		if((1U == new_status.noload_reactive) && (0U == current_status.noload_reactive))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Reactive Load Event (Phase)!");
			ack_new_line_flag = 1U;
		}
		else if((0U == new_status.noload_reactive) && (1U == current_status.noload_reactive))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Reactive Load Recovered (Phase)!");
			ack_new_line_flag = 1U;
		}
		else
		{
			/* Do Nothing*/
		}

		if((1U == new_status.noload_active2) && (0U == current_status.noload_active2))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Active Load Event (Neutral)!");
			ack_new_line_flag = 1U;
		}
		else if((0U == new_status.noload_active2) && (1U == current_status.noload_active2))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Active Load Recovered (Neutral)!");
			ack_new_line_flag = 1U;
		}
		else
		{
			/* Do Nothing*/
		}

		if((1U == new_status.noload_reactive2) && (0U == current_status.noload_reactive2))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Reactive Load Event (Neutral)!");
			ack_new_line_flag = 1U;
		}
		else if((0U == new_status.noload_reactive2) && (1U == current_status.noload_reactive2))
		{
			DEBUG_Printf((uint8_t*)"\n\rNo Reactive Load Recovered (Neutral)!");
			ack_new_line_flag = 1U;
		}
		else
		{
			/* Do Nothing*/
		}

		/* Check operating voltage*/
		current_vdd_range = R_LVD_Check();
		if(current_vdd_range != vdd_range)
		{
			vdd_range = current_vdd_range;
			DEBUG_Printf((uint8_t*)"\n\rVDD Changed!\n\r");
			DEBUG_Printf((uint8_t*)R_LVD_range_to_str(current_vdd_range));
			ack_new_line_flag = 1U;
		}

		if(1U == ack_new_line_flag)
		{
			DEBUG_AckNewLine();
		}

		/* Update the current status*/
		memcpy(&current_status, &new_status, sizeof(EM_STATUS));
	}
}
/* END OF FUNCTION*/

void EVENT_RTC_Callback(void)
{
	if(1U != event_check_flag)
	{
		++event_check_count;
		if(event_check_count >= EVENT_CHECK_COUNT_VALUE)
		{
			event_check_flag = 1U;
			event_check_count = 0U;
		}
	}
}
/* END OF FUNCTION*/
