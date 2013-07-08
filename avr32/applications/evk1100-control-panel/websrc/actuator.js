var aActuator = new Array("ledb1","ledb2","ledm1","ledm2","lcd");

window.onload = function () {
	var index;

   	if (zXmlHttp.isSupported()) {
		for (index in aActuator) {	
			var divAdditionalLinks = document.getElementById(aActuator[index]);
			if(divAdditionalLinks) {
				get_actuator(aActuator[index]);
			}
		}			
    	}
	else {
		alert("zXmlHttp not Supported");
	}
}	

function get_actuator(actuator) {
	var oXmlHttp = zXmlHttp.createRequest();
	var commande="get_actuator_value.cgi?actuator="+actuator;
	var red;
	var green;
	oXmlHttp.open("get",commande , true);
    	oXmlHttp.onreadystatechange = function () {        
		if (oXmlHttp.readyState == 4) {				
           		if (oXmlHttp.status == 200) {
				var divAdditionalLinks = document.getElementById(actuator);
				if(divAdditionalLinks) {
					var line=new RegExp("[|]+", "g");
					var value=new RegExp("[,]+", "g");
			    		var aLine=oXmlHttp.responseText.split(line);
						
					if((aLine[0].match(/ledb1/i))||(aLine[0].match(/ledb2/i))) {
						var aValue=aLine[2].split(value);
						if(aValue[0]!=undefined) {
							var subDivAdditionalLinks = document.getElementById(actuator+"_green");
							subDivAdditionalLinks.value=aValue[0];
							green=aValue[0];
						}
						if(aValue[1]!=undefined) {	
							var subDivAdditionalLinks = document.getElementById(actuator+"_red");
							subDivAdditionalLinks.value=aValue[1];
							red=aValue[1];
						}
					}
					else if((aLine[0].match(/ledm1/i))||(aLine[0].match(/ledm2/i))) {
						var aValue=aLine[2].split(value);
						if(aValue[0]!=undefined) {
							var subDivAdditionalLinks = document.getElementById(actuator+"_state");
							if(aValue[0].match(/on/i)) {
								subDivAdditionalLinks.checked=true;
							}
							else if (aValue[0].match(/off/i)) {
								subDivAdditionalLinks.checked=false;	
							}
						}					
					}
					else if(aLine[0].match(/lcd/i)) {	
						var aValue=aLine[2].split(value);
						if(aValue[0]!=undefined) {
							var subDivAdditionalLinks = document.getElementById(actuator+"_usrmsg");
							subDivAdditionalLinks.value=aValue[0];
						}					
					}
					else {
						alert(oXmlHttp.responseText);
					}
				}
	    		}
		}
	}
	oXmlHttp.send(null);	
}

function  check_actuator_config(actuator)
{
	var sMessage="";
	if(actuator==1) { // "ledb1"
	  var divAdditionalLinks = document.getElementById("ledb1");
	  message = "actuator=ledb1&green="+parseInt(divAdditionalLinks.ledb1_green.value)+
	                            "&red="+parseInt(divAdditionalLinks.ledb1_red.value);
	  var time = divAdditionalLinks.ledb1_time.value;
	  if(time.length>0) {
		message=message+"&time="+divAdditionalLinks.ledb1_time.value;
	  }
	  set_actuator_value(message);
	}
	if(actuator==2) { // "ledb2"
	  var divAdditionalLinks = document.getElementById("ledb2");		
	  message = "actuator=ledb2&green="+parseInt(divAdditionalLinks.ledb2_green.value)+
	                            "&red="+parseInt(divAdditionalLinks.ledb2_red.value);
	  var time = divAdditionalLinks.ledb2_time.value;
	  if(time.length>0) {
		message=message+"&time="+divAdditionalLinks.ledb2_time.value;
	  }
	  set_actuator_value(message);
	}
	if(actuator==3) { // "ledm1"
	  var divAdditionalLinks = document.getElementById("ledm1");		
	  message = "actuator=ledm1&state=";
	  if(divAdditionalLinks.ledm1_state.checked==true) {
		  message = message+"on";
	  }
	  else {
		message = message+"off";
	  }
	  var time = divAdditionalLinks.ledm1_time.value;
	  if(time.length>0) {
		message=message+"&time="+divAdditionalLinks.ledm1_time.value;
	  }
	  set_actuator_value(message);
	}
	if(actuator==4) { // "ledm2"
	  var divAdditionalLinks = document.getElementById("ledm2");		
	  message = "actuator=ledm2&state=";
	  if(divAdditionalLinks.ledm2_state.checked==true) {
		  message = message+"on";
	  }
	  else {
		message = message+"off";
	  }
	  var time = divAdditionalLinks.ledm2_time.value;
	  if(time.length>0) {
		message=message+"&time="+divAdditionalLinks.ledm2_time.value;
	  }
	  set_actuator_value(message);
	}
	if(actuator==5) { // "lcd"
	  var divAdditionalLinks = document.getElementById("lcd");
	  message = "actuator=lcd&usrmsg="+"\""+divAdditionalLinks.lcd_usrmsg.value+"\"";		
	  var time = divAdditionalLinks.lcd_time.value;
	  if(time.length>0) {
		 message=message+"&time="+divAdditionalLinks.lcd_time.value;
	  }
	  set_actuator_value(message);
	}	
}

function set_actuator_value(value) {
	var oXmlHttp = zXmlHttp.createRequest();
	var commandline="set_actuator_value.cgi?"+value;
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
