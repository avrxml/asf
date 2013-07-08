function y = dsp_down_sample(x,l)
 n = length(x);
 y = x(1:l:(n-1));
