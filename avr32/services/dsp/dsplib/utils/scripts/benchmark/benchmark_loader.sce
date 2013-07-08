exec('./benchmark_init.sce');disp('exec done');


[fd, err] = mopen(FILE_REAL_COEF, 'r');
n = 1;
j = 1;
y_result = 0;
while n <> -1,
  [n, a] = mfscanf(fd, "%f\n");
  if n == 1 then
	y_result(j) = a;
	j = j + 1;
  end;
end;
mclose(fd);

[fd, err] = mopen(FILE_IMAG_COEF, 'r');
if err == 0 then
	n = 1;
	j = 1;
	while n <> -1,
	  [n, a] = mfscanf(fd, "%f\n");
	  if n == 1 then
		y_result(j) = y_result(j) + a*%i;
		j = j + 1;
	  end;
	end;
	mclose(fd);
	is_imag = 1;
else
	is_imag = 0;
end;



// reference signal: y
exec(SCRIPT_ERR);disp('exec done');




err = 0;
err_max = 0;

SMALLEST_VAL = 1/(2^QB);
RESOLUTION = int(abs(log10(SMALLEST_VAL))+2);



for i=1:length(y),

  a = real(y(i));
  b = real(y_result(i));
  c = abs(a-b);

  if err_max < c then
    err_max = c;
  end;

  err = err + c;

  if is_imag == 1 then

	a = imag(y(i));
	b = imag(y_result(i));
	c = abs(a-b);

	if err_max < c then
		err_max = c;
	end;

	err = err + c;

  end;

end;

if is_imag == 1 then
	err = err/(length(y)*2);
else
	err = err/(length(y));
end;


printf("\n\nError amplitude:\n");
str_format = sprintf("Average: %%.%df", RESOLUTION);
printf(str_format, err);
printf("\n");
str_format = sprintf("Maximum: %%.%df", RESOLUTION);
printf(str_format, err_max);
printf("\n");




exit
