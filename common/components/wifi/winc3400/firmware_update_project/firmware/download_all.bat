@ECHO off
setlocal EnableDelayedExpansion

:: %1 type of bus
:: %2 type of layout (D21-SAMW25)
:: %3 (3400-2B0-3A0) 
:: %4 (aardvark_serial_number or 0)
:: %5 com port for edbg usb device or 0
:: %6 (test) wait 5 seconds after programming for the chip to restart or 'dev'
:: %7 TLS_CLIENT_KEY_RSA   file eg ..\..\..\tls_cert_store\winc_rsa.key or 'none'
:: %8 TLS_CLIENT_CRT_RSA   file eg ..\..\..\tls_cert_store\winc_rsa.cer or 'none'
:: %9 TLS_CLIENT_CRT_ECDSA file eg ..\..\..\tls_cert_store\winc_ecdsa.cer or 'none'

SET FIRMWARE_2B0_PATH=../../../firmware/m2m_aio_2b0.bin		
SET FIRMWARE_3A0_PATH=../../../firmware/m2m_aio_3a0.bin		
SET FIRMWARE_3400_PATH=../../../firmware/m2m_aio_3400.bin

SET OTA_2B0=../../../ota_firmware/m2m_ota_2b0.bin
SET OTA_3A0=../../../ota_firmware/m2m_ota_3a0.bin
SET OTA_3400=../../../ota_firmware/m2m_ota_3400.bin

SET CA_DIR=../../../tls_cert_store/CA

rem Either specify all certs, even if that is to explicitly set them to none or specify none for defaults
if /I "a%7a" == "aa" (
	SET TLS_CLIENT_KEY_RSA=../../../tls_cert_store/winc_rsa.key
	SET TLS_CLIENT_CRT_RSA=../../../tls_cert_store/winc_rsa.cer
	echo Defaulting to RSA key cert pair @ %TLS_CLIENT_CRT_RSA% and %TLS_CLIENT_KEY_RSA%

	SET TLS_CLIENT_CRT_ECDSA=../../../tls_cert_store/winc_ecdsa.cer
	echo Defaulting to ECDSA certificate @ %TLS_CLIENT_CRT_ECDSA%
) else (
	if /I "a%7a" == "anonea" (
	
		SET TLS_CLIENT_KEY_RSA=

		if /I "a%8a" == "anonea" (
			SET TLS_CLIENT_CRT_RSA=
		) else (
			SET TLS_CLIENT_CRT_RSA=%8
			echo RSA needs a key and cert [cert with no key]
			goto usage
		)
	
	) else (
	
		SET TLS_CLIENT_KEY_RSA=%7

		if /I "a%8a" == "anonea" (
			SET TLS_CLIENT_CRT_RSA=
			echo RSA needs a key and cert [key with no cert]
			goto usage
		) else (
			SET TLS_CLIENT_CRT_RSA=%8
		)
	)
	
	if /I "a%9a" == "aa" (
		rem RSA specifically switched off, but no ECDSA
		echo No ECDSA certificate specified [use 'none' if this is intentional]
		goto usage
	) else (
		if /I "a%9a" == "anonea" (
			SET TLS_CLIENT_CRT_ECDSA=
		) else (
			SET TLS_CLIENT_CRT_ECDSA=%9
		)
	)
)

echo Mode %1
if /I "%1" == "UART"  Goto contine_UART
if /I "%1" == "I2C"   Goto contine_I2C
if /I "%1" == "OTA"   Goto contine_OTA

goto usage

:contine_I2C
Set  FMode=debug_i2c
set Mode=I2C
goto START

:contine_OTA
Set  FMode=debug_ota
set Mode=OTA
goto START

:contine_UART
Set  FMode=debug_uart
set Mode=UART
goto START

:START

rem must jump past else the file check gives a syntax error that delayedexpansion cannot fix?
IF "a%TLS_CLIENT_CRT_RSA%a"=="aa" goto rsacertchkd
pushd Tools\root_certificate_downloader\%FMode%
if not exist %TLS_CLIENT_CRT_RSA% (
	echo Cannot find certificate. %TLS_CLIENT_CRT_RSA%
	pwd
	popd
	goto usage
)
popd
:rsacertchkd

