@echo off

echo **************************************************
echo *    ^> WINC1500 Binary to Header File Script ^<   *
echo *           Owner:  Microchip Technology Inc            *
echo **************************************************
echo.
echo Note: the required firmware binary files can be found
echo       in the Firmware Update Project from the ASF.
echo.

:: WINC1500 firmware location.
set /p fwPath=Please enter full path to WINC1500 firmware folder: 
IF (%fwPath%) == () (
	set fwPath=..\..\firmware_update_project\firmware\firmware
)
IF NOT EXIST "%fwPath%" (
	echo [ERROR] Provided file path does not exist!
)

:: Call bin2h tool to generate header files.
echo.
echo Generating header files...
echo.
bin2h.exe %fwPath%\m2m_aio_3a0.bin m2m_aio_3a0.h winc_fw
echo.
echo Done.
pause