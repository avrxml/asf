#*******************************************************
#
#  Connect to J-Link and debug application in psram on SAM3U.
#

# Define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2331

# Reset the chip to get to a known state
monitor reset

# Perpheral reset RSTC_CR
set *0x400e1200 = 0xA5000004

# Open EBI clock
set *0x400E0410 = 0x00000200

# Configure I/O
set *0x400E0E70 = 0x000001C0
set *0x400E0E04 = 0xFE99FFC0
set *0x400E1070 = 0x00000000
set *0x400E1004 = 0x03F08FFF

# Setup the PSRAM
set *0x400E0070 = 0x01010101
set *0x400E0074 = 0x05050505
set *0x400E0078 = 0x00060006
set *0x400E007C = 0x00000000
set *0x400E0080 = 0x10001003

# Load the program
load

# Initializing PC and stack pointer
mon reg sp=(0x60000000)
#set *0x60000004 = *0x60000004 & 0xFFFFFFFE
mon reg pc=(0x60000004)

info reg

# End of 'reset' command
end
