/**
 *
 * \file
 *
 * \brief This module contains NMC1500 ASIC specific internal APIs.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "common/include/nm_common.h"
#include "driver/source/nmbus.h"
#include "bsp/include/nm_bsp.h"
#include "driver/source/nmasic.h"

#define NMI_GLB_RESET_0 (NMI_PERIPH_REG_BASE + 0x400)
#define NMI_GLB_RESET_1 (NMI_PERIPH_REG_BASE + 0x404)
#define NMI_INTR_REG_BASE (NMI_PERIPH_REG_BASE+0xa00)
#define NMI_PIN_MUX_0 (NMI_PERIPH_REG_BASE + 0x408)
#define NMI_INTR_ENABLE (NMI_INTR_REG_BASE)
#define GET_UINT32(X,Y) (X[0+Y] + ((uint32)X[1+Y]<<8) + ((uint32)X[2+Y]<<16) +((uint32)X[3+Y]<<24))

#define TIMEOUT				(2000)
#define M2M_DISABLE_PS        0xD0UL

#if (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B)
#ifndef CONF_WILC_USE_SDIO
static uint32 WILC_CLK_STATUS_REG = 0xf; /* Assume initially it is B0 chip */
#endif
#endif

/**
*	@fn		nm_clkless_wake
*	@brief	Wakeup the chip using clockless registers
*	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*	@author	Samer Sarhan
*	@date	06 June 2014
*	@version	1.0
*/
sint8 nm_clkless_wake(void)
{
	sint8 ret = M2M_SUCCESS;
	uint32 reg, clk_status_reg,trials = 0;

	ret = nm_read_reg_with_ret(WILC_WAKEUP_REG, &reg);
	if(ret != M2M_SUCCESS) {
		M2M_ERR("Bus error (1). Wake up failed\n");
		goto _WAKE_EXIT;
	}
	/*
	 * At this point, I am not sure whether it is B0 or A0
	 * If B0, then clks_enabled bit exists in register 0xf
	 * If A0, then clks_enabled bit exists in register 0xe
	 */
	do
	{
	#if (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B)
	#ifdef CONF_WILC_USE_SPI
		nm_write_reg(0x0b,1);
	#else
		nm_write_reg(0xFA,1);
	#endif
	#endif
		/* Set bit 1 */
		nm_write_reg(WILC_WAKEUP_REG, reg | WILC_WAKEUP_BIT);
		nm_bsp_sleep(1);
		// Check the clock status
		ret = nm_read_reg_with_ret(WILC_CLK_STATUS_REG, &clk_status_reg);
		if( (ret != M2M_SUCCESS) || ((ret == M2M_SUCCESS) && ((clk_status_reg & WILC_CLK_STATUS_BIT) == 0)) ) {
		#if ((defined CONF_WILC_SPI) && (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B))
			/* Register 0xf did not exist in A0.
			 * If register 0xf fails to read or if it reads 0,
			 * then the chip is A0.
			 */
			WILC_CLK_STATUS_REG = 0xe;
			ret = nm_read_reg_with_ret(WILC_CLK_STATUS_REG, &clk_status_reg);
		#endif
			if(ret != M2M_SUCCESS) {
				M2M_ERR("Bus error (2). Wake up failed\n");
				goto _WAKE_EXIT;
			}
		}

		// in case of clocks off, wait 2ms, and check it again.
		// if still off, wait for another 2ms, for a total wait of 6ms.
		// If still off, redo the wake up sequence
		while( ((clk_status_reg & WILC_CLK_STATUS_BIT) == 0) && (((++trials) %3) == 0))
		{
			/* Wait for the chip to stabilize*/
			nm_bsp_sleep(1);

			// Make sure chip is awake. This is an extra step that can be removed
			// later to avoid the bus access overhead
			nm_read_reg_with_ret(WILC_CLK_STATUS_REG, &clk_status_reg);

			if((clk_status_reg & WILC_CLK_STATUS_BIT) == 0)
			{
				M2M_ERR("clocks still OFF. Wake up failed\n");
			}
		}
		// in case of failure, Reset the wakeup bit to introduce a new edge on the next loop
		if((clk_status_reg & WILC_CLK_STATUS_BIT) == 0)
		{
			// Reset bit 0
			nm_write_reg(WILC_WAKEUP_REG, reg | WILC_WAKEUP_BIT);
		}
	} while((clk_status_reg & WILC_CLK_STATUS_BIT) == 0);

_WAKE_EXIT:
	return ret;
}
void chip_idle(void)
{
	uint32 reg =0;
	nm_read_reg_with_ret(WILC_WAKEUP_REG, &reg);
	if(reg & WILC_WAKEUP_BIT)
	{
		reg &=~WILC_WAKEUP_BIT;
		nm_write_reg(WILC_WAKEUP_REG, reg);
	}
}

