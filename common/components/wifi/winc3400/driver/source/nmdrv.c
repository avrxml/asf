/**
 *
 * \file
 *
 * \brief This module contains NMC1000 M2M driver APIs implementation.
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
#include "driver/source/nmdrv.h"
#include "driver/source/nmasic.h"
#include "driver/include/m2m_types.h"

#ifdef CONF_WINC_USE_SPI
#include "driver/source/nmspi.h"
#endif

/**
*	@fn		nm_get_firmware_full_info(tstrM2mRev* M2mRev)
*	@brief	Get Firmware version info
*	@param [out]	M2mRev
*				pointer holds address of structure "tstrM2mRev" that contains the firmware version parameters
*	@version	1.0
*/

sint8 nm_get_firmware_full_info(tstrM2mRev* pstrRev)
{
	uint16  drv_hif_level, fw_hif_level;
	uint32	reg = 0;
	sint8	ret = M2M_SUCCESS;
	tstrGpRegs strgp = {0};

	m2m_memset((uint8*)pstrRev,0,sizeof(tstrM2mRev));
	ret = nm_read_reg_with_ret(NMI_REV_REG, &reg);
	reg &= 0x0000FFFF;
	if (reg == 0)
	{
		ret = M2M_ERR_FAIL;
		goto EXIT;
	}
	fw_hif_level = M2M_GET_HIF_LEVEL(reg);

	// Check we're not talking to legacy fw
	if (fw_hif_level == 0)
	{
		ret = M2M_ERR_FW_VER_MISMATCH;
		M2M_ERR("Firmware HIF Level unknown\n");
		goto EXIT;
	}
	//M2M_INFO("Firmware HIF Level : %u\n", fw_hif_level);
	drv_hif_level = M2M_HIF_LEVEL;
	/*
	 * Check HIF Levels are sufficiently compatible before parsing fw version number
	 * This check may be relaxed in future.
	 */
	if (drv_hif_level != fw_hif_level)
	{
		ret = M2M_ERR_FW_VER_MISMATCH;
		goto EXIT;
	}

	ret = nm_read_reg_with_ret(rNMI_GP_REG_0, &reg);
	if(ret == M2M_SUCCESS)
	{
		if(reg != 0)
		{
			ret = nm_read_block(reg|0x30000,(uint8*)&strgp,sizeof(tstrGpRegs));
			if(ret == M2M_SUCCESS)
			{
				reg = strgp.u32Firmware_Ota_rev;
				reg &= 0x0000ffff;
				if(reg != 0)
				{
					ret = nm_read_block(reg|0x30000,(uint8*)pstrRev,sizeof(tstrM2mRev));
					if(ret == M2M_SUCCESS)
					{
						if(fw_hif_level != M2M_GET_HIF_LEVEL(pstrRev->u16FirmwareHifInfo))
						{
							ret = M2M_ERR_FAIL;
							goto EXIT;
						}
						M2M_INFO("Firmware ver   : %u.%u.%u\n", pstrRev->u8FirmwareMajor, M2M_GET_MINOR(pstrRev->u8FirmwareMinor), M2M_GET_PATCH(pstrRev->u8FirmwareMinor));
						//M2M_INFO("Firmware Build %s Time %s\n",pstrRev->BuildDate,pstrRev->BuildTime);
						// This check is currently redundant, but would be needed if above hif level check is relaxed in future
						if (drv_hif_level != M2M_GET_HIF_LEVEL(pstrRev->u16FirmwareHifInfo))
						{
							ret = M2M_ERR_FW_VER_MISMATCH;
							goto EXIT;
						}
					}
				}else {
					ret = M2M_ERR_FAIL;
				}
			}
		}else{
			ret = M2M_ERR_FAIL;
		}
	}
EXIT:
	return ret;
}
/**
*	@fn		nm_get_ota_firmware_info(tstrM2mRev* pstrRev)
*	@brief	Get Firmware version info
*	@param [out]	M2mRev
*			    pointer holds address of structure "tstrM2mRev" that contains the firmware version parameters
			
*	@version	1.0
*/
sint8 nm_get_ota_firmware_info(tstrM2mRev* pstrRev)
{
	uint16  drv_hif_level, ota_hif_level;
	uint32	reg = 0;
	sint8	ret = M2M_SUCCESS;
	tstrGpRegs strgp = {0};

	m2m_memset((uint8*)pstrRev,0,sizeof(tstrM2mRev));
	ret = nm_read_reg_with_ret(NMI_REV_REG, &reg);

	reg >>= 16;
	if (reg == 0)
	{
		ret = M2M_ERR_FAIL;
		goto EXIT;
	}
	ota_hif_level = M2M_GET_HIF_LEVEL(reg);
	// Check if ota image is legacy fw
	if (ota_hif_level == 0)
	{
		ret = M2M_ERR_FW_VER_MISMATCH;
		M2M_ERR("OTA HIF Level unknown\n");
		goto EXIT;
	}
	//M2M_INFO("OTA HIF Level : %u\n", ota_hif_level);
	drv_hif_level = M2M_HIF_LEVEL;
	/*
	 * Check HIF Levels are sufficiently compatible before parsing ota version number
	 * This check may be relaxed in future.
	 */
	if (drv_hif_level != ota_hif_level)
	{
		ret = M2M_ERR_FW_VER_MISMATCH;
		goto EXIT;
	}

	ret = nm_read_reg_with_ret(rNMI_GP_REG_0, &reg);
	if(ret == M2M_SUCCESS)
	{
		if(reg != 0)
		{
			ret = nm_read_block(reg|0x30000,(uint8*)&strgp,sizeof(tstrGpRegs));
			if(ret == M2M_SUCCESS)
			{
				reg = strgp.u32Firmware_Ota_rev;
				reg >>= 16;
				if(reg != 0)
				{
					ret = nm_read_block(reg|0x30000,(uint8*)pstrRev,sizeof(tstrM2mRev));
					if(ret == M2M_SUCCESS)
					{
						if(ota_hif_level != M2M_GET_HIF_LEVEL(pstrRev->u16FirmwareHifInfo))
						{
							ret = M2M_ERR_FAIL;
							goto EXIT;
						}
						//M2M_INFO("OTA ver   : %u.%u.%u\n", pstrRev->u8FirmwareMajor, M2M_GET_MINOR(pstrRev->u8FirmwareMinor), M2M_GET_PATCH(pstrRev->u8FirmwareMinor));
						//M2M_INFO("OTA Build %s Time %s\n",pstrRev->BuildDate,pstrRev->BuildTime);
						// This check is currently redundant, but would be needed if above hif level check is relaxed in future
						if (drv_hif_level != M2M_GET_HIF_LEVEL(pstrRev->u16FirmwareHifInfo))
						{
							ret = M2M_ERR_FW_VER_MISMATCH;
							goto EXIT;
						}
					}
				}else {
					ret = M2M_ERR_FAIL;
				}
			}
		}else{
			ret = M2M_ERR_FAIL;
		}
	}
EXIT:
	return ret;
}

