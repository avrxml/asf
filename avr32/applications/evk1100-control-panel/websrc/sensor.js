var iInterval   = 200;
var iGraphWidth  = 100;
var iGraphMiny = 0;
var iGraphMaxy = 100;
var iArrayLength  = 100;
var aStore = new Array();
var aStoreGraph      = new Array();
var aStoreGraphIndex = new Array();
var aStoreArray      = new Array();
var aStoreArrayIndex = new Array();
var aSensorList = new Array("temp","pot","light","pb1","pb2","pb3","js");
var sSensorAll = "all";
sNavigator="";


function downloadAllSensors() {
	var oXmlHttp = zXmlHttp.createRequest();
    	var divAdditionalLinks = document.getElementById(sSensorAll);
	var command_line="get_sensor_value.cgi?sensor="+sSensorAll;
	var repeatTime = iInterval;
	var imax = 0;
    	if (divAdditionalLinks.logsize != undefined) {
		command_line=command_line+"&logzise="+divAdditionalLinks.logsize.value;			
	}
    	if (divAdditionalLinks.repeattime != undefined) {
		repeatTime = divAdditionalLinks.repeattime.value;
	}
	oXmlHttp.open("get",command_line , true);
    	oXmlHttp.onreadystatechange = function () {
		if (oXmlHttp.readyState == 4) {				
            		if (oXmlHttp.status == 200) {
				// For Each Sensor
				var regLine=new RegExp("[\n]+", "g");
				var regPipe=new RegExp("[|]+", "g");
				var aLine=oXmlHttp.responseText.split(regLine);
				if(sNavigator=="IE6") {imax = aLine.length; }
				else {imax = (aLine.length - 1); }
				for (var i=0; i<imax ; i++) {						
					var newText="<table border=\"1\"  width=\"100%\" bordercolor=\"black\"><colgroup><col width=\"1*\"><col width=\"3*\"><col width=\"2*\"><col width=\"2*\"></colgroup><tr><th>Sensor</th><th>Date</th><th>Value</th><th>Alarm</th></tr>";
					var ligne=aLine[i].split(regPipe);
					newText=newText+("<tr>");
					for (var j=0; j<ligne.length; j++) {
						 newText=newText+("<td>"+ligne[j]+"</td>");
					}	
					newText=newText+("</tr></table>");
					var id = ligne[0].replace(/ /,"");
					var divSensorLinks = document.getElementById(id);
					divSensorLinks.innerHTML = newText; 
	                		divSensorLinks.style.display = "block";
	         		}
			  
			 	setTimeout(function(){downloadAllSensors()},repeatTime);
        		}
    		}
	}
	oXmlHttp.send(null);	
}

function downloadSensors(sensor) {
	var oXmlHttp = zXmlHttp.createRequest();
    	var divAdditionalLinks = document.getElementById(sensor);
	var command_line="get_sensor_value.cgi?sensor="+sensor;
	var repeatTime = iInterval;
    	if (divAdditionalLinks.logsize != undefined) {
		command_line=command_line+"&logzise="+divAdditionalLinks.logsize.value;			
	}
    	if (divAdditionalLinks.repeattime != undefined) {
		repeatTime = divAdditionalLinks.repeattime.value;
	}
	oXmlHttp.open("get",command_line , true);
    	oXmlHttp.onreadystatechange = function () {
		if (oXmlHttp.readyState == 4) {				
            		if (oXmlHttp.status == 200) {
				var newText="<table border=\"1\"  width=\"100%\" bordercolor=\"black\"><colgroup><col width=\"1*\"><col width=\"3*\"><col width=\"2*\"><col width=\"2*\"></colgroup><tr><th>Sensor</th><th>Date</th><th>Value</th><th>Alarm</th></tr>";
				var regpipe=new RegExp("[|]+", "g");
		 		if (divAdditionalLinks.logsize == undefined) { 
					var ligne=oXmlHttp.responseText.split(regpipe);
					newText=newText+("<tr>");
					for (var j=0; j<ligne.length; j++) {
					 	newText=newText+("<td>"+ligne[j]+"</td>");
					}	
					newText=newText+("</tr></table>");
					divAdditionalLinks.innerHTML = newText; 
	                		divAdditionalLinks.style.display = "block";
	         		}
		 		else { 
					var reg=new RegExp("[\n]+", "g");
		    			var tableau=oXmlHttp.responseText.split(reg);
					for (var i=0; i<tableau.length; i++) {
					 	newText=newText+("<tr>");
				 		var ligne=tableau[i].split(regpipe);
					 	for (var j=0; j<ligne.length; j++) {
					 		newText=newText+("<td>"+ligne[j]+"</td>");
					 	}
					 	newText=newText+("</tr>");
					}
					newText=newText+"</table>";
        		        	divAdditionalLinks.innerHTML = "Log Size is:"+divAdditionalLinks.logsize.value+
				                                       "<br>refresh time is :"+divAdditionalLinks.repeattime.value+"ms<br>"+
							               newText;
			                divAdditionalLinks.style.display = "block";        
			 	}
			 	setTimeout(function(){downloadSensors(sensor)},repeatTime);
        		}
    		}
	}
	oXmlHttp.send(null);	
}

