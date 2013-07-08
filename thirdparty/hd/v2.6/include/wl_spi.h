/*!
 *  \file wl_spi.h
 *  \brief SPI interface for wl_api.
 *  Copyright (C) 2010 HD Wireless AB
 *
 *  You should have received a copy of the license along with this library.
 */

#ifndef WL_SPI_H
#define WL_SPI_H

#ifndef WITHOUT_STDINT
#include <stdint.h>
#endif

/** \defgroup wl_spi SPI Interface
 *
 * These functions implement the interface that the wl_api library
 * needs to work with a SPI transport layer.
 *
 * The functions prototyped here must be implemented when porting the
 * wl_api library to a new platform with a different SPI configuration
 *
 * On platforms supported by H&D Wireless these functions are
 * implemented in the file avr32_spi.c
 *
 *  @{
 */

/**
 * Maximum transfer size. This will set an upper limit on the len parameter
 * passed to wl_spi_txrx_cb().
 *
 *
 */
#define MAX_BLOCK_LEN                 512


/**
 * Indicates that the spi driver needs to be polled in order to make
 * forward progress, i.e. it does not support interrupts through SD pin 8.
 *
 * The actual polling will result in wl_spi_txrx_cb() being call to
 * request status information from the device.
 *
 * To activate polling, this flag should be set in wl_spi_init_cb().
 */
#define SPI_FLAG_POLL (1 << 0)


/**
 * This function will be invoked when wlan device initialization should be
 * performed, this happens when the wl_fw_download() function in the transport
 * group of wl_api is invoked.
 *
 * The wifi device requires spi mode 3, i.e. clock polarity high and sample
 * on second phase. This corresponds to CPOL=1, CPHA=1. Maximum frequency on
 * spi clock is 30 MHz.
 *
 * The function is also responsible for doing any necessary spi initialization
 * such as allocating gpio's, setting up the SPI master, one time allocations of
 * dma buffers etc.
 *
 *
 * If the SPB105 device is used, two signals; POWER (pin 10 on SPB105) and
 * SHUTDOWN (pin 4 on SPB105) might be connected to gpio's on the host.
 * The GPIO_POWER_PIN is the main power supply to the device. The
 * GPIO_SHUTDOWN_PIN (active low) should be defined as an input.
 *
 * After GPIO_POWER_PIN is pulled high by the host, the device will pull the
 * GPIO_SHUTDOWN_PIN high once the device is properly powered.
 *
 * However, if pin 4 (GPIO_SHUTDOWN_PIN) is not connected to the host, a delay
 * of up to 250 ms must be added after GPIO_POWER_PIN is pulled high to ensure
 * that startup is completed. The actual time is usually much shorter, therefore
 * one might try to reduce the delay for a particualar hardware design.
 *
 * On SPB104, the GPIO_POWER_PIN will be connected to VCC and GPIO_SHUTDOWN_PIN
 * will be unconnected; hence we have to make sure that we have enough delay
 * after powering on the host. Since the device power-on usually happens at the
 * same time as the host power-on, the startup time of the host can be
 * subtracted from any delay put into wl_spi_init_cb().
 *
 * @param flags is an out parameter that should hold any spi flags upon return.
 *        The avaible flags are prefixed with SPI_FLAG_
 *
 * @return 0 on success
 *         -1 if any error occurs
 *
 */
int wl_spi_init_cb(uint8_t *flags);


/**
 * Invoked when a spi transfer should be performed.
 *
 * All buffers that are allocated by the wl library will have a size that is
 * aligned to 4. If size-unaligned data is passed to this function, it is
 * always allocated by the ip stack. If 4-byte size alignment (e.g. for DMA)
 * is required, 1-3 extra padding bytes can be transmitted after the in buffer.
 * These bytes must be 0xff.
 *
 * Since size-unaligned data always comes from the ip stack, the out ptr is
 * always NULL for such data.
 *
 * @param in points a buffer which holds the data to be transmitted. If NULL,
 *        then \a len bytes with the value 0xff should be transmitted on the
 *        bus.
 * @param out points a buffer should hold the data received from the device. If
 *        NULL, any received data can be discarded.
 * @param len is the length of the in and out buffers.
 *
 */
void wl_spi_txrx_cb(const uint8_t *in, uint8_t* out, uint16_t len);


/**
 * Invoked when spi rx interrupts from the device should be enabled or disabled.
 * Note that the spi interrupts are obtained from pin 8 on SPB104 or pin 3 from
 * SPB105. This pin can be be connected to a gpio on the host. The irq line
 * will signal an interrupt on both edges.
 *
 * In general, the wifi device will not issue a new interrupt unless the
 * last interrupt has been handled. Also, during normal operation (i.e after
 * the complete callback registered in wl_init() has been invoked),
 * wl_spi_irq_cb() will never be invoked so interrupts will be enabled all
 * the time. For the SPI-mode, the purpose of wl_spi_irq_cb() is basically to
 * make sure that the first interrupt (coming after the reset performed in
 * wl_spi_init_cb()) is ignored.
 *
 * If SPI_FLAG_POLL was set in wl_spi_init_cb(), then this function can be
 * left empty and the wifi device will be used in polled mode. In polled mode,
 * the interrupt line is not used. Regardless of polled or interrupt-mode,
 * wl_poll() must be called to ensure progress of the driver.
 *
 * @param enable specifies if interrupts should be enabled or disabled.
 *
 */
void wl_spi_irq_cb(uint8_t enable);


/**
 * Invoked when the spi cs for the wifi device should be enabled. Note that
 * multiple calls to wl_spi_txrx_cb() might be done during a 'single' chip
 * select.
 *
 * @param enable specifies whether chip select should be asserted or deasserted,
 *        The chip select signal is active low, so if enable is '1' then the
 *        chip select connected to the wifi device should be set to '0'.
 *
 */
void wl_spi_cs_cb(uint8_t enable);


/**
 * This function should be called whenever an interrupt is detected. It can
 * be called from an interrupt context.
 *
 * If SPI_FLAG_POLL was set in wl_spi_init_cb(), then wl_spi_irq()
 * should never be called.
 *
 */
extern void wl_spi_irq(void);

/*!  @} */

#endif