void enable_rf_blocks(void)
{
#if (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B)
#ifdef CONF_WILC_USE_SPI
	nm_write_reg(0x6, 0xdb);
	nm_write_reg(0x7, 0x6);
	nm_bsp_sleep(10);
	nm_write_reg(0x1480, 0);
	nm_write_reg(0x1484, 0);
	nm_bsp_sleep(10);

	nm_write_reg(0x6, 0x0);
	nm_write_reg(0x7, 0x0);
#else
	#error "enable rf blocks SDIO clockless registers undefined"
#endif
#endif
}

sint8 enable_interrupts(void) 
{
	uint32 reg;
	sint8 ret;
	/**
	interrupt pin mux select
	**/
	ret = nm_read_reg_with_ret(NMI_PIN_MUX_0, &reg);
	if (M2M_SUCCESS != ret) {
		return M2M_ERR_BUS_FAIL;
	}
	reg |= ((uint32) 1 << 8);
	ret = nm_write_reg(NMI_PIN_MUX_0, reg);
	if (M2M_SUCCESS != ret) {
		return M2M_ERR_BUS_FAIL;
	}
	/**
	interrupt enable
	**/
	ret = nm_read_reg_with_ret(NMI_INTR_ENABLE, &reg);
	if (M2M_SUCCESS != ret) {
		return M2M_ERR_BUS_FAIL;
	}
	reg |= ((uint32) 1 << 16);
	ret = nm_write_reg(NMI_INTR_ENABLE, reg);
	if (M2M_SUCCESS != ret) {
		return M2M_ERR_BUS_FAIL;
	}
	return M2M_SUCCESS;
}

sint8 cpu_start(void) {
	uint32 reg;
	sint8 ret;

	/**
	reset regs 
	*/
#ifdef M2M_WILC1000
	/*disable boot rom*/
	nm_write_reg(0xc0000, 0x71);
#else
	nm_write_reg(BOOTROM_REG,0);
#endif
	nm_write_reg(NMI_STATE_REG,0);
	nm_write_reg(NMI_REV_REG,0);
	
	/**
	Go...
	**/
	ret = nm_read_reg_with_ret(0x1118, &reg);
	if (M2M_SUCCESS != ret) {
		ret = M2M_ERR_BUS_FAIL;
		M2M_ERR("[nmi start]: fail read reg 0x1118 ...\n");
	}
	reg |= (1 << 0);
	ret = nm_write_reg(0x1118, reg);
#if defined CONF_WILC_USE_SPI
	nm_write_reg(NMI_VMM_CORE_CFG, 1);
#elif defined CONF_WILC_USE_SDIO
	nm_write_reg(NMI_VMM_CORE_CFG, (1 << 3));
#endif
	ret += nm_read_reg_with_ret(NMI_GLB_RESET_0, &reg);
	if ((reg & (1ul << 10)) == (1ul << 10)) {
		reg &= ~(1ul << 10);
		ret += nm_write_reg(NMI_GLB_RESET_0, reg);
	}

	reg |= (1ul << 10);
	ret += nm_write_reg(NMI_GLB_RESET_0, reg);
	nm_bsp_sleep(1); /* TODO: Why bus error if this delay is not here. */
	return ret;
}

#ifdef CONF_WILC_USE_3000_REV_A
sint8 cpu_start_bt(void) {
	uint32 reg;
	sint8 ret = M2M_SUCCESS;

	nm_write_reg(0x4F000c, 0x10add09e);
	
	/**
	Go...
	**/
	reg = nm_read_reg(0x3B0400);
	reg &= ~((1ul << 2) | (1ul << 3));
	nm_write_reg(0x3B0400, reg);
	nm_bsp_sleep(100);
	reg |= ((1ul << 2) | (1ul << 3));
	nm_write_reg(0x3B0400, reg);
	
	nm_bsp_sleep(1); /* TODO: Why bus error if this delay is not here. */
	return ret;
}
#endif

