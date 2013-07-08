/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <delay.h>
#include <gpio.h>
#include <intc.h>
#include <stdint.h>
#include <stdlib.h>
#include <wl_sdio.h>
#include <util/printf-stdarg.h>
#include <ports/avr32/board_init.h>

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

__attribute__((__interrupt__)) static void avr32_irq_handler(void);

void wl_sdio_init_cb(U8 *flags)
{
#ifdef _ASSERT_ENABLE_ /* To silence warning if Assert() macro is empty */
	volatile avr32_pm_t *pm = &AVR32_PM;
#endif
        volatile avr32_mci_t *mci = &AVR32_MCI;
	const gpio_map_t GPIO_MAP = {
                WL_SDIO_CLK, WL_SDIO_CMD,
                WL_SDIO_DAT0, WL_SDIO_DAT1, WL_SDIO_DAT2, WL_SDIO_DAT3
        };
	U32 i;

	gpio_enable_module(GPIO_MAP, ARRAY_SIZE(GPIO_MAP));
	for (i = 0; i < ARRAY_SIZE(GPIO_MAP); i++)
		gpio_enable_pin_pull_up((GPIO_MAP)[i].pin);

	/* Note: MCI_CLK enabled at reset in pm->pbbmask (see 8.6) */
	Assert(pm->pbbmask & (1 << 8));

	*flags = 0;

#ifdef USE_1BIT_MODE
	*flags |= SDIO_1BIT_MODE;
#endif

#ifdef USE_POLL
	*flags |= SDIO_FLAG_POLL;
	return;
#else
	INTC_register_interrupt(&avr32_irq_handler, AVR32_MCI_IRQ,
				AVR32_INTC_INT0);
#endif

        /* Reset MCI controller. It is not specified in the data sheet how long
         * to wait for reset to complete, so we'll just wait a few ms here.
         */
        mci->CR.swrst = 1;
        delay_ms(10);

        /* Disable MCI controller during configuration */
        mci->CR.mcidis = 1;

        /* Use read proof, otherwise we might not be able to read all data and
         * will get overrun errors.
         */
        mci->MR.rdproof = 1;

        /* Write proof is needed to make 4-bit mode stable since we might not
         * be able to fill the fifo fast enough with the CPU.
         */
        mci->MR.wrproof = 1;

        /* Use maximum values for data timeout */
        mci->DTOR.dtocyc = 0xf;
        mci->DTOR.dtomul = 0x7;

        /* MCI_CLK = FMCK / (2 * (clkdiv + 1)), FMCK is 60 MHz */
        mci->MR.clkdiv = 0;

        /* 4 bit or 1 bit */
#ifdef USE_1BIT_MODE
        mci->SDCR.sdcbus = 0;
#else
        mci->SDCR.sdcbus = 2;
#endif

        /* Use MCI slot A/B */
#ifdef SDIO_SLOT_A
        mci->SDCR.sdcsel = 0;
#else
        mci->SDCR.sdcsel = 1;
#endif

        /* Enable MCI controller */
        mci->CR.mcien = 1;
}

