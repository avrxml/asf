#*******************************************************
#
#  Connect to J-Link and debug application in flash on SAM4L.
#

# Define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2331

# Reset the chip to get to a known state
monitor reset

# Select flash device
monitor flash device = ATSAM4LC4C

# Enable flash download and flash breakpoints
monitor flash download = 1

# Load the program
load

# Initialize PC and stack pointer
mon reg sp=(0x0)
mon reg pc=(0x4)

info reg

# End of 'reset' command
end
