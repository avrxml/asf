@echo off
setlocal enableextensions enabledelayedexpansion

: This command script programs the ISP (flash array), the ISP
: configuration word (User page) and the general-purpose fuse bits.

: Copyright (C) 2006-2018 Microchip Technology Inc. and its subsidiaries. 
:
: Subject to your compliance with these terms, you may use Microchip
: software and any derivatives exclusively with Microchip products. 
: It is your responsibility to comply with third party license terms applicable 
: to your use of third party software (including open source software) that 
: may accompany Microchip software.
:
: THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
: WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
: INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
: AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
: LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
: LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
: SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
: POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
: ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
: RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
: THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.


echo.
echo Performing a JTAG Chip Erase command.
avr32program -p AVRONE chiperase -F
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

echo.
echo Programming MCU memory from `atucd3-isp-1.1.3.hex'.
avr32-objcopy -I ihex -O binary atucd3-isp-1.1.3.hex atucd3-isp-1.1.3.bin
avr32program -p AVRONE program -finternal@0x80000000 -cint -e -v -O0x80000000 -Fbin atucd3-isp-1.1.3.bin
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)
del atucd3-isp-1.1.3.bin

echo.
echo Programming ISP configuration word \(default for UC3D_EK i.e. Word2 == 0x929E0D6B and Word1 == 0xE11EFFD7\)
avr32program -p AVRONE program -finternal@0x80000000 -cint -e -v -O0x808000F8 -Fbin atucd3-isp_cfg-1.1.3.dat
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

echo.
echo Programming general-purpose fuse bits.
avr32program -p AVRONE writefuses -finternal@0x80000000 gp=0xFFF5FFFF
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

echo.
echo Resetting MCU.
avr32program -p AVRONE run -R
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

color 27
pause
