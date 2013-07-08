var aFadeMessage =  new Array(
		              '<img alt="AVR32" src="avr32logo.jpg">',
			      '<img alt="AT32UC3A0512" src="AT32UC3A0512.jpg">',
			      'World\'s Lowest Power 32-bit Flash MCU',
			      'Delivers 80 Dhrystone MIPS and Draws Only 40mA at 66MHz',
			      'AVR32 UC3A Series MCUs offers DSP, 512KB Flash, 64KB RAM,<br>Ethernet MAC, USB OTG,<br>External Bus Interface'
			      );
window.onload = function () {
	var fade = document.getElementById("fade1");
	if(fade) {
		fading0();
	}
}			      
			      
/// Fading Effect
function $(v) { return(document.getElementById(v)); }

function $S(v) { return($(v).style); }

function uni(v,id,s,top) { var ob=$S(id),b=v/100; ob.opacity=b; ob.top=top+'px'; ob.MozOpacity=b; ob.KhtmlOpacity=b; ob.filter="alpha(opacity="+v+")"; }

function zero(v) { v=parseInt(v); return(!isNaN(v)?v:0); }

function fade(id,ln,s) { 	
	var top=zero(parseInt($S(id).top));
	function opacity(oStart,oEnd,ln) { 
		var speed=Math.round(ln/100),timer=0;
		if(oStart>oEnd) { 
			for(i=oStart; i>=oEnd; i--) { 
				setTimeout("uni("+i+",'"+id+"','','"+(top--)+"')",timer*speed); 
				timer++; 
			} 
			//setTimeout("$S('"+id+"').display='none';",timer*speed); 
		}
		else if(oStart<oEnd) { 
			$S(id).display='inline'; 
			for(i=oStart; i<=oEnd; i++) { 
				setTimeout("uni("+i+",'"+id+"','"+1+"','"+(top++)+"')",timer*speed); timer++; 
			} 
		}

	}

	if(s==1 || (!s && $S(id).opacity==0)) {
		opacity(0,100,ln); 
	}
	else  {
		opacity(100,0,ln);
	}

}

function fading0() {
  var fade2 = document.getElementById("fade2");
  var fade3 = document.getElementById("fade3");
  fade2.innerHTML="";
  fade3.innerHTML="";
	var divAdditionalLinks = document.getElementById("fade1");
	divAdditionalLinks.innerHTML=aFadeMessage[0];
	fade("fade1",1000,1);
	setTimeout(function(){fadout0()},3000);		
}

function fadout0() {
	fade("fade1",1000,0);
	setTimeout(function(){fading1()},1100);
}

function fading1() {
	var divAdditionalLinks = document.getElementById("fade1");
	divAdditionalLinks.innerHTML=aFadeMessage[1];
	fade("fade1",1000,1);
	setTimeout(function(){fadout1()},3000);		
}

function fadout1() {
	fade("fade1",1000,0);
	setTimeout(function(){fading2()},1100);
}


function fading2() {
	fade("fade1",1000,0);
	var divAdditionalLinks = document.getElementById("fade1");
	divAdditionalLinks.innerHTML=aFadeMessage[2];
	fade("fade1",1000,1);
	setTimeout(function(){fading3()},2000);		
}

function fading3() {
	var divAdditionalLinks = document.getElementById("fade2");
	divAdditionalLinks.innerHTML=aFadeMessage[3];
	fade("fade2",1000,1);
	setTimeout(function(){fading4()},3000);
}

function fading4() {
	var divAdditionalLinks = document.getElementById("fade3");
	divAdditionalLinks.innerHTML=aFadeMessage[4];
	fade("fade3",1000,1);
	setTimeout(function(){fadoutall()},4000);
}

function fadoutall() {
	fade("fade1",1000,0);
	fade("fade2",1000,0);
	fade("fade3",1000,0);
	setTimeout(function(){fading0()},1100);
}
