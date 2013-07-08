NTAP = 25;
// cutoff frequency
fc = 400;
// Sampling frequency
fs = 8000;
// Signal size
sigsize = 1000;

N = 64;

pi = 3.141592653;

// Signal generation
f = 1:fc;
f = f.*f;
t = 1:length(f/fs);
x = sin(2*pi*f/fs);

// Signal generation
t = 0:(N-1);
F1 = 400;
F2 = 2000;
x1 = sin(2*pi*F1/40000*t);
x2 = cos(2*pi*F2/40000*t);
x = x1.*x2;
//plot(x);

// low-pass with rectangular window - order NTAP
fir_coef = wfir('lp', NTAP, [fc/fs, 0], 're', [0 0]);

for i=1:N,
  printf("DSP32_Q(%.11f),\n", x(1, i));
end

[y] = convol(fir_coef, x);


yo = y;
length(y)
length(x)

y = y(NTAP:length(y));

length(y)

t = 0:(length(x)-1);
t = t/fs;

subplot(3, 1, 1);
plot(t, x);
xtitle('Original signal (Time domaine)')

subplot(3, 1, 2);
plot(t, y);
xtitle('Resulting signal (Time domaine)')

// plot(abs(fft(x)));
h1 = poly(fir_coef, 'z', 'c');
f = (0:1/fs:1);
hf = freq(h1, 1, exp(2*%pi*%i*f));

subplot(3, 1, 3);
plot(f*fs, abs(hf));
xtitle("Module du filtre en fonction de la fréquence normalisée")