#if (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B)
uint32 nmi_get_chipid(void)
{
	static uint32 chipid = 0;
	if (chipid == 0) {
		/*
		 * SDIO can't read into global variables
		 * Use this variable as a temp, then copy to the global
		 */
		uint32 temp_chipid = 0;
		uint32 rfrevid;

		if((nm_read_reg_with_ret(0x1000, &temp_chipid)) != M2M_SUCCESS) {
			chipid = 0;
			return 0;
		}
		//if((ret = nm_read_reg_with_ret(0x11fc, &revid)) != M2M_SUCCESS) {
		//	return 0;
		//}
		if((nm_read_reg_with_ret(0x13f4, &rfrevid)) != M2M_SUCCESS) {
			chipid = 0;
			return 0;
		}
		chipid = temp_chipid;

		if (chipid == 0x1002a0)  {
			if (rfrevid == 0x1) { /* 1002A0 */
			} else /* if (rfrevid == 0x2) */ { /* 1002A1 */
				chipid = 0x1002a1;
			}
		} else if(chipid == 0x1002b0) {
			if(rfrevid == 3) { /* 1002B0 */
			} else if(rfrevid == 4) { /* 1002B1 */
				chipid = 0x1002b1;
			} else /* if(rfrevid == 5) */ { /* 1002B2 */
				chipid = 0x1002b2;
			}
		} else {
		}
//#define PROBE_FLASH
#ifdef PROBE_FLASH
		if(chipid) {
			UWORD32 flashid;

			flashid = probe_spi_flash();
			if(flashid == 0x1230ef) {
				chipid &= ~(0x0f0000);
				chipid |= 0x050000;
			}
			if(flashid == 0xc21320c2) {
				chipid &= ~(0x0f0000);
				chipid |= 0x050000;
			}
		}
#else
		/*M2M is by default have SPI flash*/
		chipid &= ~(0x0f0000);
		chipid |= 0x050000;
#endif /* PROBE_FLASH */
	}
	return chipid;
}
#elif defined CONF_WILC_USE_3000_REV_A
uint32 nmi_get_chipid(void)
{
	static uint32 chipid = 0;

	if (chipid == 0) {
		/*
		 * SDIO can't read into global variables
		 * Use this variable as a temp, then copy to the global
		 */
		uint32 temp_chipid = 0;
		uint32 rfrevid;

		if((nm_read_reg_with_ret(0x3b0000, &temp_chipid)) != M2M_SUCCESS) {
			chipid = 0;
			return 0;
		}
		chipid = temp_chipid;
		//if((ret = nm_read_reg_with_ret(0x11fc, &revid)) != M2M_SUCCESS) {
		//	return 0;
		//}
		if (chipid == 0x3000D0)  {
			if((nm_read_reg_with_ret(0x13f4, &rfrevid)) != M2M_SUCCESS) {
				chipid = 0;
				return 0;
			}

			if (rfrevid == 0x6) { /* 1002A0 */
				chipid |= 0xD1;
			}
		
		}
//#define PROBE_FLASH
#ifdef PROBE_FLASH
		if(chipid) {
			UWORD32 flashid;

			flashid = probe_spi_flash();
			if((flashid != 0xffffffff) && (flashid != 0x0)) {
				chipid &= ~(0x0f0000);
				chipid |= 0x040000;
			}
		}
#else
		/*M2M is by default have SPI flash*/
		//chipid &= ~(0x0f0000);
		//chipid |= 0x050000;
#endif /* PROBE_FLASH */
	}
	return chipid;
}
#endif
uint32 nmi_get_rfrevid(void)
{
    uint32 rfrevid;
    if((nm_read_reg_with_ret(0x13f4, &rfrevid)) != M2M_SUCCESS) {
        rfrevid = 0;
        return 0;
    }
    return rfrevid;
}

void restore_pmu_settings_after_global_reset(void)
{
	/* 
	* Must restore PMU register value after 
	* global reset if PMU toggle is done at 
	* least once since the last hard reset.
	*/
#if (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B)
	if(REV(nmi_get_chipid()) >= REV_B0) {
		nm_write_reg(0x1e48, 0xb78469ce);
	}
#endif
}

void nmi_update_pll(void)
{
	uint32 pll;

	pll = nm_read_reg(0x1428);
	pll &= ~0x1ul;
	nm_write_reg(0x1428, pll);
	pll |= 0x1ul;
	nm_write_reg(0x1428, pll);

}
void nmi_set_sys_clk_src_to_xo(void) 
{
#if (defined CONF_WILC_USE_1000_REV_A || defined CONF_WILC_USE_1000_REV_B)
	uint32 val32;

	/* Switch system clock source to XO. This will take effect after nmi_update_pll(). */
	val32 = nm_read_reg(0x141c);
	val32 |= (1 << 2);
	nm_write_reg(0x141c, val32);
	
	/* Do PLL update */
	nmi_update_pll();
#endif
}
sint8 chip_wake(void)
{
	sint8 ret = M2M_SUCCESS;

	ret  = nm_clkless_wake();
	if(ret != M2M_SUCCESS) return ret;
	
	enable_rf_blocks();

	return ret;
}
sint8 chip_reset_and_cpu_halt(void)
{
	sint8 ret = M2M_SUCCESS;
	uint32 reg = 0;

	ret = chip_wake();
	if(ret != M2M_SUCCESS) {
		return ret;
	}
	chip_reset();
	ret = nm_read_reg_with_ret(0x1118, &reg);
	if (M2M_SUCCESS != ret) {
		ret = M2M_ERR_BUS_FAIL;
		M2M_ERR("[nmi start]: fail read reg 0x1118 ...\n");
	}
	reg |= (1 << 0);
	ret = nm_write_reg(0x1118, reg);
	ret += nm_read_reg_with_ret(NMI_GLB_RESET_0, &reg);
	if ((reg & (1ul << 10)) == (1ul << 10)) {
		reg &= ~(1ul << 10);
		ret += nm_write_reg(NMI_GLB_RESET_0, reg);
		ret += nm_read_reg_with_ret(NMI_GLB_RESET_0, &reg);
	}
#if 0
	reg |= (1ul << 10);
	ret += nm_write_reg(NMI_GLB_RESET_0, reg);
	ret += nm_read_reg_with_ret(NMI_GLB_RESET_0, &reg);
#endif
	nm_write_reg(BOOTROM_REG,0);
	nm_write_reg(NMI_STATE_REG,0);
	nm_write_reg(NMI_REV_REG,0);
	nm_write_reg(NMI_PIN_MUX_0, 0x11111000);
	return ret;
}
sint8 chip_reset(void)
{
	sint8 ret = M2M_SUCCESS;
#ifndef CONF_WILC_USE_UART
	nmi_set_sys_clk_src_to_xo();
#endif
#ifdef CONF_WILC_USE_SPI
	ret += nm_write_reg(NMI_GLB_RESET_0, 0);
#elif defined CONF_WILC_USE_SDIO
 /* Don't reset the SDIO modules*/
	ret += nm_write_reg(NMI_GLB_RESET_0, 0x80000301);
	ret += nm_write_reg(NMI_GLB_RESET_1, 0xFFFFFFFF);
#endif
	nm_bsp_sleep(50);
#ifndef CONF_WILC_USE_UART
	restore_pmu_settings_after_global_reset();
#endif
	return ret;
}
//#define __ROM_TEST__
#ifdef __ROM_TEST__
#include <stdlib.h>
#define ROM_FIRMWARE_FILE	"../../../firmware/dram_lib/Debug/wifi_release.bin"
#define ROM_DATA_FILE	"../../../firmware/dram_lib/Debug/wifi_release_data.bin"

