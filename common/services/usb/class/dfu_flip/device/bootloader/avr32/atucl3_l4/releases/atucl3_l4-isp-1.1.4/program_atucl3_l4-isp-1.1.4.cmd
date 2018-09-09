@echo off
setlocal enableextensions enabledelayedexpansion

: This command script programs the ISP (flash array), the ISP
: configuration word (User page) and the general-purpose fuse bits.

: Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries. 
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
avr32program chiperase -F
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

echo.
echo Programming MCU memory from `atucl3_l4-isp-1.1.4.hex'.
avr32-objcopy -I ihex -O binary atucl3_l4-isp-1.1.4.hex atucl3_l4-isp-1.1.4.bin
avr32program program -finternal@0x80000000 -cint -e -v -O0x80000000 -Fbin atucl3_l4-isp-1.1.4.bin
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)
del atucl3_l4-isp-1.1.4.bin

echo.
echo Programming ISP configuration word \(default for STK600 RCUCL3 i.e. Word2 == 0x929E0E62 and Word1 == 0xE11EFFD7\)
avr32program program -finternal@0x80000000 -cint -e -v -O0x808001F8 -Fbin atucl3_l4-isp_cfg-1.1.4.dat
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

echo.
echo Programming general-purpose fuse bits.
avr32program writefuses -finternal@0x80000000 gp=0xE077FFFF
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

echo.
echo Resetting MCU.
avr32program run -R
if ERRORLEVEL 1 (
	color 47
	pause
	exit
)

color 27
pause
