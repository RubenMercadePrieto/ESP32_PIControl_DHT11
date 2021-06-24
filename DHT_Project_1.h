//============
//Webpage Code
//============
String webpageCodeDHT = R"***(
<head>
  <!-- Plotly.js -->
<script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
<script src="https://code.jquery.com/jquery-3.3.1.min.js"></script>
<meta http-equiv="content-type" content="text/html; charset=UTF-8" />
<meta name="viewport" content="width=device-width, initial-scale=0.8">
<meta name="author" content="Ruben Mercade Prieto">
<title>Having fun with the ESP32, by Ruben - Sensor Logging</title>

  <!-- Button style https://codepen.io/liamj/pen/yrEXNx   -->
<link href="https://fonts.googleapis.com/css?family=Lato" rel="stylesheet">
  <style>


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
  </style>
</head>

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


<table border=0 cellspacing=0 cellpadding=10 style="max-width:1000px;"><tr><td>
<h2>Sensor Logging and Plotting - Simple weather station</h2>
<h3>by Ruben Mercade Prieto - June 2021</h3>
<h4>Values in <font color=red>red</font> are automatically generated when the page is loaded, and the one in <font color=blue>blue</font> is automatically refreshed.</h4>
<p>Watch explanation <a href="https://youtu.be/iLYnsSbcMas" target="video">Video</a>.</p>

<p>The ESP32 microcontroller is a product of the Shanghai company Espressiff, released to market in 2016. Despite its low cost, about  
<a href="https://www.amazon.com/ESP32-Module-Bluetooth-Flash-Development/dp/B07N1168YB" target="amazon">$10-20</a>, it is very technically capable - using TSMC 40 nm process technology,
but it is different from a simpler microcontroller like an Arduino Uno due to its Bluetooth, Wifi and Webserver capabilities: this project is an example of the later two.
</p>
<p>
According to <a href="https://openweathermap.org/" target="OpenWeatherMap">OpenWeatherMap.org</a>, the weather now in <font color=red><span id="location"></span></font>
 is <font color=red><span id="description"></span></font>, with an outside temperature of <font color=red><span id="temp"></span></font> 
and a humidity of <font color=red><span id="Humidity"></span></font>, you can verify it <a href="https://openweathermap.org/city/1526273" target="OpenWeatherMap">here</a>.</p>
<p>However, the real conditions inside Ruben's family home are slightly different, as measured with a 
<a href="https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/" target="DHT_info"> DHT11 sensor</a>  every 2 s. Such logging data is stored internally in the ESP32
flash memory. The file size is currently <font color=blue><span id="SizeDataDHT"></span></font> bytes, and there are about <font color=red><span id="SizeFlashFree"></span></font>
 bytes left in the LittleFS Flash memory. From such file, a plot is automatically created every 4 s as shown below, and in addition, it can be send elsewhere for furhter processing if desired, like though email.</p>
<p></p>
    
</td></tr><tr><td>

<img src="/img">

</td></tr><tr><td>	   
	
    


  <script lang="text/javascript">
    // must change the API key
function weatherBalloon( ) {
var key = '********************';
fetch('https://api.openweathermap.org/data/2.5/weather?q=Nur-sultan,Kz&units=metric&appid=' + key)  
.then(function(resp) { return resp.json() }) // Convert data to json
.then(function(data) {
<!-- console.log(data); -->
drawWeather(data);
})
.catch(function() {
// catch any errors
});
}

function drawWeather( d ) {

	document.getElementById('description').innerHTML = d.weather[0].description;
	document.getElementById('temp').innerHTML = d.main.temp + '&degC;';
	document.getElementById('Humidity').innerHTML = d.main.humidity + '%';
	document.getElementById('location').innerHTML = d.name;
}

