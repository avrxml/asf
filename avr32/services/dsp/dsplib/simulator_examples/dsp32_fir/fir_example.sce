clear

// Number of TAP
FIR_COEF_SIZE = 25;
// Cutoff frequency
fc = 400;
// Sampling frequency
fs = 8000;

// Number of Points 
SIZE = 64;

pi = %pi;

// Signal generation
f = 1:fc;
f = f.*f;
t = 1:length(f/fs);
x = sin(2*pi*f/fs);

// Signal generation
t = 0:(SIZE-1);
F1 = 400;
F2 = 2000;
x1 = sin(2*pi*F1/40000*t);
x2 = cos(2*pi*F2/40000*t);
x = x1.*x2;

// low-pass with rectangular window - order FIR_COEF_SIZE
fir_coef = wfir('lp', FIR_COEF_SIZE, [fc/fs, 0], 're', [0 0]);


printf("--------------------------------------------\n");
printf("-------------FIR Coefficients---------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=1:FIR_COEF_SIZE,
  printf("DSP32_Q(%.11f),\n", fir_coef(i));
end

printf("--------------------------------------------\n");
printf("----------------X Signal -------------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=1:SIZE,
  printf("DSP32_Q(%.11f),\n", x(1, i));
end

[y] = convol(fir_coef, x);

y = y(FIR_COEF_SIZE:length(y));

printf("--------------------------------------------\n");
printf("----------------Y Signal--------------------\n");
printf("--------------------------------------------\n");
printf("--------------------------------------------\n");
for i=1:SIZE-FIR_COEF_SIZE+1,
  printf("%i    %.11f \n",i,y(i));
end








