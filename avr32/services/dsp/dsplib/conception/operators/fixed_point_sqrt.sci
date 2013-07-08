// Fixed point 16-bit cosine and sine

clear

// Emulate the clz instruction for avr32uc3
function res=clz(x)

  res = 0;
  
  while x != 0,
    x = floor(x/2);
    res = res + 1;
  end;
  
  res = 32 - res;

endfunction

// Reciproot Iteration
function res=fp_sqrt(a, b, y)

  if (y < 0) then,
    res = -1
    return
  end;

  // Find an approximation of 1/sqrt(x);
  // Value between 0 && 16
  under_bit_val = floor((32-clz(y))/2);

	x = 2^(-under_bit_val);

  //printf("Approx: %f (%f^2 -> %f)\t", sqrt(1/(y+0.00000001)), 1/(x+0.00000001), x*x);

	z = y/2;

	x = x*(1.5 - z*x*x);
	x = x*(1.5 - z*x*x);
	x = x*(1.5 - z*x*x);
	x = x*(1.5 - z*x*x);

  error_corr = 1;
  if (modulo(b, 2) == 1) then,
    error_corr = 1/sqrt(2);
  end;

	res = (x*y)*2^(-floor(b/2))*error_corr;

endfunction

// Reciproot Iteration
function res=fix_sqrt(a, b, y)

  if (y < 0) then,
    res = -1
    return
  end;

  // Find an approximation of 1/sqrt(x);
  // Value between 0 && 16
  //under_bit_val = b-floor((32-clz(y))/2);
  under_bit_val = (b-16)+floor(clz(y)/2);

	// fixed point version of x
	x = 2^(under_bit_val);

  a = x*x;
  a = floor(y*a/(2^(b+1)));
	x = floor(x*(1.5*2^b - a)/2^b);

  printf("[%i]\n", x);

  error_corr = 1;
  if (modulo(b, 2) == 1) then,
    error_corr = 1/sqrt(2);
  end;

  res = y*error_corr;

  res = floor((x*res)/2^b);
  res = res*2^(-floor(b/2));

endfunction

function res=DSP_Q(a, b, x)
  res = floor(x*2^b);
endfunction

QA = 1;
QB = 15;

e = 0;
for nbr=0:0.1:1,
  e = e + abs(sqrt(nbr) - fp_sqrt(QA, QB, DSP_Q(QA, QB, nbr)));
  printf("%f\t%.11f %.11f *%.11f\n", nbr, sqrt(nbr), fp_sqrt(QA, QB, DSP_Q(QA, QB, nbr)), fix_sqrt(QA, QB, DSP_Q(QA, QB, nbr)));
end;


fix_sqrt(QA, QB, DSP_Q(QA, QB, 0.636))

