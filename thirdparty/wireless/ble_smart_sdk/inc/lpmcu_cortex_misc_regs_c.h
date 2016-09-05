/**************************************************************************//**
  \file lpmcu_cortex_misc_regs_c.h
 
  \brief Includes datatypes and signatures for lpmcu_cortex_misc_regs_c
 
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

/* **************************** */
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CHIP_ID                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CHIP_ID_DEFAULT                                                    0x2000B0
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CHIP_ID_REV_ID                                                     (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18+Bit19+Bit20+Bit21+Bit22+Bit23)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CHIP_ID_REV_ID_MASK                                                (Bit23+Bit22+Bit21+Bit20+Bit19+Bit18+Bit17+Bit16+Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CHIP_ID_REV_ID_SHIFT                                               (0)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0                                                     (LPMCU_CORTEX_MISC_REGS_BASE + 0x4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_DEFAULT                                             0xFFFFFFFD
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_GLOBAL_RSTN                                         (Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CPU_RSTN                                            (Bit1)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_SPIFLASH_SYS_RSTN                                   (Bit2)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_SPIFLASH_RSTN                                       (Bit3)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CORTUS_SPI0_CORE_RSTN                               (Bit4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CORTUS_SPI0_IF_RSTN                                 (Bit5)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CORTUS_SPI1_CORE_RSTN                               (Bit6)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CORTUS_SPI1_IF_RSTN                                 (Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CORTUS_I2C0_CORE_RSTN                               (Bit8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CORTUS_I2C0_IF_RSTN                                 (Bit9)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_GPIO_RSTN                                           (Bit10)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_COUNTER_0_RSTN                                      (Bit11)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_UART_CORE_RSTN                                      (Bit12)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_UART_IF_RSTN                                        (Bit13)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_UART_1_CORE_RSTN                                    (Bit14)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_UART_1_IF_RSTN                                      (Bit15)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_WATCHDOG_0_RSTN                                     (Bit16)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_WATCHDOG_1_RSTN                                     (Bit17)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_IRQ_CTRLR_CORE_RSTN                                 (Bit18)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_MBIST_RSTN                                          (Bit19)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CALIB_RSTN                                          (Bit20)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_CALIB_XBAR_IF_RSTN                                  (Bit21)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_DBUG_RSTN                                           (Bit22)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_ARM_FREE_CLK_RSTN                                   (Bit23)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_ARM_PRESETN_RSTN                                    (Bit24)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_QUAD_DEC_1_RSTN                                     (Bit25)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_QUAD_DEC_2_RSTN                                     (Bit26)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_QUAD_DEC_3_RSTN                                     (Bit27)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_PWM_1_RSTN                                          (Bit28)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_PWM_2_RSTN                                          (Bit29)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_PWM_3_RSTN                                          (Bit30)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_0_PWM_4_RSTN                                          (Bit31)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1                                                     (LPMCU_CORTEX_MISC_REGS_BASE + 0x8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_DEFAULT                                             0xFFF
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_DUALTIMER_RSTN                                      (Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_CORTUS_I2C1_CORE_RSTN                               (Bit1)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_CORTUS_I2C1_IF_RSTN                                 (Bit2)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_SECURITY_SHA_CORE_RSTN                              (Bit3)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_SECURITY_SHA_AHB_RSTN                               (Bit4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_SECURITY_AES_CORE_RSTN                              (Bit5)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_SECURITY_AES_AHB_RSTN                               (Bit6)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_CORTUS_SPI0_SCK_CLK_RSTN                            (Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_CORTUS_SPI1_SCK_CLK_RSTN                            (Bit8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_CORTUS_SPI0_SCK_PHASE_INT_CLK_RSTN                  (Bit9)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_CORTUS_SPI1_SCK_PHASE_INT_CLK_RSTN                  (Bit10)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_GLOBAL_RESET_1_DMA_CONTROLLER_RSTN                                 (Bit11)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0                                                    (LPMCU_CORTEX_MISC_REGS_BASE + 0xc)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_DEFAULT                                            0x627FF9BE
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_SPIFLASH_CLK_EN                                    (Bit1)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_SPI0_CORE_CLK_EN                            (Bit2)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_SPI1_CORE_CLK_EN                            (Bit3)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_I2C0_CORE_CLK_EN                            (Bit4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_DUALTIMER_CLK_EN                                   (Bit5)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_GPIO_CLK_EN                                        (Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_COUNTER_0_CLK_EN                                   (Bit8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_WATCHDOG_0_CLK_EN                                  (Bit11)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_WATCHDOG_1_CLK_EN                                  (Bit12)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_CORE_CLK_EN                                   (Bit13)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_IF_CLK_EN                                     (Bit14)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_1_CORE_CLK_EN                                 (Bit15)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_1_IF_CLK_EN                                   (Bit16)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_IRQ_CTRLR_CORE_CLK_EN                              (Bit17)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_IDRAM_1_GL_MEM_CLK_EN                              (Bit18)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_IDRAM_2_GL_MEM_CLK_EN                              (Bit19)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_ROM_MEM_CLK_EN                                     (Bit20)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CALIB_XBAR_IF_CLK_EN                               (Bit21)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_AON_WRAPPER_CLK_EN                                 (Bit22)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_ARM_PCLK_EN                                        (Bit23)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_ARM_PCLKG_EN                                       (Bit24)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_BLE_MEM_CLK_EN                                     (Bit25)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_QUAD_DEC_1_CLK_EN                                  (Bit26)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_QUAD_DEC_2_CLK_EN                                  (Bit27)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_QUAD_DEC_3_CLK_EN                                  (Bit28)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_I2C1_CORE_CLK_EN                            (Bit29)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CALIB_CLK_EN                                       (Bit30)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1                                                    (LPMCU_CORTEX_MISC_REGS_BASE + 0x10)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_DEFAULT                                            0xF8783F
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_1_CLK_EN                                     (Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_2_CLK_EN                                     (Bit1)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_3_CLK_EN                                     (Bit2)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_4_CLK_EN                                     (Bit3)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_5_CLK_EN                                     (Bit4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_6_CLK_EN                                     (Bit5)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_PWM_1_CLK_EN                                       (Bit6)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_PWM_2_CLK_EN                                       (Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_PWM_3_CLK_EN                                       (Bit8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_PWM_4_CLK_EN                                       (Bit9)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SENS_ADC_CLK_EN                                    (Bit10)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SPI0_SCK_PHASE_INT_CLK_EN                          (Bit11)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SPI1_SCK_PHASE_INT_CLK_EN                          (Bit12)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_GPIO_GCLK_EN                                       (Bit13)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_COUNTER_0_PGCLK_EN                                 (Bit14)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SHA_CORE_CLK_EN                                    (Bit15)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SHA_AHB_CLK_EN                                     (Bit16)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_AES_CORE_CLK_EN                                    (Bit17)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_AES_AHB_CLK_EN                                     (Bit18)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_IDRAM_1_0_MEM_CLK_EN                               (Bit19)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_IDRAM_1_1_MEM_CLK_EN                               (Bit20)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_IDRAM_1_2_MEM_CLK_EN                               (Bit21)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_IDRAM_2_0_MEM_CLK_EN                               (Bit22)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_IDRAM_2_1_MEM_CLK_EN                               (Bit23)


#define LPMCU_CORTEX_MISC_REGS_BTMCU_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x14)
#define LPMCU_CORTEX_MISC_REGS_BTMCU_CONTROL_DEFAULT                                                    0x7
#define LPMCU_CORTEX_MISC_REGS_BTMCU_CONTROL_RXTX_SEQ_CLK_EN                                            (Bit0)
#define LPMCU_CORTEX_MISC_REGS_BTMCU_CONTROL_AHB_CLK_EN                                                 (Bit1)
#define LPMCU_CORTEX_MISC_REGS_BTMCU_CONTROL_PERIPH_REGS_CLK_EN                                         (Bit2)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x18)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_DEFAULT                                                    0x303100
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_LPMCU_CLK_SEL                                              (Bit0+Bit1)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_LPMCU_CLK_SEL_MASK                                         (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_LPMCU_CLK_SEL_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_USE_BT26M_CLK                                              (Bit2)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_USE_ARM_LP_CLK                                             (Bit3)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_DUALTIMER_CLK_SEL                                          (Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_DUALTIMER_CLK_SEL_MASK                                     (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_DUALTIMER_CLK_SEL_SHIFT                                    (4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_BYPASS_WIC                                                 (Bit8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_USE_XO_FOR_LP_CAL_CLK                                      (Bit9)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPIFLASH_CLKSEL                                            (Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPIFLASH_CLKSEL_MASK                                       (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPIFLASH_CLKSEL_SHIFT                                      (12)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPIFLASH_DIV_CLK_SEL                                       (Bit16)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_IDRAM_1_MEM_IQ_BYP_EN                                      (Bit19)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPI_FLASH_CLOCK_DIV_VALUE                                  (Bit20+Bit21+Bit22+Bit23+Bit24+Bit25+Bit26+Bit27)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPI_FLASH_CLOCK_DIV_VALUE_MASK                             (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_SPI_FLASH_CLOCK_DIV_VALUE_SHIFT                            (20)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_INVERT_UART_IF_CLK                                         (Bit28)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_INVERT_UART_1_IF_CLK                                       (Bit29)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_EXT_SPI_MODE_CPHA                                          (Bit30)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_EXT_SPI_MODE_CPOL                                          (Bit31)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_MBIST                                                              (LPMCU_CORTEX_MISC_REGS_BASE + 0x1c)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_MBIST_DEFAULT                                                      0x0
#define LPMCU_CORTEX_MISC_REGS_LPMCU_MBIST_MBIST_MODE                                                   (Bit0)


#define LPMCU_CORTEX_MISC_REGS_BLE_CTRL                                                                 (LPMCU_CORTEX_MISC_REGS_BASE + 0x20)
#define LPMCU_CORTEX_MISC_REGS_BLE_CTRL_DEFAULT                                                         0x0
#define LPMCU_CORTEX_MISC_REGS_BLE_CTRL_BLE_WAKEUP_REQ                                                  (Bit0)
#define LPMCU_CORTEX_MISC_REGS_BLE_CTRL_BLE_WAKEUP_REQ_CORE                                             (Bit1)


#define LPMCU_CORTEX_MISC_REGS_MBIST_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x24)
#define LPMCU_CORTEX_MISC_REGS_MBIST_CONTROL_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x28)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_DEFAULT                                                     0x0
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_BUS                                                   (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_BUS_MASK                                              (Bit20+Bit19+Bit18+Bit17+Bit16+Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_BUS_SHIFT                                             (0)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_BLOCK_SEL                                             (Bit21+Bit22+Bit23+Bit24+Bit25)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_BLOCK_SEL_MASK                                        (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_BLOCK_SEL_SHIFT                                       (21)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_WRAPPER_SEL                                           (Bit26+Bit27+Bit28+Bit29+Bit30)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_WRAPPER_SEL_MASK                                      (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_WRAPPER_SEL_SHIFT                                     (26)
#define LPMCU_CORTEX_MISC_REGS_MBIST_STATUS_MBIST_MODE                                                  (Bit31)


#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL                                                       (LPMCU_CORTEX_MISC_REGS_BASE + 0x2c)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_DEFAULT                                               0x4F
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_MARCH2_ENABLE                                         (Bit0)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_UNIQUE_ENABLE                                         (Bit1)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_CHECKERBOARD_ENABLE                                   (Bit2)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_DIAGONAL_ENABLE                                       (Bit3)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_ADDRDEC0_ENABLE                                       (Bit4)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_ADDRDEC1_ENABLE                                       (Bit5)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_COLMARCH1_ENABLE                                      (Bit6)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_FILLWITH0_ENABLE                                      (Bit7)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_READ_ZEROS_ENABLE                                 (Bit8)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_WRITE_ZEROS_ENABLE                                (Bit9)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_READ_ONES_ENABLE                                  (Bit10)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_WRITE_ONES_ENABLE                                 (Bit11)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_READ_CB_ENABLE                                    (Bit12)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_WRITE_CB_ENABLE                                   (Bit13)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_READ_INVCB_ENABLE                                 (Bit14)
#define LPMCU_CORTEX_MISC_REGS_MBIST_SRAM_ALG_SEL_RET_WRITE_INVCB_ENABLE                                (Bit15)


#define LPMCU_CORTEX_MISC_REGS_MBIST_BG                                                                 (LPMCU_CORTEX_MISC_REGS_BASE + 0x30)
#define LPMCU_CORTEX_MISC_REGS_MBIST_BG_DEFAULT                                                         0x0


#define LPMCU_CORTEX_MISC_REGS_TEST_BUS_CONTROL                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x38)
#define LPMCU_CORTEX_MISC_REGS_TEST_BUS_CONTROL_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_TEST_BUS_CONTROL_TEST_ENABLE_ARM                                         (Bit0)


#define LPMCU_CORTEX_MISC_REGS_WP3_MISR_DATAOUT                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x3c)
#define LPMCU_CORTEX_MISC_REGS_WP3_MISR_DATAOUT_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL                                                       (LPMCU_CORTEX_MISC_REGS_BASE + 0x40)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_DEFAULT                                               0x0
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_0                                             (Bit0)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_1                                             (Bit1)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_2                                             (Bit2)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_3                                             (Bit3)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_4                                             (Bit4)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_5                                             (Bit5)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_6                                             (Bit6)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_7                                             (Bit7)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_8                                             (Bit8)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_9                                             (Bit9)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_10                                            (Bit10)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_11                                            (Bit11)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_12                                            (Bit12)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_13                                            (Bit13)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_14                                            (Bit14)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_15                                            (Bit15)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_16                                            (Bit16)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_17                                            (Bit17)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_18                                            (Bit18)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_19                                            (Bit19)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_20                                            (Bit20)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_21                                            (Bit21)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_22                                            (Bit22)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_23                                            (Bit23)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_GPIO_24                                            (Bit24)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_SIP_0                                              (Bit27)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_SIP_1                                              (Bit28)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_SIP_2                                              (Bit29)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_SIP_3                                              (Bit30)
#define LPMCU_CORTEX_MISC_REGS_INVERT_OUTPUT_CTRL_LP_SIP_4                                              (Bit31)


#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x44)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_DEFAULT                                                     0x22
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_0_SEL                                               (Bit0+Bit1+Bit2)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_0_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_0_SEL_SHIFT                                         (0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_1_SEL                                               (Bit4+Bit5+Bit6)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_1_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_1_SEL_SHIFT                                         (4)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_2_SEL                                               (Bit8+Bit9+Bit10)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_2_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_2_SEL_SHIFT                                         (8)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_3_SEL                                               (Bit12+Bit13+Bit14)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_3_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_3_SEL_SHIFT                                         (12)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_4_SEL                                               (Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_4_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_4_SEL_SHIFT                                         (16)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_5_SEL                                               (Bit20+Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_5_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_5_SEL_SHIFT                                         (20)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_6_SEL                                               (Bit24+Bit25+Bit26)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_6_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_6_SEL_SHIFT                                         (24)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_7_SEL                                               (Bit28+Bit29+Bit30)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_7_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_0_LP_GPIO_7_SEL_SHIFT                                         (28)


#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x48)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_DEFAULT                                                     0x33
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_8_SEL                                               (Bit0+Bit1+Bit2)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_8_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_8_SEL_SHIFT                                         (0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_9_SEL                                               (Bit4+Bit5+Bit6)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_9_SEL_MASK                                          (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_9_SEL_SHIFT                                         (4)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_10_SEL                                              (Bit8+Bit9+Bit10)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_10_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_10_SEL_SHIFT                                        (8)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_11_SEL                                              (Bit12+Bit13+Bit14)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_11_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_11_SEL_SHIFT                                        (12)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_12_SEL                                              (Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_12_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_12_SEL_SHIFT                                        (16)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_13_SEL                                              (Bit20+Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_13_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_13_SEL_SHIFT                                        (20)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_14_SEL                                              (Bit24+Bit25+Bit26)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_14_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_14_SEL_SHIFT                                        (24)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_15_SEL                                              (Bit28+Bit29+Bit30)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_15_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_1_LP_GPIO_15_SEL_SHIFT                                        (28)


#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x4c)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_DEFAULT                                                     0x0
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_16_SEL                                              (Bit0+Bit1+Bit2)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_16_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_16_SEL_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_17_SEL                                              (Bit4+Bit5+Bit6)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_17_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_17_SEL_SHIFT                                        (4)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_18_SEL                                              (Bit8+Bit9+Bit10)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_18_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_18_SEL_SHIFT                                        (8)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_19_SEL                                              (Bit12+Bit13+Bit14)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_19_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_19_SEL_SHIFT                                        (12)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_20_SEL                                              (Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_20_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_20_SEL_SHIFT                                        (16)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_21_SEL                                              (Bit20+Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_21_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_21_SEL_SHIFT                                        (20)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_22_SEL                                              (Bit24+Bit25+Bit26)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_22_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_22_SEL_SHIFT                                        (24)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_23_SEL                                              (Bit28+Bit29+Bit30)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_23_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_2_LP_GPIO_23_SEL_SHIFT                                        (28)


#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE                                                              (LPMCU_CORTEX_MISC_REGS_BASE + 0x50)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_DEFAULT                                                      0x0
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_0                                                    (Bit0)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_1                                                    (Bit1)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_2                                                    (Bit2)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_3                                                    (Bit3)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_4                                                    (Bit4)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_5                                                    (Bit5)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_6                                                    (Bit6)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_7                                                    (Bit7)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_8                                                    (Bit8)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_9                                                    (Bit9)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_10                                                   (Bit10)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_11                                                   (Bit11)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_12                                                   (Bit12)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_13                                                   (Bit13)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_14                                                   (Bit14)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_15                                                   (Bit15)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_16                                                   (Bit16)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_17                                                   (Bit17)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_18                                                   (Bit18)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_19                                                   (Bit19)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_20                                                   (Bit20)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_21                                                   (Bit21)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_22                                                   (Bit22)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_23                                                   (Bit23)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_GPIO_24                                                   (Bit24)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_SIP_0                                                     (Bit27)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_SIP_1                                                     (Bit28)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_SIP_2                                                     (Bit29)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_SIP_3                                                     (Bit30)
#define LPMCU_CORTEX_MISC_REGS_PULL_ENABLE_LP_SIP_4                                                     (Bit31)


#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0                                                              (LPMCU_CORTEX_MISC_REGS_BASE + 0x54)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_DEFAULT                                                      0x0
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_0                                                    (Bit0)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_1                                                    (Bit1)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_2                                                    (Bit2)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_3                                                    (Bit3)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_4                                                    (Bit4)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_5                                                    (Bit5)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_6                                                    (Bit6)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_7                                                    (Bit7)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_8                                                    (Bit8)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_9                                                    (Bit9)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_10                                                   (Bit10)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_11                                                   (Bit11)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_12                                                   (Bit12)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_13                                                   (Bit13)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_14                                                   (Bit14)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_15                                                   (Bit15)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_16                                                   (Bit16)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_17                                                   (Bit17)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_18                                                   (Bit18)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_19                                                   (Bit19)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_20                                                   (Bit20)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_21                                                   (Bit21)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_22                                                   (Bit22)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_23                                                   (Bit23)
#define LPMCU_CORTEX_MISC_REGS_RTYPE_PAD_0_LP_GPIO_24                                                   (Bit24)


#define LPMCU_CORTEX_MISC_REGS_FPGA_PROG_XROM_MEM_REG                                                   (LPMCU_CORTEX_MISC_REGS_BASE + 0x60)
#define LPMCU_CORTEX_MISC_REGS_FPGA_PROG_XROM_MEM_REG_DEFAULT                                           0x0


#define LPMCU_CORTEX_MISC_REGS_FPGA_PROG_XROM_MEM_DATA_REG                                              (LPMCU_CORTEX_MISC_REGS_BASE + 0x64)
#define LPMCU_CORTEX_MISC_REGS_FPGA_PROG_XROM_MEM_DATA_REG_DEFAULT                                      0x0


#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x68)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_DEFAULT                                                     0x0
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_0_SEL                                                (Bit0+Bit1+Bit2)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_0_SEL_MASK                                           (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_0_SEL_SHIFT                                          (0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_1_SEL                                                (Bit4+Bit5+Bit6)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_1_SEL_MASK                                           (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_1_SEL_SHIFT                                          (4)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_2_SEL                                                (Bit8+Bit9+Bit10)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_2_SEL_MASK                                           (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_2_SEL_SHIFT                                          (8)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_3_SEL                                                (Bit12+Bit13+Bit14)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_3_SEL_MASK                                           (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_3_SEL_SHIFT                                          (12)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_4_SEL                                                (Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_4_SEL_MASK                                           (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_3_LP_SIP_4_SEL_SHIFT                                          (16)


#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x6c)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_DEFAULT                                                     0x55555
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_0                                                    (Bit0+Bit1+Bit2)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_0_MASK                                               (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_0_SHIFT                                              (0)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_1                                                    (Bit4+Bit5+Bit6)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_1_MASK                                               (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_1_SHIFT                                              (4)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_2                                                    (Bit8+Bit9+Bit10)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_2_MASK                                               (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_2_SHIFT                                              (8)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_3                                                    (Bit12+Bit13+Bit14)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_3_MASK                                               (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_3_SHIFT                                              (12)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_4                                                    (Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_4_MASK                                               (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_ISHAPE_PAD_3_LP_SIP_4_SHIFT                                              (16)


#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_4                                                             (LPMCU_CORTEX_MISC_REGS_BASE + 0x80)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_4_DEFAULT                                                     0x0
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_4_LP_GPIO_24_SEL                                              (Bit0+Bit1+Bit2)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_4_LP_GPIO_24_SEL_MASK                                         (Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PINMUX_SEL_4_LP_GPIO_24_SEL_SHIFT                                        (0)


#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_2                                                          (LPMCU_CORTEX_MISC_REGS_BASE + 0x90)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_2_DEFAULT                                                  0x0
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_2_SPI_FLASH_CLOCK_FRAC_VALUE                               (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_2_SPI_FLASH_CLOCK_FRAC_VALUE_MASK                          (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_CONTROL_2_SPI_FLASH_CLOCK_FRAC_VALUE_SHIFT                         (0)


#define LPMCU_CORTEX_MISC_REGS_SPIFLASH_VDDIO_CTRL                                                      (LPMCU_CORTEX_MISC_REGS_BASE + 0xa0)
#define LPMCU_CORTEX_MISC_REGS_SPIFLASH_VDDIO_CTRL_DEFAULT                                              0x1
#define LPMCU_CORTEX_MISC_REGS_SPIFLASH_VDDIO_CTRL_ENABLE                                               (Bit0)


#define LPMCU_CORTEX_MISC_REGS_SPIFLASH_BYPASS                                                          (LPMCU_CORTEX_MISC_REGS_BASE + 0xa4)
#define LPMCU_CORTEX_MISC_REGS_SPIFLASH_BYPASS_DEFAULT                                                  0x0
#define LPMCU_CORTEX_MISC_REGS_SPIFLASH_BYPASS_ENABLE                                                   (Bit0)


#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0xc0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_0                                                   (Bit0+Bit1+Bit2+Bit3+Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_0_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_0_SHIFT                                             (0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_1                                                   (Bit8+Bit9+Bit10+Bit11+Bit12)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_1_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_1_SHIFT                                             (8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_2                                                   (Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_2_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_2_SHIFT                                             (16)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_3                                                   (Bit24+Bit25+Bit26+Bit27+Bit28)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_3_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_0_MUX_3_SHIFT                                             (24)


#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0xc4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_4                                                   (Bit0+Bit1+Bit2+Bit3+Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_4_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_4_SHIFT                                             (0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_5                                                   (Bit8+Bit9+Bit10+Bit11+Bit12)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_5_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_5_SHIFT                                             (8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_6                                                   (Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_6_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_6_SHIFT                                             (16)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_7                                                   (Bit24+Bit25+Bit26+Bit27+Bit28)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_7_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_1_MUX_7_SHIFT                                             (24)


#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0xc8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_8                                                   (Bit0+Bit1+Bit2+Bit3+Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_8_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_8_SHIFT                                             (0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_9                                                   (Bit8+Bit9+Bit10+Bit11+Bit12)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_9_MASK                                              (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_9_SHIFT                                             (8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_10                                                  (Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_10_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_10_SHIFT                                            (16)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_11                                                  (Bit24+Bit25+Bit26+Bit27+Bit28)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_11_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_2_MUX_11_SHIFT                                            (24)


#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0xcc)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_12                                                  (Bit0+Bit1+Bit2+Bit3+Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_12_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_12_SHIFT                                            (0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_13                                                  (Bit8+Bit9+Bit10+Bit11+Bit12)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_13_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_13_SHIFT                                            (8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_14                                                  (Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_14_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_14_SHIFT                                            (16)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_15                                                  (Bit24+Bit25+Bit26+Bit27+Bit28)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_15_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_3_MUX_15_SHIFT                                            (24)


#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0xd0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_16                                                  (Bit0+Bit1+Bit2+Bit3+Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_16_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_16_SHIFT                                            (0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_17                                                  (Bit8+Bit9+Bit10+Bit11+Bit12)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_17_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_17_SHIFT                                            (8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_18                                                  (Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_18_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_18_SHIFT                                            (16)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_19                                                  (Bit24+Bit25+Bit26+Bit27+Bit28)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_19_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_4_MUX_19_SHIFT                                            (24)


#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_5                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0xd4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_5_DEFAULT                                                 0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_5_MUX_20                                                  (Bit0+Bit1+Bit2+Bit3+Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_5_MUX_20_MASK                                             (Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_MUX_IO_SEL_5_MUX_20_SHIFT                                            (0)


#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x160)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_DEFAULT                                                    0x0
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_PWM_EN                                                     (Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_OUTPUT_POLARITY                                            (Bit1)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_AGCDATA_FMT                                                (Bit2)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_SAMPLE_METHOD                                              (Bit3)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_USE_OLD_PWM                                                (Bit4)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_PWM_PERIOD                                                 (Bit5+Bit6+Bit7+Bit8)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_PWM_PERIOD_MASK                                            (Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_PWM_PERIOD_SHIFT                                           (5)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_AGCDATA_IN                                                 (Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_AGCDATA_IN_MASK                                            (Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_AGCDATA_IN_SHIFT                                           (9)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_USE_AGCUPDATE                                              (Bit19)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_AGCUPDATE                                                  (Bit20)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_CLOCK_SEL                                                  (Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_CLOCK_SEL_MASK                                             (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_1_CONTROL_CLOCK_SEL_SHIFT                                            (21)


#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x164)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_DEFAULT                                                    0x0
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_PWM_EN                                                     (Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_OUTPUT_POLARITY                                            (Bit1)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_AGCDATA_FMT                                                (Bit2)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_SAMPLE_METHOD                                              (Bit3)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_USE_OLD_PWM                                                (Bit4)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_PWM_PERIOD                                                 (Bit5+Bit6+Bit7+Bit8)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_PWM_PERIOD_MASK                                            (Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_PWM_PERIOD_SHIFT                                           (5)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_AGCDATA_IN                                                 (Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_AGCDATA_IN_MASK                                            (Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_AGCDATA_IN_SHIFT                                           (9)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_USE_AGCUPDATE                                              (Bit19)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_AGCUPDATE                                                  (Bit20)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_CLOCK_SEL                                                  (Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_CLOCK_SEL_MASK                                             (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_2_CONTROL_CLOCK_SEL_SHIFT                                            (21)


#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x168)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_DEFAULT                                                    0x0
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_PWM_EN                                                     (Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_OUTPUT_POLARITY                                            (Bit1)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_AGCDATA_FMT                                                (Bit2)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_SAMPLE_METHOD                                              (Bit3)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_USE_OLD_PWM                                                (Bit4)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_PWM_PERIOD                                                 (Bit5+Bit6+Bit7+Bit8)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_PWM_PERIOD_MASK                                            (Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_PWM_PERIOD_SHIFT                                           (5)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_AGCDATA_IN                                                 (Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_AGCDATA_IN_MASK                                            (Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_AGCDATA_IN_SHIFT                                           (9)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_USE_AGCUPDATE                                              (Bit19)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_AGCUPDATE                                                  (Bit20)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_CLOCK_SEL                                                  (Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_CLOCK_SEL_MASK                                             (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_3_CONTROL_CLOCK_SEL_SHIFT                                            (21)


#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x16c)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_DEFAULT                                                    0x0
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_PWM_EN                                                     (Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_OUTPUT_POLARITY                                            (Bit1)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_AGCDATA_FMT                                                (Bit2)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_SAMPLE_METHOD                                              (Bit3)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_USE_OLD_PWM                                                (Bit4)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_PWM_PERIOD                                                 (Bit5+Bit6+Bit7+Bit8)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_PWM_PERIOD_MASK                                            (Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_PWM_PERIOD_SHIFT                                           (5)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_AGCDATA_IN                                                 (Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_AGCDATA_IN_MASK                                            (Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_AGCDATA_IN_SHIFT                                           (9)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_USE_AGCUPDATE                                              (Bit19)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_AGCUPDATE                                                  (Bit20)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_CLOCK_SEL                                                  (Bit21+Bit22)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_CLOCK_SEL_MASK                                             (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PWM_4_CONTROL_CLOCK_SEL_SHIFT                                            (21)


#define LPMCU_CORTEX_MISC_REGS_MBIST_DUAL_STATUS                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x170)
#define LPMCU_CORTEX_MISC_REGS_MBIST_DUAL_STATUS_DEFAULT                                                0x0
#define LPMCU_CORTEX_MISC_REGS_MBIST_DUAL_STATUS_MBIST_BUS                                              (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18+Bit19+Bit20)
#define LPMCU_CORTEX_MISC_REGS_MBIST_DUAL_STATUS_MBIST_BUS_MASK                                         (Bit20+Bit19+Bit18+Bit17+Bit16+Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MBIST_DUAL_STATUS_MBIST_BUS_SHIFT                                        (0)


#define LPMCU_CORTEX_MISC_REGS_DUALTIMER_CTRL                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x188)
#define LPMCU_CORTEX_MISC_REGS_DUALTIMER_CTRL_DEFAULT                                                   0x0
#define LPMCU_CORTEX_MISC_REGS_DUALTIMER_CTRL_CNTR_1_ENABLE                                             (Bit0)
#define LPMCU_CORTEX_MISC_REGS_DUALTIMER_CTRL_CNTR_2_ENABLE                                             (Bit1)


#define LPMCU_CORTEX_MISC_REGS_ARM_SLEEP_WAKEUP_CTRL                                                    (LPMCU_CORTEX_MISC_REGS_BASE + 0x190)
#define LPMCU_CORTEX_MISC_REGS_ARM_SLEEP_WAKEUP_CTRL_DEFAULT                                            0x1000000
#define LPMCU_CORTEX_MISC_REGS_ARM_SLEEP_WAKEUP_CTRL_SLEEP_1                                            (Bit0)
#define LPMCU_CORTEX_MISC_REGS_ARM_SLEEP_WAKEUP_CTRL_WAKEUP                                             (Bit16)
#define LPMCU_CORTEX_MISC_REGS_ARM_SLEEP_WAKEUP_CTRL_RXTX_WAKEUP                                        (Bit24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1a0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_DEFAULT                                                0x3F3F3F3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_0                                              (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_0_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_0_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_1                                              (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_1_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_1_SHIFT                                        (8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_2                                              (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_2_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_2_SHIFT                                        (16)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_3                                              (Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_3_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_0_LP_GPIO_3_SHIFT                                        (24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1a4)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_DEFAULT                                                0x3F3F3F3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_4                                              (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_4_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_4_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_5                                              (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_5_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_5_SHIFT                                        (8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_6                                              (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_6_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_6_SHIFT                                        (16)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_7                                              (Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_7_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_1_LP_GPIO_7_SHIFT                                        (24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1a8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_DEFAULT                                                0x3F3F3F3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_8                                              (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_8_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_8_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_9                                              (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_9_MASK                                         (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_9_SHIFT                                        (8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_10                                             (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_10_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_10_SHIFT                                       (16)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_11                                             (Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_11_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_2_LP_GPIO_11_SHIFT                                       (24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1ac)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_DEFAULT                                                0x3F3F3F3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_12                                             (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_12_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_12_SHIFT                                       (0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_13                                             (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_13_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_13_SHIFT                                       (8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_14                                             (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_14_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_14_SHIFT                                       (16)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_15                                             (Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_15_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_3_LP_GPIO_15_SHIFT                                       (24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1b0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_DEFAULT                                                0x3F3F3F3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_16                                             (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_16_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_16_SHIFT                                       (0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_17                                             (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_17_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_17_SHIFT                                       (8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_18                                             (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_18_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_18_SHIFT                                       (16)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_19                                             (Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_19_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_4_LP_GPIO_19_SHIFT                                       (24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1b4)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_DEFAULT                                                0x3F3F3F3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_20                                             (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_20_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_20_SHIFT                                       (0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_21                                             (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_21_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_21_SHIFT                                       (8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_22                                             (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_22_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_22_SHIFT                                       (16)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_23                                             (Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_23_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_5_LP_GPIO_23_SHIFT                                       (24)


#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_6                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1b8)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_6_DEFAULT                                                0x3F
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_6_LP_GPIO_24                                             (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_6_LP_GPIO_24_MASK                                        (Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_MEGA_MUX_IO_SEL_6_LP_GPIO_24_SHIFT                                       (0)


#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1c0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_DEFAULT                                                0x1A00
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_FRAC_PART                                              (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_FRAC_PART_MASK                                         (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_FRAC_PART_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_INT_PART                                               (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18+Bit19)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_INT_PART_MASK                                          (Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_INT_PART_SHIFT                                         (8)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CLK_CTRL_INVERT                                                 (Bit20)


#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS                                                      (LPMCU_CORTEX_MISC_REGS_BASE + 0x1c4)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_DEFAULT                                              0x0
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_OUT                                              (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_OUT_MASK                                         (Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_OUT_SHIFT                                        (0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_CH                                               (Bit12+Bit13)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_CH_MASK                                          (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_CH_SHIFT                                         (12)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_RAW_STATUS_ADC_DONE                                             (Bit16)


#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH0_DATA                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1c8)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH0_DATA_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH1_DATA                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1cc)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH1_DATA_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH2_DATA                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1d0)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH2_DATA_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH3_DATA                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x1d4)
#define LPMCU_CORTEX_MISC_REGS_SENS_ADC_CH3_DATA_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL                                                                 (LPMCU_CORTEX_MISC_REGS_BASE + 0x1e8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_DEFAULT                                                         0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_OSC_EN_IRQ_EN                                           (Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_OSC_EN_IRQ_CLR                                          (Bit1)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_WAKEUP_LP_IRQ_EN                                        (Bit2)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_WAKEUP_LP_IRQ_CLR                                       (Bit3)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_BLE_LP_OUT_OF_RESET_IRQ_EN                              (Bit4)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_BLE_LP_OUT_OF_RESET_IRQ_CLR                             (Bit5)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_BLE_CORE_OUT_OF_RESET_IRQ_EN                            (Bit6)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_BLE_LVL_BLE_CORE_OUT_OF_RESET_IRQ_CLR                           (Bit7)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_RXTX_LVL_RXTX_SEQ_IN_SLEEP_IRQ_EN                               (Bit8)
#define LPMCU_CORTEX_MISC_REGS_IRQ_CTRL_RXTX_LVL_RXTX_SEQ_IN_SLEEP_IRQ_CLR                              (Bit9)


#define LPMCU_CORTEX_MISC_REGS_IRQ_STS                                                                  (LPMCU_CORTEX_MISC_REGS_BASE + 0x1ec)
#define LPMCU_CORTEX_MISC_REGS_IRQ_STS_DEFAULT                                                          0x0
#define LPMCU_CORTEX_MISC_REGS_IRQ_STS_BLE_OSC_EN_IRQ                                                   (Bit0)
#define LPMCU_CORTEX_MISC_REGS_IRQ_STS_BLE_WAKEUP_LP_IRQ                                                (Bit1)
#define LPMCU_CORTEX_MISC_REGS_IRQ_STS_BLE_LP_OUT_OF_RESET_IRQ                                          (Bit2)
#define LPMCU_CORTEX_MISC_REGS_IRQ_STS_BLE_CORE_OUT_OF_RESET_IRQ_EN                                     (Bit3)
#define LPMCU_CORTEX_MISC_REGS_IRQ_STS_EFUSE_OUT_OF_RESET                                               (Bit4)


#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x200)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_DEFAULT                                                   0x0
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD2A_MEM_RWM_EN                                           (Bit0)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD2A_MEM_WM_EN                                            (Bit1)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD2A_MEM_RM_EN                                            (Bit2)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD2B_MEM_RWM_EN                                           (Bit4)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD2B_MEM_WM_EN                                            (Bit5)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD2B_MEM_RM_EN                                            (Bit6)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3A_MEM_RWM_EN                                           (Bit8)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3A_MEM_WM_EN                                            (Bit9)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3A_MEM_RM_EN                                            (Bit10)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3B_MEM_RWM_EN                                           (Bit12)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3B_MEM_WM_EN                                            (Bit13)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3B_MEM_RM_EN                                            (Bit14)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3C_MEM_RWM_EN                                           (Bit16)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3C_MEM_WM_EN                                            (Bit17)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD3C_MEM_RM_EN                                            (Bit18)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD5_MEM_RWM_EN                                            (Bit20)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD5_MEM_WM_EN                                             (Bit21)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD5_MEM_RM_EN                                             (Bit22)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD8_MEM_RWM_EN                                            (Bit24)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD8_MEM_WM_EN                                             (Bit25)
#define LPMCU_CORTEX_MISC_REGS_MSEMI_MEM_CTRL_PD8_MEM_RM_EN                                             (Bit26)


#define LPMCU_CORTEX_MISC_REGS_EFUSE_1_STATUS_3                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x204)
#define LPMCU_CORTEX_MISC_REGS_EFUSE_1_STATUS_3_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL                                                      (LPMCU_CORTEX_MISC_REGS_BASE + 0x210)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_DEFAULT                                              0xF7F
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_SCLK_EN                                              (Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_SCLK_VAL                                             (Bit1)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_DCLK_EN                                              (Bit2)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_DCLK_VAL                                             (Bit3)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_PCLKG_EN                                             (Bit4)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_PCLKG_VAL                                            (Bit5)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_WICENREQ_EN                                          (Bit6)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_WICENREQ_VAL                                         (Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_GPIO_GCLK_EN                                         (Bit8)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_COUNTER_PGCLK_EN                                     (Bit9)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_DUALTIMER_PGCLK_EN                                   (Bit10)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_WICENACK_EN                                          (Bit11)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_FORCE_CONTROL_WICENACK_VAL                                         (Bit12)


#define LPMCU_CORTEX_MISC_REGS_ARM_IRQ_STATUS_0                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x214)
#define LPMCU_CORTEX_MISC_REGS_ARM_IRQ_STATUS_0_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_ARM_IRQ_STATUS_1                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x218)
#define LPMCU_CORTEX_MISC_REGS_ARM_IRQ_STATUS_1_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_BLE_DEEP_SLEEP_ENABLES                                                   (LPMCU_CORTEX_MISC_REGS_BASE + 0x240)
#define LPMCU_CORTEX_MISC_REGS_BLE_DEEP_SLEEP_ENABLES_DEFAULT                                           0x1
#define LPMCU_CORTEX_MISC_REGS_BLE_DEEP_SLEEP_ENABLES_FW_TIMER_CORR_EN                                  (Bit0)
#define LPMCU_CORTEX_MISC_REGS_BLE_DEEP_SLEEP_ENABLES_HW_TIMER_CORR_EN                                  (Bit1)


#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR                                                 (LPMCU_CORTEX_MISC_REGS_BASE + 0x244)
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_DEFAULT                                         0xF424000
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_WAIT                                            (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9)
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_WAIT_MASK                                       (Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_WAIT_SHIFT                                      (0)
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_RTC_TOSC                                        (Bit12+Bit13+Bit14+Bit15+Bit16+Bit17+Bit18+Bit19+Bit20+Bit21+Bit22+Bit23+Bit24+Bit25+Bit26+Bit27+Bit28+Bit29)
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_RTC_TOSC_MASK                                   (Bit17+Bit16+Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_DEEP_SLEEP_HW_TIMER_CORR_RTC_TOSC_SHIFT                                  (12)


#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM1_BASE_ADDR                                                   (LPMCU_CORTEX_MISC_REGS_BASE + 0x254)
#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM1_BASE_ADDR_DEFAULT                                           0x8000


#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM1_END_ADDR                                                    (LPMCU_CORTEX_MISC_REGS_BASE + 0x258)
#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM1_END_ADDR_DEFAULT                                            0x1FFFF


#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM2_BASE_ADDR                                                   (LPMCU_CORTEX_MISC_REGS_BASE + 0x25c)
#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM2_BASE_ADDR_DEFAULT                                           0x0


#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM2_END_ADDR                                                    (LPMCU_CORTEX_MISC_REGS_BASE + 0x260)
#define LPMCU_CORTEX_MISC_REGS_REMAP_IDRAM2_END_ADDR_DEFAULT                                            0x7FFF


#define LPMCU_CORTEX_MISC_REGS_REMAP_BOOT_BASE_ADDR                                                     (LPMCU_CORTEX_MISC_REGS_BASE + 0x264)
#define LPMCU_CORTEX_MISC_REGS_REMAP_BOOT_BASE_ADDR_DEFAULT                                             0x10000000


#define LPMCU_CORTEX_MISC_REGS_REMAP_BOOT_END_ADDR                                                      (LPMCU_CORTEX_MISC_REGS_BASE + 0x268)
#define LPMCU_CORTEX_MISC_REGS_REMAP_BOOT_END_ADDR_DEFAULT                                              0x1001FFFF


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_IRQS                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x280)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_IRQS_DEFAULT                                                    0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_IRQS_QUAD_DEC_1_IRQ                                             (Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_IRQS_QUAD_DEC_2_IRQ                                             (Bit1)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_IRQS_QUAD_DEC_3_IRQ                                             (Bit2)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_STATUS                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x284)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_STATUS_DEFAULT                                                0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_STATUS_COUNT                                                  (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_STATUS_COUNT_MASK                                             (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_STATUS_COUNT_SHIFT                                            (0)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_STATUS                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x288)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_STATUS_DEFAULT                                                0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_STATUS_COUNT                                                  (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_STATUS_COUNT_MASK                                             (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_STATUS_COUNT_SHIFT                                            (0)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_STATUS                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x28c)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_STATUS_DEFAULT                                                0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_STATUS_COUNT                                                  (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_STATUS_COUNT_MASK                                             (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_STATUS_COUNT_SHIFT                                            (0)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD                                                     (LPMCU_CORTEX_MISC_REGS_BASE + 0x290)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_DEFAULT                                             0xFFFF00
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_UPPER                                               (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_UPPER_MASK                                          (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_UPPER_SHIFT                                         (0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_LOWER                                               (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21+Bit22+Bit23+Bit24+Bit25+Bit26+Bit27+Bit28+Bit29+Bit30+Bit31)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_LOWER_MASK                                          (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_THRESHOLD_LOWER_SHIFT                                         (16)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL                                                          (LPMCU_CORTEX_MISC_REGS_BASE + 0x294)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL_DEFAULT                                                  0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL_ENABLE                                                   (Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL_CLR_IRQ                                                  (Bit1)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL_CLOCK_SEL                                                (Bit2+Bit3)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL_CLOCK_SEL_MASK                                           (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_1_CTRL_CLOCK_SEL_SHIFT                                          (2)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD                                                     (LPMCU_CORTEX_MISC_REGS_BASE + 0x2a0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_DEFAULT                                             0xFFFF00
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_UPPER                                               (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_UPPER_MASK                                          (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_UPPER_SHIFT                                         (0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_LOWER                                               (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21+Bit22+Bit23+Bit24+Bit25+Bit26+Bit27+Bit28+Bit29+Bit30+Bit31)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_LOWER_MASK                                          (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_THRESHOLD_LOWER_SHIFT                                         (16)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL                                                          (LPMCU_CORTEX_MISC_REGS_BASE + 0x2a4)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL_DEFAULT                                                  0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL_ENABLE                                                   (Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL_CLR_IRQ                                                  (Bit1)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL_CLOCK_SEL                                                (Bit2+Bit3)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL_CLOCK_SEL_MASK                                           (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_2_CTRL_CLOCK_SEL_SHIFT                                          (2)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD                                                     (LPMCU_CORTEX_MISC_REGS_BASE + 0x2b0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_DEFAULT                                             0xFFFF00
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_UPPER                                               (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7+Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_UPPER_MASK                                          (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_UPPER_SHIFT                                         (0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_LOWER                                               (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21+Bit22+Bit23+Bit24+Bit25+Bit26+Bit27+Bit28+Bit29+Bit30+Bit31)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_LOWER_MASK                                          (Bit15+Bit14+Bit13+Bit12+Bit11+Bit10+Bit9+Bit8+Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_THRESHOLD_LOWER_SHIFT                                         (16)


#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL                                                          (LPMCU_CORTEX_MISC_REGS_BASE + 0x2b4)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL_DEFAULT                                                  0x0
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL_ENABLE                                                   (Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL_CLR_IRQ                                                  (Bit1)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL_CLOCK_SEL                                                (Bit2+Bit3)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL_CLOCK_SEL_MASK                                           (Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_QUAD_DEC_3_CTRL_CLOCK_SEL_SHIFT                                          (2)


#define LPMCU_CORTEX_MISC_REGS_FP_COMP0_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2c0)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP0_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP1_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2c4)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP1_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP2_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2c8)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP2_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP3_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2cc)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP3_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP4_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2d0)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP4_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP5_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2d4)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP5_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP6_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2d8)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP6_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP7_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2dc)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP7_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP8_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2e0)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP8_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP9_COMP                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x2e4)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP9_COMP_DEFAULT                                                    0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP10_COMP                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x2e8)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP10_COMP_DEFAULT                                                   0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP11_COMP                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x2ec)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP11_COMP_DEFAULT                                                   0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP12_COMP                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x2f0)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP12_COMP_DEFAULT                                                   0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP13_COMP                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x2f4)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP13_COMP_DEFAULT                                                   0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP14_COMP                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x2f8)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP14_COMP_DEFAULT                                                   0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP15_COMP                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x2fc)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP15_COMP_DEFAULT                                                   0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP0_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x300)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP0_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP1_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x304)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP1_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP2_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x308)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP2_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP3_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x30c)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP3_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP4_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x310)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP4_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP5_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x314)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP5_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP6_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x318)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP6_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP7_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x31c)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP7_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP8_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x320)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP8_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP9_REPLACE                                                         (LPMCU_CORTEX_MISC_REGS_BASE + 0x324)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP9_REPLACE_DEFAULT                                                 0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP10_REPLACE                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x328)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP10_REPLACE_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP11_REPLACE                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x32c)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP11_REPLACE_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP12_REPLACE                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x330)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP12_REPLACE_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP13_REPLACE                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x334)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP13_REPLACE_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP14_REPLACE                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x338)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP14_REPLACE_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP15_REPLACE                                                        (LPMCU_CORTEX_MISC_REGS_BASE + 0x33c)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP15_REPLACE_DEFAULT                                                0x0


#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE                                                           (LPMCU_CORTEX_MISC_REGS_BASE + 0x340)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_DEFAULT                                                   0x0
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_0_EN                                                  (Bit0)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_1_EN                                                  (Bit1)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_2_EN                                                  (Bit2)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_3_EN                                                  (Bit3)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_4_EN                                                  (Bit4)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_5_EN                                                  (Bit5)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_6_EN                                                  (Bit6)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_7_EN                                                  (Bit7)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_8_EN                                                  (Bit8)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_9_EN                                                  (Bit9)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_10_EN                                                 (Bit10)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_11_EN                                                 (Bit11)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_12_EN                                                 (Bit12)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_13_EN                                                 (Bit13)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_14_EN                                                 (Bit14)
#define LPMCU_CORTEX_MISC_REGS_FP_COMP_ENABLE_CMP_15_EN                                                 (Bit15)


#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS                                                            (LPMCU_CORTEX_MISC_REGS_BASE + 0x410)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_DEFAULT                                                    0x0
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG1                                                   (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG1_MASK                                              (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG1_SHIFT                                             (0)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG2                                                   (Bit8+Bit9+Bit10+Bit11+Bit12+Bit13+Bit14+Bit15)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG2_MASK                                              (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG2_SHIFT                                             (8)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG3                                                   (Bit16+Bit17+Bit18+Bit19+Bit20+Bit21+Bit22+Bit23)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG3_MASK                                              (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_PMU_READ_REGS_READREG3_SHIFT                                             (16)


#define LPMCU_CORTEX_MISC_REGS_MS_GPIO                                                                  (LPMCU_CORTEX_MISC_REGS_BASE + 0x414)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_DEFAULT                                                          0xF0
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_TYPE_44                                                     (Bit0)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_TYPE_45                                                     (Bit1)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_TYPE_46                                                     (Bit2)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_TYPE_47                                                     (Bit3)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_ENABLE_44                                                   (Bit4)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_ENABLE_45                                                   (Bit5)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_ENABLE_46                                                   (Bit6)
#define LPMCU_CORTEX_MISC_REGS_MS_GPIO_PULL_ENABLE_47                                                   (Bit7)


#define LPMCU_CORTEX_MISC_REGS_AON_SLEEP_TIMER_COUNTER                                                  (LPMCU_CORTEX_MISC_REGS_BASE + 0x420)
#define LPMCU_CORTEX_MISC_REGS_AON_SLEEP_TIMER_COUNTER_DEFAULT                                          0x0


#define LPMCU_CORTEX_MISC_REGS_FPGA_REGS_0                                                              (LPMCU_CORTEX_MISC_REGS_BASE + 0x428)
#define LPMCU_CORTEX_MISC_REGS_FPGA_REGS_0_DEFAULT                                                      0x0


#define LPMCU_CORTEX_MISC_REGS_LPMCU_NMI_REVID                                                          (LPMCU_CORTEX_MISC_REGS_BASE + 0xffc)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_NMI_REVID_DEFAULT                                                  0x41
#define LPMCU_CORTEX_MISC_REGS_LPMCU_NMI_REVID_REV_ID                                                   (Bit0+Bit1+Bit2+Bit3+Bit4+Bit5+Bit6+Bit7)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_NMI_REVID_REV_ID_MASK                                              (Bit7+Bit6+Bit5+Bit4+Bit3+Bit2+Bit1+Bit0)
#define LPMCU_CORTEX_MISC_REGS_LPMCU_NMI_REVID_REV_ID_SHIFT                                             (0)

