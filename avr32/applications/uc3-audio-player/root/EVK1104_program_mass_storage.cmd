avr32program chiperase
avr32program program -finternal@0x80000000,256Kb -cxtal -e -v -R -r uc3a3256-mass_storage_data_flash.elf
pause
