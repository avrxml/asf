@ECHO Off
image_builder -no_wait -fw_path ../../../firmware/wifi_v111/ASIC_2B0/wifi_firmware.bin -op_path ../../../firmware/m2m_aio_2b0.bin
image_builder -1003A0 -fw_path ../../../firmware/wifi_v111/ASIC_3A0/wifi_firmware.bin -op_path ../../../firmware/m2m_aio_3A0.bin