/*
*	@fn		nm_drv_init_download_mode
*	@brief	Initialize NMC1000 driver
*	@return	M2M_SUCCESS in case of success and Negative error code in case of failure
*   @param [in]	arg
*				Generic argument
*	@author	Viswanathan Murugesan
*	@date	10 Oct 2014
*	@version	1.0
*/
sint8 nm_drv_init_download_mode()
{
	sint8 ret = M2M_SUCCESS;
	
	ret = nm_bus_iface_init(NULL);
	if (M2M_SUCCESS != ret) {
		M2M_ERR("[nmi start]: fail init bus\n");
		goto ERR1;
	}


#ifdef CONF_WINC_USE_SPI
	/* Must do this after global reset to set SPI data packet size. */
	nm_spi_init();
#endif

	M2M_INFO("Chip ID %lx\n", nmi_get_chipid());
	
	/*disable all interrupt in ROM (to disable uart) in 2b0 chip*/
	nm_write_reg(0x20300,0);

ERR1:
	return ret;
}

/*
*	@fn		nm_drv_init
*	@brief	Initialize NMC1000 driver
*	@return	M2M_SUCCESS in case of success and Negative error code in case of failure
*   @param [in]	arg
*				Generic argument
*	@author	M. Abdelmawla
*	@date	15 July 2012
*	@version	1.0
*/
sint8 nm_drv_init(void * arg)
{
	sint8 ret = M2M_SUCCESS;
	uint8 u8Mode = M2M_WIFI_MODE_NORMAL;
	
	if(NULL != arg) {
		if(M2M_WIFI_MODE_CONFIG == *((uint8 *)arg)) {
			u8Mode = M2M_WIFI_MODE_CONFIG;
		} else {
			/*continue running*/
		}
	} else {
		/*continue running*/
	}
	
	ret = nm_bus_iface_init(NULL);
	if (M2M_SUCCESS != ret) {
		M2M_ERR("[nmi start]: fail init bus\n");
		goto ERR1;
	}

#ifdef BUS_ONLY
	return;
#endif
	
	ret = chip_wake();
	nm_bsp_sleep(10);
	if (M2M_SUCCESS != ret) {
		M2M_ERR("[nmi start]: fail chip_wakeup\n");
		goto ERR2;
	}
	/**
	Go...
	**/
	ret = chip_reset();
	if (M2M_SUCCESS != ret) {
		goto ERR2;
	}
	M2M_INFO("Chip ID %lx\n", nmi_get_chipid());
#ifdef CONF_WINC_USE_SPI
	/* Must do this after global reset to set SPI data packet size. */
	nm_spi_init();
#endif
	/*return power save to default value*/
	chip_idle();

	ret = cpu_start();
	if (M2M_SUCCESS != ret) {
		goto ERR2;
	}
	ret = wait_for_bootrom(u8Mode);
	if (M2M_SUCCESS != ret) {
		goto ERR2;
	}
		
	ret = wait_for_firmware_start(u8Mode);
	if (M2M_SUCCESS != ret) {
		goto ERR2;
	}
	
	if(M2M_WIFI_MODE_CONFIG == u8Mode) {
		goto ERR1;
	} else {
		/*continue running*/
	}
	
	ret = enable_interrupts();
	if (M2M_SUCCESS != ret) {
		M2M_ERR("failed to enable interrupts..\n");
		goto ERR2;
	}
	
	return ret;
ERR2:
	nm_bus_iface_deinit();
ERR1:	
	return ret;
}

/*
*	@fn		nm_drv_deinit
*	@brief	Deinitialize NMC1000 driver
*	@author	M. Abdelmawla
*	@date	17 July 2012
*	@version	1.0
*/
sint8 nm_drv_deinit(void * arg) 
{
	sint8 ret;
	
	ret = chip_deinit();
	if (M2M_SUCCESS != ret) {
		M2M_ERR("[nmi stop]: chip_deinit fail\n");
		goto ERR1;
	}

	ret = nm_bus_iface_deinit();
	if (M2M_SUCCESS != ret) {
		M2M_ERR("[nmi stop]: fail init bus\n");
		goto ERR1;
	}
#ifdef CONF_WINC_USE_SPI
	/* Must do this after global reset to set SPI data packet size. */
	nm_spi_deinit();
#endif

ERR1:
	return ret;
}
