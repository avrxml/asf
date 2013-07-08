// Fixed point 16-bit cosine and sine

clear

function res=dsp32_k_cos(y)

  pi = 3.1415926535897;

  // Q1.31 emulation
  y = y*2^31;

  one =  1.0000000000e+00*2^31;
  half = 0.61685027507*2^31; //0.5*(pi^2/8);
  C1 = 0.50733903092*2^31; //pi^2*(4.1666667908e-02*pi^2)/8;
  C2 = -0.16690785015*2^31; //(-1.3888889225e-03*pi^4)*(pi^2/8);
  C3 = 0.02941632920*2^31; //(2.4801587642e-05*pi^6)*(pi^2/8);
  C4 = -0.00322586085*2^31; //(-2.7557314297e-07*pi^8)*(pi^2/8);
  C5 = 0.00024118485*2^31; //(2.0875723372e-09*pi^10)*(pi^2/8);
  C6 = -0.00001295308*2^31; //(-1.1359647598e-11*pi^12)*(pi^2/8);


	z = y*y/2^31;

  produ = z*C6/2^31;
  suma = C5 + produ;
  produ = z*suma/2^31;
  suma = C4 + produ;
  produ = z*suma/2^31;
  suma = C3 + produ;
  produ = z*suma/2^31;
  suma = C2 + produ;
  produ = z*suma/2^31;
  suma = C1 + produ;
  r = z*suma/2^31;

  res = one - ((half*z - (z*r)))/2^28;
  
  res = res/2^31;

endfunction

function res=dsp32_k_sin(y)

  pi = 3.1415926535897;
  
  // Q1.31 emulation
  y = y*2^31;

  S0 = 0.78539816340*2^31; //pi/4;
  S1  = -0.64596411675*2^31; //-1.6666667163e-01*pi^3/8;
  S2  =  0.31877052162*2^31; //8.3333337680e-03*pi^5/8;
  S3  = -0.07490806720*2^31; //-1.9841270114e-04*pi^7/8;
  S4  =  0.01026823400*2^31; //2.7557314297e-06*pi^9/8;
  S5  = -0.00092125426*2^31; //-2.5050759689e-08*pi^11/8;
  S6  =  0.00005769937*2^31; //1.5896910177e-10*pi^13/8;

	z	=  y*y/2^31;
	v	=  z*y/2^31;

  produ = z*S6/2^31;
  suma = S5 + produ;
  produ = z*suma/2^31;
  suma = S4 + produ;
  produ = z*suma/2^31;
  suma = S3 + produ;
  produ = z*suma/2^31;
  suma = S2 + produ;

  produ = z*suma/2^31;
  suma = S1 + produ;
  produ = v*suma/2^30;

  suma = S0*y/2^31 + produ;

	res = suma/2^29;

endfunction




// [-2*pi 2*pi] => [-1 1]
function res=dsp32_sin(angle)

// convertion from float to Q1.15
  pi = 3.1415926535897;

  x = angle/(pi);
  x = x;

// Input of this function is in binary angular measure format (Q15
// with the full -1 to 1 range representing -pi to pi). */

// Determine the quadrant the input resides in

  temp = (x + 0.25)
  n = abs(int(temp));
  
//  printf("%f\t", temp/2^15);

 // n = temp
  //temp=abs(x) + 0.25*2^15;

//  n = int(abs(int((temp + 0.25*2^15)/2^14)));
// translate input down to +/- pi/4
  x = modulo(x - n * 0.5, 1);
//  printf("%f %f\t", n, x/2^15);

// call the appropriate function given the quadrant of the input

 // printf("%f\t", x);

  if n == 0 then
    res = dsp32_k_sin(x);
  elseif n == 1 then
    res = dsp32_k_cos(x);
  elseif n == 2 then
    res = -dsp32_k_sin(x);
  elseif n == 3 then
    res = -dsp32_k_cos(x);
  else
    res = dsp32_k_sin(x);
  end

//  printf("%f\t", res);
endfunction

function res=dsp32_cos(angle)
  pi = 3.1415926535897;
  res = dsp32_sin(angle + pi/2);
endfunction




// Input angle from -pi/4 to pi/4
function res=dsp16_k_sin(y)

  // series coefficients, tweaked with Chebyshev 
  s1 = 0.785369873046875*2^15; //0x6487;
  s3 = 0.322784423828125*2^15; //0x2951;
  s5 = 0.03875732421875*2^15;  //0x4f6;

  z = (y * y)/2^12;
  produ = (z * s5)/2^16;
  suma = s3 - produ;
  produ = (z * suma)/2^16;
  suma = s1 - produ;

  res=(y * suma)/2^13;
endfunction

function res=dsp16_k_cos(y)
  c0 = 1.*2^15; //0x7fff;
  c2 = 0.61651611328125*2^15; //0x4eea; 
  c4 = 0.1231689453125*2^15; //0x0fc4;

  z = (y * y)/2^12;
  produ = (z * c4)/2^16;
  suma = c2 - produ;
  produ = (z * suma)/2^15;
  res = (c0 - produ);
endfunction

// [-2*pi 2*pi] => [-1 1]

function res=dsp16_sin(angle)

// convertion from float to Q1.15
  pi = 3.1415926535897;

  x = angle/(pi);
  x = x*2^15;

// Input of this function is in binary angular measure format (Q15
// with the full -1 to 1 range representing -pi to pi). */

// Determine the quadrant the input resides in

  temp = (x + 0.25*2^15)
  if temp > 0 then
    n = abs(int(temp/2^14));
  else
    n = 3 - abs(int(temp/2^14));
  end
  
 printf("%f\t", temp/2^15);

 // n = temp
  //temp=abs(x) + 0.25*2^15;

//  n = int(abs(int((temp + 0.25*2^15)/2^14)));
// translate input down to +/- pi/4
  x = modulo(x - n * 0.5*2^15, 2^16);
  printf("%f %f\t", n, x/2^15);

// call the appropriate function given the quadrant of the input
  if n == 0 then
    res = dsp16_k_sin(x)/2^15;
  elseif n == 1 then
    res = dsp16_k_cos(x)/2^15;
  elseif n == 2 then
    res = -dsp16_k_sin(x)/2^15;
  elseif n == 3 then
    res = -dsp16_k_cos(x)/2^15;
  else
    res = dsp16_k_sin(x)/2^15;
  end
endfunction

function res=dsp16_cos(angle)
  pi = 3.1415926535897;
  res = dsp16_sin(angle + pi/2);
endfunction


pi = 3.1415926535897;


err_cos_moy = 0;
err_cos_max = 0;
for i=-pi/2:0.01:pi/2,
  err = abs((dsp32_cos(i) - cos(i)));
  err_cos_moy = err_cos_moy + err;
  if err > err_cos_max then
    err_cos_max = err;
  end
end
err_cos_moy = err_cos_moy/length(-pi:0.1:pi);
printf("Error average for cosinus:\t%.10f\n", err_cos_moy);
printf("Error maximum for cosinus:\t%.10f\n", err_cos_max);

disp('');

err_sin_moy = 0;
err_sin_max = 0;
for i=-pi/2:0.01:pi/2,
  err = abs((dsp32_sin(i) - sin(i)));
  err_sin_moy = err_sin_moy + err;
  if err > err_sin_max then
    err_sin_max = err;
  end
end
err_sin_moy = err_sin_moy/length(-pi:0.1:pi);
printf("Error average for sinus:\t%.10f\n", err_sin_moy);
printf("Error maximum for sinus:\t%.10f\n", err_sin_max);


