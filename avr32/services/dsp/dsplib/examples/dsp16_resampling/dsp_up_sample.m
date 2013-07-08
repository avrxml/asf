function y = dsp_up_sample(x,l)
 n = length(x)*l;
 y = zeros(1,n);
 y(1:l:(n-1) )=x;

