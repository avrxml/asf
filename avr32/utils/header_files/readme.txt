avr32-headers.zip:
------------------
Installation instructions for patched 32-bit AVR device header files.


GCC:
The target directory is the \avr32\include\ subdirectory of your AVR32 tool
chain install directory, e.g., C:\Program Files\Atmel\AVR Tools\AVR32 Toolchain\
avr32\include\.

Optional step: back up the \avr32\ subdirectory, e.g., by renaming or moving it.

Unzip the file avr32-headers.zip, and copy the extracted \avr32\ subdirectory to
the target directory. If the old directory is still in place, Windows will ask
for confirmation whether you want to overwrite the existing files or not.
Select Yes to all.


IAR EWAVR32:
The target directory is the \avr\inc\ subdirectory of your IAR EWAVR32 install
directory, e.g., C:\Program Files\IAR Systems\Embedded Workbench 5.6\avr32\inc\.

Optional step: back up the \avr32\ subdirectory, e.g., by renaming or moving it.

Unzip the file avr32-headers.zip, and copy the extracted \avr32\ subdirectory to
the target directory. If the old directory is still in place, Windows will ask
for confirmation whether you want to overwrite the existing files or not.
Select Yes to all.


Other files:
------------
* uc3d_defines_fix.h: contains missing toolchain definition for the UC3 D series. 
  This file is included in avr32\utils\parts.h for the UC3 D series.  
