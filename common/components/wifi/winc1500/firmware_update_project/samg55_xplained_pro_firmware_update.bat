@ECHO Off
cd firmware
download_all_sb.bat edbg ATSAMG55J19 Tools\serial_bridge\samg55_xplained_pro_serial_bridge.elf SAMG55 0x00400000 0x00080000
pause