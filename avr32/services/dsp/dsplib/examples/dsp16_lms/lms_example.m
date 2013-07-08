clear all;

% ---------- Generate Input Signal ----------------
% Input signal size of 64
SIZE = 64;
% Sampling frequency
fs = 40000;
% Cutoff frequency
fc = 400;
% Signal generation
t = 0:(SIZE-1);
F1 = 400;
F2 = 2000;
x1 = sin(2*pi*F1/fs*t);
x2 = sin(2*pi*F2/fs*t);
x = (x1+x2)/10;


% --------- Design the Unknown filter --------------
% We will try to identify the fir_coef value through
% the LMS filter
% Number of TAP
FIR_COEF_SIZE = 11;
Wn = (fc/fs);
% Low Pass Filter
fir_coef = FIR1(FIR_COEF_SIZE,Wn,'low');
% Filter x with the Low Pass Filter 
d = filter(fir_coef,1,x);


% ---------Compute LMS filter---------------
% ---------Matlab Implementation------------
% µ Value
mu = 0.5;
% Identification of a 12 coefficients filter
ha = adaptfilt.lms(12,mu);
% ha/x = d/y
[y,e] = filter(ha,x,d);
figure(1);
stem([fir_coef.' ha.coefficients.'])


% ---------Compute LMS filter---------------
% ---------Generic Implementation-----------
sortie = 0;
Beta = 0.5;
h(1:FIR_COEF_SIZE+1) = 0;

int_x(1:FIR_COEF_SIZE+1)=0;

for j=1:SIZE

    int_x(1) = x(j);
    
    % Ouput Compute
    sortie = 0;
    for i=1:FIR_COEF_SIZE+1
        sortie = sortie + h(i)*int_x(i);
    end
    
    % Error Update
    int_d = d(j);
    error  = int_d - sortie;
    
    % Update LMS Coefficients
    for i=1:FIR_COEF_SIZE+1
        h(i) = h(i)+ Beta*int_x(i)*error;
    end
    
    % Shift circular buffer
     for i=FIR_COEF_SIZE+1:-1:2
        int_x(i) = int_x(i-1)
    end
end

figure(2);
stem([fir_coef.' h.'])