#define CODE_SZ		(6 * 1024 - 256)
#define CODE_BASE	(0x30100ul)
#define DATA_BASE	(0xd0000 + CODE_SZ)

void rom_test()
{
	uint8	*pu8DataSec;
	uint8	*pu8TextSec;
	FILE	*fp;
	uint32	u32CodeSize = 0;
	uint32	u32DataSize = 0;

	nm_bsp_sleep(1000);

	/* Read text section.
	*/
	fp = fopen(ROM_FIRMWARE_FILE,"rb");
	if(fp)
	{
		/* Get the code size.
		*/
		fseek(fp, 0L, SEEK_END);
		u32CodeSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		pu8TextSec = (uint8*)malloc(u32CodeSize);
		if(pu8TextSec != NULL)
		{
			M2M_INFO("Code Size %f\n",u32CodeSize / 1024.0);
			fread(pu8TextSec, u32CodeSize, 1, fp);
			nm_write_block(CODE_BASE, pu8TextSec, (uint16)u32CodeSize);
			//nm_read_block(CODE_BASE, tmpv, sz);
			fclose(fp);
			free(pu8TextSec);
		}
	}
#if 0
	/* Read data section.
	*/
	fp = fopen(ROM_DATA_FILE,"rb");
	if(fp)
	{
		/* Get the data size.
		*/
		fseek(fp, 0L, SEEK_END);
		u32DataSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		pu8DataSec = (uint8*)malloc(u32DataSize);
		if(pu8DataSec != NULL)
		{
			M2M_INFO("Data Size %f\n",u32DataSize / 1024.0);
			fread(pu8DataSec, u32DataSize, 1, fp);
			nm_write_block(DATA_BASE, pu8DataSec, (uint16)u32DataSize);
			fclose(fp);
		}
		free(pu8DataSec);
	}
#endif
	nm_write_reg(0x108c, 0xdddd);

}
#endif /* __ROM_TEST__ */

#ifdef __KERNEL__ 
#include "linux/string.h"
void* linux_wlan_malloc(uint32_t sz);
#define LINUX_DMA_COHERENT_MEM
#include <linux/kernel.h>
#include <linux/slab.h> 
#endif

#ifdef CONF_WILC_USE_1000_REV_A
#error "Firmware for rev A is no longer supported!"
#elif defined CONF_WILC_USE_1000_REV_B
#include "driver/include/wifi_firmware_1000b.h"
#elif defined CONF_WILC_USE_3000_REV_A
#include "driver/include/wifi_firmware_3000.h"
#ifdef CONF_WILC_USE_BLE_ONLY
#include "driver/include/ble_firmware_3000.h"
#else
#include "driver/include/bt_firmware_3000.h"
#endif
#endif

