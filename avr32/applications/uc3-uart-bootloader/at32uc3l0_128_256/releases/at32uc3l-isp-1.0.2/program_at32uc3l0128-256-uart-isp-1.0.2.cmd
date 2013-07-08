@echo off
setlocal enableextensions enabledelayedexpansion

: This command script programs the ISP (flash array), the ISP configuration words
: (User page) and the general-purpose fuse bits.

: Copyright (c) 2011 Atmel Corporation. All rights reserved.
:
: Redistribution and use in source and binary forms, with or without
: modification, are permitted provided that the following conditions are met:
:
: 1. Redistributions of source code must retain the above copyright notice, this
: list of conditions and the following disclaimer.
:
: 2. Redistributions in binary form must reproduce the above copyright notice,
: this list of conditions and the following disclaimer in the documentation
: and/or other materials provided with the distribution.
:
: 3. The name of Atmel may not be used to endorse or promote products derived
: from this software without specific prior written permission.
:
: 4. This software may only be redistributed and used in connection with an Atmel
: AVR product.
:
: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
: WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
: MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
: EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
: ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
: (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
: LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
: ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
: (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
: SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
:


echo.
echo Performing a JTAG Chip Erase command.
avr32program chiperase -F

echo.
echo Programming MCU memory from `at32uc3l0128-256-uart-isp-1.0.2.bin'.
: avr32-objcopy -I ihex -O binary at32uc3l0128-256-uart-isp-1.0.2.a82 at32uc3l0128-256-uart-isp-1.0.2.bin
avr32program program -finternal@0x80000000 -cint -e -v -O0x80000000 -Fbin at32uc3l0128-256-uart-isp-1.0.2.bin

echo.
echo Programming ISP configuration words (default for STK600-RCUC3L0 i.e. Word2 == 0x929E0B79 and Word1 == 0xE11EFFD7)
avr32program program -finternal@0x80000000 -ev -O0x808001F8 -Fbin at32uc3l-uart-isp_cfg-1.0.2.dat

echo.
echo Programming general-purpose fuse bits: everything as default except BOOTPROT set to 3 (for 8192 Bytes).
avr32program writefuses -finternal@0x80000000 gp=0xE077FFFF

echo.
echo Resetting MCU.
avr32program reset