window.onload = function() {
weatherBalloon( );
dataDHTSize();
}
  
  
function dataDHTSize() {
  var XMLRequest = new XMLHttpRequest();
  XMLRequest.onreadystatechange = function()
  {
	if(this.readyState == 4 && this.status == 200)
	{
		if (this.responseXML != null) {
			//extract data from XML file from ESP32: Setpoint, K, Ti
			document.getElementById("SizeDataDHT").innerHTML = this.responseXML.getElementsByTagName('SizeDataDHT')[0].childNodes[0].nodeValue;
			document.getElementById("SizeFlashFree").innerHTML = this.responseXML.getElementsByTagName('SizeFlashFree')[0].childNodes[0].nodeValue;
		}
	}
  };
  XMLRequest.open("GET", "dataDHTSize", true);
  XMLRequest.send();
}
  </script>
  
  
  


  <div id="myDiv"><!-- Plotly chart will be drawn inside this DIV --></div>
  <script>
	function makeplot() {
	  Plotly.d3.csv("dataDHT.csv", function(data){ processData(data) } );
	};
	  
	function processData(allRows) {
	  var x = [], Temp = [], Hum = [], standard_deviation = [];

	  for (var i=0; i<allRows.length; i++) {
		row = allRows[i];
		x.push( row['AAPL_time'] );
		Temp.push( row['AAPL_Temp'] );
		Hum.push( row['AAPL_Hum'] );
		
	  }
	  <!--  console.log( 'X',x, 'Temp',Temp, 'Hum', Hum);  -->
	  makePlotly( x, Temp, Hum );
	}

	function makePlotly( x, Temp, Hum){
	  var plotDiv = document.getElementById("plot");
	  var traces1 = {
		type: 'scatter',
		x: x, 
		y: Temp,
		mode: 'lines',
		line: {	color: 'rgb(219, 64, 82)'},
		name: 'Temperature'
	  };
	  
	  var traces2 = {
		type: 'scatter',
		x: x, 
		y: Hum,
		yaxis: 'y2',
		mode: 'lines',
		line: {	color: 'rgb(55, 128, 191)'},
		name: 'Humidity'
	  };
	  
	  var data = [traces1, traces2];
	  
	  var layout = {
		showlegend: true,
		legend: {
			x: 0.3,
			xanchor: 'right',
			y: 1
		},
		
		xaxis: {
			showline: true,
			title: 'Time',
			titlefont: {color: '#000'},
			tickfont: {color: '#000'},
			tickwidth: 2,
			linewidth: 2,
			linecolor: '#000',
			tickcolor: '#000',
			nticks: 10
		},
		
		yaxis: {
			showline: true,
			title: 'Temperature (°C)',
			titlefont: {color: 'rgb(219, 64, 82)'},
			tickfont: {color: 'rgb(219, 64, 82)'},
			tickwidth: 2,
			linewidth: 2,
			linecolor: '#000',
			tickcolor: '#000'
		},
		yaxis2: {
			showline: true,
			title: 'Humidity (%)',
			titlefont: {color: 'rgb(55, 128, 191)'},
			tickfont: {color: 'rgb(55, 128, 191)'},
			tickwidth: 2,
			linewidth: 2,
			linecolor: '#000',
			tickcolor: '#000',
			overlaying: 'y',
			side: 'right'
		}	
		};

	  Plotly.react('myDiv', data, layout);
	};
	
	
	setInterval(function(){
	makeplot();
	dataDHTSize();
	}, 4000);

  </script>
  
  <p>If you find such data interesting, you can recieved it by email by filling up this form.</p>
  
  <p>
<form action="/formulariDHT" method="post" id="my_form">
    <label>Name</label>
    <input type="text" name="name" />
    <label>Email</label>
    <input type="email" name="email" />
    <input type="submit" name="submit" value="Send Data" />
</form>
<div id="EmailResponse"></div>
</p>
</td></tr><tr><td>	
  <p align=left>The code files, explanations, and references used in this project are given in my GitHub repository at
 <a href="https://github.com/RubenMercadePrieto/ESP32_PIControl_DHT11" target=""  style="text-decoration: none">
 https://github.com/RubenMercadePrieto/ESP32_PIControl_DHT11</a></p>
</td></tr></table>
</center>

<script>
$("#my_form").submit(function(event){
	event.preventDefault(); //prevent default action 
	var form_data = $(this).serialize(); //Encode form elements for submissions
	var post_url = $(this).attr("action"); //get form action url
	
	//$.get( post_url, form_data);
	$.get( post_url, form_data, function( response ) {
	  $("#EmailResponse").html( response );
	});
});
</script>
</body>
)***";