sint8 firmware_download(void)
{
	sint8 ret = M2M_SUCCESS;
	uint32 u32SecSize, u32SecAddress;
	uint8_t* pu8FirmwareBuffer;
	sint32 BuffIndex = 0, CurrentSecSize = 0;
	uint8_t u8TransferChunk[32], ChunkSize = 32;

	pu8FirmwareBuffer = (uint8_t *)firmware;
	M2M_DBG("firmware size = %d\n",sizeof(firmware));

	while((uint32_t)pu8FirmwareBuffer < (uint32_t)(firmware+sizeof(firmware)))
	{		
		/*get text section address and size*/	
		u32SecAddress 	= (((uint32_t)(pu8FirmwareBuffer[3]))<<24)|(((uint32_t)(pu8FirmwareBuffer[2]))<<16)|
		(((uint32_t)(pu8FirmwareBuffer[1]))<<8)|(((uint32_t)(pu8FirmwareBuffer[0]))<<0);
		u32SecSize 	= (((uint32_t)(pu8FirmwareBuffer[7]))<<24)|(((uint32_t)(pu8FirmwareBuffer[6]))<<16)|
		(((uint32_t)(pu8FirmwareBuffer[5]))<<8)|(((uint32_t)(pu8FirmwareBuffer[4]))<<0);
		M2M_DBG("write sec %x size %d\n",u32SecAddress,u32SecSize);
		CurrentSecSize = u32SecSize;		
		ChunkSize = 32;
		BuffIndex = 8;
		while(CurrentSecSize>0)
		{
			if(CurrentSecSize < ChunkSize)
				ChunkSize = CurrentSecSize;
			
			m2m_memcpy(u8TransferChunk,pu8FirmwareBuffer+BuffIndex,ChunkSize);
			nm_write_block(u32SecAddress,u8TransferChunk,ChunkSize);
			u32SecAddress += ChunkSize;
			BuffIndex += ChunkSize;
			CurrentSecSize -= ChunkSize;
		}		
		pu8FirmwareBuffer += BuffIndex;
		
	}	
	return ret;
}

#ifdef CONF_WILC_USE_3000_REV_A
sint8 firmware_download_bt(void)
{
	sint8 ret = M2M_SUCCESS;
	uint32 u32SecSize, u32SecAddress;
	uint8_t* pu8FirmwareBuffer;
	sint32 BuffIndex = 0, CurrentSecSize = 0;
	uint8_t u8TransferChunk[32], ChunkSize = 32;

	/* Set cortus reset register to register control. */
	nm_write_reg(0x3b0090, 1);
	nm_write_reg(0x4f0000, 0x71);

#ifdef CONF_WILC_USE_BLE_ONLY
	pu8FirmwareBuffer = (uint8_t *)firmware_ble;
	u32SecSize = sizeof(firmware_ble);
#else
	pu8FirmwareBuffer = (uint8_t *)firmware_bt;
	u32SecSize = sizeof(firmware_bt);
#endif
	
	M2M_DBG("BT firmware size = %d\n", u32SecSize);
	ChunkSize = 32;
	u32SecAddress = 0x400000;
	CurrentSecSize = u32SecSize;
	while(CurrentSecSize>0)
	{
		if(CurrentSecSize < ChunkSize)
			ChunkSize = CurrentSecSize;
		
		m2m_memcpy(u8TransferChunk,pu8FirmwareBuffer+BuffIndex,ChunkSize);
		nm_write_block(u32SecAddress,u8TransferChunk,ChunkSize);
		u32SecAddress += ChunkSize;
		BuffIndex += ChunkSize;
		CurrentSecSize -= ChunkSize;
	}		
	pu8FirmwareBuffer += BuffIndex;
	
	return ret;
}
#endif

#ifndef M2M_WILC1000
sint8 wait_for_bootrom(void)
{
	sint8 ret = M2M_SUCCESS;
	uint32 reg = 0, cnt = 0;
	
	reg = 0;
	while(1) {
		reg = nm_read_reg(0x1014);	/* wait for efuse loading done */
		if (reg & 0x80000000) {
			break;
		}
		nm_bsp_sleep(1); /* TODO: Why bus error if this delay is not here. */
	}
	reg = nm_read_reg(M2M_WAIT_FOR_HOST_REG);
	reg &= 0x1;

	/* check if waiting for the host will be skipped or not */
	if(reg == 0)
	{
		reg = 0;
		while(reg != M2M_FINISH_BOOT_ROM)
		{
			nm_bsp_sleep(1);
			reg = nm_read_reg(BOOTROM_REG);

			if(++cnt > TIMEOUT)
			{
				M2M_DBG("failed to load firmware from flash.\n");
				ret = M2M_ERR_INIT;
				goto ERR2;
			}
		}
	}

	nm_write_reg(BOOTROM_REG,M2M_START_FIRMWARE);

#ifdef __ROM_TEST__
	rom_test();
#endif /* __ROM_TEST__ */

ERR2:
	return ret;
}
#endif
sint8 wait_for_firmware_start(void)
{
	sint8 ret = M2M_SUCCESS;
	uint32 reg = 0, cnt = 0;;
	
	while (reg != M2M_FINISH_INIT_STATE)
	{
		nm_bsp_sleep(1); /* TODO: Why bus error if this delay is not here. */
		M2M_DBG("%x %x %x\n",(unsigned int)nm_read_reg(0x108c),(unsigned int)nm_read_reg(0x108c),(unsigned int)nm_read_reg(0x14A0));
		reg = nm_read_reg(NMI_STATE_REG);
		if(++cnt > TIMEOUT)
		{
			M2M_DBG("Time out for wait firmware Run\n");
			ret = M2M_ERR_INIT;
			goto ERR;
		}
	}
	nm_write_reg(NMI_STATE_REG,0);
ERR:
	return ret;
}

