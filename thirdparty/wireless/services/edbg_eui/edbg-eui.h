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
   
 /*------------------------------------Function Prototypes--------------------------------------------------------------*/
 uint8_t*  edbg_eui_read_eui64(void);