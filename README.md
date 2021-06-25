# PI control (photoresistor + LED) and DHT11 Weather station using a ESP32

Watch this project in action in this video:

[![Youtube - Microcontroller ESP 32 Project: PI Control (photoresistor + LEDs) and Weather Station using a DHT11](https://img.youtube.com/vi/iLYnsSbcMas/mqdefault.jpg)](https://youtu.be/iLYnsSbcMas)

This project started from my desire to refresh process control concepts, which are essential to a chemical engineer, in a practical hands-on approach way; and its conception
was substantially influenced by the work done at APmonitor.com and their [Temperature Control Lab](http://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl). 
There are nevertheless some differences:
1. Due to the lack of a suitable heat source to perform a temperature control loop, with the items I had available I decided to do a control system based on a photoresistor and
several LEDs.
2. All programing was done using Arduino IDE, not Python.
3. An ESP32 was used instead of an Arduino, in order to explore the intrinsic Webserver capabilities of this microcontroller. In fact, most of the workload was on the web pages.

This is how it looks like:

![PI Control & DHT](/data/PIControlS.jpg)

This project assumes that you are already familiar with Arduino and the basic commands and sketches. Hence, the main novelty is based on the use of the ESP32. 
The first thing to notice is which microcontroller you actually have. Notice that meanwhile there is the ['official' DevKit from Espressif](https://www.espressif.com/en/products/devkits/esp32-devkitc/overview),
there are in fact many other DevKits, as shown in [esp32.net](http://esp32.net/). The one I purchased was in fact not an original one, unknowingly, but from another company
called [GOOUUU](https://world.taobao.com/item/547075951035.htm). The only relevance that I could see was that the pin numbers may be different, mine are shown here: 
[Pinlayout](https://img.alicdn.com/imgextra/i1/672934643/TB2r4JyixXkpuFjy0FiXXbUfFXa_!!672934643.jpg_760x760Q50s50.jpg_.webp).

The following is a step-by-step guide using online tutorials that I more or less followed in the development of this project. Note that many items could be done in different order.

[1. Learn how to use Arduino to monitor a photoresistor, modulate LED, and read the DHT11 sensor](#1-learn-how-to-use-Arduino-to-monitor-a-photoresistor-modulate-LED-and-read-the-DHT11-sensor)
[2. Install the ESP32 into the Arduino IDE](#2-install-the-esp32-into-the-arduino-ide)
[3. Connect the ESP32 to your local Wifi network](#3-connect-the-esp32-to-your-local-wifi-network)
[4. Read photoresistor analog signal](#4-read-photoresistor-analog-signal)
[5. DHT sensor readings](#5-dnt-sensor-readings)
[6. Get time from a NTP server](#6-get-time-from-a-ntp-server)
[7. ESP32 Webserver](#7-esp32-webserver)
[8. ESP32 Webserver controlling LEDs](#8-espP32-webserver-controlling-leds)
[9. Asynchronous communication between the Webserver and a Client](#9-asynchronous-communication-between-the-webserver-and-a-client)
[10. Sending many parameters using XML and AJAX](#10-sending-many-parameters-using-xml-and-ajax)
[11. Input Elements and CSS - Range and Checkbox](#11-input-elements-and-css-range-and-checkbox)
[12. Forms](#12-forms)
[13. File system - LittleFS](#13-file-system-littlefs)
[14. Data Logging in a CSV file](#14-data-logging-in-a-csv-file)
[15. Streaming files in the Webserver](#15-streaming-files-in-the-webserver)
[16. Displaying an image in the Webserver](#16-displaying-an-image-in-the-webserver)
[17. Plotting DHT data from a CSV file using Plotly Javascript](#17-plotting-dht-data-from-a-csv-file-using-plotly-javascript)
[18. AJAX, XML and Files](#18-ajax-xml-and-files)
[19. Sending an Email with an Attachment](#19-sending-an-email-with-an-attachment)
[20. Using a public API - OpenWeatherMap.org](#20-using-a-public-api-openweathermaporg)
[21. Streaming data to a Plotly chart](#21-streaming-data-to-a-plotly-chart)
[22. PI Control](#22-pi-control)
[23. Multitasking with the ESP32 Dual Core](#23-multitasking-with-the-esp32-dual-core)
[24. Final touches - MathJax and CSS Buttons](#24-final-touches-mathjax-and-css-buttons)
[25. Making the ESP32 available from the Internet](#25-making-the-esp32-available-from-the-internet)



# 1. Learn how to use Arduino to monitor a photoresistor, modulate LED, and read the DHT11 sensor.

Before moving to the ESP32, it is good that you know how to work with the little hardware needed for this project, the following should be suitable online tutorials:
* How to read the analog response of the photoresistor https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInput.
* Hot to control a LED light intensity using Pulsed Width Modulation (PWM) https://www.arduino.cc/en/Tutorial/BuiltInExamples/Fading.
* Use the DHT temperature and humidity sensor using a suitable library, that would have be installed. Note that the DHT readings are digital, unlike those of the photoresistor,
and that they require 1-2 s, hence do not use short intervals. https://create.arduino.cc/projecthub/pibots555/how-to-connect-dht11-sensor-with-arduino-uno-f4d239.

# 2. Install the ESP32 into the Arduino IDE.

If it is the first time you use the ESP32, first you will have to decide how you will program it, as there are indeed quite a few ways to do it: from the official 
Espressif IoT Development Framework (ESP-IDF), to MicroPython, or using the Arduino IDE with is based on C++. In this project I used the latter, as it is the easyest
for Arduino users.
Find a suitable tutorial on how to install the ESP32 into the Arduino IDE, such as https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/.
When all is done, you should likely select the "ESP32 Dev Module" as you board.

# 3. Connect the ESP32 to your local Wifi network.

Despite you will communicate with the ESP32 using the USB serial, like in the Arduino, as it is the easiest way (but not the only! check Arduino OTA), the wireless
capabilities of the ESP32 should be used right away. Hence, connect to your Wifi network following examples in the IDE or e.g. https://www.megunolink.com/articles/wireless/how-do-i-connect-to-a-wireless-network-with-the-esp32/.
I believe it is a good idea to save your network credentials into a separate file, as explained in the previous tutorial, and thus this file should be included at the beginning, such as:
```cpp
#include "WifiEmailCredentials.h"
```
where this file should have:
```cpp
const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPass";
```
Later on, we will also use this file to save your email access credentials. Finally, note that you will also need to add a library to connect to the Wifi network:
```cpp
#include <WiFi.h>
```
You will need many more libraries along the way.

# 4. Read photoresistor analog signal.

Before going in depth with Wifi and so on, recreate the basic hardware setup you did with Arduino but now with the ESP32. As the signal of the photoresistor
is analogic, it will have to be further processed in one of the two ADC converters the ESP32 has, with 12 bit resolution. Note importantly, that the ADC2 of the
ESP32 cannot work if Wifi is connected. As Wifi is essential in this project, you must connect the photoresistor signal to a pin using the ADC1, likely pins 32-36
among a few others. If in doubt, check the pint layout of your Dev Module. In my case I chose pin 34 - which can only work as an input. Then, follow this tutorial https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/.

**5. Pulsed Width Modulation (PWM) of LEDs**
Likewise, learn how to control the light intensity using PWM, noting that any suitable output pin in the ESP32 can provide PWM (unlike Arduino), and realize that
pins >=34 only work as input pins, and thus cannot do PWM. In my case, I connected a red, green and blue LEDs in the pins 12, 14 and 27, respectively.
```cpp
int redpin = 12; // select the pin for the red LED
int bluepin = 27; // select the pin for the blue LED
int greenpin = 14 ;// select the pin for the green LED
```
Additional PWM parameters have to be defined in the ESP32, unlike with Arduino, as explained here https://randomnerdtutorials.com/esp32-pwm-arduino-ide/.

# 5. DHT sensor readings.

Like for Arduino, the key issue is to find a suitable library. I used [DHTesp](https://github.com/beegee-tokyo/DHTesp), as in this example https://circuits4you.com/2019/01/25/esp32-dht11-22-humidity-temperature-sensor-interfacing-example/,
 but you can find other examples
using different libraries, such as [here](https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/). 
Choose whatever works well for you. Connect the signal of the DHT to a suitable pin, in my case I chose 26.
```cpp
int dhtPin = 26;  //DHT sensor pin
```
Other relevant code involves first defining a dht object in the setup()
```cpp
dht.setup(dhtPin, DHTesp::DHT11);
Serial.println("DHT initiated");
```

and then simply performing the temperature and humidity readings by doing
```cpp
TempAndHumidity newValues = dht.getTempAndHumidity();
Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity));

```

# 6. Get time from a NTP server

In section 5, you should achieve readings from the DHT sensor every several seconds, to be displayed in the serial console.
Improve that by adding the time that such measurements were taken. To do that, a simple way is to connect to a
NTP server which will provide the ESP32 what is the current time, as explained here https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/.
As the ESP32 needs to connect to the internet, you should do step 3 first. First you shall define the following variables,
noting that one of them is to consider the different time zones.

```cpp
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*6;   //6h ahead for Nursultan time
const int   daylightOffset_sec = 0;
struct tm timeinfo;
```

and inside the setup()
```cpp
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
```
then, you can get the real time by calling this function:

```cpp
void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
//  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&timeinfo, "%D, %H:%M:%S");
}
```
Note the different ways to format time according to your needs using the [strftime](https://pubs.opengroup.org/onlinepubs/009695399/functions/strftime.html) standard method. 

# 7. ESP32 Webserver

By now you should be able to do the same things than in step 1 with the Arduino, but using the ESP32 instead, particularly using the serial console
to make sure all is working well. The idea would be to show such measurements and control some simple LEDs, not by using the
serial console, but by building up a WebServer directly in the ESP32. The simplest test is the example that you can find in the Arduino IDE:
File -> Examples -> Examples for ESP32 Dev Modules -> WebServer -> HelloServer.ino, or in [here](https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino).
It should run straight away after modifying the Wifi parameters (Step 3). First, as server object must be defined

```cpp
WebServer server(80);
```
The value 80 is the port we would like to select for our Webserver. 80 is the default for HTTP [wiki](https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers),
but you could choose any that doesnt interfere  with any other program. In order to be able to connect with your browser (e.g. Chrome) to the ESP32
you must need to know its IP address; the simplest way is by including the following code in setup() and checking the serial console.
```cpp
Serial.print("Connected to ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
```
In my case I would get something like this:  IP address: 192.168.1.103. If you copy & paste this IP address into a browser, you should see
the ESP32 Webserver in action. Obviously, your computer and the ESP32 must be connected to the same local network. If you have
selected you Webserver to be in port 8888, for example, then you would need to use in the browser: 192.168.1.103:8888.  
An important aspect of the Webserver is how to handle HTTP requests - as you must code what the ESP32 should do whenever something is
requested by a client, e.g. a browser. In the HelloServer example, the key line inside setup() is this:
```cpp
server.on("/", handleRoot);
```
Meaning that if 192.168.1.103/ is requested, then it will exectute the function *handleRoot*, which is defined as:

```cpp
void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}
```
In addition of switching on and off a LED, for fun, the key line is that in the middle, whereby the following information is sent: 200 means that
everything is good, a [success](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/200). "text/plain" is the type of information to be sent,
in this case plain text. Such descritption, called [MIME types](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types),
is very important because it tells the browser what the information being sent is: text, html, image, video, etc. The third argument of *server.send*
is the information to send, in this example "hello from esp32!". In the future, it will be a bit more complicated.   
Handling HTTP requests are a bit of a pain because everything that the Webserver (the ESP32) has to send to the client (e.g. browser) has
to be defined in such way: first by specifying what is the HTTP request using *server.on*, and then by defining a handling function of what to do,
likely finishing with a *server.send*(200, etc, etc). The other example given in the example HelloServer could be accessed by
calling in the browser 192.168.1.103/inline.

```cpp
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
```
Note that inside the loop(), where usually the main code is located, it is almost empty, it only has *server.handleClient()* for the
Webserver to do its job every time a client (browser) connects to it. Hence, the loop() is available to do whatever else you want.
```cpp
void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
```

# 8. ESP32 Webserver controlling LEDs

The previous example is a bit silly, so it is time to do something more interesting with the Webserver, such as controlling some LEDs.
This will be a core issue of this project, how the client (browser) and the server (ESP32) communicate in different directions. Follow this
tutorial to have a simple Webserver controlling LEDs https://randomnerdtutorials.com/esp32-web-server-arduino-ide/, however while easy to
run and it is useful to understand HTTP requests, its code is certainly cumbersome it was not used further on. 
A similar example with nicer code is shown here https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/.

These examples not used because if you want to switch an LED ON, for example, and LED in pin 26 - this requires an address like: 192.168.1.103/26/on.
First, it is ugly, as everybody can see in their browser whatever is going on, and second, this will only work with simple information, e.g. on/off.

These initial examples should be used to learn the basics of [HTML coding](https://www.w3schools.com/html/), and a little bit on [CSS styles](https://www.w3schools.com/css/default.asp).

# 9. Asynchronous communication between the Webserver and a Client

In order to be able to transfer information easily between the server and a client, without having the reload the web page, an asynchronous
system should be used. Many tutorials on the topic, e.g. [here](https://randomnerdtutorials.com/esp32-web-server-slider-pwm/) and 
[here](https://randomnerdtutorials.com/esp32-esp8266-web-server-physical-button/), use the library
[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer), which allows the ESP32 webserver to work asynchronously. Such library,
certainly allows to display the DHT data online, as discussed [here](https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/).
However, in another tutorial from the same team, doing a nice multiple sensors [weather station](https://randomnerdtutorials.com/build-an-all-in-one-esp32-weather-station-shield/), 
such library was not used, and instead, the data was refreshed in the web page using AJAX.

AJAX (Asynchronous JavaScript and XML) allows asynchronous communication from the client side, and thus there was no need to use the 
ESPAsyncWebServer library - even if it may have other nice advantages that I havent yet explored. This was a key decision of the project.
In addition, an [old project by David Bird](https://github.com/G6EJD/ESP32_Data_Logging_Webserver) convinced me that I could do all I wanted with the default webserver library of the ESP32.

Hence, the job is now to [learn how to use AJAX](https://www.w3schools.com/asp/asp_ajax_intro.asp) with the ESP32 to send information to a web page in a client. Briefly, AJAX uses a combination of:

* XMLHttpRequest object (to exchange data asynchronously with a server)
* JavaScript/DOM (to display/interact with the information)
* CSS (to style the data)
* XML (often used as the format for transferring data)

I recommend first to do this tutorial: https://circuits4you.com/2018/11/20/web-server-on-esp32-how-to-update-and-display-sensor-values/,
and then have a look at this one using the DHT sensor https://www.teachmemicro.com/sensor-display-on-esp32-web-server/. 
One thing you will notice is that the HTML code for the webpage(s) is now increasing substantially, as we are using JavaScript there. Hence,
it is not convenient to include the HTML code in the *.ino code, but it should be saved as an external file, e.g. *file.h*, with the following
format:

```html
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
... etc etc ....
</html>
)=====";
```
A substantial amount of time will be required to improve such client-side HTML code. Let us see an example from the above [code](https://circuits4you.com/2018/11/20/web-server-on-esp32-how-to-update-and-display-sensor-values/) how AJAX works:
In the webpage (client side), there is a [Span HTML tag](https://www.w3schools.com/tags/tag_span.asp) with *id="ADCValue"*. The value in this span
element, which initially is 0, will be updated using AJAX with the new data from the sensor.

```cpp
<h1>Sensor Value:<span id="ADCValue">0</span></h1><br>
```
The Javascript code is the following. The top code simply specifies that every 2 s (given in milliseconds), the function *getData()* should be called.
Such function makes an XMLHttpRequest (a key AJAX object), and will send *GET* request to the server, named *readADC*.
Once the server replies and sends a value, the code will take the reply (*this.responseText*) and it will use it in the Span
element with id="ADCValue" (*document.getElementById("ADCValue").innerHTML*).

```javascript
<script>
setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //2000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
</script>
```
This is the essence of how AJAX will interact with the ESP32 Webserver: 1) make a GET XMLHttpRequest of a specific name (readADC); 2) the Webserver
will check what to do when a HTTP request with such name (readADC) is received, in this case read an analog pin; 3) the webserver will send back some data;
and 4) the Javascript code will process the response, for example by displaying the new value inside an span element with a specific id.

The code at the server side is simple. First, define in the setup() what to do when a client request something like
192.168.1.103/readADC. The code below specifies that the ESP32 will run the handle function *handleADC()* if that happens.
```cpp
server.on("/readADC", handleADC);
```
This function simply reads the value of an analog pin (pin 0 in this case), the value is converted into a string, and it is sent back to
the client (browser), where it will be processed in the Javascript code above.
```cpp
void handleADC() {
 int a = analogRead(A0);
 String adcValue = String(a);
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
```

# 10. Sending many parameters using XML and AJAX

In the previous example only one value was sent in the XMLHttpRequest, but what if you need to send many? e.g. temperature, humidity, pressure, etc.
Obviously, you can create separate functions, but if all data can be sent together at once it will be much more efficient, as less requests would have
to be made. To send many values using AJAX, first they must be combined. There are two formats that can be used: [JSON](https://www.w3schools.com/js/js_json_intro.asp)
 and [XML](https://www.w3schools.com/xml/). An example of AJAX using JSON is found in https://circuits4you.com/2019/01/25/esp8266-dht11-humidity-temperature-data-logging/.
The JSON object *data* includes the information from the analog pin, the temperature and humidity.

```cpp
void handleADC() {
int a = analogRead(A0);
humidity = dht.getHumidity();
temperature = dht.getTemperature();
String data = "{\"ADC\":\""+String(a)+"\", \"Temperature\":\""+ String(temperature) +"\", \"Humidity\":\""+ String(humidity) +"\"}";
server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request
}
```
In this project, however, XML was used, for example in the function *handleParameters()* located in the file WebHandlers.h
```cpp
void handleParameters(){
  // Send Control parameters
  XML="<?xml version='1.0'?>";
  XML+="<inputs>";
  XML+="<Control_SetPoint>" + String(Control_SetPoint) + "</Control_SetPoint>";
  XML+="<Control_Kp>" + String(Control_Kp) + "</Control_Kp>";
  XML+="<Control_Ti>" + String(Control_Ti) +"</Control_Ti>";
  XML+="<MinPhotoResValue>" + String(MinPhotoResValue) +"</MinPhotoResValue>";
  XML+="<MaxPhotoResValue>" + String(MaxPhotoResValue) +"</MaxPhotoResValue>";
  XML+="<LedRed>" + LedRed + "</LedRed>";
  XML+="<LedGreen>" + LedGreen + "</LedGreen>";
  XML+="<LedBlue>" + LedBlue + "</LedBlue>";
  XML+="</inputs>";
  server.send(200,"text/xml",XML);
}
```
This function is called when

```cpp
server.on("/readParameters", handleParameters);
```
and the HTTP request "/readParameters" occurs inside the javascript function *getParameters()* in the file PI_Control_Project_1.h, where the
the HTML and Javascript code for the PI control website is located. Note that as the information sent is in XML format, it is used
*this.responseXML* instead. Plus, the Tag names used here, e.g. 'Control_SetPoint', must be the same as in the XML code above.

```javascript
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
```
All the information sent as XML by the server is used in different elements of the webpage, all of them identified by an unique
id, such as "sliderRange1", or "LedRED".

# 11. Input Elements and CSS - Range and Checkbox

In order to understand better the above code, you have to be familiar with the input elements [Range](https://www.w3schools.com/tags/att_input_type_range.asp)
and [Checkbox](https://www.w3schools.com/tags/att_input_type_checkbox.asp). An example of a range element is:

```html
<div>
<label for="myRange2">$K_P$ value (0.005-0.1): <b><span id="sliderRange2"></span></b></label></td><td>
<input type="range" min="0.005" max="0.1" step="0.005" id="myRange2" name="myRange2"  onchange="updateTextInput(this.value,'sliderRange2');">
</div>
```
Note this part: *onchange="updateTextInput(this.value,'sliderRange2')*. What this Javascript does is that when the slider is changed,
the new numeric value is shown in the label, which has an id of 'sliderRange2'. The code is quite simple:

```javascript
<script>
function updateTextInput(val,NumRange) {
	  document.getElementById(NumRange).innerHTML=val; }
</script>
```

All the labels, using a span, and the range elements have unique ids, e.g. "sliderRange2" and "myRange2" respectively, which were used
in the previous Javascript code. An example of input Checkboxes is the following for 3 LEDs, again each with a different id.

```html
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
```
Whereas the previous range element will be shown with a default look, these checkboxes are much more stylish. This is accomplished by using
CSS styles, in my case I used one from [here](https://www.w3schools.com/howto/howto_css_custom_checkbox.asp). These checkbox
are defined with a *class="container"*. Such class name is used to style the element using CSS notation. The CSS styles code is often
given [inside the <head></head> tag of an HTML document](https://www.w3schools.com/tags/tag_style.asp). The CSS code looks like this:

```css
/* Customize the label (the container) */
.container {
  display: block;
  position: relative;
  padding-left: 35px;
  margin-bottom: 12px;
  cursor: pointer;
  font-size: 22px;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

etc etc
```
If a nice, beautiful webside is desired, the style CSS section can be very long if you start copying styles from other sources.
For example, the CSS code from the Toggle Switch to activate the control (which is still a checkbox) comes from [here](https://www.w3schools.com/howto/howto_css_switch.asp).

```html
<label class="switch"> <input type="checkbox" name="ActivatePIControl"><span class="slider round"></span>
```

# 12. Forms

By now, you should be able to send DHT data and other information from the Webserver to a web page using AJAX. What about if we have
to send some input parameters from the web page to the server. The classic approach is using [forms](https://www.w3schools.com/html/html_forms.asp). 
In the file PI_Control_Project_1.h there is the following form:

```html
<form action="/formulari" method="post" id="my_form" class="form-inline">
... many inputs...
<button type="submit">Send information to ESP32 </button></td></tr></table>
</form>
```
When the type="submit" button is clicked, the values of all the form inputs (e.g. sliders, checkboxes) is sent (method="post")
to "/formulari". Normally, a new webpage would be requested, at "/formulari", when the button is clicked - but this is not what is
wanted here. In order to send the information to the server, yet stay in the same page, we can use the following [JQuery snippet](https://www.sanwebe.com/2016/07/ajax-form-submit-examples-using-jquery):

```javascript
<script>
$("#my_form").submit(function(event){
event.preventDefault(); //prevent default action 
var form_data = $(this).serialize(); //Encode form elements for submissions
var post_url = $(this).attr("action"); //get form action url
$.get( post_url, form_data);
});
```

The ESP32 Webserver will process this data as follows. First, the HTTP handler is defined, directing to the function *handleFormulari()*
```cpp
server.on("/formulari", handleFormulari);
```
Such function splits the different parameters sent using *server.arg* and the name of the input element where it came from, 
e.g. "myRange1", check the previous section.

```cpp
void handleFormulari(){
  Serial.println("Data recieved!");
  String Form1 = server.arg("myRange1");
  String Form2 = server.arg("myRange2");
  String Form3 = server.arg("myRange3");
  LedRed = server.arg("LedRED");
  LedGreen = server.arg("LedGREEN");
  LedBlue = server.arg("LedBLUE");
  etc...
```
Such data is sent as a String, and therefore sometimes it has to be converted into a number, e.g.
```cpp
Control_SetPoint = Form1.toFloat();
Control_Kp = Form2.toFloat();
Control_Ti = Form3.toFloat();
```
An additional problem came when dealing with checkboxes. If a checkbox is selected, it sends its assigned value ("on") with the form,
but if it is not selected, nothing is sent to the form. For this reason, the following code was added to keep track of all the 
relevant variables, if they are "on" or "off":

```cpp
//need to give off to the arguments that are not returned by checkbox
Control_Active = server.arg("ActivatePIControl");
if (Control_Active==""){
Control_Active = "off";
}
if (LedRed != "on"){
LedRed = "off";
}
if (LedGreen != "on"){
LedGreen = "off";
}
if (LedBlue != "on"){
LedBlue = "off";
}
```

# 13. File system - LittleFS

All right! now you should be able to send information both ways, from the Webserver to a client and vice versa, always making
use of the serial console to keep track. It is time to save some of that data, such at the DHT measurements, in a local file.
The ESP32 has 4 Mb of Flash memory, used to save the code you upload every time, but there is extra space for other files.
Before you can use such space to copy or write data, you must learn about the file systems in the ESP32. The most established one
is called SPIFFS, and there are many tutorials about it. There is a new and faster system called LittleFS with additional benefits,
yet there are not many examples out there. In many situations, LittleFS code is the same than for SPIFFS, and for this reason I 
decided to use LittleFS.

First, however, you need to add files and configure the Arduino IDE by following this tutorial https://www.mischianti.org/2021/04/01/esp32-integrated-littlefs-filesystem-5/.
It is likely you need to copy an extra file called mklittlefs.exe as explained in the tutorial.

The files to be sent to the ESP32 are saved locally inside your Arduino IDE project folder, inside a subfolder called *data*.
When it is all done, you will see a new menu under Tools, called ESP32 Sketch Data Upload, which you can use to format the file system
(first time is used) and to upload the files inside the folder *data*. Then, in order to use LittleFS with your code, you will
need to install the relevant library, as explained in the tutorial above.

For further information on how to use files in a ESP32, best to check more abundant tutorials using SPIFFS, [here](https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/) 
or [here](https://diyprojects.io/esp32-sketch-data-upload-arduino-ide-upload-spiffs-files-flash-memory/#.YNOXsGgzZPY). In my project,
I use two little functions using LittleFS (they would like very similar with SPIFFS) for basic file management; they are quite
self evident what they do:

```cpp
void listAllFiles() {
  // List all available files (if any) in the SPI Flash File System
  File root = LITTLEFS.open("/");
  File file = root.openNextFile();
  while(file) {
    Serial.print("FILE: ");
    Serial.println(file.name());
    Serial.print(" , number of Bytes: ");
    Serial.println(file.size());
    file = root.openNextFile();
  }
  root.close();
  file.close();  
}

void readFile(const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = LITTLEFS.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}
```
The funtion *listAllFiles()* is used in the setup(), just to show in the serial console the files saved in the Flash memory.

```cpp
//inside setup()
listAllFiles();   //show all files in flash memory

if (LITTLEFS.remove("/dataDHT.csv")){
Serial.println("old dataDHT.csv file deleted");
}

File datafile = LITTLEFS.open("/dataDHT.csv", FILE_APPEND);
datafile.println("AAPL_time,AAPL_Temp,AAPL_Hum");
datafile.close();
readFile("/dataDHT.csv");
```
Then, if there is an existing file called "dataDHT.csv" it will be removed (every time ESP32 resets, otherwise the data will
grow and grow...) using the commande *LITTLEFS.remove*. Next, a new empty file is created with the same name. Files can be open in two
main ways: to read and to append (write). How to write (append) something into a file is done using *datafile.println*, like the command
*Serial.println* you are using for debugging. The top line of the "dataDHT.csv" will be "AAPL_x,AAPL_Temp,AAPL_Hum" and it has to be
written before we save any DHT measurements. The reason is because this top row will act as headers and will be used when we plot
this file; to specify which column is what. In this case, the first column (AAPL_time) with be the measurement time, the second value (AAPL_Temp) will be
the temperature, and the third (AAPL_Hum) the humidity. We will have to follow this order when saving the data later on. The final code 
line (*readFile("/dataDHT.csv");*) is just there to verify in the serial console that the newly created file looks good.


# 14. Data Logging in a CSV file

Next step is to store the relevant data in a file; in my case I used a classic comma separated value (CSV) format. The actual format
is of little relevance, meanwhile it is good for the desired subsequent processing. In this project, such file will be processed
later on to make a nice plot, hence you shall make sure that your plotting app can use the format of the file. CSV seemed to me to be
a safe format to use. The code to store the measurement time, and the temperature and humidity values from the DHT is the following:

```cpp
//obtain DHT data measurments, and append them in the CSV file
void getDHTdata(){
  File datafile = LITTLEFS.open("/dataDHT.csv", FILE_APPEND);
  if (datafile == true) { // if the file is available, write to it
    TempAndHumidity newValues = dht.getTempAndHumidity();
    Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity));
    
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
      return;
    }
    datafile.print(&timeinfo, "%H:%M:%S");
    datafile.print(',');
    datafile.print(String(newValues.temperature));
    datafile.print(',');
    datafile.println(String(newValues.humidity));
    datafile.close();
  }
}
```
Simply call the *getDHTdata()* whenever you need to save new readings in the file. Note, however, that a minimum interval of 1-2 s should
be considered as measurements using a DHT are quite slow. To make sure that the data is appended correctly, use the previous
call **readFile("/dataDHT.csv"*.

# 15. Streaming files in the Webserver

In the previous section you have created a file (at "/dataDHT.csv"), saved in the ESP32 Flash memory - but how to access it in the
Webserver? If you try for example "192.168.1.103/dataDHT.csv" it will not work, it will say *Not found*. The reason is because
we must specify in the ESP32 setup() what to do whenever we recieve a HTTP request such as that one, e.g.:

```cpp
server.on("/dataDHT.csv", handledataDHT);
```
The function *handledataDHT()* inside WebHandlers.h will *stream* the data as binary data ("application/octet-stream"), inspired from this project
https://github.com/G6EJD/ESP32_Data_Logging_Webserver.

```cpp
void handledataDHT() {
  File datafile = LITTLEFS.open("/dataDHT.csv", FILE_READ); // Now read data from FS
  SizeDataDHT = datafile.size();
  SizeFlashFree = LITTLEFS.totalBytes() - LITTLEFS.usedBytes();
  Serial.print("SizeDataDHT ");
//  Serial.println(SizeDataDHT);
  
  if (datafile) {
    if (datafile.available()) { // If data is available and present
      String dataType = "application/octet-stream";
      if (server.streamFile(datafile, dataType) != datafile.size()) {Serial.print(F("Sent less data than expected!")); }
    }
  }
  datafile.close(); // close the file:
} 
```
After doing all this, you should be able now to access the data file when you type for example "192.168.1.103/dataDHT.csv" (obviously, using your IP address).
We will use the same function *server.streamFile* to send to the client (browser) other data, for example an image. As in this project
only one image was used, named "PIControlS.jpg", the code was: 1) the HTTP handler

```cpp
server.on("/img", handleImg);
```
and 2) the function *handleImg()* inside WebHandlers.h will stream the data like before, but this time using *dataType = "image/jpeg"*.
Note that the HTTP request ("/img") does not incorporate the name of the file we want to show, nor its expension (.jpg). It is not
needed, yet you could certainly have used instead *server.on("/PIControlS.jpg", handleImg);*.

```cpp
void handleImg() {
    File datafile = LITTLEFS.open("/PIControlS.jpg", FILE_READ); // Now read data from FS
  if (datafile) {
    if (datafile.available()) { // If data is available and present
      String dataType = "image/jpeg";
      if (server.streamFile(datafile, dataType) != datafile.size()) {
        Serial.print(F("Sent less data than expected!")); }
    }
  }
  datafile.close(); // close the file:
}
```
For a more general file server, where each individual file does not have to be specified like above, check http://myiot.co.uk/min_spiffs.html.
If you type now in your browser "192.168.1.103/img", you shall see the "PIControlS.jpg" image displayed.

# 16. Displaying an image in the Webserver

Once a file, such an image, is accessible from the file system as shown in the previous section, it can be easily used inside
your HTML files. In this project, the image "PIControlS.jpg" is displayed inside the two HTML pages we will create, which are stored
in DHT_Project_1.h and in PI_Control_Project_1.h. An image is added into an HTML file using an [img tag](https://www.w3schools.com/tags/tag_img.asp),
and in this case it is simply:

```html
<img src="/img">
```
If you have used the ESPAsyncWebServer library to make an asynchronous Webserver, check these tutorials on how display images 
[here](https://randomnerdtutorials.com/display-images-esp32-esp8266-web-server/) or 
[here](https://techtutorialsx.com/2018/08/24/esp32-http-web-server-serving-image-from-file-system/).

# 17. Plotting DHT data from a CSV file using Plotly Javascript

There are many [Javascript libraries](https://flatlogic.com/blog/best-19-javascript-charts-libraries/) that can be used for plotting data, 
but I decided to use [Plotly with Javascript](https://plotly.com/javascript/). In addition to find out how such library works regarding
styles and so on, the two key tutorials to check are on how to read a CSV with an AJAX call https://plotly.com/javascript/ajax-call/,
and how to stream data https://plotly.com/javascript/streaming/.

In DHT_Project_1.h you can find the code on how to display the DHT data. The following HTML element is where the plot will be
inserted in the web page, note its id="myDiv".

```cpp
<div id="myDiv"><!-- Plotly chart will be drawn inside this DIV --></div>
```
Then, the main Javascript code follows. *Plotly.d3.csv* loads our CSV data file, and in the function *processData(allRows)*
splits each data column - recall that it was saved in the first line of the file the headers AAPL_time, AAPL_Temp and AAPL_Hum.
The three vectors (x, Temp and Hum) are used to make the chart with the call * makePlotly( x, Temp, Hum )*. 
```javascript
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
```

The key code to format the Plotly char is inside *makePlotly()*. It is easy to understand what everything is doing, mostly about formating, including a secondart y axis.

```javascript

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
```
The final part of code needed it so specify how oftern to refresh the plot, using the function *setInterval*, 
in this case every 4 s (4000 ms).

```javascript
	setInterval(function(){
	makeplot();
	dataDHTSize();
	}, 4000);
```

# 18. AJAX, XML and Files

At this point you should quite familiar using AJAX to communicate between the Webserver and the client. This section is certainly
quite superficial, but it will cement how to do this properly. In the previous script, you should notice that there was a function
*dataDHTSize()* called every 4 s. This Javascript function, as shown below, recieves XML data from the server: 'SizeDataDHT' and
'SizeFlashFree'; which is then displayed in different parts of the text: e.g. look for *<span id="SizeDataDHT"></span>* in the
DHT_Project_1.h file, and another one with an id="SizeFlashFree".

```cpp
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
```
The HTTP request called "dataDHTSize" is defined in the ESP32 setup() as usual:

```cpp
 server.on("/dataDHTSize", handledataDHTSize);
```
Where the handle function simply packages the relevant information into a XML element
```cpp
void handledataDHTSize(){
  XML="<?xml version='1.0'?>";
  XML+="<inputs>";
  XML+="<SizeDataDHT>" + String(SizeDataDHT) + "</SizeDataDHT>";
  XML+="<SizeFlashFree>" + String(SizeFlashFree) + "</SizeFlashFree>";
  XML+="</inputs>";
  server.send(200,"text/XML",XML);
}
```
The global variables *SizeDataDHT* and *SizeFlashFree* are measured in the function *handledataDHT()* as shown in section 15. 
They provide the currect size of data file, as well as the free memory left, both in bytes.

# 19. Sending an Email with an Attachment

An interesting, bur arguably superfluous, feature of the DHT webpage is the ability to send the "dataDHT.csv" file by email.
Certainly, any real project should be able to send the recorded data somewhere for long term storage, analysis, etc. For example,
you could send your data to Google Sheets https://randomnerdtutorials.com/esp32-esp8266-publish-sensor-readings-to-google-sheets/.
As usual, there are different libraries to send emails with the ESP32 ([here](https://randomnerdtutorials.com/esp32-send-email-smtp-server-arduino-ide/)), 
however the only one I could manage to work with LittleFS
was the one from Renzo Mischianti called EMailSender  https://www.mischianti.org/2020/06/16/send-email-with-attachments-emailsender-v2-x-library-esp32-and-esp8266-part-2/ 
(barely updated a few months ago to achieve LittleFS compatibility). This library works very easily when the Smtp information is from 
Gmail.com; so much so that by default it will consider Gmail.com as the Smtp server to use. In this project, it only required
to send my Gmail account Email and password.

```cpp
EMailSender emailSend(EmailLogin , Emailpassword);
```
Both of them were stored for safety inside a separate file called WifiEmailCredentials.h:

```cpp
const char* EmailLogin = "***@***.***";
const char* Emailpassword = "***";
```

To make the webpage more interactive, the user has to fill up the name and email address to receive the DHT data file in a form, the HTML code is

```html
<form action="/formulariDHT" method="post" id="my_form">
    <label>Name</label>
    <input type="text" name="name" />
    <label>Email</label>
    <input type="email" name="email" />
    <input type="submit" name="submit" value="Send Data" />
</form>
<div id="EmailResponse"></div>
```

Once again, a JQuery snippet is used in order to send the information to the server, but to not change the page, like for "my_form" in
PI_Control_Project_1.h. However, unlike in that form, when we process this email form we will expect to receive a reply from the 
server - an it will be shown in between *<div id="EmailResponse"></div>*.

```javascript
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
```
This form action relates to the HTTP request "/formulariDHT", which is handled in the ESP32 setup() as:

```cpp
server.on("/formulariDHT", handleFormulariDHT);
```
The handler function *handleFormulariDHT()* recieves the Recepient Name and Email, and sends the email using the function
*sendEmailDHT()*. In that function it will be defined the variable *EmailSuccsess*, and based on its value the server will
send to the client a small sentence confirming if the email was sent correctly or not. Such sentence (HTML code) will be shown
inside *<div id="EmailResponse"></div>*. Unfortunatelly, there is almost no validation of the email address inserted.

```cpp
void handleFormulariDHT(){
  Serial.println("Data recieved!");
  RecipientName = server.arg("name");
  RecipientEmail = server.arg("email");
  Serial.println(RecipientName);
  Serial.println(RecipientEmail);
  EmailSuccsess=0;
  if (RecipientEmail != ""){
    sendEmailDHT();
  }
  else {
    Serial.println("Empy email adresss - cannot send");
  }
  if (EmailSuccsess==1){
    server.send(200, "text/html", "<font color=green>Email sent succesfully!</font>");
  }
  else {
    server.send(200, "text/html", "<font color=red>Email not sent</font>");
  }  
}
```

The workhorse function sending the Email is this one, where the Recipient name's is included in the HTML message.
Note how the "dataDHT.csv" file is attached. The variable *EmailSuccsess*, from *resp.status*, is what is used to check
if the email has been sent succesful or not.


```cpp
void sendEmailDHT(){
    EMailSender::EMailMessage message;
    message.subject = "DHT data from ESP32, by Ruben Mercade";
    message.message = "Greetings " + RecipientName + ", <br>Find attached the temperature and humidity data recorded in Ruben's house.<br>Obtained using an DHT11 sensor and a ESP32<br>Cheers<br>Ruben";
    EMailSender::FileDescriptior fileDescriptor[1];
    fileDescriptor[0].filename = F("dataDHT.csv");
    fileDescriptor[0].url = F("/dataDHT.csv");
    fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_LITTLE_FS;
    fileDescriptor[0].mime = MIME_TEXT_PLAIN;
    EMailSender::Attachments attachs = {1, fileDescriptor};
    EMailSender::Response resp = emailSend.send(RecipientEmail, message, attachs);
    Serial.println("Sending Email status: ");
    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);
    EmailSuccsess = resp.status;
}
```
# 20. Using a public API - OpenWeatherMap.org

The last part worth discussing of the Weather station webpage is how to use a public API, in this case from OpenWeatherMap.org.
There are several suitable tutorials to introduce API, how to register to that website, obtain the API Keys, and perform the
HTTP Get requests: [here](https://randomnerdtutorials.com/esp32-http-get-open-weather-map-thingspeak-arduino/),
[here](https://microcontrollerslab.com/http-get-esp32-arduino-openweathermap-thingspeak-examples/),
and [here](https://techtutorialsx.com/2018/03/17/esp32-arduino-getting-weather-data-from-api/). However, such tutorials make use of the
ESP32, which makes it more cumbersome, and I really thought to be unnecessary - for the simple reason that the server, the ESP32,
does not need to know the weather - only the client (browser) does. Hence, I decided to use directly Javascript
on the client side only. The tutorial I mostly followed is this one
https://www.bithacker.dev/fetch-weather-openweathermap-api-javascript. The relevant code is the following, using the same funny function name 
* weatherBalloon( )* than in the example. The variable *key* must include the API key that you should recieve after registering
to the website, at https://home.openweathermap.org/api_keys.

```javascript
  <script lang="text/javascript">
    // future javascript
function weatherBalloon( ) {
var key = '***********************';
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
```

The data fetched from the API is embedded into the following text, where different colors are used to highlight the
text that is automatically generated:

```html
... the weather now in <font color=red><span id="location"></span></font>
 is <font color=red><span id="description"></span></font>, with an outside temperature of <font color=red><span id="temp"></span></font> 
and a humidity of <font color=red><span id="Humidity"></span></font> ...
```
This section concludes all about the DHT Weather Station. The remaining section will focus on the PI control webpage.

# 21. Streaming data to a Plotly chart

Before focusing on how to do the PI control between the photoresistor and the 3 LEDs, it is convenient to continuously plot such data first.
As the data should be processed and displayed much faster than in the previous DHT webpage, it requires a different approach. 
Here, instead of loading a complete CSV file, only the newest data points will be sent and added to the existing graph - much faster.
The function *getDataXML()* is used to get new data from the ESP32 using again AJAX. The outcome of such function is a new
*updateData* variable, where the 5 new values to plot are formatted in the way required by Plotly. Notice that the XML values are strings, and
thus have to be changed to numbers using *parseFloat*.

```cpp
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
```

From the server side, the above HTTTP request "readData" is addressed with the function *handleData()*

```cpp
server.on("/readData", handleData);
```
which is another function to combine all the information into XML, before being sent back to the client.

```xml
void handleData() {
  XML="<?xml version='1.0'?>";
  XML+="<inputs>";
  XML+="<PhotoResVal>" + String(analogRead(PhotoResPin)) + "</PhotoResVal>";
  XML+="<Control_SetPoint>" + String(Control_SetPoint) + "</Control_SetPoint>";
  XML+="<LEDRedValue>" + String(LEDRedValue) + "</LEDRedValue>";
  XML+="<LEDGreenValue>" + String(LEDGreenValue) + "</LEDGreenValue>";
  XML+="<LEDBlueValue>" + String(LEDBlueValue) + "</LEDBlueValue>";
  XML+="</inputs>";
  server.send(200,"text/xml",XML);
}
```

Once you are able to receive all the new data, it is a matter to appending it to a Plotly chart. The following long Javascript is to 
create an empty chart with 5 data groups: the photoresistor value, the desired Set point, and the 3 LEds. Note the different
formatting involved, and realize that the plot will not have *x* data, for the simple reason that we are not doing time management in this
example.

```javascript	
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
	legend: {	"orientation": "h"	},
		
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

```

Once the plot is created, using *Plotly.newPlot*, it is easy to add new data using *Plotly.extendTraces*, the name of the chart,
which in this case is 'graph' (find where it is defined: *<div id="graph"></div>*), as well as the new *y* values given in 
*updateData*. The *if* loop at the end is to have a [moving axis](https://plotly.com/javascript/plotlyjs-function-reference/) if there are too many points.
Finally, the figure will be updated (refreshed) every 0.5 s, using *setInterval*.

```javascript
	var interval = setInterval(function() {
		getDataXML();
		Plotly.extendTraces('graph', updateData, [0, 1, 2, 3, 4])
		if(cnt > 200){				//easy way to limit the number of points shown in a plot, by adjusting x axis
			var xRange = [cnt-200, cnt];
			Plotly.relayout('graph',{'xaxis.range': xRange});
		}
	}, 500);
```

# 22. PI Control

If the previous section required plenty of coding on the client side (in the file PI_Control_Project_1.h which includes the HTML
for the PI control web page), the PI control is done completely on the ESP32. The first thing to do, if you havent done it already,
is to familiarize yourself with the PI control, the basics and the fundamentals - I suggest the following page from APmonitor.com
https://apmonitor.com/pdc/index.php/Main/ProportionalIntegralControl. In a typical control system, such as the [TCLab fom APmonitor](https://apmonitor.com/pdc/index.php/Main/ArduinoTemperatureControl)
based on a heat source and a temperature sensor, there is a substantial time delay between switching on the actuator (e.g. heater),
and achieving a steady state value of the process variable (e.g. the sensor temperature).
Such delays are very important in control. However, in the setup used here using a photoresistor and LEDs, such delay is very short,
due to the electronics itself - not because of the time for the light to travel. For this reason, in order to avoid an extremely fast control impossible to visualize in a chart,
and artificial time delay was implemented, termed *ControlMillisInterval*, and defined at 500 ms.

```cpp
currentMillis = millis();
// ------------ PI Control ------------------
if (currentMillis - PrevControlMillis > ControlMillisInterval){
```
Next, there is an if statement to check if the control should be active or no, based on the toggle switch checkbox discussed in section 11.
Remember that the value of the variable *Control_Active* is sent from the client using AJAX.

```cpp
PhotoResValue = analogRead(PhotoResPin);
if (Control_Active =="on"){
```

The following is the actual PI control itself using the two constants Kp and Ti. Note that for time interval it is used the same
variable *ControlMillisInterval* used previously.

```cpp
        error = Control_SetPoint - PhotoResValue;
        //Control_Ti should be in seconds, hence the 1000
        Interror = Interror + Control_Kp/Control_Ti*error*ControlMillisInterval/1000;
        float P = Control_Kp*error;
        float I = Interror;
        int opTemp = 0 + P + I;
```
Perform [anit-reset windup](https://www.controlglobal.com/blogs/controltalkblog/how-do-you-effectively-use-anti-reset-windup-options-tips/),
basically limiting the range of your actuator - the LED intensity in this case. As the resolution of the PWM LED was selected to
be 8 bit (check tutorials in section 5), then the maximum value possible for the LED is 255.

```cpp
        //Implement anit-reset windup
        if (opTemp < 0){
          LEDValue = 0;
          Interror = Interror - Control_Kp/Control_Ti*error*ControlMillisInterval/1000;
        }
        else if (opTemp>255){
          LEDValue = 255;
          Interror = Interror - Control_Kp/Control_Ti*error*ControlMillisInterval/1000;
        }
        else {
          LEDValue = opTemp;
        }
```

The new control value is copied to *LEDValue*, which has be sent to the LEDs. As in this project you could use any combination
of red, green and blue LEDs, the following *if* statements are needed to lighten up each LED correctly.

```cpp
if (LedRed=="on"){
  LEDRedValue = LEDValue;
  ledcWrite(ledChannel1, LEDValue);   //put the op value to the LED
}
else {
  LEDRedValue = 0;
  ledcWrite(ledChannel1, 0);      //no light
}
if (LedBlue=="on"){
  LEDBlueValue = LEDValue;
  ledcWrite(ledChannel2, LEDValue);   //put the op value to the LED
}
else {
  LEDBlueValue = 0;
  ledcWrite(ledChannel2, 0);      //no light
}
if (LedGreen=="on"){
  LEDGreenValue = LEDValue;
  ledcWrite(ledChannel3, LEDValue);   //put the op value to the LED
}
else {
  LEDGreenValue = 0;
  ledcWrite(ledChannel3, 0);      //no light
}
```

The final part of the PI control code is what to do if the actuator is not switch on (follwing *if (Control_Active =="on")*).
In this *else* statement, all the relevant LED variables are turned to 0, as well as the LED light intensity.

```cpp
else {    //if acutator is off, switch off LEDs
	LEDValue = 0;
	LEDRedValue = 0;
	LEDBlueValue = 0;
	LEDGreenValue = 0;
	ledcWrite(ledChannel1, LEDValue);   //put the op value to the LED
	ledcWrite(ledChannel2, LEDValue);   //put the op value to the LED
	ledcWrite(ledChannel3, LEDValue);   //put the op value to the LED
	error = 0;      //reset all errors
	Interror = 0;
  }
  PhotoResValuePrev = PhotoResValue;
  PrevControlMillis = currentMillis;
}
```

# 23. Multitasking with the ESP32 Dual Core

One of the very interesting things of the ESP32 is that it has two cores, hence you could split the workload for speed.
In this project, however, there is nothing computationally intensive that needs parallel computing. Nevertheless, there are two
very clear functionalities: a) the above PI control and DHT data logging; and b) everything to do with the Webserver.
The way this project is designed, the clients (browsers with webpages open) will send many requests to the server to update
data and the figures. If there are too many requests, certainly the server can be slowed down. Hence, by separating web applications
from the 'serious' ones of control and logging - by using different cores, we should minimize (in principle) any disruption to 
control and sensor measurement and recording. Follow this tutorial and add all the 'serious' code into the Core 0, which normally it
is not used https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/. The Core 1 will take care everything in the setup() and 
loop() - yet in the loop there is only handling web clients as intended.

```cpp
void loop() {
// In the loop using Core 1, only take care of the webserver
  server.handleClient(); delay(1);
}
```

# 24. Final touches - MathJax and CSS Buttons

Once the web pages works as intended, it is a matter formatting them to look as good as desired, for instance by making sure that
they are dispayed correctly in a computer and in a mobile phone by using this simple code:

```html
<meta name="viewport" content="width=device-width, initial-scale=0.8">
```

In addition, I wanted to add the PI control equations in the web page, and I wanted them to look nice. For this, I used another Javascript
library called [MathJax](https://www.mathjax.org/). Whereas there are other ways to incorporate equations into a web page, like
[MathML](https://www.w3.org/Math/), I found it extremely cumbersome and tedious, unlike of the much more elegant TeX language that
MathJax uses. In order to show math equations inside a normal paragraph with other text, you should add the following code at the
 [web page head](https://docs.mathjax.org/en/latest/web/configuration.html):

```javascript
<script>
MathJax.Hub.Config({
tex2jax: {inlineMath: [['$','$'], ['\\(','\\)']]}
});
</script>
```
This code is used to specify that MathJax should format into equations whatever is between $ $. But if you want an equation to stand alone by itself,
then write the TeX code between two $$ $$. For example, the first PI control equation in TeX format is:

$$Led(t)=Led_{bias}+ K_P(P_{SP} - P_{PV})+ \frac{K_P}{\tau _I} \int_{0}^{t}{(P_{SP} - P_{PV})dt} $$

If you run this line using MathJax it will look much better. As usual when using Javascript libraries, do not forget to reference
them inside the webpage head, as follows:

```cpp
<script src='https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.7/MathJax.js?config=TeX-MML-AM_CHTML'></script>
```

A final beautifying touch for the website was to add cool CSS buttons between the two webpages of this project, between the PI Control
and the Weather station. After long searching, I decided to use the following one created by Liam at https://codepen.io/liamj/pen/yrEXNx,
with some modifications to fit my taste.

```cpp
<a href="/DHT" class="corner-button"><span>Weather Station</span></a>
<a href="/" class="corner-button"><span>PI Controller</span></a>
```

The CSS code of the class="corner-button" is all saved in the syle tag at the webpage head, such as

```css
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

etc etc
```

# 25. Making the ESP32 available from the Internet

You should have noticed that you can only access your ESP32 webserver from inside your network, e.g. 192.168.1.***. 
Unless you do it the proper way and get a dedicated public IP address, you will need to find a way to redirect a public URL to your local ESP32 IP address.
It should be possible to use Dynamic DNS (for example [here](https://www.instructables.com/ESP8266-Automation-With-Web-Interface-and-DDNS/)
or [here](https://www.open-electronics.org/arduino-ddns-dynamic-dns/), usin a typical provider like noip.com, but I did not succeeded with my local network...
A second option is to use ngrok, which requires to install a little program in you computer, to register at ngrok.com and get an Authtoken, and then execute
the installed program mentioning the local IP of the ESP32 as well as the given Authtoken. There is a full tutorial here
https://microcontrollerslab.com/accessing-esp32-web-server-anywhere-world-esp8266/. You will get a public URL like http://0.tcp.ngrok.io:11161/ which can
be accessed from anywhere. ngrok is
ok for testing your project throught the internet, not for long term use, as you will need your computer switch on with ngrok running in order for the
redirection to work.