sint8 chip_deinit(void)
{
	uint32 reg = 0;
	sint8 ret;
	uint8 timeout = 10;

	/**
	stop the firmware, need a re-download
	**/
	ret = nm_read_reg_with_ret(NMI_GLB_RESET_0, &reg);
	if (ret != M2M_SUCCESS) {
		M2M_ERR("failed to de-initialize\n");
	}
	reg &= ~(1 << 10);
	ret = nm_write_reg(NMI_GLB_RESET_0, reg);

	if (ret != M2M_SUCCESS) {
		M2M_ERR("Error while writing reg\n");
		return ret;
	}

	do {
		ret = nm_read_reg_with_ret(NMI_GLB_RESET_0, &reg);
		if (ret != M2M_SUCCESS) {
			M2M_ERR("Error while reading reg\n");
			return ret;
		}
		/*Workaround to ensure that the chip is actually reset*/
		if ((reg & (1 << 10))) {
			M2M_DBG("Bit 10 not reset retry %d\n", timeout);
			reg &= ~(1 << 10);
			ret = nm_write_reg(NMI_GLB_RESET_0, reg);
			timeout--;
		} else {
			break;
		}

	} while (timeout);
	
	return ret;		
}

sint8 set_gpio_dir(uint8 gpio, uint8 dir)
{
	uint32 val32;
	sint8 ret;

	ret = nm_read_reg_with_ret(0x20108, &val32);
	if(ret != M2M_SUCCESS) goto _EXIT;
	
	if(dir) {
		val32 |= (1ul << gpio);
	} else {
		val32 &= ~(1ul << gpio);
	}

	ret = nm_write_reg(0x20108, val32);

_EXIT:
	return ret;
}
sint8 set_gpio_val(uint8 gpio, uint8 val)
{
	uint32 val32;
	sint8 ret;

	ret = nm_read_reg_with_ret(0x20100, &val32);
	if(ret != M2M_SUCCESS) goto _EXIT;
	
	if(val) {
		val32 |= (1ul << gpio);
	} else {
		val32 &= ~(1ul << gpio);
	}

	ret = nm_write_reg(0x20100, val32);

_EXIT:
	return ret;
}

sint8 get_gpio_val(uint8 gpio, uint8* val)
{
	uint32 val32;
	sint8 ret;

	ret = nm_read_reg_with_ret(0x20104, &val32);
	if(ret != M2M_SUCCESS) goto _EXIT;
	
	*val = (uint8)((val32 >> gpio) & 0x01);

_EXIT:
	return ret;
}

sint8 pullup_ctrl(uint32 pinmask, uint8 enable)
{
	sint8 s8Ret;
	uint32 val32;
	s8Ret = nm_read_reg_with_ret(0x142c, &val32);
	if(s8Ret != M2M_SUCCESS) {
		M2M_ERR("[pullup_ctrl]: failed to read\n");
		goto _EXIT;
	}
	if(enable) {
		val32 &= ~pinmask;
		} else {
		val32 |= pinmask;
	}
	s8Ret = nm_write_reg(0x142c, val32);
	if(s8Ret  != M2M_SUCCESS) {
		M2M_ERR("[pullup_ctrl]: failed to write\n");
		goto _EXIT;
	}
_EXIT:
	return s8Ret;
}

sint8 nmi_get_otp_mac_address(uint8 *pu8MacAddr,  uint8 * pu8IsValid)
{
	sint8 ret;
	uint32	u32RegValue;
	uint8	mac[6];
	
	ret = nm_read_reg_with_ret(rNMI_GP_REG_0, &u32RegValue);
	if(ret != M2M_SUCCESS) goto _EXIT_ERR;
	
	if(!EFUSED_MAC(u32RegValue)) {
		M2M_DBG("Default MAC\n");
		m2m_memset(pu8MacAddr, 0, 6);
		goto _EXIT_ERR;
	}
	
	M2M_DBG("OTP MAC\n");
	u32RegValue >>=16;
	nm_read_block(u32RegValue|0x30000, mac, 6);
	m2m_memcpy(pu8MacAddr,mac,6); 
	if(pu8IsValid) *pu8IsValid = 1;	
	return ret;

_EXIT_ERR:
	if(pu8IsValid) *pu8IsValid = 0;
	return ret;
}

sint8 nmi_get_mac_address(uint8 *pu8MacAddr)
{
	sint8 ret;
	uint32	u32RegValue;
	uint8	mac[6];
	
	ret = nm_read_reg_with_ret(rNMI_GP_REG_0, &u32RegValue);
	if(ret != M2M_SUCCESS) goto _EXIT_ERR;

	u32RegValue &=0x0000ffff;
	nm_read_block(u32RegValue|0x30000, mac, 6);
	m2m_memcpy(pu8MacAddr, mac, 6);
	
	return ret;
	
_EXIT_ERR:
	return ret;
}

#ifdef CONF_WILC_USE_3000_REV_A

