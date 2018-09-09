@echo off

REM Copyright (c) 2006-2018 Microchip Technology Inc. and its subsidiaries.
REM
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted provided that the following conditions are met:
REM
REM Subject to your compliance with these terms, you may use Microchip
REM software and any derivatives exclusively with Microchip products. 
REM It is your responsibility to comply with third party license terms applicable 
REM to your use of third party software (including open source software) that 
REM may accompany Microchip software.
REM
REM THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
REM WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
REM INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
REM AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
REM LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
REM LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
REM SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
REM POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
REM ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
REM RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
REM THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.


REM ********************* Configuration *********************
REM ********* Arguments *******************
set COM_PORT=COM1
set COM_BAUDRATE=57600
REM ********* Software path **************
REM  Init SF Project Cleaner
for /f %%b in ('pwd') do set CUR_PATH=%%b
set DATAGET="%CUR_PATH%\..\..\PROGRAMS\WINDOWS\DATA_GET\DataGet.exe"
set DATAEXTRACT="%CUR_PATH%\..\..\PROGRAMS\WINDOWS\DATA_EXTRACT\DataExtract.exe"
set DATAPRINT="%CUR_PATH%\..\..\PROGRAMS\WINDOWS\DATA_PRINT\DataPrint.exe"
REM *********************************************************

set TEMP1_FILE=temp1
set TEMP2_FILE=temp2

REM To retreive data from the COM port
%DATAGET% -f %TEMP1_FILE% %COM_PORT% %COM_BAUDRATE%
REM To extract them
%DATAEXTRACT% %TEMP1_FILE% "%%i %%f" 2 > %TEMP2_FILE%
REM And finaly print them
%DATAPRINT% %TEMP2_FILE%

del %TEMP1_FILE%
del %TEMP2_FILE%
