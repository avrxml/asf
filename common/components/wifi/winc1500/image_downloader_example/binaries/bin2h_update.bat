@echo off

echo **************************************************
echo *    ^> WINC1500 Binary to Header File Script ^<   *
echo *           Owner:  Atmel Corporation            *
echo **************************************************
echo.
echo Note: the required firmware ^& root certificate binary 
echo       files can be found in the Firmware Update Project
echo       from the ASF.
echo.

:: WINC1500 firmware location.
set /p fwPath=Please enter full path to WINC1500 firmware folder: 
IF (%fwPath%) == () (
	set fwPath=..\..\firmware_update_project\firmware\firmware
)
IF NOT EXIST "%fwPath%" (
	echo [ERROR] Provided file path does not exist!
)

:: WINC1500 root certificate location.
set /p fwCert=Please enter full path to WINC1500 root certificate folder: 
IF (%fwCert%) == () (
	set fwCert=..\..\firmware_update_project\firmware\Tools\root_certificate_downloader\crt
)
IF NOT EXIST "%fwCert%" (
	echo [ERROR] Provided file path does not exist!
)

:: Call bin2h tool to generate header files.
echo.
echo Generating header files...
echo.
bin2h.exe %fwPath%\m2m_aio_2b0.bin m2m_aio_2b0.h winc_fw
bin2h.exe %fwPath%\m2m_aio_3a0.bin m2m_aio_3a0.h winc_fw
bin2h.exe %fwCert%\DigiCert_Root.cer DigiCert_Root.h DigiCert_Root
bin2h.exe %fwCert%\DigiCertSHA2_Root.cer DigiCertSHA2_Root.h DigiCertSHA2_Root
bin2h.exe %fwCert%\GeoTrustGlobalCA_Root.cer GeoTrustGlobalCA_Root.h GeoTrustGlobalCA_Root
bin2h.exe %fwCert%\Radius_Root.cer Radius_Root.h Radius_Root
bin2h.exe %fwCert%\VeriSign_Root.cer VeriSign_Root.h VeriSign_Root
echo.
echo Done.
pause