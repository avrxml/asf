/**************************************************************************//**
  \file reg_common.h
 
  \brief Includes datatypes and signatures for reg_common
 
  Copyright (c) 2016, Atmel Corporation. All rights reserved.
  Released under NDA
  Licensed under Atmel's Limited License Agreement.
 
 
  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 
  Atmel Corporation: http://www.atmel.com
 
******************************************************************************/

#ifndef __REG_COMMON_H__
#define __REG_COMMON_H__


// Bit definition
#define Bit0			(1<<0)
#define Bit1			(1<<1)
#define Bit2			(1<<2)
#define Bit3			(1<<3)
#define Bit4			(1<<4)
#define Bit5			(1<<5)
#define Bit6			(1<<6)
#define Bit7			(1<<7)
#define Bit8			(1<<8)
#define Bit9			(1<<9)
#define Bit10			(1<<10)
#define Bit11			(1<<11)
#define Bit12			(1<<12)
#define Bit13			(1<<13)
#define Bit14			(1<<14)
#define Bit15			(1<<15)
#define Bit16			(1<<16)
#define Bit17			(1<<17)
#define Bit18			(1<<18)
#define Bit19			(1<<19)
#define Bit20			(1<<20)
#define Bit21			(1<<21)
#define Bit22			(1<<22)
#define Bit23			(1<<23)
#define Bit24			(1<<24)
#define Bit25			(1<<25)
#define Bit26			(1<<26)
#define Bit27			(1<<27)
#define Bit28			(1<<28)
#define Bit29			(1<<29)
#define Bit30			(1<<30)
#define Bit31			(1<<31)

// Base Addresses
#define LPMCU_CORTEX_MISC_REGS_BASE 			0x4000B000
#define LP_CLK_CALIBRATION_REGS_BASE            0x4000C000
#define AON_SLEEP_TIMER_BASE					0x4000D000
#define AON_PWR_SEQ_REGS_BASE 				    0x4000E000
#define AON_GP_REGS_BASE						0x4000F000
#define PERIPH_BT_GLOBAL_REGISTERS_BASE         0x40020000
#define BT_REGS_BASE                            0x40024000
#define RXTX_SEQ_CTRL_REGISTERS_BASE 			0x40025000
#define BLE_FE_REGS_BASE                        0x40041800
#define TRIGGER_BLE_PLL_MEM_BASE					0x40022800

//#include "aon_pwr_seq_regs_c.h"
#include "lpmcu_cortex_misc_regs_c.h"
//#include "rxtx_seq_ctrl_registers_c.h"
//#include "aon_gp_regs_c.h"
//#include "aon_pwr_seq_regs_c.h"
//#include "aon_sleep_timer_c.h"
//#include "lp_clk_calibration_regs_c.h"
//#include "periph_bt_global_registers_c.h"
//#include "bt_regs_c.h"
//#include "ble_fe_regs_c.h"
//#include "reg_blecore.h"

#define USE_NMI_FPGA						0


#endif
