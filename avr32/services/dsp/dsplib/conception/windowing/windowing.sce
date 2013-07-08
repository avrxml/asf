PI = 3.1415926535898

// Gaussienne
// w(n) = exp(-1/2*((n-(N-1)/2)/(teta*(N-1)/2))^2)
// avec
//   N = signal width
//   teta = a coefficient < 0.5

N = 100;
teta = 0.2;

n = 0:N-1;
w1 = exp(-1/2*((n-(N-1)/2)/(teta*(N-1)/2))^2);



// Hamming
// w(n) = 0.53836 - 0.46164*cos(2*PI*n/(N-1))
// avec
//   N = signal width

N = 1024;

n = 0:N-1;
w2 = 0.53836 - 0.46164*cos(2*PI*n/(N-1));


// Hann
// w(n) = 0.5*(1-cos(2*PI*n/(N-1)))
// avec
//   N = signal width

N = 1024;

n = 0:N-1;
w3 = 0.5*(1-cos(2*PI*n/(N-1)));


// Bartlett
// w(n) = 2/(N-1)*((N-1)/2-abs(n-(N-1)/2))
// avec
//   N = signal width

N = 1024;

n = 0:N-1;
w4 = 2/(N-1)*((N-1)/2-abs(n-(N-1)/2));

// Triangular
// w(n) = 2/N*(N/2-abs(n-(N-1)/2))
// avec
//   N = signal width

N = 1024;

n = 0:N-1;
w5 = 2/N*(N/2-abs(n-(N-1)/2));



// Bartlett-Hann
// w(n) = a0 - a1*abs(n/(N-1)-1/2)-a2*cos(2*PI*n/(N-1))
// avec
//   N = signal width
//   a0 = 0.62
//   a1 = 0.48
//   a2 = 0.38

N = 1024;

n = 0:N-1;
a0 = 0.62;
a1 = 0.48;
a2 = 0.38;
w6 = a0 - a1*abs(n/(N-1)-1/2)-a2*cos(2*PI*n/(N-1));



// Blackman
// w(n) = a0 - a1*cos(2*PI*n/(N-1)) + a2*cos(4*PI*n/(N-1))
// avec
//   N = signal width
//   a0 = 0.42
//   a1 = 0.5
//   a2 = 0.08

N = 1024;

n = 0:N-1;
a0 = 0.42;
a1 = 0.5;
a2 = 0.08;
w7 = a0 - a1*cos(2*PI*n/(N-1)) + a2*cos(4*PI*n/(N-1));



// Welch
// w(n) = 2/(N-1)*((N-1)/2-abs(n-(N-1)/2))
// avec
//   N = signal width

N = 1024;

n = 0:N-1;
w8 = 1 - ((n - N/2)/(N/2))^2;




