clear all;

cd "D:\trunk\SERVICES\DSPLIB\CONCEPTION\AEC"

N = 2000;

temp1=read('temp1.txt', -1, N);
temp1 = temp1(1, :);
temp2=read('temp2.txt', -1, N);
temp2 = temp2(1, :);

plot(temp1);
plot(temp2, 'r');