void wl_sdio_cmd_cb(U8 idx, U32 arg, U8 flags, U32 *rsp,
                    const U8 *data, U16 len)
{
        volatile avr32_mci_t *mci = &AVR32_MCI;
        U32 sr;
        union {
                avr32_mci_cmdr_t CMDR;
                U32 cmdr;
        } reg = { { 0 } };

        Assert(idx < (1 << 6));
        Assert(mci->SR.cmdrdy);

        reg.CMDR.cmdnb = idx;

        switch (idx) {
        case 3:
                reg.CMDR.rsptyp = 1; /* R6 */
                break;

        case 7:
                reg.CMDR.rsptyp = 3; /* R1b */
                break;

        case 52:
                reg.CMDR.rsptyp = 1; /* R5 */
                break;

        case 53:
                /* Set length */
                Assert(len <= MAX_BLOCK_LEN);
                mci->BLKR.bcnt = len % MAX_BLOCK_LEN;

                reg.CMDR.rsptyp = 1; /* R5 */

                /* Start data transfer */
                reg.CMDR.trcmd = 1;

                /* SDIO byte transfer */
                reg.CMDR.trtyp = 4;

                if (flags & CMD_FLAG_TO_HOST)
                        reg.CMDR.trdir = 1;
                break;

        default:
                Assert(0);
        }

        /* Command is transmitted when CMDR is written */
        mci->argr = arg;
        mci->cmdr = reg.cmdr;

        /* Wait until command is transmitted and a respone is recvd,
         * (only required if rsp is != NULL)
         */
        while (!mci->SR.cmdrdy);
        if (rsp)
                *rsp = mci->rspr0;

        /* Reading sr might reset some status bits, so make a shadow copy */
        sr = mci->sr;
#if 0
        Assert(!(sr & AVR32_MCI_SR_DTOE_MASK));
        Assert(!(sr & AVR32_MCI_SR_OVRE_MASK));
        Assert(!(sr & AVR32_MCI_SR_CSTOE_MASK));
        Assert(!(sr & AVR32_MCI_SR_RCRCE_MASK));
        Assert(!(sr & AVR32_MCI_SR_RDIRE_MASK));
        Assert(!(sr & AVR32_MCI_SR_RENDE_MASK));
        Assert(!(sr & AVR32_MCI_SR_RINDE_MASK));
        Assert(!(sr & AVR32_MCI_SR_RTOE_MASK));
#endif
}


void wl_sdio_tx_cb(const U8 *data, U16 len)
{
        volatile avr32_mci_t *mci = &AVR32_MCI;
        UnionCPtr ptr;

        Assert(data);
        Assert(len);

        ptr.u8ptr = data;
        while (len) {
                while (!mci->SR.txrdy);

                /* XXX when len is not size aligned to 4 bytes, we might
                 * read outside the buffer. Should probably be ok since
                 * word size is 4 bytes.
                 */

                /* access data using byte pointers since we might get unaligned
                 * data from lwip. The cpu will issue a data abort if we try
                 * to access data which is not aligned to 4 bytes using a
                 * word pointer.
                 */
                mci->tdr = (ptr.u8ptr[0] << 24) | (ptr.u8ptr[1] << 16) |
			(ptr.u8ptr[2] << 8) | ptr.u8ptr[3];

                /* ok, 4 bytes transmitted */
                ptr.u32ptr++;
                if (len >= 4)
                        len -= 4;
                else
                        len = 0;
        }

        /* Wait for the bus to become idle */
        while (!mci->SR.xfrdone);
}

void wl_sdio_rx_cb(U8 *data, U16 len)
{
        volatile avr32_mci_t *mci = &AVR32_MCI;
        UnionPtr ptr;

        Assert(data);
        Assert(len);

        ptr.u8ptr = data;
        while (len) {
                U32 rdr;
                while (!mci->SR.rxrdy);

                /* XXX when len is not size aligned to 4 bytes, we might
                 * write outside the buffer. For now we'll just hope that
                 * nothing else live there.
                 */

                /* access data using byte pointers, see wl_sdio_tx_cb()  */
                rdr = mci->rdr;
                ptr.u8ptr[0] = (rdr >> 24) & 0xff;
                ptr.u8ptr[1] = (rdr >> 16) & 0xff;
                ptr.u8ptr[2] = (rdr >> 8) & 0xff;
                ptr.u8ptr[3] = rdr & 0xff;

                /* 4 bytes recv'd */
                (void) *ptr.u32ptr++;
                if (len >= 4)
                        len -= 4;
                else
                        len = 0;
        }
}

void wl_sdio_irq_cb(U8 enable)
{
        volatile avr32_mci_t *mci = &AVR32_MCI;

#ifdef USE_POLL
        return;
#endif

#ifdef SDIO_SLOT_A
        if (enable)
                mci->IER.sdioirqa = 1;
        else
                mci->IDR.sdioirqa = 1;
#else
        if (enable)
                mci->IER.sdioirqb = 1;
        else
                mci->IDR.sdioirqb = 1;
#endif
}

__attribute__((__interrupt__)) static void avr32_irq_handler(void)
{
        volatile avr32_mci_t *mci = &AVR32_MCI;

	if (mci->SR.sdioirqa || mci->SR.sdioirqb)
                wl_sdio_irq();
}
