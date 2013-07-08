#!/usr/bin/perl
use CGI qw(:standard :netscape);
use CGI::Carp qw(fatalsToBrowser);

print "Content-type: text/html; charset=ISO-8859-1\n";
print "Cache-Control: no-cache,no-store\n\n";

print  "set_sys_config.cgi?sys=".param("sys")."&";
if (defined param("macaddr")) {
  print "=".param("macaddr")."\n";
}
if (defined param("ipaddr")){
  print "ipaddr=".param("ipaddr")."\n";
}
if (defined param("submask")){
  print "submask=".param("submask")."\n";
}
if (defined param("gwaddr")){
  print "gwaddr=".param("gwaddr")."\n";
}
if (defined param("remoteipaddr")){
  print "remoteipaddr=".param("remoteipaddr")."\n";
}
if (defined param("remoteport")){
  print "remoteport=".param("remoteport")."\n";
}
if (defined param("retrydelay")){
  print "retrydelay=".param("retrydelay")."\n";
}
if (defined param("curtime")){
  print "curtime=".param("curtime")."\n";
}
if (defined param("port")){
  print "port=".param("port")."\n";
}
if (defined param("temprate")){
  print "temprate=".param("temprate")."\n";
}
if (defined param("potrate")){
  print "potrate=".param("potrate")."\n";
  }
if (defined param("ligrate")){
    print "ligrate=".param("ligrate")."\n";
  }
if (defined param("persmtp")){
  print "persmtp=".param("persmtp")."\n";
  }
if (defined param("maxfilelen")){
  print "maxfilelen=".param("maxfilelen")."\n";
}
if (defined param("maxfilelen")){
  print "maxfilelen=".param("maxfilelen")."\n";
}
if (defined param("cpufreq")){
  print "cpufreq=".param("cpufreq")."\n";
}
