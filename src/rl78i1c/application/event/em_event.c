/*
 * em_event.c
 *
 *  Created on: 24 Oct 2023
 *      Author: a5126135
 */

#include "em_core.h"
#include "em_event.h"
#include "debug.h"

/** @brief check for events every 3 seconds*/
#define EVENT_CHECK_COUNT_VALUE	(6U)

static volatile EM_STATUS current_status = {0U, 0U, 0U, 0U, 0U, 0U};
static volatile uint8_t event_check_count = 0U;
static volatile uint8_t event_check_flag = 1U;

void EVENT_PollingProcessing(void)
{
	if(1U == event_check_flag)
	{
		volatile EM_STATUS new_status;
		volatile uint8_t ack_new_line_flag = 0U;
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

		if(1U == ack_new_line_flag)
		{
			DEBUG_AckNewLine();
		}

		/* Update the current status*/
		current_status.is_voltage_sag = new_status.is_voltage_sag;
		current_status.is_voltage_swell = new_status.is_voltage_swell;
		current_status.noload_active = new_status.noload_active;
		current_status.noload_active2 = new_status.noload_active2;
		current_status.noload_reactive = new_status.noload_reactive;
		current_status.noload_reactive2 = new_status.noload_reactive2;
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
