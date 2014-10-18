/**
 * \file
 *
 * \brief SAM D21/D10/D11 ADC with DMA quick start
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
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
#include <asf.h>

void configure_dma_resource(struct dma_resource *resource);
void setup_transfer_descriptor(DmacDescriptor *descriptor);

void configure_adc(void);
void configure_dac(void);
void configure_dac_channel(void);

//! [dac_module_inst]
struct dac_module dac_instance;
//! [dac_module_inst]

//! [adc_module_inst]
struct adc_module adc_instance;
//! [adc_module_inst]

//! [dma_resource]
struct dma_resource example_resource;
//! [dma_resource]

// [transfer_descriptor]
COMPILER_ALIGNED(16)
DmacDescriptor example_descriptor;
// [transfer_descriptor]

//! [setup]
//! [configure_adc]
void configure_adc(void)
{
//! [setup_adc_config]
	struct adc_config config_adc;
//! [setup_adc_config]

//! [setup_adc_config_defaults]
	adc_get_config_defaults(&config_adc);
//! [setup_adc_config_defaults]

//! [setup_adc_config_extra]
	config_adc.gain_factor     = ADC_GAIN_FACTOR_DIV2;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN4;
	config_adc.resolution      = ADC_RESOLUTION_10BIT;
	config_adc.freerunning     = true;
	config_adc.left_adjust     = false;
//! [setup_adc_config_extra]

//! [setup_adc_set_config]
	adc_init(&adc_instance, ADC, &config_adc);
//! [setup_adc_set_config]

//! [setup_adc_enable]
	adc_enable(&adc_instance);
//! [setup_adc_enable]
}
//! [configure_adc]

//! [configure_dac]
void configure_dac(void)
{
//! [setup_dac_config]
	struct dac_config config_dac;
//! [setup_dac_config]

//! [setup_dac_config_defaults]
	dac_get_config_defaults(&config_dac);
//! [setup_dac_config_defaults]

//! [setup_dac_config_extra]
	config_dac.reference = DAC_REFERENCE_AVCC;
//! [setup_dac_config_extra]

//! [setup_dac_set_config]
	dac_init(&dac_instance, DAC, &config_dac);
//! [setup_dac_set_config]

//! [setup_dac_enable]
	dac_enable(&dac_instance);
//! [setup_dac_enable]
}
//! [configure_dac]

//! [configure_dac_channel]
void configure_dac_channel(void)
{
//! [setup_dac_ch_config]
	struct dac_chan_config config_dac_chan;
//! [setup_dac_ch_config]

//! [setup_dac_ch_config_defaults]
	dac_chan_get_config_defaults(&config_dac_chan);
//! [setup_dac_ch_config_defaults]

//! [setup_dac_ch_set_config]
	dac_chan_set_config(&dac_instance, DAC_CHANNEL_0, &config_dac_chan);
//! [setup_dac_ch_set_config]

//! [setup_dac_ch_enable]
	dac_chan_enable(&dac_instance, DAC_CHANNEL_0);
//! [setup_dac_ch_enable]
}
//! [configure_dac_channel]

//! [config_dma_resource]
void configure_dma_resource(struct dma_resource *resource)
{
//! [setup_dma_config]
	struct dma_resource_config config;
//! [setup_dma_config]

//! [setup_dma_set_config_default]
	dma_get_config_defaults(&config);
//! [setup_dma_set_config_default]

//! [setup_dma_set_config_extra]
	config.peripheral_trigger = ADC_DMAC_ID_RESRDY;
	config.trigger_action = DMA_TRIGGER_ACTON_BEAT;
//! [setup_dma_set_config_extra]

//! [allocate_dma_resource]
	dma_allocate(resource, &config);
//! [allocate_dma_resource]
}
//! [config_dma_resource]

//! [setup_dma_transfer_descriptor]
void setup_transfer_descriptor(DmacDescriptor *descriptor)
{
//! [setup_dma_desc_config]
	struct dma_descriptor_config descriptor_config;
//! [setup_dma_desc_config]

//! [setup_dma_desc_config_set_default]
	dma_descriptor_get_config_defaults(&descriptor_config);
//! [setup_dma_desc_config_set_default]

//! [setup_dma_desc_config_set_extra]
	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.src_increment_enable = false;
	descriptor_config.block_transfer_count = 1000;
	descriptor_config.source_address = (uint32_t)(&adc_instance.hw->RESULT.reg);
	descriptor_config.destination_address = (uint32_t)(&dac_instance.hw->DATA.reg);
	descriptor_config.next_descriptor_address = (uint32_t)descriptor;
//! [setup_dma_desc_config_set_extra]

//! [setup_dma_desc_config_create]
	dma_descriptor_create(descriptor, &descriptor_config);
//! [setup_dma_desc_config_create]
}
//! [setup_dma_transfer_descriptor]
//! [setup]

int main(void)
{
	system_init();

//! [setup_init]
//! [setup_adc]
	configure_adc();
//! [setup_adc]

//! [setup_dac]
	configure_dac();
//! [setup_dac]

//! [setup_dac_channel]
	configure_dac_channel();
//! [setup_dac_channel]

//! [setup_dma_resource]
	configure_dma_resource(&example_resource);
//! [setup_dma_resource]

//! [setup_transfer_descriptor]
	setup_transfer_descriptor(&example_descriptor);
//! [setup_transfer_descriptor]

//! [add_descriptor_to_resource]
	dma_add_descriptor(&example_resource, &example_descriptor);
//! [add_descriptor_to_resource]
//! [setup_init]

//! [main]
//! [start_adc_conversion]
	adc_start_conversion(&adc_instance);
//! [start_adc_conversion]

//! [start_transfer]
	dma_start_transfer_job(&example_resource);
//! [start_transfer]

//! [endless_loop]
	while (true) {
	}
//! [endless_loop]
//! [main]
}
