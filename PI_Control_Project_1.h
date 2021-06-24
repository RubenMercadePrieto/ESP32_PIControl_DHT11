//============
//Webpage Code 
//============
String webpageCode = R"***(
<!DOCTYPE html>
<head>
<script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
<script src="https://code.jquery.com/jquery-3.3.1.min.js"></script>
<script src='https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.7/MathJax.js?config=TeX-MML-AM_CHTML'></script>
<meta http-equiv="content-type" content="text/html; charset=UTF-8" />
<meta name="viewport" content="width=device-width, initial-scale=0.8">
<meta name="author" content="Ruben Mercade Prieto">

  <script>
  MathJax.Hub.Config({
  tex2jax: {inlineMath: [['$','$'], ['\\(','\\)']]}
});
  </script>

<link href="https://fonts.googleapis.com/css?family=Lato" rel="stylesheet">
  <style>

/* Buttons style */
.corner-button {
  font-family: "Lato", sans-serif;
  letter-spacing: 0.02rem;
  cursor: pointer;
  background: transparent;
  border: 0.4rem solid currentColor;
  padding: 1.15rem 1.3rem;
  font-size: 1.5rem;
  color: black;
  position: relative;
  transition: color 0.3s;
  text-decoration: none;
  white-space: nowrap;
}
.corner-button:hover {
  color: red;
}
.corner-button:hover::before {
  width: 0;
}
.corner-button:hover::after {
  height: 0;
}
.corner-button:active {
  border-width: 0.25rem;
}
.corner-button span {
  position: relative;
  z-index: 2;
}
.corner-button::before, .corner-button::after {
  content: "";
  position: absolute;
  background: white;
  z-index: 1;
  transition: all 0.3s;
}
.corner-button::before {
  width: calc(100% - 3rem);
  height: calc(101% + 1rem);
  top: -0.5rem;
  left: 50%;
  transform: translateX(-50%);
}
.corner-button::after {
  height: calc(100% - 3rem);
  width: calc(101% + 1rem);
  left: -0.5rem;
  top: 50%;
  transform: translateY(-50%);
}


/* SLIDER The switch - the box around the slider */
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

/* Hide default HTML checkbox */
.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

/* The slider */
.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}

/* Style the form - display items horizontally */
.form-inline {
  flex-flow: row wrap;
  align-items: center;
  border-radius: 10px;
  background-color: #f2f2f2;
  padding: 20px;
}

/* Add some margins for each label */
.form-inline label {
  margin: 10px 10px 0x 0px;
}


/* Style the submit button */
.form-inline button {
  padding: 10px 10px;
  background-color: dodgerblue;
  border: 1px solid #ddd;
  color: white;
  margin: 10px 10px 10x 10px;
}

.form-inline button:hover {
  background-color: royalblue;
}

</style>
</head>
<html>
<body>
<center>
<table border=0 cellspacing=0 cellpadding=10><tr><td>
<a href="https://www.espressif.com/en/products/socs/esp32" target="ESP32" style="text-decoration: none">
<p style="text-align: center; font-weight: bold; color:blue; font-size: 1.4rem">Microcontroller <br>ESP32</p></a>
<p style="text-align: center; font-weight: bold; color:black; font-size: 0.8rem">by Ruben Mercade Prieto</p>
</td><td width=253>
<a href="/DHT" class="corner-button"><span>Weather Station</span></a>
</td><td  width=253>
<a href="/" class="corner-button"><span>PI Controller</span></a>
</td></tr></table>

<script>
function updateTextInput(val,NumRange) {
	  document.getElementById(NumRange).innerHTML=val; 
	}

</script>


<table border=0 cellspacing=0 cellpadding=10  style="max-width:1000px;"><tr><td>

<h2>PI Control - Photoresistor + LED</h2>
<h3>by Ruben Mercade Prieto - June 2021</h3>
<p>Watch explanation <a href="https://youtu.be/iLYnsSbcMas" target="video">Video</a>.</p>

<p>This is an example of a PI control using a photoresistor as a sensor and LED(s) as the controller. You can choose between a Red, Green or Blue LED, or a combination thereof.
Obviously, the more LED selected, the more light - which may be a problem to control low light conditions. The ESP32 is currently used in several commercial industrial control
solutions, e.g.: <a href="https://www.industrialshields.com/industral-esp32-plc-products-family-ideal-for-iot-solutions" target="1">[1]</a>, 
<a href="https://norvi.lk/industrial-esp32-norvi-iiot/" target="2">[2]</a>, <a href="https://moduino.techbase.eu/" target="3">[3]</a>.</p>

<p>The minimum value of the photoresistor is measured from the enviromental conditions at the startup of the ESP32; the maximum value is found similarly with all the 3 LEDs
at maximum power.</p>

<p>Let us revisit the key equation in a PI control. $Led(t)$ is the controlled variable, an 8bit PWM signal from the ESP32, and $Led_{bias}$ is the initial value, often zero.
$P_{SP}$ and $P_{PV}$ are the set point and process variable, respectively, which in this case corresponds to the 12bit (0-4096) value from the photoresistor. Their difference
is the key error to calculate, $e(t)$.
The tunable parameters are $K_P$, the proportional constant, and $\tau _I$, the integral time constant</p>

