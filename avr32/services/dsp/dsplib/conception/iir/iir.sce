clear
// cutoff frequency
fc1 = 4000;
fc2 = 500;
// Sampling frequency
fs = 8000;
// Signal size
sigsize = 1000;

N = 720;

pi = 3.141592653;

// Signal generation
t = 0:(N-1);
F1 = 400;
F2 = 4000;
x1 = sin(2*pi*F1/fs*t);
x2 = cos(2*pi*F2/fs*t);
x = (x1+x2)/10;
//plot(x);

//[cells,fact,zzeros,zpoles]=eqiir('lp','butt',[2*%pi/10, 4*%pi/10], 1, 1);
//hz = fact*poly(zzeros,'z')/poly(zpoles,'z');
//
//hz

hz = iir(3, 'lp', 'butt', [fc1/fs 0], [0 0]);
coef_num = coeff(hz("num"));
coef_den = coeff(hz("den"));

// IIR Computation

//void iir_arma_arm9e(short *y, short *x, short *h2, short *h1, int ny)
//{
//  int i, j;
//  int sum;
//  for (i=0; i<ny; i++)
//  {
//    sum = h2[0] * x[4+i];
//    for (j = 1; j <= 4; j++)
//      sum += h2[j]*x[4+i-j]-h1[j]*y[4+i-j];
//    y[4+i] = (sum >> 15);
//  }
//}

//void DSP_iir(short *r1, short *x, short *r2, short *h2, short *h1, int nr)
//{
//  int j,i;
//  int sum;
//  for (i=0; i<nr; i++)
//  {
//    sum = h2[0] * x[4+i];
//    for (j = 1; j <= 4; j++)
//      sum += h2[j]*x[4+i?j]?h1[j]*r1[4+i?j];
//    r1[4+i] = (sum >> 15);
//    r2[i] = r1[4+i];
//  }
//}

// y(n) = SUM(m, 0, length(b)) b(m)*x(n-m) - SUM(m, 1, length(y)) a(m)*y(n-m)
// N_a = length(a)
// y[n] = a[0]*x[n] + a[1]*x[n - 1] + a[2]*x[n - 2] + ...
//      + b[1]*y[n - 1] + b[2]*y[n - 2] + b[3]*y[n - 3] + ...


y = zeros(1, length(x));


a = [ 0.00002744109823078591,
        0.00049393976815414629,
        0.00419848802931024370,
        0.02239193615632129700,
        0.08396976058620486700,
        0.23511532964137363000,
        0.50941654755630950000,
        0.87328551009653066000,
        1.20076757638272970000,
        1.33418619598081060000,
        1.20076757638272970000,
        0.87328551009653066000,
        0.50941654755630950000,
        0.23511532964137363000,
        0.08396976058620486700,
        0.02239193615632129700,
        0.00419848802931024370,
        0.00049393976815414629,
        0.00002744109823078591
     ];

a = a;

// a = coef_num./coef_den(1);
N_a = length(a);
b = [ 1.00000000000000000000,
        -0.00000000000000429274,
        2.44467548130587000000,
        -0.00000000000000109814,
        2.32226671586706600000,
        -0.00000000000000144755,
        1.10337789459600220000,
        -0.00000000000000344418,
        0.28174420730891248000,
        -0.00000000000000037749,
        0.03872247855382713500,
        -0.00000000000000002086,
        0.00272007253936749470,
        -0.00000000000000000216,
        0.00008640036584533501,
        -0.00000000000000000004,
        0.00000094513542242120,
        -0.00000000000000000000,
        0.00000000150605912418 ];

b = b;


s = poly(a, "z", "coeff")/poly(b, "z", "coeff");
y_temp = flts(x, s);

// b = coef_den./coef_den(1);
N_b = length(b);


//	for(; n<=(size-num_size); n++)
//	{
//		sum1 = 0;
//		sum2 = 0;
//		for(m=0; m<num_size; m++)
//			sum1 += num[m]*vect2[n+num_size-m-1];
//		for(m=1; m<den_size; m++)
//			sum2 += den[m]*vect1[n-m];
//		vect1[n] = (sum1-sum2) >> (DSP16_QB);
//	}


for n=N_a:N,

  sum1_ = 0;
  for m=0:(N_a-1),
    sum1_ = sum1_ + a(m+1)*x(n - m);
  end;

  sum2_ = 0;
  for m=1:(N_b-1),
   sum2_ = sum2_ + b(m+1)*y(n - m);
  end;

  y(n) = sum1_ - sum2_;

end;

y = y(max(N_a, N_b):N);

[hzm, fr] = frmag(hz, 256);

plot2d(fr',hzm')
xtitle('Discrete IIR filter band pass',' ',' ');

f = 0:0.001:1;
hf = freq(hz("num"), hz("den"), exp(2*%pi*%i*f));


// affichage
xbasc(); xset("font size", 4);
xsetech([0,0,1,.5]); plot2d(f, abs(hf))
xtitle("module du filtre en fonction de la fréquence normalisée")
xsetech([0,.5,1,.5]); plot2d(f, atan(imag(hf), real(hf)));
xtitle("phase du filtre en fonction de la fréquence normalisée")

