/*
 * calibration.h
 *
 *  Created on: 23 Oct 2023
 *      Author: a5126135
 */

#ifndef SRC_RL78I1C_APPLICATION_CALIBRATION_CALIBRATION_H_
#define SRC_RL78I1C_APPLICATION_CALIBRATION_CALIBRATION_H_

#include "em_core.h"

/** @brief checks if calibration is occuring.
 * @return 0U if not, 1U if it is.
 */
uint8_t CALIBRATION_Occuring(void);

/***********************************************************************************************************************
* Function Name : COMMAND_InvokeCalibration
* Interface     : void COMMAND_InvokeCalibration(void)
* Description   :
* Arguments     :	calib_c - Number of line cycle used for fs, gain and coefficient calibration
* 					calib_cp - Number of line cycle used for signals angle calibration
* 					calib_imax - Max measuring current for calibrating meter
* 					calib_v - Voltage value on reference power supply
* 					calib_i - Current value on reference power supply
* 					calib_neutral - calibrate neutral (0 = no, non-zero = yes)
* Function Calls:
* Return Value  :	EM_OK = Good calibration
* 					3 = Backup to dataflash failed
* 					Otherwise needs debugging for specific cause.
*
***********************************************************************************************************************/
uint8_t CALIBRATION_Invoke(uint16_t calib_c, uint16_t calib_cp, float32_t calib_imax, float32_t calib_v, float32_t calib_i, uint8_t calib_neutral);

#endif /* SRC_RL78I1C_APPLICATION_CALIBRATION_CALIBRATION_H_ */
