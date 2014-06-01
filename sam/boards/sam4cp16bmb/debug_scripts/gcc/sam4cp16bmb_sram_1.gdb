#*************************************************
#
#  Connect to J-Link and debug application in sram.
#

# Note:
# Use below command line to start GDB server for core 1 debug:
# > jlinkgdbserver -scriptfile ../SAM4CP.JLinkScript -port 2345 -noreset

# define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2345

# Load the program
# The core 1 image should be loaded by core 0 application already.
info reg

# end of 'reset' command
end