$$Led(t)=Led_{bias}+ K_P(P_{SP} - P_{PV})+ \frac{K_P}{\tau _I} \int_{0}^{t}{(P_{SP} - P_{PV})dt} $$
$$= Led_{bias}+ K_P e(t)+ \frac{K_P}{\tau _I} \int_{0}^{t}{e(t)dt} $$
$$Led(t)\approx Led_{bias}+K_P e(t)+\frac{K_P}{\tau _I}\sum\limits_{i=1}^{n_t}{e_i(t) \Delta t}$$

<p>where in this ESP32 example, it has been selected $\Delta t = 0.5s$.</p>
<p></p>

</td></tr><tr><td>

<img src="/img">

</td></tr><tr><td>		

	<form action="/formulari" method="post" id="my_form" class="form-inline">
	<table border=0 cellspacing=0 cellpadding=10>
	<tr><td align="right"><div>
	<label for="myRange1">Photoresistor target value $P_{SP}$ 
	(<span id="sliderRange1Min"></span>-<span id="sliderRange1Max"></span>): <b><span id="sliderRange1"></span></b></label></td><td>
	<input type="range" min="2000" max="3000" step="10" id="myRange1" name="myRange1" onchange="updateTextInput(this.value,'sliderRange1');">
	</div></td></tr>
	<tr><td align="right"><div>
	<label for="myRange2">$K_P$ value (0.005-0.1): <b><span id="sliderRange2"></span></b></label></td><td>
	<input type="range" min="0.005" max="0.1" step="0.005" id="myRange2" name="myRange2"  onchange="updateTextInput(this.value,'sliderRange2');">
	</div></td></tr>
	<tr><td align="right"><div>
	<label for="myRange3">$\tau _I$ value (0.1-5): <b><span id="sliderRange3"></span></b></label></td><td>
	<input type="range" min="0.1" max="5" step="0.1" id="myRange3" name="myRange3"  onchange="updateTextInput(this.value,'sliderRange3');">
	</div></td></tr>
	
	<tr><td align="right">Actuator(s):  </td><td>
	<label class="container">
	<input type="checkbox" id="LedRED" name="LedRED"  value="on" checked="true">LED Red
	<span class="checkmark"></span>
	</label>
	<label class="container">
	<input type="checkbox" id="LedGREEN" name="LedGREEN" value="on">LED Green
	<span class="checkmark"></span>
	</label>
	<label class="container">
	<input type="checkbox" id="LedBLUE" name="LedBLUE" value="on">LED Blue
	<span class="checkmark"></span>
	</label>
	</td></tr>
	
	<tr><td align="right"> Activate PI control</td><td>
	<label class="switch"> <input type="checkbox" name="ActivatePIControl"><span class="slider round"></span></label></td></tr>
	<tr><td><p style="font-size:80%;text-align:center;">Existing control paremeters in the ESP32<br> are refreshed in this form every 15 s.</p></td><td>

	<button type="submit">Send information to ESP32 </button></td></tr></table>
	</form>
</td></tr><tr><td>



<script>
$("#my_form").submit(function(event){
	event.preventDefault(); //prevent default action 
	var form_data = $(this).serialize(); //Encode form elements for submissions
	var post_url = $(this).attr("action"); //get form action url
	
	$.get( post_url, form_data);
	//$.get( post_url, form_data, function( response ) {
	//  $("#server-results").html( response );
	//});
});
</script>

  <div id="graph"></div>

</td></tr><tr><td>	
 <p align=left>The code files, explanations, and references used in this project are given in my GitHub repository at
 <a href="https://github.com/RubenMercadePrieto/ESP32_PIControl_DHT11" target=""  style="text-decoration: none">
 https://github.com/RubenMercadePrieto/ESP32_PIControl_DHT11</a></p>
