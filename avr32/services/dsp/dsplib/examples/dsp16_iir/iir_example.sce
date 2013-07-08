clear
// cutoff frequency
fc1 = 2000;
// Sampling frequency
fs = 48000;
// cutoff frequency normalized
fc1n = fc1/fs;
// Signal size
SIZE = 48;

pi = %pi;

NUM_SIZE = 6;
DEN_SIZE = 5;

// Signal generation
t = 0:(SIZE-1);
F1 = 1000;
F2 = 4000;
x1 = cos(2*pi*F1/fs*t);
x2 = cos(2*pi*F2/fs*t);
x = (x1+x2)/20;

// Initialize the 'order' first values
input_x = [x(6) x(5) x(4) x(3) x(2) x(1:48)];
x = input_x;

// Order
order = 5;
// Compute Butterworth coefficient calculation
lisys = iir (order, 'hp' , 'butt', [fc1n 0], [0 0]);

// Filter x
y = flts(x,lisys);

printf("--------------------------------------------\n");
printf("-------------IIR Coefficients---------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
num = lisys('num')
den = lisys('den')


printf("--------------------------------------------\n");
printf("----------------X Signal -------------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=1:SIZE+NUM_SIZE-1,
  printf("DSP16_Q(%.11f),\n", x(1, i));
end

printf("--------------------------------------------\n");
printf("----------------Y Signal--------------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=DEN_SIZE:SIZE+DEN_SIZE,
  printf("%i    %.11f \n",i-DEN_SIZE,y(i));
end