function initSensorsGraph(sensor) 
{
	var maxWidth = iGraphWidth;
	var miny  = iGraphMiny;
	var divConfig = document.getElementById(sensor+"_config");
	if (divConfig.width != undefined) { 
		maxWidth =  parseInt(divConfig.width.value);
	}
	if (divConfig.miny != undefined) { 
		miny =  parseInt(divConfig.miny.value);
	}
	aStoreGraphIndex[sensor]=-1*maxWidth;
	for(i=0;i<maxWidth;i++) {
		aStoreGraph[sensor].push([aStoreGraphIndex[sensor]++,miny] );	
	}
}

function downloadSensorsGraph(sensor) {
	var repeatTime = iInterval ;
	var maxWidth   = iGraphWidth ;
	var maxy  = iGraphMaxy 
	var miny  = iGraphMiny
	
 	var divAdditionalLinks = document.getElementById(sensor);
	var divConfig = document.getElementById(sensor+"_config");
	var reg=new RegExp("[_]", "");
	var sendValue=sensor.split(reg);
	var command_line="get_sensor_value.cgi?sensor="+sendValue[0];

    	if (divConfig.repeattime != undefined) {
		repeatTime = parseInt(divConfig.repeattime.value);
	}
	if (divConfig.width != undefined) { 
		maxWidth =  parseInt(divConfig.width.value);
	}
	if (divConfig.maxy != undefined) { 
		maxy =  parseInt(divConfig.maxy.value);
	}
	if (divConfig.miny != undefined) { 
		miny = parseInt(divConfig.miny.value);
	}
	var oXmlHttp = zXmlHttp.createRequest();
	oXmlHttp.open("get",command_line , true);	
    	oXmlHttp.onreadystatechange = function () {
        
		if (oXmlHttp.readyState == 4) {				
            		if (oXmlHttp.status == 200) {				
			
				var regColone=new RegExp("[|]+", "g");
				var colone=oXmlHttp.responseText.split(regColone);
				colone[2]=parseFloat(colone[2]);
				aStoreGraph[sensor].push([aStoreGraphIndex[sensor]++,colone[2]] );
				if(aStoreGraph[sensor].length>maxWidth) {
					aStoreGraph[sensor].shift();
				}
				divAdditionalLinks.innerHTML="";
				if(aStoreGraph[sensor].length<maxWidth) {
					var options = {"xAxis": [0,maxWidth], "yAxis":[miny,maxy]};
				} 
				else {
					var minx=aStoreGraphIndex[sensor] - aStoreGraph[sensor].length;
					var maxx = aStoreGraphIndex[sensor];
					var options = {"xAxis": [minx,maxx], "yAxis":[miny,maxy]};
				}	
				var plotter = EasyPlot("line",options , $(sensor), [aStoreGraph[sensor]]);
	                	divAdditionalLinks.style.display = "block";
			 	setTimeout(function(){downloadSensorsGraph(sensor)},repeatTime);
        		}
    		}
	}
	oXmlHttp.send(null);	
}

