clear;

// For the FFT 2-radix without optimisation

function [y1, y2, nb_mul] = fft_2_radix_e(x, k, size_x);
  global PI;

  mod = modulo(size(x, '*'), 2);
  if mod == 0 then

    x_even = x(1:2:size(x, '*'));
    x_odd = x(2:2:size(x, '*'));
    [y_even, a, nb_mul_e] = fft_2_radix_e(x_even, k, size_x/2);
    [y_odd, a, nb_mul_o] = fft_2_radix_e(x_odd, k, size_x/2);

    w = exp(-2*PI*(k-1)*%i/size_x);

    y1 = y_even + w*y_odd;
    y2 = y_even - w*y_odd;
    nb_mul = nb_mul_e + nb_mul_o + 1;
  else
    nb_mul = 0;
    y1 = x(1);
    y2 = 0;

  end
endfunction

function [y, nb_mul] = fft_2_radix(x);
  nb_mul = 0;
  for k = 1:(size(x, '*')/2),
    [y(k), y(k+size(x, '*')/2), temp] = fft_2_radix_e(x, k, size(x, '*'));
    nb_mul = nb_mul + temp;
  end
endfunction

// For the FFT 2-radix optimised

function a = make_swap_tab(nlog, size_a)
  nlog = nlog - 1;
  if nlog > 0 then
    a = make_swap_tab(nlog, size_a);
  else
    nlog = 0;
    a = zeros(size_a, 1);
  end
  for i=1:size_a,
    a(i) = a(i)*2;
  end
  for i=0:2^(nlog+1):size_a-1,
    for j=1:2^nlog,
      a(i+j+2^nlog) = a(i+j+2^nlog) + 1;
    end
  end
endfunction

// 2-radix DIT FFT algorithm
function [y, nb_mul] = fft_2_radix2(x)
  nb_mul = 0;
  nlog = log2(size(x, '*'));
  N = size(x, '*');
  swap_tab = make_swap_tab(nlog, size(x, '*')) + 1;

  // Reorganize the table
  for i=1:N,
    y(swap_tab(i)) = x(i);
  end

  for stage=1:nlog,
    nb_butterflies = 2^(stage-1);

    // Recursivity: (N/2)/stage
    rec = (N/2)/nb_butterflies;
    for i_rec=1:rec,

      for i=1:nb_butterflies,
        // W(k, N)
        k = (i-1)*rec;
        w = exp(-2*PI*k*%i/N);

        printf("%i  pos:%i               %i*%i (x*k)       N=%i\n", stage, (i_rec-1)*nb_butterflies*2+i-1, i-1, rec, N);

        nb_mul = nb_mul+1;
        i_cur_butterfly_a = (i_rec-1)*nb_butterflies*2+i;
        i_cur_butterfly_b = i_cur_butterfly_a+nb_butterflies;

        // Butterfly
        i_a = abs(i_cur_butterfly_a);
        i_b = abs(i_cur_butterfly_b);

        A = y(i_a) + w*y(i_b);
        B = y(i_a) - w*y(i_b);

        y(i_a) = A;
        y(i_b) = B;

      end
    end
  end

endfunction

function [y, nb_mul] = fft_2_radix2_dif_rec(x)
  nb_mul = 0;
  N = size(x, '*');
  nlog = log(N)/log(2);
  
  if nlog > 0 then

    a = x(1:N/2);
    b = x(N/2+1:N);
    
    k = 0:N/2-1;
    w = exp(-2*PI*k*%i/N);
    
    A = a + b;
    B = (a - b).*w;

    [y(1:N/2), mul1] = fft_2_radix2_dif_rec(A);
    [y(N/2+1:N), mul2] = fft_2_radix2_dif_rec(B);

    nb_mul = mul1 + mul2 + N/2;

  else
    y = x(1);
    nb_mul = 0;
  end
endfunction

function [y, nb_mul] = fft_2_radix2_dif(x)

  N = size(x, '*');

  [y_temp, nb_mul] = fft_2_radix2_dif_rec(x);
  
  // Reorganize the table
  swap_tab = make_swap_tab(log2(N), N) + 1;

  for i=1:N,
    y(swap_tab(i)) = y_temp(i);
  end

endfunction

// FFT 2-radix test
N = 8;
PI = 3.141592654;
Fs = 40000;

disp('FFT N-points 2-radix');
disp('');
printf('\tN = %i\n\n", N);

// Base signal
t = 0:(N-1);
F1 = 400;
F2 = 2000;
x1 = sin(2*PI*F1/Fs*t);
x2 = cos(2*PI*F2/Fs*t);
x = x1.*x2;
plot(x);

// Desired result
y = fft(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
clf()
subplot(411)
xtitle("Scilab FFT")
plot2d(f, abs(y(1:n)));

// FFT 2-radix implementation
disp("--- FFT 2-radix no optimised ---");
[y_test, nb_mul] = fft_2_radix(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
subplot(412)
xtitle("FFT 2-radix no optimised")
plot2d(f, abs(y_test(1:n)));
disp('Number of multiplications:');
disp(nb_mul);

// FFT 2-radix implementation
disp("--- FFT 2-radix DIT optimised ---");
[y_test, nb_mul] = fft_2_radix2(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
subplot(413)
xtitle("FFT 2-radix DIT optimised")
plot2d(f, abs(y_test(1:n)));
disp('Number of multiplications:');
disp(nb_mul);

// FFT 2-radix DIF implementation
disp("--- FFT 2-radix DIF optimised ---");
[y_test, nb_mul] = fft_2_radix2_dif(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
subplot(414)
xtitle("FFT 2-radix DIF optimised")
plot2d(f, abs(y_test(1:n)));
disp('Number of multiplications:');
disp(nb_mul);

