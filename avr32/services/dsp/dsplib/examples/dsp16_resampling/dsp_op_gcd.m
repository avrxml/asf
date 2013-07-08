function y=dsp_op_gcd(m,n)
  tmp = m;
  while ( ( mod(m,tmp) ~= 0 ) || ( mod(n,tmp) ~= 0 ) )
     tmp = tmp - 1 ;
  end;
  y = tmp;