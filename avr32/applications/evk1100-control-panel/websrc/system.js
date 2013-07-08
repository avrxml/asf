var aSysConfig = new Array("net","time","tftp","http","smtp","pop3","telnet","log");


window.onload = function () {
	var index;

   	if (zXmlHttp.isSupported()) {
		for (index in aSysConfig) {	
			var divAdditionalLinks = document.getElementById(aSysConfig[index]);
			if(divAdditionalLinks) {
				get_sys_config(aSysConfig[index]);
			}
		}			
    	}
	else {
		alert("zXmlHttp not Supported");
	}
}	


function check_sys_config(sys)
{
	if(sys==1) { // "net"
	  var divAdditionalLinks = document.getElementById("net");		
	  set_sys_config("net_macaddr",divAdditionalLinks.net_macaddr.value);
	  set_sys_config("net_ipaddr" ,divAdditionalLinks.net_ipaddr.value);
	  set_sys_config("net_submask",divAdditionalLinks.net_submask.value);
	  set_sys_config("net_gwaddr" ,divAdditionalLinks.net_gwaddr.value);
	}
	if(sys==2) { // "time"
	  var divAdditionalLinks = document.getElementById("time");		
	  set_sys_config("time_remoteipaddr",divAdditionalLinks.time_remoteipaddr.value);
	  set_sys_config("time_remoteport",divAdditionalLinks.time_remoteport.value);
	  set_sys_config("time_retrydelay",divAdditionalLinks.time_retrydelay.value);
	  set_sys_config("time_curtime",divAdditionalLinks.time_curtime.value);
	}
	if(sys==3) { // "tftp"
	  var divAdditionalLinks = document.getElementById("tftp");		
	  set_sys_config("tftp_port",divAdditionalLinks.tftp_port.value);
	}
	if(sys==4) { // "http"
	  var divAdditionalLinks = document.getElementById("http");		
	  set_sys_config("http_port",divAdditionalLinks.http_port.value);
	}
	if(sys==5) { // "smtp"
	  var divAdditionalLinks = document.getElementById("smtp");		
	  set_sys_config("smtp_mailto",divAdditionalLinks.smtp_mailto.value);
	  set_sys_config("smtp_mailfrom",divAdditionalLinks.smtp_mailfrom.value);
	  set_sys_config("smtp_port",divAdditionalLinks.smtp_port.value);
	  set_sys_config("smtp_server",divAdditionalLinks.smtp_server.value);
	}
	if(sys==6) { // "pop3"
	  var divAdditionalLinks = document.getElementById("pop3");		
	  set_sys_config("pop3_remoteipaddr",divAdditionalLinks.pop3_remoteipaddr.value);
	  set_sys_config("pop3_remoteport",divAdditionalLinks.pop3_remoteport.value);
	  set_sys_config("pop3_retrydelay",divAdditionalLinks.pop3_retrydelay.value);
	}
	if(sys==7) { // "telnet"
	  var divAdditionalLinks = document.getElementById("telnet");		
	  set_sys_config("telnet_port",divAdditionalLinks.telnet_port.value);
	}
	if(sys==8) { // "log"
	  var divAdditionalLinks = document.getElementById("log");		
	  set_sys_config("log_persmtp",divAdditionalLinks.log_persmtp.value);
	  set_sys_config("log_maxfilelen",divAdditionalLinks.log_maxfilelen.value);
	}
}

function set_sys_config(field,value) {
	var oXmlHttp = zXmlHttp.createRequest();
	var reg=new RegExp("[_]", "");
	var sendValue=field.split(reg);
	
	if(sendValue[1] == "curtime") {
	  var commandline="set_sys_config.cgi?sys="+sendValue[0]+"&"+sendValue[1]+"="+"\""+value+"\"";
	}
	else {
	  var commandline="set_sys_config.cgi?sys="+sendValue[0]+"&"+sendValue[1]+"="+value;
  }
	oXmlHttp.open("get",commandline , true);
    	oXmlHttp.onreadystatechange = function () {        
		if (oXmlHttp.readyState == 4) {				
           		if (oXmlHttp.status == 200) {
				if(oXmlHttp.responseText.length>1) {
					alert(oXmlHttp.responseText);
				}
	    		}
		}
	}
	oXmlHttp.send(null);
}


function get_sys_config(sys) {
	var oXmlHttp = zXmlHttp.createRequest();
	oXmlHttp.open("get","get_sys_config.cgi?sys="+sys , true);
    	oXmlHttp.onreadystatechange = function () {        
		if (oXmlHttp.readyState == 4) {				
           		if (oXmlHttp.status == 200) {
				var divAdditionalLinks = document.getElementById(sys);
				if(divAdditionalLinks) {	
					var line=new RegExp("[\n]+", "g");
					var colum=new RegExp("[=]+", "g");
			    		var aLine=oXmlHttp.responseText.split(line);
					for (var i=0; i<aLine.length; i++) {
						var aColum=aLine[i].split(colum);
						var subDivAdditionalLinks = document.getElementById(sys+"_"+aColum[0]);	
						if(subDivAdditionalLinks != undefined) {
							subDivAdditionalLinks.value=aColum[1];
						}
					}
				}
	    		}
		}
	}
	oXmlHttp.send(null);	
}

