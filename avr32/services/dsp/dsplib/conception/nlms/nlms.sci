clear all

i=0:1:16000;

i=rand(1,16001)*100-50;

x=sin(2*%pi*i/8);



y=rand(1,16001);

z=x+y;

// figure(1):plot(z);

n=1000;

length_=15001;

w=zeros(1000,1);

e=zeros(1,length_);

u=0.1;

r=0.0;



for i=1:length_
    e(i)=x(i)-z(i:i+999)*w;

    w=w+u*e(i)*z(i:i+999)'/(r+z(i:i+999)*z(i:i+999)');

    o(i)=z(i:i+999)*w;
end

figure(1):plot((1:length_),e)

// figure(2):plot((1:length_),10*log10(abs(e)));

// figure(3):plot((1:20001),x);

// figure(4):plot(o);