function downloadSensorsArray(sensor) {
	var repeatTime = iInterval ;
	var maxLength =  iArrayLength ;
    	var divAdditionalLinks = document.getElementById(sensor);
	var divConfig = document.getElementById(sensor+"_config");
	var reg=new RegExp("[_]", "");
	var sendValue=sensor.split(reg);
	var command_line="get_sensor_value.cgi?sensor="+sendValue[0];
    	if (divConfig.repeattime != undefined) {
		repeatTime = divConfig.repeattime.value;
	}
	if (divConfig.length != undefined) { 
		maxLength = divConfig.length.value;
	}
	var oXmlHttp = zXmlHttp.createRequest();
	oXmlHttp.open("get",command_line , true);
	oXmlHttp.onreadystatechange = function () {
		if (oXmlHttp.readyState == 4) {				
            		if (oXmlHttp.status == 200) {
				aStoreArray[sensor].unshift(oXmlHttp.responseText);
				if(aStoreArray[sensor].length>maxLength) {
					aStoreArray[sensor].pop();
				}
			
				var regpipe=new RegExp("[|]+", "g");
					var newText="<table border=\"1\"  width=\"100%\" bordercolor=\"black\"><colgroup><col width=\"1*\"><col width=\"3*\"><col width=\"2*\"><col width=\"2*\"></colgroup><tr><th>Sensor</th><th>Date</th><th>Value</th><th>Alarm</th></tr>";
				for (index in aStoreArray[sensor]) {
		    			var ligne=aStoreArray[sensor][index].split(regpipe);
					newText=newText+("<tr>");
					for (var j=0; j<ligne.length; j++) {
						newText=newText+("<td>"+ligne[j]+"</td>");
					}
					newText=newText+("</tr>");
				}
				newText=newText+"</table></center>";
				divAdditionalLinks.innerHTML= "refresh time is :"+repeatTime+"ms<br>"+
							      " Storing the last :" +aStoreArray[sensor].length + " values<br>"+
							    newText;
	                	divAdditionalLinks.style.display = "block";
			 	setTimeout(function(){downloadSensorsArray(sensor)},repeatTime);
        		}
    		}
	}
	oXmlHttp.send(null);	
}

window.onload = function () {
	var index;

	FindNavigator();

   	if (zXmlHttp.isSupported()) {
		var divAdditionalLinks = document.getElementById(sSensorAll);
		if(divAdditionalLinks) {
			downloadAllSensors();
		}
		else {
	   		for (index in aSensorList) {	
				var divAdditionalLinks = document.getElementById(aSensorList[index]);
				if(divAdditionalLinks) {
					downloadSensors(aSensorList[index]);				
				}
			}
		}
		for (index in aSensorList) {	
			var divAdditionalLinks = document.getElementById(aSensorList[index]+"_graph");
			if(divAdditionalLinks) {
				aStoreGraph[aSensorList[index]+"_graph"] = new Array();
				aStoreGraphIndex[aSensorList[index]+"_graph"] = 0;
				initSensorsGraph(aSensorList[index]+"_graph");
				downloadSensorsGraph(aSensorList[index]+"_graph");
			}
		}
		for (index in aSensorList) {				
			var divAdditionalLinks = document.getElementById(aSensorList[index]+"_array");
			if(divAdditionalLinks) {
				aStoreArray[aSensorList[index]+"_array"] = new Array();
				aStoreArrayIndex[aSensorList[index]+"_array"] = 0;
				downloadSensorsArray(aSensorList[index]+"_array");
			}
		}
	
    	}
	else {
		alert("zXmlHttp not Supported");
	}
}	

function FindNavigator() {
	 sNavigator="";
	var strChUserAgent = navigator.userAgent;
	var intSplitStart = strChUserAgent.indexOf("(",0);
	var intSplitEnd = strChUserAgent.indexOf(")",0);
	var strChStart = strChUserAgent.substring(0,intSplitStart);
	var strChMid = strChUserAgent.substring(intSplitStart, intSplitEnd);
	var strChEnd = strChUserAgent.substring(strChEnd);

	if(strChMid.indexOf("MSIE 7") != -1)
		 sNavigator="IE7"
	else if(strChMid.indexOf("MSIE 6") != -1)
		 sNavigator="IE6";
	else if(strChEnd.indexOf("Firefox/2") != -1)
		 sNavigator="FIREFOX2";
	else if(strChEnd.indexOf("Firefox") != -1)
		 sNavigator="FIREFOX";
	else if(strChEnd.indexOf("Netscape/7") != -1)
		 sNavigator="NETSCAPE7";
	else if(strChEnd.indexOf("Netscape") != -1)
		 sNavigator="NETSCAPE";
	else if(strChStart.indexOf("Opera/9") != -1)
		 sNavigator="OPERA9";
	else if(strChStart.indexOf("Opera") != -1)
		 sNavigator="OPERA";
	else
		 sNavigator="AUTRE";

}