sint8 nmi_coex_init(void)
{
	uint32_t ret = M2M_SUCCESS;
	uint32_t u32Val, u32NULLDuration = 1000;

	/* Configure Coexistance */
	u32Val = nm_read_reg(rCOE_TOP_CTL);
	u32Val &= ~(NBIT0 | NBIT1);
	u32Val |= NBIT2 | NBIT1;	// Force the Coexistence clock on with 40Mhz
	nm_write_reg(rCOE_TOP_CTL, u32Val);

	/*	[0]		Enable counting the idle time before active BT to qualify the 1st slot of the BT burst lumped as one transaction  
		[15]	Idle time before the the 1st slot of the BT burst lumped as one transaction = 1
		[16]	Limit of the idle time between of the BT bursts to be considered as one transaction (default 450us). 
				This is also introduce a delay of the exiting BT at the last BT slot.   = 1
		[31]	Enable counting the idle time between the active slots to determine whether to consider them as single transaction 
	*/
	u32Val = nm_read_reg(rBT_SLOT_LUMP_CTL1);
	u32Val = NBIT0 | NBIT15 | NBIT16 | NBIT31;
	nm_write_reg(rBT_SLOT_LUMP_CTL1, u32Val);

	/* 	[7:0]	Counts to generate 1 microsecond ticks from system clock  
		[15]	The duration of wlan_active in high when blocking the BT request 0: Only use the programmable time "blocking_bt_duration" if it is activated; 1: Keep WLAN_ACTIVE high till BT_ACTIVE going low or block till time specified in "block_bt_duration" whichever comes first (works only in 3-wire mode) 
		[27:16]	Duration of wlan_active is raised to block the bt_active of the current slot; The unit is 8us; Default to 150x8 = 1200 us (slightly less than one slot pair) 
	*/
	u32Val = nm_read_reg(rCOEXIST_TIMER);
	u32Val &= ~(0xff |0xffff0000);
	u32Val |= (COUNT_TO_ONE_US) | NBIT15 | ( 60  << 16);
	nm_write_reg(rCOEXIST_TIMER, u32Val);

	/*	[6:0]	microseconds to sample BT priority after rising edge of BT_ACTIVE wire In 0.5us in 1-wire mode 
	*/
	u32Val = nm_read_reg(rBT_WIRE_SAMPLE_TIME);
	u32Val &= ~(0x7f);
	u32Val |= NBIT3;
	nm_write_reg(rBT_WIRE_SAMPLE_TIME, u32Val);

	/* 	[8:6]	WL-BT(ACL) arbitration mode;
					0: no mux 
					1: timer mux 
					2: dual counters mux 
					3: rejection counter threshold mux 
	*/
	u32Val = nm_read_reg(rCOEXIST_CTL);
	u32Val &= ~(NBIT6|NBIT7|NBIT8);
	u32Val |= NBIT6;
	nm_write_reg(rCOEXIST_CTL, u32Val);

	/*	[11:0]	BT rejection timer in arb_mode=1 
		[15:12]	BT rejection / acceptance timer time base ( 2^[15:12] us ) 
		[27:16]	BT acceptance timer in arb_mode=1 
	*/
	u32Val = BT_REJECTION_TIMER | (BT_REJ_ACPT_TIMER_TIME_BASE<<12) |
				(BT_ACCEPTANCE_TIMER<<16);
	nm_write_reg(rBT_CNT_THR, u32Val);

	/* 	[12:0]	Interrupt will be generated when rej/acc arb counter reach this value for BT rejection timer. 
				Null packet will be sent
		[28:16]	Interrupt will be generated when rej/acc arb counter reach this value for BT rejection timer
	*/
	u32Val = BT_REG_TIMER_NULL_THRE | (BT_ACPT_TIMER_THRE << 16);
	nm_write_reg(rBT_CNT_INT, u32Val);

	/* [30]		Disable auto HW NULL pkt with PS-ON header upon Coe_bt_rej_exp timer; 
				To send out NULL pkt to hold AP from sending us wifi pkts during ACL BT bursts  
	*/
	u32Val = nm_read_reg(rCOE_AUTO_PS_ON_NULL_PKT);
	u32Val &= ~NBIT30;
	nm_write_reg(rCOE_AUTO_PS_ON_NULL_PKT, u32Val);

	/* 	[30]		Disable auto HW NULL pkt with PS-OFF header upon Coe_bt_rej_exp timer; 
				To send out NULL pkt to tell AP resume sending us the wifi packets 
	*/
	u32Val = nm_read_reg(rCOE_AUTO_PS_OFF_NULL_PKT);
	u32Val &= ~NBIT30;
	u32Val |= 0x99;//time out of txabort
	nm_write_reg(rCOE_AUTO_PS_OFF_NULL_PKT, u32Val);
	/*	[15:0]	Value to be inserted into the Duration/ID field in the Null frame used for TX abort. This is expressed in microseconds. 
		[31:16]	The timeout period for the transmission of the Null frame after the TX abort request is asserted. This is expressed in microseconds. 
	*/
	u32Val = ( 0xfff0000 | u32NULLDuration );
	nm_write_reg(rTX_ABORT_NULL_FRM_DURATION_TIMEOUT_VALUE, u32Val);

	/*	[7:0]	Data rate to be used for the TX abort Null frame. 
		[15:8]	Power level at which the TX abort Null frame should be transmitted.
	*/
	u32Val = TX_RATE_24_MBPS;
	nm_write_reg(rTX_ABORT_NULL_FRM_RATE_POWER_LEVEL, u32Val);
		
	u32Val = PHY_MODE_24_MBPS;
	nm_write_reg(rTX_ABORT_NULL_FRM_PHY_TX_MODE_SETTING, u32Val);

	/*	[16]	Enable self-CTS upon SCO timer from coexist 
		[29]	Enable auto HW CTS pkt upon granted any BT slot request 
		[30]	Enable auto HW CTS pkt upon granted HP BT slot request 
	*/
	u32Val = nm_read_reg(rCOE_AUTO_CTS_PKT);
	u32Val &= ~( NBIT16| NBIT29| NBIT30); 
	nm_write_reg(rCOE_AUTO_CTS_PKT, u32Val);

	/*	
		[28]	Transmission will be aborted and PA will enter aborted state in 
				which no transmission will happen. Any transmission which is in
				progress will stop abruptly.
		[29]	1 _ PA will transmit self-CTS with in PIFS after a TX abort 
		[30]	1 _ TX abort can be initiated only by H/W 
	*/
	u32Val = nm_read_reg(rPA_CONTROL);
	u32Val &= ~NBIT29;
	u32Val |= NBIT28|NBIT30;
	nm_write_reg(rPA_CONTROL, u32Val);

	/*
		[15:12] ctl_combo_pti_thrd,Threshold of the 4-bit BT priority value above which are considered as high-priority BT packets 
	*/
	u32Val = nm_read_reg(rCOEXIST_CTL);
	u32Val &= ~(NBIT14);
	u32Val |= (NBIT12|NBIT13|NBIT15);
	nm_write_reg(rCOEXIST_CTL, u32Val);
	return ret;
}

