clear;


% ---------- Input Signal ---------
Fsignal   = 10000;
Fsin      = 44100;
Fsout     = 48510;
Amplitude = 1;
SIZE      = 200;
t = 0:1/Fsin:(SIZE*(1/Fsin))-(1/Fsin);
t = t(1:length(t)-1);
u = Amplitude*sin(2*pi*Fsignal*t);

w = resampling(u,Fsout,Fsin);


% ----------- Display -----------------
subplot(2,1,1)
plot(u)
title('Original Signal');

subplot(2,1,2)
plot(w)
title('Re-sampled Signal');
