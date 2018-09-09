@ECHO Off

set varPath=%PROGRAMFILES%

:CheckOS
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO RUN)
:64BIT
set varPath=%PROGRAMFILES(X86)%
:RUN


set /A edbgCnt=0
set SN=0
for /f "tokens=1-2" %%i in ('"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" list') do (
	if "%%i" == "edbg" (
		set SN=%%j
		set /A edbgCnt+=1
	)
)	

if %edbgCnt%==0 (
	echo Cannot find and EDBG boards?
	echo see  '"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" list'
	exit /b 1
)

if %edbgCnt% GTR 1 (
	echo This batch file is unsuitable if more than one EDBG based development board is installed, found %edbgCnt%
	echo Use download_all_sb.bat with options
	echo		edbg 
	echo		ATSAMD21J18A 
	echo		Tools\serial_bridge\samd21_xplained_pro_serial_bridge.elf 
	echo		SAMD21 
	echo		3400 
	echo		serial number of the dev board attached to the board you wish to program - see '"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" list'
	echo		0 
	echo		com port number assigned to the dev board attached to the board you wish to program by the OS
	echo		..\..\..\tls_cert_store\winc_rsa.key 
	echo		..\..\..\tls_cert_store\winc_rsa.cer 
	echo		none
	exit /b 1
)
cd firmware
download_all_sb.bat edbg ATSAMD21J18A Tools\serial_bridge\samd21_xplained_pro_serial_bridge.elf SAMD21 3400 %SN% 0 0 none none none

pause

