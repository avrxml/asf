@echo off

if /I a%1a==aa (
	echo Usage egs: 
	echo   download_all_sb 
	echo       edbg 
	echo       [mcu eg ATSAMD21J18A]
	echo       [path to serial bridge elf file]
	echo       [gain format SAMD21 or SAMD25]
	echo       [Target 2B0, 3A0 or 3400]
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
set varPath=%PROGRAMFILES%

set TGTCHIP=xx
if /I a%5a==a2B0a (
  set TGTCHIP=2B0
)
if /I a%5a==a3A0a (
  set TGTCHIP=3A0
)
if /I a%5a==a3400a (
  set TGTCHIP=3400
)
if /I %TGTCHIP%==xx (
  echo Need to specify target chip 2B0, 3A0 or 3400
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
  set TOOLSN=%D21SN%
) else (
  set TOOLSN=%AARDVARKSN%
)

if a%8a==aa (
  set PRGPORTNUM=0
) else (
  set PRGPORTNUM=%8
)

set prm09=%9
shift
set prm10=%9
shift
set prm11=%9

:: what combination of certs has user supplied
if a%prm09%a==aa (
  :: none so defaults
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
    :: specified no rsa - just load ecdsa if present
	set KEYCERTS=none none %prm11%
  ) else (
    :: some rsa
	:: and possibly and ecdsa cert too
	set KEYCERTS=%prm09% %prm10% %prm11%
  )
)


:CheckOS
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO RUN)
:64BIT
set varPath=%PROGRAMFILES(X86)%
:RUN

echo %GAIN_MODE% flashing script: please connect %TOOL% and power up the board.
"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" -t %TOOL% -s %TOOLSN% -i SWD -d %MCU% chiperase
:failout
IF %ERRORLEVEL% NEQ 0 ( echo Fail
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
)
"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" -t %TOOL% -s %TOOLSN% -i SWD -d %MCU% program -f %IMAGE_FILE%
IF %ERRORLEVEL% NEQ 0 ( echo Fail
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
)

"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" -t %TOOL% -s %TOOLSN% -i SWD -d %MCU% -xr reset
"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" -t %TOOL% -s %TOOLSN% -i SWD -d %MCU% version

ping 192.0.0.1 -w 500 > NUL

download_all.bat UART %GAIN_MODE% %TGTCHIP% %AARDVARKSN% %PRGPORTNUM% dev %KEYCERTS%
IF %ERRORLEVEL% NEQ 0 ( echo Fail
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
echo Downloading ends sucessfully

pause
