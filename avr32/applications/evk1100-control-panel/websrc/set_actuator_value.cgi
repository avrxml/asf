#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

print "Content-type: text/html\n";
print "Cache-Control: no-cache,no-store\n\n";

print  "set_actuator_value.cgi?actuator=".param("actuator")."&";
if (defined param("green")) {
  print "green=".param("green")."\n";
}
if (defined param("red")){
  print "red=".param("red")."\n";
}
if (defined param("time")){
  print "time=".param("time")."\n";
}
if (defined param("state")){
  print "state=".param("state")."\n";
}
if (defined param("usrmsg")){
  print "usrmsg=".param("usrmsg")."\n";
}
