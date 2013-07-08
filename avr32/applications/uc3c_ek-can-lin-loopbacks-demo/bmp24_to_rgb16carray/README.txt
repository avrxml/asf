imageconv converts a 24bits bitmap(input) to an unsigned short int C array(output).

1) Open imageconv.c:
Set WIDTH, HEIGHT, FILENAME_IN, FILENAME_OUT.
WIDTH and HEIGHT should be set to the width and height of the 24bit bitmap input
file (name specified in FILENAME_IN). FILENAME_OUT content will be that of a C
header file, defining a static const unsigned short int avr32_logo[] array.

2) Compile
gcc imageconv.c -o imageconv.exe

3) Execute
./imageconv.exe

By default, WIDTH==320, HEIGHT==240, FILENAME_IN==background_image.bmp and FILENAME_OUT==background_image.h.