IF "a%TLS_CLIENT_KEY_RSA%a"=="aa" goto rsakeychkd
pushd Tools\root_certificate_downloader\%FMode%
if not exist %TLS_CLIENT_KEY_RSA% (
	echo Cannot find certificate. %TLS_CLIENT_KEY_RSA%
	pwd
	popd
	goto usage
)
popd
:rsakeychkd

IF "a%TLS_CLIENT_CRT_ECDSA%a"=="aa" goto ecdsacertchkd
pushd Tools\root_certificate_downloader\%FMode%
if not exist %TLS_CLIENT_CRT_ECDSA% (
	echo Cannot find certificate. %TLS_CLIENT_CRT_ECDSA%
	pwd
	popd
	goto usage
)
popd
:ecdsacertchkd

if /I "a%4a" == "aa" (
	set AARDVARK=0
) else (
	set AARDVARK=%4
)

if /I "a%5a" == "aa" (
  set COMPORT=
) else (
  if /I "a%5a" == "a0a" (
  	set COMPORT=
  ) else (
	set COMPORT=-port %5
  )
)

if /I "a%2a" == "aD21a" goto goodlayout
if /I "a%2a" == "aSAMD21a" goto goodlayout
if /I "a%2a" == "aSAMW25a" goto goodlayout
if /I "a%2a" == "aSAM4Sa" goto goodlayout
goto usage
:goodlayout



echo Chip %3
if /I "%3" == "3400"  Goto chip3400
if /I "%3" == "2B0"   Goto chip2B0
if /I "%3" == "3A0"   Goto chip3A0

:USAGE
echo Usage %0 (I2C-UART-OTA) (D21-SAMD21-SAMW25-SAM4S) (3400-2B0-3A0) (aardvark_serial_number or 0) (comport or 0) (test or dev) (rsa key or none) (rsa cert or none if key is none) (ecdsa cert or none)
@ECHO %CMDCMDLINE% | FIND /I /C "/C" > NUL && PAUSE
exit /b 2

:chip3400
set VARIANT=3400
goto parmsok
:chip3A0
set VARIANT=3A0
goto parmsok
:chip2B0
set VARIANT=2B0
goto parmsok

:parmsok


IF %FMode%==debug_ota (
	echo Creating Image...
	pushd Tools\image_builder\debug\

	image_builder ^
		-no_wait ^
		-ota_img %OTA_2B0% ^
		-fw_path ../../../firmware/wifi_v111/ASIC_2B0/wifi_firmware.bin ^
		-op_path %FIRMWARE_2B0_PATH% ^
		-bf_bin  ../../../boot_firmware/release2B0/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release2B0/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release2B0/downloader_firmware.bin ^
		-ate_img ../../../ate_firmware/m2m_ate.bin 
	IF ERRORLEVEL 1 goto FAILED
	
	image_builder ^
		-1003A0 ^
		-no_wait ^
		-ota_img  %OTA_3A0% ^
		-fw_path ../../../firmware/wifi_v111/ASIC_3A0/wifi_firmware.bin ^
		-op_path %FIRMWARE_3A0_PATH% ^
		-bf_bin  ../../../boot_firmware/release3A0/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release3A0/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release3A0/downloader_firmware.bin ^
		-ate_img ../../../ate_firmware/m2m_ate.bin 
	IF ERRORLEVEL 1 goto FAILED
	
	image_builder ^
		-no_wait ^
		-ota_img  %OTA_3400% ^
		-fw_path ../../../firmware/wifi_v111/ASIC_3400/wifi_firmware.bin ^
		-op_path %FIRMWARE_3400_PATH% ^
		-bf_bin  ../../../boot_firmware/release3400/boot_firmware.bin ^
		-pf_bin  ../../../programmer_firmware/release3400/programmer_firmware.bin ^
		-df_bin  ../../../downloader_firmware/release3400/downloader_firmware.bin ^
		-bt_img ../../../ble/bt_firmware/fw.bin 
	IF ERRORLEVEL 1 goto FAILED

	popd
	goto SUCCESS
)

pushd Tools\image_downloader\%FMode%
echo Downloading Image... (pod %AARDVARK%) (comport %COMPORT%)

