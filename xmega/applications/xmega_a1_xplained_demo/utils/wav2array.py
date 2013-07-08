import wave
import winsound
import struct
import sys
import os
os.chdir(os.path.dirname(os.path.abspath( __file__ )))

# Yields lines narrower than 80 chars
def frmt16(st):
	line = ''
	while st:
                value = (int(struct.unpack("<h", st[0:2])[0]))
                value = (value%2**16) if value>=0 else (value%2**16-2**15)
		s_value = '0x%04X' % (value >> 4)
		st = st[2:]
		if len(line)+len(s_value)+2+4 > 80:
			yield (line + '\n')
			line = ''
		line += s_value + ', '
	yield line

# Yields lines narrower than 80 chars.
def frmt8(st):
	line = ''
	while st:
		value = ('0x%02X' % (int(struct.unpack("B", st[0:1])[0]))  )
		st = st[1:]
		if len(line)+len(value)+2+4 > 80:
			yield (line + '\n')
			line = ''
		line += value + ', '
	yield line


def main():
        u = file("out.txt", "w")
        for v in sys.argv:
                u.write(v + "\n")
        u.close()
        if (len(sys.argv) == 1) or (sys.argv[1] == '-h') or (sys.argv[1] == '--help'):
                print "Usage " + os.path.basename(sys.argv[0]) + " <inwave.wav> [outfile.c]"
                print "Note that existing c files will be overwritten mercilessly.\n"
                sys.exit(1)
                
        if not os.path.isfile(sys.argv[1]):
                print "Could not find file " + sys.argv[1]
                sys.exit(1)
                
        w = wave.open(sys.argv[1], 'r')
        frames = w.readframes(w.getnframes())
        frate  = w.getframerate()
        width  = w.getsampwidth()
        ch     = w.getnchannels()
        comp   = w.getcomptype()
        if comp != "NONE":
                raw_input("Cannot process compressed audio. Press enter to exit")
                sys.exit(1)

        if ch > 1:
                print "This script works only for mono."
                sys.exit(1)
        
        if len(sys.argv) == 3:
                filename = sys.argv[2]
        else:
                filename = sys.argv[1]
        filename_pretty = os.path.splitext(os.path.basename(filename))[0]

        f = file(filename_pretty + '.c', 'w')

        f.write("// %d frames, %d samples/sec, %d bit/sample \n" %(w.getnframes(), frate, width*8))
        f.write("uint"+width*8+"_t " + filename_pretty + "["+str(len(frames)//width)+"] PROGMEM = {\n")
        frmt = frmt8 if width == 1 else frmt16
        out = ""
        for k in frmt(frames):
                out += "\t"+k
        f.write(out[0:-2]+"\n};\n")
        f.close()

main()