sint8 nmi_coex_set_mode(tenuNmiCoexMode enuCoexMode)
{
	sint8 ret = M2M_SUCCESS;
	uint32 u32Val;
	
	M2M_DBG("Coex mode %d\n", enuCoexMode);
	
	switch(enuCoexMode)
	{
		case NMI_COEX_MODE_WIFI:
			/* Force Wifi */
			u32Val = nm_read_reg(rCOEXIST_CTL);
			u32Val |= NBIT0 | NBIT11;
			u32Val &= ~NBIT9;
			nm_write_reg(rCOEXIST_CTL, u32Val);
			/* Disable coex null frames */
			u32Val = nm_read_reg(rCOE_AUTO_PS_ON_NULL_PKT);
			u32Val &= ~NBIT30;
			nm_write_reg(rCOE_AUTO_PS_ON_NULL_PKT, u32Val);
			u32Val = nm_read_reg(rCOE_AUTO_PS_OFF_NULL_PKT);
			u32Val &= ~NBIT30;
			nm_write_reg(rCOE_AUTO_PS_OFF_NULL_PKT, u32Val);
			break;
		case NMI_COEX_MODE_BT:
			/* Force BT */
			u32Val = nm_read_reg(rCOEXIST_CTL);
			u32Val |= NBIT0 | NBIT9;
			u32Val &= ~NBIT11;
			nm_write_reg(rCOEXIST_CTL, u32Val);
			/* Disable coex null frames */
			u32Val = nm_read_reg(rCOE_AUTO_PS_ON_NULL_PKT);
			u32Val &= ~NBIT30;
			nm_write_reg(rCOE_AUTO_PS_ON_NULL_PKT, u32Val);
			u32Val = nm_read_reg(rCOE_AUTO_PS_OFF_NULL_PKT);
			u32Val &= ~NBIT30;
			nm_write_reg(rCOE_AUTO_PS_OFF_NULL_PKT, u32Val);
			break;
		case NMI_COEX_MODE_COMBO:
			u32Val = nm_read_reg(rCOEXIST_CTL);
			u32Val |= NBIT0;
			u32Val &= ~(NBIT9 | NBIT11);
			nm_write_reg(rCOEXIST_CTL, u32Val);
			/* Enable coex null frames */
			u32Val = nm_read_reg(rCOE_AUTO_PS_ON_NULL_PKT);
			u32Val |= NBIT30;
			nm_write_reg(rCOE_AUTO_PS_ON_NULL_PKT, u32Val);
			u32Val = nm_read_reg(rCOE_AUTO_PS_OFF_NULL_PKT);
			u32Val |= NBIT30;
			u32Val |= 0x99; /*Time out of txabort*/
			nm_write_reg(rCOE_AUTO_PS_OFF_NULL_PKT, u32Val);
			break;
	}
	return ret;
}
#endif
