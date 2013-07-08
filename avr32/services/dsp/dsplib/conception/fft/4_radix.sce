clear;

// For the FFT 4-radix without optimisation

function [y1, y2, y3, y4, nb_mul] = fft_4_radix_e(x, k, size_x);
  global PI;

  mod = modulo(size(x, '*'), 4);

  if mod == 0 then

    x_1 = x(1:4:size(x, '*'));
    x_2 = x(2:4:size(x, '*'));
    x_3 = x(3:4:size(x, '*'));
    x_4 = x(4:4:size(x, '*'));

    [y_1, a, b, c, nb_mul_1] = fft_4_radix_e(x_1, k, size_x/4);
    [y_2, a, b, c, nb_mul_2] = fft_4_radix_e(x_2, k, size_x/4);
    [y_3, a, b, c, nb_mul_3] = fft_4_radix_e(x_3, k, size_x/4);
    [y_4, a, b, c, nb_mul_4] = fft_4_radix_e(x_4, k, size_x/4);

    k_mul = 0:3;
    w = exp(-2*PI*k_mul*(k-1)*%i/size_x);

    y1 = y_1 + w(2)*y_2 + w(3)*y_3 + w(4)*y_4;
    y2 = y_1 - %i*w(2)*y_2 - w(3)*y_3 + %i*w(4)*y_4;
    y3 = y_1 - w(2)*y_2 + w(3)*y_3 - w(4)*y_4;
    y4 = y_1 + %i*w(2)*y_2 - w(3)*y_3 - %i*w(4)*y_4;
    nb_mul = nb_mul_1 + nb_mul_2 + nb_mul_3 + nb_mul_4 + 3;

  else

    nb_mul = 0;
    y1 = x(1);
    y2 = 0;
    y3 = 0;
    y4 = 0;

  end
endfunction

function [y, nb_mul] = fft_4_radix(x);
  nb_mul = 0;
  for k = 1:(size(x, '*')/4),
    [y1, y2, y3, y4, temp] = fft_4_radix_e(x, k, size(x, '*'));
    y(k) = y1;
    y(k+size(x, '*')/4) = y2;
    y(k+size(x, '*')/2) = y3;
    y(k+3*size(x, '*')/4) = y4;
    nb_mul = nb_mul + temp;
  end
endfunction

// For the FFT 4-radix optimised

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

function [y, nb_mul] = fft_4_radix2_dif_rec(x)
  nb_mul = 0;
  N = size(x, '*');
  nlog = log(N)/log(4);

  if nlog > 0 then

    a = x(1:N/4);
    b = x(N/4+1:N/2);
    c = x(N/2+1:3*N/4);
    d = x(3*N/4+1:N);

    k = 0:N/4-1;
    w2 = exp(-2*PI*2*k*%i/N);
    w3 = exp(-2*PI*k*%i/N);
    w4 = exp(-2*PI*3*k*%i/N);

    A = a + b + c + d;
    B = (a - b + c - d).*w2;
    C = (a - %i*b - c + %i*d).*w3;
    D = (a + %i*b - c - %i*d).*w4;

    [y(1:N/4), mul1] = fft_4_radix2_dif_rec(A);
    [y(N/4+1:N/2), mul2] = fft_4_radix2_dif_rec(B);
    [y(N/2+1:3*N/4), mul3] = fft_4_radix2_dif_rec(C);
    [y(3*N/4+1:N), mul4] = fft_4_radix2_dif_rec(D);

    nb_mul = mul1 + mul2 + mul3 + mul4 + 3*N/4;

  else

    y = x(1);
    nb_mul = 0;

  end
endfunction

function [y, nb_mul] = fft_4_radix2_dif(x)

  N = size(x, '*');

  [y_temp, nb_mul] = fft_4_radix2_dif_rec(x);
  
  // Reorganize the table
  swap_tab = make_swap_tab(log2(N), N) + 1;

  for i=1:N,
    y(swap_tab(i)) = y_temp(i);
  end

endfunction

function [y, nb_mul] = fft_4_radix2_dit(x)

  N = size(x, '*');
  nlog = log(N)/log(4);
  nb_mul = 0;

  // Reorganize the table
  swap_tab = make_swap_tab(log2(N), N) + 1;
  for i=1:N,
    y(swap_tab(i)) = x(i);
  end
  y = y';


  for stage=1:nlog,

    m = 4^stage;
    m4 = m/4;

    for j=0:m4-1,

      e = exp(1.0*j*-2*PI*%i/m);
      e2 = exp(2.0*j*-2*PI*%i/m);
      e3 = exp(3.0*j*-2*PI*%i/m);

      for r=0:m:N-1,

        i0 = j + r + 1;
        i1 = i0 + m4;
        i2 = i1 + m4;
        i3 = i2 + m4;

        a0 = y(i0)/4;
        a1 = y(i2)/4;
        a2 = y(i1)/4;
        a3 = y(i3)/4;

        a1 = a1*e;
        a2 = a2*e2;
        a3 = a3*e3;
        
        t0 = (a0+a2) + (a1+a3);
        t2 = (a0+a2) - (a1+a3);

        t1 = (a0-a2) -%i*(a1-a3);
        t3 = (a0-a2) +%i*(a1-a3);

        y(i0) = t0;
        y(i1) = t1;
        y(i2) = t2;
        y(i3) = t3;
        
        disp(y');

        nb_mul = nb_mul + 3;
      end
    end
  end

endfunction


// FFT 4-radix test
N = 64;
PI = 3.141592654;
Fs = 40000;

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

y_test = 0;
// FFT 4-radix implementation
disp("--- FFT 4-radix no optimisation ---");
[y_test, nb_mul] = fft_4_radix(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
subplot(412)
xtitle("FFT 4-radix no optimisation")
plot2d(f, abs(y_test(1:n)));
disp('Number of multiplications:');
disp(nb_mul);

y_test = 0;
// FFT 4-radix DIF implementation
disp("--- FFT 4-radix DIF optimised ---");
[y_test, nb_mul] = fft_4_radix2_dif(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
subplot(413)
xtitle("FFT 4-radix DIF optimised")
plot2d(f, abs(y_test(1:n)));
disp('Number of multiplications:');
disp(nb_mul);

y_test = 0;
// FFT 4-radix DIT implementation
disp("--- FFT 4-radix DIT optimised ---");
[y_test, nb_mul] = fft_4_radix2_dit(x);
f = Fs*(0:(N/2))/N;
n = size(f, '*');
subplot(414)
xtitle("FFT 4-radix DIT optimised")
plot2d(f, abs(y_test(1:n)));
disp('Number of multiplications:');
disp(nb_mul);
