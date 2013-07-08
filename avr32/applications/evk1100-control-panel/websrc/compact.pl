#!/usr/bin/perl
my @file_list=("actuator.htm",
	       "config.htm",
	       "index.htm",
	       "sensorc.htm",
	       "sensor.htm",
	       "sensorg.htm",
	       "help.htm",
	       );
foreach $file (@file_list) {
  my $output_file = "../DATAFLASH_CONTENT/WEB/".$file;
  print "++>Processing ".$file."\n";
  open ( INPUT, "<$file");
  @html=<INPUT>;
  close INPUT;
  open ( OUTPUT, ">$output_file");
  foreach my $line (@html) {
    #case Find CSS
    if($line =~ /sensor.css/i) {
      print "Find sensor.css\n";
      printf OUTPUT "<style  TYPE=\"text/css\">\n";
      printf OUTPUT "<!--\n";
      open ( CSS, "sensor.css");
      @css=<CSS>;
      close CSS;
      foreach my $css_line (@css) {
        $css_line =~ s/[\t]+/ /g;
	$css_line =~ s/[ ]+/ /g;
	printf OUTPUT "".$css_line;
      }
      printf OUTPUT "-->\n";
      printf OUTPUT "</style>\n";
    }
    elsif($line =~ /zxml.js/i) {      
      print "Find zxml.js\n";
      printf OUTPUT "<script language=\"javascript\">\n";
      printf OUTPUT "<!--\n";
      open ( ZXML, "zxml.js");
      @zxml=<ZXML>;
      close ZXML;
      foreach my $zxml_line (@zxml) {
	$zxml_line =~ s/[\t]+/ /g;
	$zxml_line =~ s/[ ]+/ /g;
    	printf OUTPUT "".$zxml_line;
      }
      printf OUTPUT "-->\n";
      printf OUTPUT "</script>\n";
    }
   elsif($line =~ /sensor.js/i) {
     print "Find sensor.js\n";
     printf OUTPUT "<script language=\"javascript\">\n";
     printf OUTPUT "<!--\n";
     open ( SENSOR, "sensor.js");
     @sensor=<SENSOR>;
     close SENSOR;
     foreach my $sensor_line (@sensor) {
       $sensor_line =~ s/[\t]+/ /g;
       $sensor_line =~ s/[ ]+/ /g;
       printf OUTPUT "".$sensor_line;
     }
     printf OUTPUT "-->\n";
     printf OUTPUT "</script>\n";
   }
   elsif($line =~ /sensor_config.js/i) {
     print "Find sensor_config.js\n";
     printf OUTPUT "<script language=\"javascript\">\n";
     printf OUTPUT "<!--\n";
     open ( SENSOR_CONF, "sensor_config.js");
     @sensor_conf=<SENSOR_CONF>;
     close SENSOR_CONF;
     foreach my $sensor_conf_line (@sensor_conf) {
       $sensor_conf_line =~ s/[\t]+/ /g;
       $sensor_conf_line =~ s/[ ]+/ /g;
       printf OUTPUT "".$sensor_conf_line;
     }
     printf OUTPUT "-->\n";
     printf OUTPUT "</script>\n";
   }
   elsif($line =~ /actuator.js/i) {
     print "Find actuator.js\n";
     printf OUTPUT "<script language=\"javascript\">\n";
     printf OUTPUT "<!--\n";
     open ( ACTUATOR, "actuator.js");
     @actuator=<ACTUATOR>;
     close ACTUATOR;
     foreach my $actuator_line (@actuator) {
       $actuator_line =~ s/[\t]+/ /g;
       $actuator_line =~ s/[ ]+/ /g;
       printf OUTPUT "".$actuator_line;
     }
     printf OUTPUT "-->\n";
     printf OUTPUT "</script>\n";
   }
   elsif($line =~ /system.js/i) {
     print "Find system.js\n";
     printf OUTPUT "<script language=\"javascript\">\n";
     printf OUTPUT "<!--\n";
     open ( SYSTEM, "system.js");
     @system=<SYSTEM>;
     close SYSTEM;
     foreach my $system_line (@system) {
       $system_line =~ s/[\t]+/ /g;
       $system_line =~ s/[ ]+/ /g;
       printf OUTPUT "".$system_line;
     }
     printf OUTPUT "-->\n";
     printf OUTPUT "</script>\n";
   }
   elsif($line =~ /fade.js/i) {
     print "Find fade.js\n";
     printf OUTPUT "<script language=\"javascript\">\n";
     printf OUTPUT "<!--\n";
     open ( FADE, "fade.js");
     @fade=<FADE>;
     close FADE;
     foreach my $fade_line (@fade) {
       $fade_line =~ s/[\t]+/ /g;
       $fade_line =~ s/[ ]+/ /g;
       printf OUTPUT "".$fade_line;
     }
     printf OUTPUT "-->\n";
     printf OUTPUT "</script>\n";
   }
   else {
      $line =~ s/[\t]+/ /g;
      $line =~ s/[ ]+/ /g;
      printf OUTPUT  $line;
    }
  }
  close(OUTPUT);
}
