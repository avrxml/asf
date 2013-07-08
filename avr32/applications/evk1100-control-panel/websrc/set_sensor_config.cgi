#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

print "Content-type: text/html\n";
print "Cache-Control: no-cache,no-store\n\n";

print  "set_sensor_config.cgi?sensor=".param("sensor")."&";
if (defined param("lograte")) {
  print "lograte=".param("lograte")."\n";
}
if (defined param("min")){
  print "min=".param("min")."\n";
}
if (defined param("max")){
  print "max=".param("max")."\n";
}
if (defined param("alarm")){
  print "alarm=".param("alarm")."\n";
}

