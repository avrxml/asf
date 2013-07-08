#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

my $date =`date +"%D %T"`;
chomp($date);

print "Content-type: text/html; charset=ISO-8859-1\n";
print "Cache-Control: no-cache,no-store\n\n";

if(defined param("actuator")) {
  if(param("actuator") eq "ledb1") {
    print  "ledb1 | $date | 0 , 255";
  }
  elsif(param("actuator") eq "ledb2") {
    print  "ledb2 | $date | 255 , 0";
  }
  elsif(param("actuator") eq "ledm1") {
    print  "ledm1 | $date | on";
  }
  elsif(param("actuator") eq "ledm2") {
    print  "ledm2 | $date | on";
  }
  elsif(param("actuator") eq "lcd") {
    print  "lcd | $date | Hello World !!!!";
  }
  else {
    print  "Don't know how to handle ".param("actuator")."\n";;
  }
}
else {
  print  "missing actuator in command line\n";
}
