/**
 * \file
 *
 * \brief Class B Oven Simulation plant
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include "oven.h"
#include "oven_ui.h"
#include "oven_plant.h"

/**
 * \name Physical properties of the model
 *
 * @{
 */

/** Pot heat capacity */
#define C_POT 20.0
/** Plate heat capacity */
#define C_PLATE 2.0

/** Pot heat conductivity */
#define K_POT 7
/** Plate heat conductivity */
#define K_PLATE 2

/** Environment temperature */
#define ENV_TEMP 25

/** @} */

/** State variables for the simulation: Plate and Pot temperature */
static float state[2] = {20 * C_PLATE, 20 * C_POT};

/**
 * \name Internal functions
 *
 * @{
 */

/**
 * \brief Read the period of the frequency signal capture
 *
 * \return A value representing the power with which the plate element is
 * actuated.
 */
static uint16_t oven_plant_read_signal_period(void)
{
	uint16_t val = 0;

	if (tc_is_cc_interrupt(&OVEN_FREQ_CAPT_TC, TC_CCA)) {
		val = tc_read_cc_buffer(&OVEN_FREQ_CAPT_TC, TC_CCA);
		tc_clear_cc_interrupt(&OVEN_FREQ_CAPT_TC, TC_CCA);
	}

	return val >> 1;
}

/**
 * \brief Map the signal period to a power level for the simulation plant.
 *
 * In effect, this function acts as the induction coil in the oven.
 *
 * Its return value reflects `wattage` as used in the controller and the user
 * interface.
 *
 * \param p Period of frequency signal, from
 * \ref oven_plant_read_signal_period()
 *
 * \return Float value mapping captured signal period to energy
 */
static float oven_plant_calculate_u_from_period(uint16_t p)
{
	if (!p) {
		return 0;
	}

	return 512.0 / (float)p;
}

/** @} */

/**
 * \brief Initialize the oven's physical state.
 */
void oven_plant_init(void)
{
	state[0] = 20 * C_PLATE;
	state[1] = 20 * C_POT;
}

/**
 * \brief Perform a simulation step.
 *
 * This function first reads the received frequency signal representing the
 * power level the plate element is actuated with, then performs a simulation
 * step and outputs the simulated temperature of the plate element as an analog
 * value via DACB on the pin marked ADC2.
 *
 * The simulation plant is a discretized version of the continuous system:
 * \f$ \dot{x} = \mathbf{F}*\vec{x} + \mathbf{B}*\vec{u} \f$, where:
 * \f[
   \begin{array}{c}
   \mathbf{F} = \left[ \begin{array}{c c}
           \frac{-1}{C_\textnormal{plate} * K_\textnormal{plate}} & 0 \\
	   \\
	   0 & \frac{-1}{C_\textnormal{pot} * K_\textnormal{pot}}
       \end{array} \right] \\
   \vec{x} = \left[ \begin{array}{c}
           E_\textnormal{plate} \\
	   \\
	   E_\textnormal{pot}
       \end{array} \right] \\
   \mathbf{B} = \left[ \begin{array}{c c}
           1 & 0 \\
	   \\
	   0 & 1
       \end{array} \right] \\
   \vec{u} = \left[ \begin{array}{c}
           \frac{T_\textnormal{environment}}{K_\textnormal{plate}} \\
	   \\
           \frac{T_\textnormal{environment}}{K_\textnormal{pot}}
       \end{array} \right]
   \end{array} \\
   \f]
 * in the case when the pot is OFF the plate.
 *
 * This corresponds to the plate and the pot cooling separately by Newton's law
 * of cooling because no power can be induced when the pot is not on the plate.
 *
 * When the pot is ON the stove, the system is similar, but \b F is modified by
 * adding heat transfer between the plate and the contents of the pot, and
 * disregarding heat loss from the plate to the environment.
 * Likewise \b u is modified by adding energy induction into the bottom of the
 * pot, simplified here as being the same point as the plate:
 * \f[
   \begin{array}{c}
   \mathbf{F} = \left[ \begin{array}{c c}
           \frac{-1}{C_\textnormal{plate}* K_\textnormal{plate}} &
	   		\frac{1}{C_\textnormal{pot}* K_\textnormal{plate}} \\
	   \\
           \frac{1}{C_\textnormal{plate}* K_\textnormal{plate}} &
	   		\frac{-1}{C_\textnormal{pot}* K_\textnormal{plate}}
                   - \frac{1}{C_\textnormal{pot} * K_\textnormal{pot}}
	\end{array} \right] \\
   \vec{u} = \left[ \begin{array}{c}
           \delta E_\textnormal{induced} \\
	   \\
           \frac{T_\textnormal{environment}}{K_\textnormal{pot}}
       \end{array} \right]
   \end{array}
   \f]
 *
 *
 * \param pot Whether the pot is on or off the plate.
 */
void oven_plant_sim_step(enum pot_t pot)
{
	/* Remap period of incoming signal to power */
	float u = oven_plant_calculate_u_from_period(
			oven_plant_read_signal_period());

	switch (pot) {
	case POT_OFF:
		state[0] = state[0] + u - (state[0] / C_PLATE - ENV_TEMP)
				/ K_PLATE;
		state[1] = state[1] - (state[1] / C_POT - ENV_TEMP) / K_POT;
		break;

	case POT_ON:
		state[0] = state[0] + u - (state[0] / C_PLATE - state[1]
				/ C_POT) / K_PLATE;
		state[1] = state[1] + (state[0] / C_PLATE - state[1] / C_POT)
				/ K_PLATE - (state[1] / C_POT - ENV_TEMP)
				/ K_POT;
		break;

	default:
		break;
	}

	/* Output DAC value, simulating an analog temperature sensor. */
	dac_wait_for_channel_ready(&DACB, DAC_CH0);
	dac_set_channel_value(&DACB, DAC_CH0, state[0] * 1);

	return;
}

/**
 * \brief Read the internal simulation states
 *
 * \param no The state variable to get, 0 for plate and 1 for pot
 *
 * \return Temperature of the selected state variable
 */
float oven_plant_get_state_temp(uint8_t no)
{
	if (no == 0) {
		return state[0] / C_PLATE / 4;
	}

	if (no == 1) {
		return state[1] / C_POT / 4;
	}

	return 0;
}
