/*
 * em_event.h
 *
 *  Created on: 24 Oct 2023
 *      Author: a5126135
 */

#ifndef SRC_RL78I1C_APPLICATION_EVENT_EM_EVENT_H_
#define SRC_RL78I1C_APPLICATION_EVENT_EM_EVENT_H_

/**@brief Performs event processing*/
void EVENT_PollingProcessing(void);

/** @brief Enables event processing timing*/
void EVENT_RTC_Callback(void);

#endif /* SRC_RL78I1C_APPLICATION_EVENT_EM_EVENT_H_ */
