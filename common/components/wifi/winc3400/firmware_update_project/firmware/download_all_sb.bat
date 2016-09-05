@echo off

set TOOL=%1
set MCU=%2
set IMAGE_FILE=%3
set IMAGE_FILE_FULL_PATH=%CD%\%3
set MCU_ALIAS=%4
set varPath=%PROGRAMFILES%
:CheckOS
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO RUN)
:64BIT
set varPath=%PROGRAMFILES(X86)%
:RUN
echo %MCU_ALIAS% flashing script: please connect %TOOL% and power up the board.

:: Test path length.
if NOT "%IMAGE_FILE_FULL_PATH:~210,1%"=="" (
	echo.
	echo [ERROR] File path is too long. Please move firmware update tool at the root of your hard drive and try again.
	echo.
	pause
	exit
)

:: Try to locate atprogram tool from Atmel Studio.
set atprogPath=
IF EXIST "%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" (
	set "atprogPath=%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe"
) ELSE (
	IF EXIST "%varPath%\Atmel\Atmel Studio 6.2\atbackend\atprogram.exe" (
		set "atprogPath=%varPath%\Atmel\Atmel Studio 6.2\atbackend\atprogram.exe"
	) ELSE (
		echo.
		echo [ERROR] Cannot find atprogram tool at default locations:
		echo "%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe"
		echo "%varPath%\Atmel\Atmel Studio 6.2\atbackend\atprogram.exe"
		echo.
		set /p atprogPath=Please provide full path to atprogram.exe to continue: 
	)
)

:: Perform chip erase.
"%atprogPath%" -t %TOOL% -i SWD -d %MCU% chiperase
IF %ERRORLEVEL% NEQ 0 ( echo Fail
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                  ########    ###     ####  ##                     ##
echo     ##                  ##         ## ##     ##   ##                     ##
echo     ##                  ##        ##   ##    ##   ##                     ##
echo     ##                  ######   ##     ##   ##   ##                     ##
echo     ##                  ##       #########   ##   ##                     ##
echo     ##                  ##       ##     ##   ##   ##                     ##
echo     ##                  ##       ##     ##  ####  ########               ##
echo     ##                                                                   ##
echo     #######################################################################
pause
exit
)

:: Write fuse bit to boot from flash for SAM4/G.
set needfuse=false
IF "%MCU_ALIAS%" == "SAMG53" (set needfuse=true)
IF "%MCU_ALIAS%" == "SAMG55" (set needfuse=true)
IF "%MCU_ALIAS%" == "SAM4S"  (set needfuse=true)
IF "%MCU_ALIAS%" == "SAM4E"  (set needfuse=true)
IF "%needfuse%" == "true" (
	"%atprogPath%" -t %TOOL% -i SWD -d %MCU% write -fs --values 02
)

:: Program serial bridge.
"%atprogPath%" -t %TOOL% -i SWD -d %MCU% program -f %IMAGE_FILE%
IF %ERRORLEVEL% NEQ 0 ( echo Fail
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                  ########    ###     ####  ##                     ##
echo     ##                  ##         ## ##     ##   ##                     ##
echo     ##                  ##        ##   ##    ##   ##                     ##
echo     ##                  ######   ##     ##   ##   ##                     ##
echo     ##                  ##       #########   ##   ##                     ##
echo     ##                  ##       ##     ##   ##   ##                     ##
echo     ##                  ##       ##     ##  ####  ########               ##
echo     ##                                                                   ##
echo     #######################################################################
pause
exit
)

echo Please wait...
ping 192.0.0.1 -w 1000 > NUL

download_all.bat UART
IF %ERRORLEVEL% NEQ 0 ( echo Fail
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                  ########    ###     ####  ##                     ##
echo     ##                  ##         ## ##     ##   ##                     ##
echo     ##                  ##        ##   ##    ##   ##                     ##
echo     ##                  ######   ##     ##   ##   ##                     ##
echo     ##                  ##       #########   ##   ##                     ##
echo     ##                  ##       ##     ##   ##   ##                     ##
echo     ##                  ##       ##     ##  ####  ########               ##
echo     ##                                                                   ##
echo     #######################################################################
pause
exit
)

echo OK.
echo     #######################################################################
echo     ##                                                                   ##
echo     ##               ########     ###     ######   ######                ##
echo     ##               ##     ##   ## ##   ##    ## ##    ##               ##
echo     ##               ##     ##  ##   ##  ##       ##                     ##
echo     ##               ########  ##     ##  ######   ######                ##
echo     ##               ##        #########       ##       ##               ##
echo     ##               ##        ##     ## ##    ## ##    ##               ##
echo     ##               ##        ##     ##  ######   ######                ##
echo     ##                                                                   ##
echo     #######################################################################
echo Programming ends successfully
pause