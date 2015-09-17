@ECHO off
image_builder -no_wait -ota_img ../../../ota_firmware/m2m_ota_2b0.bin -fw_path ../../../firmware/wifi_v111/ASIC_2B0/wifi_firmware.bin -op_path ../../../firmware/m2m_aio_2b0.bin
image_builder -1003A0 -ota_img ../../../ota_firmware/m2m_ota_3a0.bin -fw_path ../../../firmware/wifi_v111/ASIC_3A0/wifi_firmware.bin -op_path ../../../firmware/m2m_aio_3a0.bin