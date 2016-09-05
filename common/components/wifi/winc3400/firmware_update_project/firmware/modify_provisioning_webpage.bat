@ECHO off
cd Tools\image_builder\debug\

:: Test path length.
if NOT "%CD:~230,1%"=="" (
	echo.
	echo [ERROR] File path is too long. Please move firmware update tool at the root of your hard drive and try again.
	echo.
	pause
	exit
)

echo.
echo Applying modification for WINC3400 image:
echo.
image_builder -http_modify ../../../provisioning_webpage/ -fw_path ../../../firmware/m2m_aio_3400.bin -no_wait
pause