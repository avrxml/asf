// Fixed point 16-bit cosine and sine

clear

function res=ln_test(x)
  res = 0;

  // Convert Q format to float
  x = x*2^(-31);

  expo = int(log(x)/log(2));
  x = x*2^(-expo);

  a = (x-1)/(x+1);
  a_sqr = a*a;

  res = a_sqr*(1/9);
  res = a_sqr*(res + 1/7);
  res = a_sqr*(res + 1/5);
  res = a_sqr*(res + 1/3);
  res = a*(res + 1);
  res = res*2;

  res = log(x) + expo*log(2);

endfunction;

function res=DSP_Q(a, b, x)
  res = floor(x*2^b);
endfunction

QA = 1
QB = 31

e_moy = 0;
0.99999998882
for nbr=-0.0001:0.1:1,
  printf("ln(%f)\t%.11f %.11f\n", nbr, log(nbr)/log(10), ln_test(DSP_Q(QA, QB, nbr)));
end;
printf("Error average: %.11f\n", e_moy/length(-1:0.1:1));





