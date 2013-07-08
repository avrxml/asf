clear
// cutoff frequency
fc1 = 400;
// Sampling frequency
fs = 40000;
// cutoff frequency normalized
fc1n = fc1/fs;
// Signal size
SIZE = 64;

pi = %pi;

// Signal generation
t = 0:(SIZE-1);
F1 = 400;
F2 = 2000;
x1 = sin(2*pi*F1/fs*t);
x2 = sin(2*pi*F2/fs*t);
x = (x1+x2)/10;


printf("--------------------------------------------\n");
printf("----------------X Signal -------------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=1:SIZE,
  printf("DSP16_Q(%.11f),\n", x(1, i));
end



