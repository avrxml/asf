// Fixed point 16-bit cosine and sine

clear

function res=exp_test(x)
  res = x;

  k = floor(res/((log(2)*0.5)));
  x = res - (k*log(2)*0.5);

  res = 1 + x + x*x/2 + x*x*x/6 + x*x*x*x/24 + x*x*x*x*x/120;

  res = res*2^(k/2);

endfunction;

function res=DSP_Q(a, b, x)
  res = floor(x*2^b);
endfunction

QA = 1
QB = 31

e_moy = 0;
0.99999998882
for nbr=-1.00000001117:0.1:1,
  printf("exp(%f)\t%.11f %.11f\n", nbr, exp(nbr), exp_test(nbr));
end;





