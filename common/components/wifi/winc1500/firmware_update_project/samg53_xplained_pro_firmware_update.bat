@ECHO Off
cd firmware
download_all_sb.bat edbg ATSAMG53N19 Tools\serial_bridge\samg53_xplained_pro_serial_bridge.elf SAMG53 0x00400000 0x00080000
pause