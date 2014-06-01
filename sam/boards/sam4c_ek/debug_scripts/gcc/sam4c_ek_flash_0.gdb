#*******************************************************
#
#  Connect to J-Link and debug application in flash.
#

# define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2331

# Reset the chip to get to a known state
monitor reset

# Select flash device
monitor flash device = ATSAM4C16C
# Enable flash download and flash breakpoints
monitor flash download = 1

# Load the program
load

# Reset peripheral  (RSTC_CR)
set *0x400e1400 = 0xA5000004

# Reset coprocessor and its peripheral  (RSTC_CPMR)
set *0x400e140C = 0x5A000000

# Initializing PC and stack pointer
mon reg sp=(0x1000000)
mon reg pc=(0x1000004)
info reg

# end of 'reset' command
end
