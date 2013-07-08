clear

// Sampling frequency
fs = 8000;
// Number of point
SIZE = 144;

pi = %pi;

// Signal generation
t = 0:(SIZE-1);
F1 = 400;
F2 = 4000;
x1 = sin(2*pi*F1/fs*t);
x2 = cos(2*pi*F2/fs*t);
x = (x1+x2)/100;

// cutoff frequency
fc1 = 2000;
// cutoff frequency normalized
fc1n = fc1/fs;
// Order
order = 18;
// Compute Butterworth coefficient calculation
lisys = iir (order, 'lp' , 'butt', [fc1n 0], [0 0]);


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
for i=1:SIZE,
  printf("DSP32_Q(%.11f),\n", x(1, i));
end

printf("--------------------------------------------\n");
printf("----------------Y Signal--------------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=order:SIZE,
  printf("%i    %.11f \n",i-order,y(i));
end




