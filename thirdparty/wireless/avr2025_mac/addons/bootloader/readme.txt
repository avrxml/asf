/**
 * @file readme.txt
 *
 * @brief explains briefly the procedure to flash the NCP using bootloader.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 */
 
/*
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

 This bootloader is to upgrade the firmware in NCP using bootloader through a host controller.
                 _________________
                |  Bootloader PC  |  COM(USB)                EXT(USART) 
     			|	  tool        | ---------> Host Board ------------> NCP board
		        |_________________|

Supported NCP boards: 
    atxmega256a3u_zigbit_ext
    atmega256rfr2_zigbit_ext
	
Other Supported Boards:	
	atmega256a3_reb_cbb
	atmegarfa1_stb
	atmegarfr2_stb

Steps to use this bootloader:
1. Flash the corresponding bootloader .hex file. Example: For atxmega256a3u device, flash atxmega256a3u_zigbit_ext.hex 
   from the location "\thirdparty\wireless\avr2025_mac\addons\bootloader\xmega" 
2. In the host controller board,for example sam4l_xplained_pro. Flash the serial bridge application from the location 
   "\thirdparty\wireless\addons\serial_bridge\example\sam4lc4c_sam4l_xplained_pro"
3. Connect the NCP board, for example atxmega256a3u_zigbit_ext to "ext1" given in sam4l_xplained_pro (host).
   Install Bootloader_PC_Tool_Setup_1.2.2.235.exe from th location "\thirdparty\wireless\avr2025_mac\addons\bootloader"
4. Open the installed Bootloader PC tool, select the corresponding com port of the host board. 
5. Keep the serial port settings as
    Bit rate       9600(atxmega256a3u_zigbit_ext,atmega256rfr2_zigbit_ext) ,38400 (atmega256a3_reb_cbb)
	Data bits      8
	Stop bits      1
	Parity         None
	HW control     None
6. Browse and select the .srec file of application which has to be flashed in NCP.
7. Before the user clicks upload, reset button in NCP board should be pressed. When the user clicks upload, 
   he will be prompted as "please restart the device".Reset button can be released at this time and the upload will start.
8. Wait till the upload gets completed.  
9. After this completion, flash the desired application in the host board. For example, choose "thirdparty\wireless\avr2025_mac\apps\mac\beacon\coord\host" for operating it as beacon coordinator host.
10.For other boards(atmega256a3_reb_cbb,atmegarfa1_stb,atmegarfr2_stb) follow steps 1,4 - 9 .

		 