var aSensorList = new Array("temp","pot","light","pb1","pb2","pb3","js");

window.onload = function () {
	var index;

   	if (zXmlHttp.isSupported()) {
		for (index in aSensorList) {	
			var divAdditionalLinks = document.getElementById(aSensorList[index]+"_conf");
			if(divAdditionalLinks) {
				get_sensor_config(aSensorList[index]);
			}
		}		
    	}
	else {
		alert("zXmlHttp not Supported");
	}
}	

function check_sensor_config(sensor)
{
	if(sensor==1) { // "temp"
	  var divAdditionalLinks = document.getElementById("temp_conf");		
	  set_sensor_config("temp_lograte" ,divAdditionalLinks.temp_lograte.value);
	  set_sensor_config("temp_min" ,divAdditionalLinks.temp_min.value);
	  set_sensor_config("temp_max" ,divAdditionalLinks.temp_max.value);
	  set_sensor_config("temp_alarm" ,divAdditionalLinks.temp_alarm.value);
	}	
	if(sensor==2) { // "pot"
	  var divAdditionalLinks = document.getElementById("pot_conf");		
	  set_sensor_config("pot_lograte" ,divAdditionalLinks.pot_lograte.value);
	  set_sensor_config("pot_min" ,divAdditionalLinks.pot_min.value);
	  set_sensor_config("pot_max" ,divAdditionalLinks.pot_max.value);
	  set_sensor_config("pot_alarm" ,divAdditionalLinks.pot_alarm.value);
	}
	if(sensor==3) { // "light"
	  var divAdditionalLinks = document.getElementById("light_conf");		
	  set_sensor_config("light_lograte" ,divAdditionalLinks.light_lograte.value);
	  set_sensor_config("light_min" ,divAdditionalLinks.light_min.value);
	  set_sensor_config("light_max" ,divAdditionalLinks.light_max.value);
	  set_sensor_config("light_alarm" ,divAdditionalLinks.light_alarm.value);
	}
	if(sensor==4) { // "pb1"
	  var divAdditionalLinks = document.getElementById("pb1_conf");		
	  set_sensor_config("pb1_alarm" ,divAdditionalLinks.pb1_alarm.value);
	}
	if(sensor==5) { // "pb2"
	  var divAdditionalLinks = document.getElementById("pb2_conf");		
	  set_sensor_config("pb2_alarm" ,divAdditionalLinks.pb2_alarm.value);
	}
	if(sensor==6) { // "pb3"
	  var divAdditionalLinks = document.getElementById("pb3_conf");		
	  set_sensor_config("pb3_alarm" ,divAdditionalLinks.pb3_alarm.value);
	}
	if(sensor==7) { // "js"
	  var divAdditionalLinks = document.getElementById("js_conf");		
	  set_sensor_config("js_alarm" ,divAdditionalLinks.js_alarm.value);
	}
}

function set_sensor_config(field,value) {
	var oXmlHttp = zXmlHttp.createRequest();
	var reg=new RegExp("[_]", "");
	var sendValue=field.split(reg);
	var commandline="set_sensor_config.cgi?sensor="+sendValue[0]+"&"+sendValue[1]+"="+value;
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

function get_sensor_config(sensor) {
	var oXmlHttp = zXmlHttp.createRequest();
	oXmlHttp.open("get","get_sensor_config.cgi?sensor="+sensor , true);
    	oXmlHttp.onreadystatechange = function () {        
		if (oXmlHttp.readyState == 4) {				
           		if (oXmlHttp.status == 200) {
				var divAdditionalLinks = document.getElementById(sensor+"_conf");
				if(divAdditionalLinks) {	
					var line=new RegExp("[\n]+", "g");
					var colum=new RegExp("[=]+", "g");
			    		var aLine=oXmlHttp.responseText.split(line);
					for (var i=0; i<aLine.length; i++) {
						var aColum=aLine[i].split(colum);
						var subDivAdditionalLinks = document.getElementById(sensor+"_"+aColum[0]);	
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

