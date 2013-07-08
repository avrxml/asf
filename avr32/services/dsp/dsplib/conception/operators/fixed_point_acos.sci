// Fixed point 16-bit cosine and sine

clear

// asin(x)= x +x^3/3*(1/2) +x^5/5*(1*3)/(2*4) +x^7/7*(1*3*5)/(2*4*6)
// acos(x)= pi/2 -x -x^3/3*(1/2) -x^5/5*(1*3)/(2*4) -x^7/7*(1*3*5)/(2*4*6)

function res=asin_test(x)
  res = 0;

  if abs(x) <= 0.5 then,
    // CST = %pi/2;
    A = 0.63661977236758134307553505349036;  //1/CST
    B = 0.10610329539459689051258917558125;  //(1/2)/3/CST;
    C = 0.047746482927568600730665129011754; //(1*3)/(2*4)/5/CST;
    D = 0.028420525552124167101586386315975; //(1*3*5)/(2*4*6)/7/CST;
    E = 0.019341746556306724833024068465127; //(1*3*5*7)/(2*4*6*8)/9/CST;

    x_sqr = x*x;
    res = x_sqr*(res + E);
    res = x_sqr*(res + D);
    res = x_sqr*(res + C);
    res = x_sqr*(res + B);
    res = x*(res + A)/2;
  else
    res = sqrt((1-x)/2);
    res = 0.5 - 2*asin_test(res)
  end;

endfunction;

function res=acos_test(x)

  res = 0.5 - asin_test(x);

endfunction;

e_moy = 0;
for nbr=-1:0.1:1,
  e_moy = e_moy + abs(acos(nbr)/(%pi) - acos_test(nbr));
  printf("%f\t%.11f %.11f\n", nbr, acos(nbr)/(%pi), acos_test(nbr));
end;
printf("Error average: %.11f\n", e_moy/length(-1:0.1:1));





