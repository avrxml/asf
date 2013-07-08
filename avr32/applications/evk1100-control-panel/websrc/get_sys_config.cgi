#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

my $date =`date +"%D %T"`;
chomp($date);

print "Content-type: text/html; charset=ISO-8859-1\n";
print "Cache-Control: no-cache,no-store\n\n";

my $sensor=$ARGV[0];

if(defined param("sys")) {
  if(param("sys") eq "net") {
    print  "macaddr=hh:hh:hh:hh:hh:hh\n";
    print  "ipaddr=nnn.nnn.nnn.nnn\n";
    print  "submask=nnn.nnn.nnn.nnn\n";
    print  "gwaddr=nnn.nnn.nnn.nnn\n";
  }
  elsif(param("sys") eq "time") {
    print  "remoteipaddr=nnn.nnn.nnn.nnn\n";
    print  "remoteport=ppppp\n";
    print  "retrydelay=rrr\n";
    print  "curtime=mm/dd/yy  hh:mm:ss\n";
  }
  elsif(param("sys") eq "tftp") {
    print  "port=ppppp\n";
  }
  elsif(param("sys") eq "http") {
    print  "port=ppppp\n";
  }
  elsif(param("sys") eq "smtp") {
    print  "remoteipaddr=nnn.nnn.nnn.nnn\n";
    print  "remoteport=ppppp\n";
    print  "retrydelay=rrr\n";
  }
  elsif(param("sys") eq "pop3") {
    print  "remoteipaddr=nnn.nnn.nnn.nnn\n";
    print  "remoteport=ppppp\n";
    print  "retrydelay=rrr\n";
  }
  elsif(param("sys") eq "telnet") {
    print  "port=ppppp\n";
  }
  elsif(param("sys") eq "log") {
    print  "temprate=nn\n";
    print  "potrate=nn\n";
    print  "ligrate=nn\n";
    print  "persmtp=nn\n";
    print  "maxfilelen=nnn\n";
  }
  elsif(param("sys") eq "sup") {
    print  "cpufreq=nn\n";
  }
  
  else {
    print  "Don't know how to handle ".param("sys")."\n";;
  }
}
else {
print  "missing sys in command line\n";
}
