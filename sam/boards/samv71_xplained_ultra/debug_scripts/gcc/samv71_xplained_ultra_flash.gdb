#*************************************************
#
#  Connect to J-Link and debug application in flash.
#

# define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2331

# Reset the chip to get to a known state
monitor reset

# Load the program
# Select flash device (Should be AT91SAMV71, but it is not available now)
monitor flash device = AT91SAMV71
# Enable flash download and flash breakpoints
monitor flash download = 1

# Reset peripheral  (RSTC_CR)
set *0x400E1800 = 0xA5000004

# Initializing PC and stack pointer
mon reg sp = (0x400000)
set *0x400004 = *0x400004 & 0xFFFFFFFE
mon reg pc=(0x400004)
info reg

# end of 'reset' command
end
