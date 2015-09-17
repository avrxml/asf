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
echo Applying modification for 2B0 image:
echo.
image_builder -http_modify ../../../provisioning_webpage/ -fw_path ../../../ota_firmware/m2m_ota_2b0.bin -ota_img -no_wait
echo.
echo Applying modification for 3A0 image:
echo.
image_builder -1003A0 -http_modify ../../../provisioning_webpage/ -fw_path ../../../ota_firmware/m2m_ota_3a0.bin -ota_img -no_wait
pause