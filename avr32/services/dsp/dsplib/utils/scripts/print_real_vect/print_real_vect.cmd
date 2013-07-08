@echo off

REM Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
REM
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted provided that the following conditions are met:
REM
REM 1. Redistributions of source code must retain the above copyright notice,
REM this list of conditions and the following disclaimer.
REM
REM 2. Redistributions in binary form must reproduce the above copyright notice,
REM this list of conditions and the following disclaimer in the documentation
REM and/or other materials provided with the distribution.
REM
REM 3. The name of ATMEL may not be used to endorse or promote products derived
REM from this software without specific prior written permission.
REM
REM THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
REM WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
REM MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
REM SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
REM INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
REM (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
REM LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
REM ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
REM (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
REM THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
