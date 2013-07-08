#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

print "Content-type: text/html\n";
print "Cache-Control: no-cache,no-store\n\n";
my $date =`date +"%D %T"`;
chomp($date);

my $sensor=$ARGV[0];
if (defined param("sensor")) {
  if (param("sensor") eq  "temp") {
    my $value = sprintf("%.2f",rand()*100);
    $value = $value-15;
    print  "temp | ".$date." | ".$value."C\n";
  } 
  elsif (param("sensor") eq  "pot") {
    my $value = sprintf("%d",rand()*100);
    print  "pot | ".$date." | ".$value."%\n";
  } 
  elsif (param("sensor") eq  "light") {
    my $value = sprintf("%d",rand()*100);
    print  "light | ".$date." | ".$value."%\n";
  } 
  elsif (param("sensor") eq "pb1") {
    my $value = sprintf("%d",rand()*100);
    my $state = "released";
    if ($value>50) {
      $state = "pressed";
    }
    print  "pb1 | ".$date." | ".$state."\n";
  } 
  elsif (param("sensor") eq "pb2") {
    my $value = sprintf("%d",rand()*100);
    my $state = "released";
    if ($value>50) {
      $state = "pressed";
    }
    print  "pb2 | ".$date." | ".$state."\n";
  } 
  elsif (param("sensor") eq "pb3") {
    my $value = sprintf("%d",rand()*100);
    my $state = "released";
    if ($value>50) {
      $state = "pressed";
    }
    print  "pb3 | ".$date." | ".$state."\n";
  } 
  elsif (param("sensor") eq "js") {
    my $value = sprintf("%d",rand()*100);
    my $state = "js_press";
    $state = "js_left"  if($value>20);
    $state = "js_right" if($value>40);
    $state = "js_up"    if($value>60);
    $state = "js_down"  if($value>80);
    print  "js | ".$date." | ".$state."\n";
  } 
  elsif (param("sensor") eq "all") {
    my $value = sprintf("%.2f",rand()*85);
    $value = $value-15;
    print  "temp | ".$date." | ".$value."C\r\n";
    #########################################
    $value = sprintf("%d",rand()*100);
    print  "pot | ".$date." | ".$value."%\r\n";
    #########################################
    my $value = sprintf("%d",rand()*100);
    print  "light | ".$date." | ".$value."%\r\n";
    #########################################
    $value = sprintf("%d",rand()*100);
    my $state = "released";
    if ($value>50) {
      $state = "pressed";
    }
    print  "pb1 | ".$date." | ".$state."\r\n";
    #########################################
    $value = sprintf("%d",rand()*100);
    $state = "released";
    if ($value>50) {
      $state = "pressed";
    }
    print  "pb2 | ".$date." | ".$state."\r\n";
    #########################################
    $value = sprintf("%d",rand()*100);
    $state = "released";
    if ($value>50) {
      $state = "pressed";
    }
    print  "pb3 | ".$date." | ".$state."\r\n";
    #########################################
    $value = sprintf("%d",rand()*100);
    $state = "js_press";
    $state = "js_left"  if($value>20);
    $state = "js_right" if($value>40);
    $state = "js_up"    if($value>60);
    $state = "js_down"  if($value>80);
    print  "js | ".$date." | ".$state."\r\n";
  } 
  else {
    print  param("all")." | ".$date." | Don't know how to handle this sensor\n";
  }
}
else {
  print  "missing sensor in command line\n";
}
exit(1);