image_downloader.exe ^
	-no_wait ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-fw2b0_path %FIRMWARE_2B0_PATH% ^
	-fw3a0_path %FIRMWARE_3A0_PATH% ^
	-fw3400_path %FIRMWARE_3400_PATH%
IF ERRORLEVEL 1 goto FAILED

popd
pushd Tools\gain_builder\%FMode%

echo Downloading Gain Values...

if /I "%2" == "SAMW25" ( 
	Set GAIN_FILE=-hp ../gain_sheets/samw25_gain_setting.csv
) else (
	Set GAIN_FILE=-gtable ../gain_sheets/samd21_gain_setting_hp.csv 
)
gain_builder.exe ^
	%GAIN_FILE% ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-no_wait
IF ERRORLEVEL 1 goto FAILED
popd

rem first cert written needs erase option
set ERASEOPT=-erase

IF "%TLS_CLIENT_CRT_RSA%"=="" (
    echo No TLS Client RSA Certificate specified.
) ELSE (
    pushd Tools\tls_cert_flash_tool\%FMode%
    echo Downloading TLS Client RSA Certificate Materials...
	tls_cert_flash_tool.exe ^
		WRITE ^
		-aardvark %AARDVARK% %COMPORT% ^
		-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
		-key  %TLS_CLIENT_KEY_RSA% ^
		-cert %TLS_CLIENT_CRT_RSA% ^
		-cadir %CA_DIR% ^
		-nowait %ERASEOPT%
	IF ERRORLEVEL 1 goto FAILED
	set ERASEOPT=
    popd
)

IF "%TLS_CLIENT_CRT_ECDSA%"=="" (
    echo No TLS Client ECDSA Certificate specified.
) ELSE (
    pushd Tools\tls_cert_flash_tool\%FMode%
    echo Downloading TLS Client ECDSA Certificate Materials...
	tls_cert_flash_tool.exe ^
		WRITE ^
		-aardvark %AARDVARK% %COMPORT% ^
		-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
		-nokey ^
		-cert %TLS_CLIENT_CRT_ECDSA% ^
		-cadir %CA_DIR% ^
		-nowait %ERASEOPT%
	IF ERRORLEVEL 1 goto FAILED
	set ERASEOPT=
    popd
)

pushd Tools\root_certificate_downloader\%FMode%

echo Downloading root certificates...
set /a c = 0
set seq=
for %%X in (..\binary\*.cer) do (
	set /a c+=1
	@set seq=!seq! %%X
)
root_certificate_downloader.exe ^
	-n %c% %seq% ^
	-no_wait ^
	-aardvark %AARDVARK% %COMPORT% ^
	-bf_bin  ../../../boot_firmware/release%VARIANT%/boot_firmware.bin ^
	-pf_bin  ../../../programmer_firmware/release%VARIANT%/programmer_firmware.bin ^
	-df_bin  ../../../downloader_firmware/release%VARIANT%/downloader_firmware.bin ^
	-e
IF ERRORLEVEL 1 goto FAILED
popd

goto SUCCESS

:FAILED
popd
echo Fail
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                    ########    ###     ####  ##                   ##
echo     ##                    ##         ## ##     ##   ##                   ##
echo     ##                    ##        ##   ##    ##   ##                   ##
echo     ##                    ######   ##     ##   ##   ##                   ##
echo     ##                    ##       #########   ##   ##                   ##
echo     ##                    ##       ##     ##   ##   ##                   ##
echo     ##                    ##       ##     ##  ####  ########             ##
echo     ##                                                                   ##
echo     #######################################################################  
pause
exit

:SUCCESS

if /I "%6" == "test" ( 
	echo Chip Restarting ...
	TIMEOUT /T 5 
	echo Done
)

echo OK
echo     #######################################################################
echo     ##                                                                   ##
echo     ##                 ########     ###     ######   ######              ##
echo     ##                 ##     ##   ## ##   ##    ## ##    ##             ##
echo     ##                 ##     ##  ##   ##  ##       ##                   ##
echo     ##                 ########  ##     ##  ######   ######              ##
echo     ##                 ##        #########       ##       ##             ##
echo     ##                 ##        ##     ## ##    ## ##    ##             ##
echo     ##                 ##        ##     ##  ######   ######              ##
echo     ##                                                                   ##
echo     #######################################################################

echo Downloading ends successfully
pause
