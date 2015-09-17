/**
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*!
 * \defgroup group_dsplib_adpcmstreaming IMA/DVI ADPCM WAVE Streaming tool
 * \ingroup group_dsplib_tools
 * \brief This tool permits to send IMA/DVI ADPCM data through the serial port of a computer.
 * It takes in parameter the IMA/DVI ADPCM encoded wav file you want to transfer and if the file is valid,
 * it will cut the file into blocks to send them trough the serial port.
 * The serial port used for transferring the data is the \b COM1 and is configured as follow: \b 57600 \b bauds, \b 8 \b bits, \b no \b parity and \b 1 \b stop \b bit.
 *
 * \section usage Usage
 * To use this tool you have two different ways:
 * -# You can just drag on the binary file the IMA/DVI ADPCM encoded wave file you want to transfer through the serial port.
 * -# Open a command window, go to the sources directory and launch the ADPCM_IMA_DVI.exe file with an argument
 * corresponding to the file path of the ADPCM encoded file you want to send (ADPCM_IMA_DVI.exe toto.wav)
 *
 * Once the transfer is running, the software will tell you about the processing advancement and some useful data about the original wav file.
 * \image html software.gif
 * You can abort the transfer when you want by pressing a key.
 *
 * \section information Information
 * All the files related to this module are located under /avr32/services/dsp/dsplib/utils/program/adpcm_streaming\n
 * In this directory you can find the source code and the binary of this module.\n
 * If you want to change the parameters of the serial port, you just have to modify the constants \b RS232_PORT,
 * \b RS232_BAUD_RATE, \b RS232_BYTE_SIZE, \b RS232_PARITY and \b RS232_STOP_BIT defined at the beginning of the "main.c" file.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
