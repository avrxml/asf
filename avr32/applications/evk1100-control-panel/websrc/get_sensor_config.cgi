#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

print "Content-type: text/html; charset=ISO-8859-1\n";
print "Cache-Control: no-cache,no-store\n\n";

my $date =`date +"%D %T"`;
chomp($date);

my $sensor=$ARGV[0];

if(defined param("sensor")) {
  if(param("sensor") eq "temp") {
    print  "lograte=5\n";
    print  "min=-15\n";
    print  "max=85\n";
    print  "alarm=35\n";
  }
  elsif(param("sensor") eq "pot") {
    print  "lograte=5\n";
    print  "min=0\n";
    print  "max=100\n";
    print  "alarm=95\n";
  }
  elsif(param("sensor") eq "light") {
    print  "lograte=5\n";
    print  "min=5\n";
    print  "max=6\n";
    print  "alarm=5.5\n";
  }
  elsif(param("sensor") eq "pb1") {
    print  "alarm=1\n";
  }
  elsif(param("sensor") eq "pb2") {
    print  "alarm=2\n";
  }
  elsif(param("sensor") eq "pb3") {
    print  "alarm=3\n";
  }
  elsif(param("sensor") eq "js") {
    print  "alarm=50\n";
  }
  else {
    print  "Don't know how to handle ".param("sensor")."\n";;
  }
}
else {
print  "missing sensor in command line\n";
}
