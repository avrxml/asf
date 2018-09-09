/*
 * Copyright (c) 2014, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "edbg-eui.h"
#include "asf.h"



#ifdef _DEBUG_
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* 128 bit Serial number stored address. It is used for seeding the pseudo random number generator - rand() */
#define S_NO_WORD0 ((uint32_t*)0x0080A00C)
#define S_NO_WORD1 ((uint32_t*)0x0080A040)
#define S_NO_WORD2 ((uint32_t*)0x0080A044)
#define S_NO_WORD3 ((uint32_t*)0x0080A048)

  /*
    4.3.1 Information Interface

    The TWI-bus can be used to request information from the EDBG. This
    is done by sending a special sequence as described below.

    ● START Condition
    ● Address + W
    ● Request token
    ● START Condition (repeated start)
    ● Address + R
    ● Response
    ● ...
    ● STOP Condition

    The sequence starts by sending a normal start condition followed
    by an address byte (default address is 0x28) with the RW bit
    cleared. Then a request token identifying the requested
    information is sent. Normally, a received byte would be put into
    the DGI buffer, but a repeated start will trigger the EDBG to
    parse the received token. Then an address with the RW bit set. The
    EDBG will then start to push the requested data onto the TWIbus.
    All response bytes must be ACKed by the master, and the final byte
    must be NAKed.


    4.3.1.2 Kit Data

    Token: 0xD2

    The EDBG has 256 bytes for storing kit-specific data such as MAC
    address, calibration values etc. See the kit documentation for
    details on how the data is organized. The information can be
    retrieved by using the Kit Data token. Note that not all kits have
    kit-specific data, and the entire section will read as 0.  After
    sending the token, the stored kit-specific data will be sent byte
    by byte starting with location 0, until a stop condition is
    detected.

   */

/*---------------------------------------------------------------------------*/


static uint8_t write_buffer[2] = {
  0x51, 0xd2,
};



#define SLAVE_ADDRESS 0x28

#define TIMEOUT 1000
#define SLAVE_WAIT_TIMEOUT 10
struct i2c_master_module i2c_master_instance;

/* Must read out the full 256 bytes of memory from the EDBG, otherwise
   its internal state will be out of synch and the chip won't return
   the first bytes the next time the chip is queried. */
#define LEN_EUI    256
uint8_t readbuf[LEN_EUI];

uint8_t *
edbg_eui_read_eui64(void)
{
  int timeout2 = 0;
  bool random_mac_address = false;
  uint8_t edbg_status = 0xFF;
  struct i2c_master_config config_i2c_master;
  i2c_master_get_config_defaults(&config_i2c_master);
  config_i2c_master.pinmux_pad0  = EDBG_I2C_SERCOM_PINMUX_PAD0;
  config_i2c_master.pinmux_pad1  = EDBG_I2C_SERCOM_PINMUX_PAD1;

  i2c_master_init(&i2c_master_instance, EDBG_I2C_MODULE, &config_i2c_master);

  i2c_master_enable(&i2c_master_instance);

  struct i2c_master_packet packet = {
    .address         = SLAVE_ADDRESS,
    .data_length     = sizeof(write_buffer),
    .data            = write_buffer,
    .ten_bit_address = false,
    .high_speed      = false,
    .hs_master_code  = 0x0,
  };
   
    do
    {
	    edbg_status = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &packet);
	    if(edbg_status == STATUS_ERR_BAD_ADDRESS)
		{
			if(timeout2++ == SLAVE_WAIT_TIMEOUT)
			{				
				PRINTF("I2C Slave Not Available");
				random_mac_address = true;
				timeout2 = 0;
				break;
			}
		}
		else if(edbg_status == STATUS_ERR_TIMEOUT) {
		PRINTF("Timeout 1\n");
		random_mac_address = true;	
		break;
		}
    } while (edbg_status!=STATUS_OK && edbg_status!=0xFF);
	
  packet.data = readbuf;
  packet.data_length = sizeof(readbuf);
if(random_mac_address == false)
{
  do 
  {
	  edbg_status = i2c_master_read_packet_wait(&i2c_master_instance, &packet) ;
	    if(edbg_status==STATUS_ERR_BAD_ADDRESS)
	    {			
			PRINTF("I2C Slave Not Available");
			random_mac_address = true;	
		    break;
	    }
		else if(edbg_status == STATUS_ERR_TIMEOUT) {
			random_mac_address = true;	
			PRINTF("Timeout 2\n");
			break;
		}		
  } while (edbg_status!=STATUS_OK && edbg_status!=0xFF);
}
  i2c_master_reset(&i2c_master_instance);
  if(random_mac_address || !(readbuf[0] ^ readbuf[1] ^ readbuf[2] ^ readbuf[3] ^ readbuf[4] ^ readbuf[5] ^ readbuf[6] ^ readbuf[7]))
  {
	  unsigned int seed;
	  seed = ((*S_NO_WORD0) ^ (*S_NO_WORD1) ^ (*S_NO_WORD2) ^ (*S_NO_WORD3));
	  srand(seed);
	  //printf("\r\n Generating Random MAC \r\n");
	  for (uint8_t i = 0; i < 8; i++) {
		  *(readbuf+i) = rand();
	  }
	  
  }
  return readbuf;
}
/*---------------------------------------------------------------------------*/
