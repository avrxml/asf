@echo off

if /I a%1a==aa (
	echo Usage egs: 
	echo   download_all_sb 
	echo       edbg 
	echo       [mcu eg ATSAMD21J18A]
	echo       [path to at cmd app or serial bridge elf file]
	echo       [gain format SAMD21 or SAMW25]
	echo       [3A0]
	echo       [SamD2x SNo [eg ATML2130021800006127] or 0]
	echo       [Aardvark SNo [eg 2238122489] or 0]
	echo       [com port or 0]
	echo       rsa key  - file eg ..\..\..\tls_cert_store\winc_rsa.key or none
	echo       rsa cert - file eg ..\..\..\tls_cert_store\winc_rsa.cer or none if key was none
	echo       ecdsa cert - file eg ..\..\..\tls_cert_store\winc_ecdsa.cer or none
	echo .
	echo This script looks in tls_cert_store for winc_rsa.cer/key and winc_ecdsa.cer and uses what it finds.
	echo Root certs are read from Tools\root_certificate_downloader\binary\...
)

set TOOL=%1
set MCU=%2
set IMAGE_FILE=%3
set GAIN_MODE=%4
set TGTIN=%5
set PRTIN=%8
set varPath=%PROGRAMFILES%

set TGTCHIP=xx
if /I a%TGTIN%a==a3A0a (
  set TGTCHIP=3A0
)
if /I %TGTCHIP%==xx (
  echo Need to specify target chip 3A0
  goto failout
)

if a%6a==aa (
  set D21SN=0
) else (
  set D21SN=%6
)
if a%7a==aa (
  set AARDVARKSN=0
) else (
  set AARDVARKSN=%7
)
if /I %TOOL%==edbg (
  set TOOLSN=-s %D21SN%
) else (
  set TOOLSN=-s %AARDVARKSN%
)

if /I "%TOOLSN%"=="-s 0" (
  set TOOLSN=
)

if a%PRTIN%a==aa (
  set PRGPORTNUM=0
) else (
  set PRGPORTNUM=%PRTIN%
)

set prm09=%9
shift
set prm10=%9
shift
set prm11=%9

if /I a%prm09%a==aa goto gotparms
if /I a%prm09%a==anonea goto got09
pushd Tools\tls_cert_flash_tool\Debug_I2C
if not exist %prm09% (
	echo Cannot find: %prm09%	
	popd
	goto failout
)
popd
:got09

if /I a%prm10%a==aa goto gotparms
if /I a%prm10%a==anonea goto got10
pushd Tools\tls_cert_flash_tool\Debug_I2C
if not exist %prm10% (
	echo Cannot find: %prm10%	
	popd
	goto failout
)
popd
:got10

if /I a%prm11%a==aa goto gotparms
if /I a%prm11%a==anonea goto got11
pushd Tools\tls_cert_flash_tool\Debug_I2C
if not exist %prm11% (
	echo Cannot find: %prm11%
	popd
	goto failout
)
popd
:got11

:gotparms

rem what combination of certs has user supplied
if a%prm09%a==aa (
  rem none so defaults
  set KEYCERTS=none none none
  if exist tls_cert_store\winc_rsa.key (
    echo Found tls_cert_store\winc_rsa.key
    if exist tls_cert_store\winc_rsa.cer (
	  echo Found tls_cert_store\winc_rsa.cer
	  if exist tls_cert_store\winc_ecdsa.cer (
	    echo Found tls_cert_store\winc_ecdsa.cer
	    set KEYCERTS=..\..\..\tls_cert_store\winc_rsa.key ..\..\..\tls_cert_store\winc_rsa.cer ..\..\..\tls_cert_store\winc_ecdsa.cer
	  ) else (
	    set KEYCERTS=..\..\..\tls_cert_store\winc_rsa.key ..\..\..\tls_cert_store\winc_rsa.cer none
	  )
    ) else (
	  if exist tls_cert_store\winc_ecdsa.cer (
	    echo Found tls_cert_store\winc_ecdsa.cer
	    set KEYCERTS=none none ..\..\..\tls_cert_store\winc_ecdsa.cer
	  )
	)
  ) else (
    if exist tls_cert_store\winc_ecdsa.cer (
      echo Found tls_cert_store\winc_ecdsa.cer
      set KEYCERTS=none none ..\..\..\tls_cert_store\winc_ecdsa.cer
	) 
  )
) else (
  if a%prm09%a==anonea (
    rem specified no rsa, just load ecdsa if present
	set KEYCERTS=none none %prm11%
  ) else (
    rem some rsa and possibly and ecdsa cert too
  	set KEYCERTS=%prm09% %prm10% %prm11%
  )
)

Echo Certs: %KEYCERTS%

:CheckOS
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO RUN)
:64BIT
set varPath=%PROGRAMFILES(X86)%
:RUN
set atprogram="%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe"

rem ######## Info #########################################################
echo Tool:    %atprogram%
echo Devices:
%atprogram% list
echo.

rem ######## Instruction #########################################################
echo %GAIN_MODE% flashing script: please connect %TOOL% and power up the board.
echo.
echo Calling...
echo %atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% chiperase
echo %atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% program ^^
echo     -f %IMAGE_FILE%
echo %atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% reboot
echo download_all.bat UART %GAIN_MODE% %TGTCHIP% %AARDVARKSN% %PRGPORTNUM%
echo.

rem ######## Program loop #########################################################
FOR /L %%H IN (1,1,5) DO (
	echo ** Programming attempt %%H of 5...

	echo Erasing host...
	%atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% chiperase
	echo Programming host...
	%atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% program -f %IMAGE_FILE%
	if a%MCU%a==aATSAM4SD32Ca (
		echo Setting Boot from Flash...
		%atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% write -fs -o 0 --values 0x42
	)
	echo Resetting host...
	%atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% reboot
	PING 127.0.0.1 -n 6 >NUL
	%atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% reset
	PING 127.0.0.1 -n 2 >NUL

	echo Verifying com port was enumerated...
	call test_edbg.cmd
	if errorlevel 1 (
		echo Rebooting host...
		%atprogram% -t %TOOL% %TOOLSN% -i SWD -d %MCU% reboot
		PING 127.0.0.1 -n 6 >NUL
	)
	
	echo Downloading FW...
	call download_all.bat UART %GAIN_MODE% %TGTCHIP% %AARDVARKSN% %PRGPORTNUM% dev %KEYCERTS% && goto okout
)

echo Downloading failed
goto failout


:okout
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
echo Downloading ends sucessfully
exit /b 0


:failout
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
exit /b 1
)
