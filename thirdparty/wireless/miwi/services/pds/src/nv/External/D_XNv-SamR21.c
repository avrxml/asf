/**
* \file  D_XNv-SamR21.c
*
* \brief D_XNv Drivers Implementation.
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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

#ifdef PDS_USE_EXTERNAL_FLASH

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "D_XNv_Bindings.h"
#include "D_XNv_Init.h"
#include "D_XNv.h"
#include "N_Types.h"
#include <appTimer.h>
#include <atsamr21.h>
#include <atomic.h>
#include <spi.h>

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#if defined(ENABLE_NV_USAGE_SIMULATION)
#include "N_Log.h"
#endif

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "D_XNv"

#if defined(ATSAMR21E19A)

#define D_XNV_PAGE_PROGRAM_CMD  0x02u
#define D_XNV_READ_CMD          0x03u
#define D_XNV_READ_STATUS_CMD   0x05u
#define D_XNV_WRITE_ENABLE_CMD  0x06u
// since block size of ext memory is 64 KB which is matching
// with existing implementation, using block erase cmd(0x52)
// for erasing D_Xnv sector
#define D_XNV_SECTOR_ERASE_CMD   0x52u

#define D_XNV_STATUS_WIP        0x01u

#define EXTFLASH_SPI_CS             (1 << 23)
#define EXTFLASH_SPI_SCK            (1 << 23)
#define EXTFLASH_SPI_MOSI           (1 << 22)
#define EXTFLASH_SPI_MISO           (1 << 22)
#define EXTFLASH_SPI_HOLD           (1 << 0)
#define EXTFLASH_SPI_WRITE_PROCTECT (1 << 12)

#define SC_SPI_DATA        SC5_SPI_DATA
#define SC_SPI_INTFLAG     SC5_SPI_INTFLAG
#define SC_SPI_INTFLAG_RXC SC5_SPI_INTFLAG_RXC

#else

#define D_XNV_PAGE_PROGRAM_CMD  0x02u
#define D_XNV_READ_CMD          0x03u
#define D_XNV_READ_STATUS_CMD   0x05u
#define D_XNV_WRITE_ENABLE_CMD  0x06u
#define D_XNV_SECTOR_ERASE_CMD  0xD8u

#define D_XNV_STATUS_WIP        0x01u

#define EXTFLASH_SPI_CS         (1 << 14)
#define EXTFLASH_SPI_SCK        (1 << 9)
#define EXTFLASH_SPI_MOSI       (1 << 8)
#define EXTFLASH_SPI_MISO       (1 << 15)

#define SC_SPI_DATA        SC2_SPI_DATA
#define SC_SPI_INTFLAG     SC2_SPI_INTFLAG
#define SC_SPI_INTFLAG_RXC SC2_SPI_INTFLAG_RXC

#endif

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/
// The Callback function to handle system integrity checks
static void (*s_pfSystemCheckCallback)(void);

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static inline void Delay100ns(void)
{
    // At 48 MHz, 5 nops is just above 100ns.
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
}

static inline void SelectFlash(void)
{
    // Let's check the system integrity before acessing the flash chip to prevent NV corruption
    if (s_pfSystemCheckCallback != NULL)
    {
       s_pfSystemCheckCallback();
    }

    Delay100ns();   // for tSHSL (a.k.a. tCPH or tCEH): >= 100ns
    // (also for tCHSL (a.k.a. tCSH): >= 10ns.)

    // Make CSn low
    PORTA_OUTCLR = EXTFLASH_SPI_CS;

    // tSLCH (a.k.a. tCSS or tCS): >= 10ns. But at 48 MHz, 2 different instructions are already 20ns apart.
}

static void DeselectFlash(void)
{
    // tCHSH (a.k.a. tCSH or tCH) : >= 10ns. But at 48 MHz, 2 different instructions are already 20ns apart.

    // Make CSn high
    PORTA_OUTSET = EXTFLASH_SPI_CS;

    // tSHCH (a.k.a. tCSS): >= 10ns. But at 48 MHz, 2 different instructions are already 20ns apart.
}

static inline void SpiTxByte(uint8_t value)
{
    SC_SPI_DATA = value;
}

static inline void SpiTxWaitReady(void)
{
    // Wait for transmission complete
    while ( !(SC_SPI_INTFLAG & SC_SPI_INTFLAG_RXC) )
    {
        // no action
    }
}

/** Receive a byte that was transferred during a transmit */
static inline uint8_t SpiRxByte(void)
{
    return SC_SPI_DATA;
}

static inline void SpiTxByteAndWait(uint8_t value)
{
    ATOMIC_SECTION_ENTER
    SpiTxByte(value);
    SpiTxWaitReady();
    ATOMIC_SECTION_LEAVE
}

static inline void SpiTxByteWaitAndDummyReadByte(uint8_t value)
{
    SpiTxByteAndWait(value);
    SpiRxByte();
}

