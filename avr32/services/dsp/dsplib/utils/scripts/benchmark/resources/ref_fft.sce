y_size = 2^NLOG;
t = 0:(y_size-1);
vect2_input = sin(2*%pi*4000*t/40000);

y = fft(vect2_input)/y_size;
