// Fixed point 16-bit cosine and sine

clear

function res=pow_test(x, y)
  res = 0;

  a = y*log(x);
  res = exp(a);

endfunction;

e_moy = 0;
for nbr=-1:0.1:1,
  printf("0.0045^%f\t%.11f %.11f\n", nbr, 0.1^nbr, pow_test(0.0045, nbr));
end;
printf("Error average: %.11f\n", e_moy/length(-1:0.1:1));