/** Wait unit a previous write or sector erase has finished. This can take really long! */
static void WaitUntilWriteFinished(void)
{
	uint8_t status;
	do
	{
		SelectFlash();
		SpiTxByteWaitAndDummyReadByte(D_XNV_READ_STATUS_CMD);
		SpiTxByteAndWait(0u);
		status = SpiRxByte();
		DeselectFlash();
	}
	while ( status & D_XNV_STATUS_WIP );
}

static inline uint8_t ReadSingleByte(void)
{
    SpiTxByteAndWait(0u);
    return SpiRxByte();
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/
#if defined(ATSAMR21E19A)
void D_XNv_Init(void)
{
    /* Configure PIOs for SPI */
    PORTA_OUTSET = EXTFLASH_SPI_CS;
    PORTA_DIRSET = EXTFLASH_SPI_CS;
    PORTB_DIRCLR = EXTFLASH_SPI_SCK | EXTFLASH_SPI_MISO;
    PORTA_DIRCLR = EXTFLASH_SPI_MOSI;

    /* Configure SPI in peripheral multiplexing */
    PORTA_PMUX11 |= PORTA_PMUX7_PMUXE(3);
    PORTB_PMUX11 = PORTB_PMUX11_PMUXE(3) | PORTA_PMUX4_PMUXO(3);

    /* Pin configurations */
    PORTA_PINCFG22 |= PORTA_PINCFG22_PMUXEN; //MOSI  A22
    PORTB_PINCFG22 |= PORTB_PINCFG22_PMUXEN; //MISO  B22
    PORTB_PINCFG23 |= PORTB_PINCFG23_PMUXEN; //SCK   B23

    //Write protect and hold pins have to be pulled up 
    //to enable and access flash.
    PORTA_DIRSET = EXTFLASH_SPI_HOLD;
    PORTA_DIRSET = EXTFLASH_SPI_WRITE_PROCTECT;
    PORTA_OUTSET = EXTFLASH_SPI_HOLD;
    PORTA_OUTSET = EXTFLASH_SPI_WRITE_PROCTECT;

    /* Enable the clock of SPI */
    PM_APBCMASK |= PM_APBCMASK_SERCOM5;

    /* Configure the generic clk settings */
    GCLK_CLKCTRL_s.id = 0x19;  // enabling clock for SERCOM5
    GCLK_CLKCTRL_s.gen = 0;
    GCLK_CLKCTRL_s.clken = 1;

    /* Reset */
    SC5_SPI_CTRLA = SC5_SPI_CTRLA_SWRST;
    while((SC5_SPI_SYNCBUSY & SC5_SPI_SYNCBUSY_SWRST));

    /* Receiver Enable*/
    SC5_SPI_CTRLB  = SC5_SPI_CTRLB_RXEN;
    while(SC5_SPI_SYNCBUSY & SC5_SPI_SYNCBUSY_CTRLB);

    /* Baud Rate Configuration */
    SC5_SPI_BAUD = SPI_CLOCK_RATE_2000;

    /* setup spi mode master, pad & enable it */
    SC5_SPI_CTRLA |=  SC5_SPI_CTRLA_MODE(3) | SC5_SPI_CTRLA_DIPO(2)| SC5_SPI_CTRLA_DOPO(3) | SC5_SPI_CTRLA_ENABLE;
    while(SC5_SPI_SYNCBUSY & SC5_SPI_SYNCBUSY_ENABLE);

}

#else /* #if defined(ATSAMR21E19A) */
void D_XNv_Init(void)
{
    /* Configure PIOs for SPI */
    PORTA_OUTSET = EXTFLASH_SPI_CS;
    PORTA_DIRSET = EXTFLASH_SPI_CS;
    PORTA_DIRCLR = EXTFLASH_SPI_SCK | EXTFLASH_SPI_MISO | EXTFLASH_SPI_MOSI;

    /* Configure SPI in peripheral multiplexing */
    PORTA_PMUX7 |= PORTA_PMUX7_PMUXO(2);
    PORTA_PMUX4 = PORTA_PMUX4_PMUXE(3) | PORTA_PMUX4_PMUXO(3);

    /* Pin configurations */
    PORTA_PINCFG15 |= PORTA_PINCFG15_PMUXEN; //MISO  A15
    PORTA_PINCFG8 |= PORTA_PINCFG8_PMUXEN; //MOSI  A8
    PORTA_PINCFG9 |= PORTA_PINCFG9_PMUXEN; //SCK   A9

    /* Enable the clock of SPI */
    PM_APBCMASK |= PM_APBCMASK_SERCOM2;

    /* Configure the generic clk settings */
    GCLK_CLKCTRL_s.id = 0x16;  // enabling clock for SERCOM2
    GCLK_CLKCTRL_s.gen = 0;
    GCLK_CLKCTRL_s.clken = 1;

    /* Reset */
    SC2_SPI_CTRLA = SC2_SPI_CTRLA_SWRST;
    while((SC2_SPI_SYNCBUSY & SC2_SPI_SYNCBUSY_SWRST));

    /* Receiver Enable*/
    SC2_SPI_CTRLB  = SC2_SPI_CTRLB_RXEN;
    while(SC2_SPI_SYNCBUSY & SC2_SPI_SYNCBUSY_CTRLB);

    /* Baud Rate Configuration */
    SC2_SPI_BAUD = SPI_CLOCK_RATE_2000;

    /* setup spi mode master, pad & enable it */
    SC2_SPI_CTRLA |=  SC2_SPI_CTRLA_MODE(3) | SC2_SPI_CTRLA_DIPO(3)| SC2_SPI_CTRLA_DOPO(0) | SC2_SPI_CTRLA_ENABLE;
    while(SC2_SPI_SYNCBUSY & SC2_SPI_SYNCBUSY_ENABLE);

}
#endif /* #if defined(ATSAMR21E19A) */

/** The function to set system integrity check callback function */
void D_XNv_SetSystemIntegrityCheckFunction(void (*pf)(void))
{
    /* Set callback function */
    s_pfSystemCheckCallback = pf;
}

void D_XNv_Read_Impl(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_XNv_Size_t numberOfBytes)
{
    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteWaitAndDummyReadByte(D_XNV_READ_CMD);
    SpiTxByteWaitAndDummyReadByte(sector);
    SpiTxByteWaitAndDummyReadByte((uint8_t)(offset >> 8));
    SpiTxByteWaitAndDummyReadByte((uint8_t)offset);

    while ( numberOfBytes != 0u )
    {
        *pBuffer = ReadSingleByte();
        pBuffer++;
        numberOfBytes--;
    }

    DeselectFlash();
}

void D_XNv_Write_Impl(uint8_t sector, uint16_t offset, const uint8_t *pBuffer, D_XNv_Size_t numberOfBytes)
{
    while (numberOfBytes != 0u)
    {
        WaitUntilWriteFinished();
        SelectFlash();
        SpiTxByteWaitAndDummyReadByte(D_XNV_WRITE_ENABLE_CMD);
        DeselectFlash();

        SelectFlash();
        SpiTxByteWaitAndDummyReadByte(D_XNV_PAGE_PROGRAM_CMD);
        SpiTxByteWaitAndDummyReadByte(sector);
        SpiTxByteWaitAndDummyReadByte((uint8_t)(offset >> 8));
        SpiTxByteWaitAndDummyReadByte((uint8_t)offset);
        do
        {
            SpiTxByteWaitAndDummyReadByte(*pBuffer);
            pBuffer++;
            numberOfBytes--;
            offset++;
        }
        while ( ( numberOfBytes != 0u ) && ( (offset & 0x00FFu) != 0x0000u ) );
        DeselectFlash();
    }
}

void D_XNv_EraseSector_Impl(uint8_t sector)
{
#if defined(ENABLE_NV_USAGE_SIMULATION)
    N_LOG_ALWAYS(("EraseSector,%hu", sector));
#endif
    WaitUntilWriteFinished();
    SelectFlash();
    SpiTxByteWaitAndDummyReadByte(D_XNV_WRITE_ENABLE_CMD);
    DeselectFlash();

    SelectFlash();
    SpiTxByteWaitAndDummyReadByte(D_XNV_SECTOR_ERASE_CMD);
    SpiTxByteWaitAndDummyReadByte(sector);
    SpiTxByteWaitAndDummyReadByte(0x00u);
    SpiTxByteWaitAndDummyReadByte(0x00u);
    DeselectFlash();
}

bool D_XNv_IsEmpty_Impl(uint8_t sector, uint16_t offset, D_XNv_Size_t numberOfBytes)
{
    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteWaitAndDummyReadByte(D_XNV_READ_CMD);
    SpiTxByteWaitAndDummyReadByte(sector);
    SpiTxByteWaitAndDummyReadByte((uint8_t)(offset >> 8));
    SpiTxByteWaitAndDummyReadByte((uint8_t)offset);

    uint8_t andedContent = 0xFFu;
    do
    {
        andedContent &= ReadSingleByte();
        numberOfBytes--; // will wrap from 0 to 0xFFFF when numberOfBytes equals 0 on entry
    }
    while ((andedContent == 0xFFu) && (numberOfBytes != 0u));

    DeselectFlash();
   return (andedContent == 0xFFu);
}

bool D_XNv_IsEqual_Impl(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_XNv_Size_t numberOfBytes)
{
    bool isEqual = TRUE;

    WaitUntilWriteFinished();

    SelectFlash();
    SpiTxByteWaitAndDummyReadByte(D_XNV_READ_CMD);
    SpiTxByteWaitAndDummyReadByte(sector);
    SpiTxByteWaitAndDummyReadByte((uint8_t)(offset >> 8));
    SpiTxByteWaitAndDummyReadByte((uint8_t)offset);

    while ((isEqual) && (numberOfBytes != 0u))
    {
        isEqual = (*pBuffer == ReadSingleByte());
        pBuffer++;

        numberOfBytes--;
    }

    DeselectFlash();

    return isEqual;
}


#endif // #ifdef PDS_USE_EXTERNAL_FLASH