</td></tr></table>
</center>

  <script>
	var updateData;
	var cnt = 0;
	
	function getParameters() {
	  var XMLRequest = new XMLHttpRequest();
      XMLRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
			if (this.responseXML != null) {
				//extract data from XML file from ESP32: Setpoint, K, Ti, Active button, Leds
				document.getElementById("sliderRange1").innerHTML = this.responseXML.getElementsByTagName('Control_SetPoint')[0].childNodes[0].nodeValue;
				document.getElementById("sliderRange2").innerHTML = this.responseXML.getElementsByTagName('Control_Kp')[0].childNodes[0].nodeValue;
				document.getElementById("sliderRange3").innerHTML = this.responseXML.getElementsByTagName('Control_Ti')[0].childNodes[0].nodeValue;
				
				document.getElementById("myRange1").value = this.responseXML.getElementsByTagName('Control_SetPoint')[0].childNodes[0].nodeValue;
				document.getElementById("myRange2").value = this.responseXML.getElementsByTagName('Control_Kp')[0].childNodes[0].nodeValue;
				document.getElementById("myRange3").value = this.responseXML.getElementsByTagName('Control_Ti')[0].childNodes[0].nodeValue;

				document.getElementById("myRange1").min = this.responseXML.getElementsByTagName('MinPhotoResValue')[0].childNodes[0].nodeValue;
				document.getElementById("myRange1").max = this.responseXML.getElementsByTagName('MaxPhotoResValue')[0].childNodes[0].nodeValue;
				document.getElementById("sliderRange1Min").innerHTML = this.responseXML.getElementsByTagName('MinPhotoResValue')[0].childNodes[0].nodeValue;
				document.getElementById("sliderRange1Max").innerHTML = this.responseXML.getElementsByTagName('MaxPhotoResValue')[0].childNodes[0].nodeValue;
				
				document.getElementById("LedRED").checked = (this.responseXML.getElementsByTagName('LedRed')[0].childNodes[0].nodeValue === 'on');
				document.getElementById("LedGREEN").checked = (this.responseXML.getElementsByTagName('LedGreen')[0].childNodes[0].nodeValue === 'on');
				document.getElementById("LedBLUE").checked = (this.responseXML.getElementsByTagName('LedBlue')[0].childNodes[0].nodeValue === 'on');

			}
        }
      };
      XMLRequest.open("GET", "readParameters", true);
      XMLRequest.send();
    }
	
	getParameters();	//do this when the webpage is loaded
	
	setInterval(function(){
	getParameters();
	}, 15000);			//update parameters from the server every 15 s, they may have been changed by someone else
	
	function getDataXML() {			//this function is used to collect data from the ESP32, before plotting
	  var XMLRequest = new XMLHttpRequest();
      XMLRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
			if (this.responseXML != null) {
				var tmp = parseFloat(this.responseXML.getElementsByTagName('PhotoResVal')[0].childNodes[0].nodeValue);
				var tmp2 = parseFloat(this.responseXML.getElementsByTagName('Control_SetPoint')[0].childNodes[0].nodeValue);
				var tmp3 = parseFloat(this.responseXML.getElementsByTagName('LEDRedValue')[0].childNodes[0].nodeValue);
				var tmp4 = parseFloat(this.responseXML.getElementsByTagName('LEDBlueValue')[0].childNodes[0].nodeValue);
				var tmp5 = parseFloat(this.responseXML.getElementsByTagName('LEDGreenValue')[0].childNodes[0].nodeValue);
				cnt++;
				updateData = {
					y: [[tmp], [tmp2], [tmp3], [tmp4], [tmp5]]
					}
			}
        }
      };
      XMLRequest.open("GET", "readData", true);
      XMLRequest.send();
    }
	
	var trace1 = {
	//x: [0],
	y: [],
	name: 'Photoresistor',
	type: 'lines',
	line: {	
		color: 'black',
		width: 3}
	};

	var trace2 = {
	//x: [0],
	y: [],
	name: 'Setpoint',
	line: {
		color: 'black',
		dash: 'dot',
		width: 3
	},
	type: 'lines'
	};
	
	var trace3 = {
	//x: [0],
	y: [],
	name: 'LED_Red',
	yaxis: 'y2',
	type: 'lines',
	line: {	
		color: 'red',
		width: 1}
	};
	
	var trace4 = {
	//x: [0],
	y: [],
	name: 'LED_Blue',
	yaxis: 'y2',
	line: {
		color: 'blue',
		dash: 'dot',
		width: 2
	},
	type: 'lines'
	};
	
	var trace5 = {
	//x: [0],
	y: [],
	name: 'LED_Green',
	yaxis: 'y2',
	line: {
		color: 'green',
		dash: 'dashdot',
		width: 2
	},
	type: 'lines'
	};
	
	var data = [trace1, trace2, trace3, trace4, trace5];
	
	var layout = {
	showlegend: true,
	legend: {

		"orientation": "h"
		},
		
	xaxis: {
	showline: true,
	titlefont: {color: '#000'},
	tickfont: {color: '#000'},
	tickwidth: 2,
	linewidth: 2,
	linecolor: '#000',
	tickcolor: '#000',
	nticks: 10,
	},
		
	yaxis: {
	title: 'Photoresistor',
	titlefont: {color: '#000'},
	tickfont: {color: '#000'},
	tickwidth: 2,
	linewidth: 2
	},
	yaxis2: {
	title: 'LED (8bit)',
	titlefont: {color: '#000'},
	tickfont: {color: '#000'},
	tickwidth: 2,
	linewidth: 2,
	overlaying: 'y',
	side: 'right'
	}
	};
	getDataXML();
	Plotly.newPlot('graph', data, layout);
	
	var interval = setInterval(function() {
		getDataXML();
		Plotly.extendTraces('graph', updateData, [0, 1, 2, 3, 4])
		if(cnt > 200){				//easy way to limit the number of points shown in a plot, by adjusting x axis
			var xRange = [cnt-200, cnt];
			Plotly.relayout('graph',{'xaxis.range': xRange});
		}
	}, 500);
  </script>
</body>
</html>
)***";
