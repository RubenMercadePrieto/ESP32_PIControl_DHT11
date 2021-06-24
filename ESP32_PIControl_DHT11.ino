#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "FS.h"
#include <LITTLEFS.h>
#include <time.h>  
#include "DHTesp.h"
#include <EMailSender.h>

//Include files with HTML pages
#include "PI_Control_Project_1.h" //Webpage for the PI Control
#include "DHT_Project_1.h"        //Webpage for the DHT Weather Station

#include "WifiEmailCredentials.h"   //File with saved Wifi and Email credentials

#define FORMAT_LITTLEFS_IF_FAILED false


//---------------------------------------------------
WebServer server(8888);           //choose whatever Port you like for the Webserver, by default 80

EMailSender emailSend(EmailLogin , Emailpassword);
//EMailSender emailSend("ruben.mercade@nu.edu.kz", "NUrupiales123");

//Pin variables
const int PhotoResPin = 34;   //with WIFI, ADC2 cannot be used, must use ADC1 with pins 32-39, 39
int redpin = 12; // select the pin for the red LED
int bluepin = 27; // select the pin for the blue LED
int greenpin = 14 ;// select the pin for the green LED
int dhtPin = 26;  //DHT sensor pin
DHTesp dht;

// Using both cores for different tasks
TaskHandle_t Task1;
//TaskHandle_t Task2; //this task is acutally not used, the mail loop will be used for core 1

//-------------- Time server ------------------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*6;   //6h ahead for Nursultan time
const int   daylightOffset_sec = 0;
struct tm timeinfo;

// variables for storing different stuff
int MinPhotoResValue;
int MaxPhotoResValue;
int PhotoResValue = 0;
int PhotoResValuePrev = 0;

String XML;
int LEDValue;
String RecipientEmail = "";
String RecipientName = "";
int EmailSuccsess = 0;
// setting PWM properties
const int freq = 5000;
const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;
const int resolution = 8;
//Setting default PI control parameters
float Control_SetPoint = 2500;
float Control_Kp = 0.04;
float Control_Ti = 1;
String Control_Active ="off";
String LedRed = "on";
String LedGreen = "off";
String LedBlue = "off";
int LEDRedValue = 0;
int LEDGreenValue = 0;
int LEDBlueValue = 0;
float error = 0;
float Interror = 0;
long currentMillis = 0;
long PrevControlMillis = 0;
int ControlMillisInterval = 500;
int DHTMillisInterval = 2000;
long PrevDHTMillis = 0;
long SizeDataDHT;
long SizeFlashFree;

#include "WebHandlers.h"    //File with many handler functions that shouldnt have to be modifed

void listAllFiles() {
  // List all available files (if any) in the SPI Flash File System
  File root = LITTLEFS.open("/");
  File file = root.openNextFile();
  while(file) {
    Serial.print("FILE: ");
    Serial.println(file.name());
    Serial.print(" , number of Bytes: ");
    Serial.println(file.size());
    file = root.openNextFile();  }
  root.close();
  file.close();  
}

void readFile(const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = LITTLEFS.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;    }
    Serial.println("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
//  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&timeinfo, "%D, %H:%M:%S");
}

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


void setup() {
  Serial.begin(115200);
  delay(100);

    //-------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while(WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(500); Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //-------------- Web handlers -----------------------
  server.on("/", handleRoot);
  server.on("/DHT", handleDHTPage);
  server.on("/formulari", handleFormulari);
  server.on("/formulariDHT", handleFormulariDHT);
  server.on("/readData", handleData);
  server.on("/readParameters", handleParameters);
  server.on("/img", handleImg);
  server.on("/dataDHT.csv", handledataDHT);
  server.on("/dataDHTSize", handledataDHTSize);
  
  server.begin();
  Serial.println("HTTP server started");

  if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed");
        return;  }
  else { Serial.println("LITTLEFS Mounted correctly");  }
    
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());

  //Setup LED PWM
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  ledcSetup(ledChannel3, freq, resolution);
  ledcAttachPin(redpin, ledChannel1);
  ledcAttachPin(bluepin, ledChannel2);
  ledcAttachPin(greenpin, ledChannel3);
  
  //Calculate minimum value of the photoresistor  
  delay(10);
  ledcWrite(ledChannel1, 0);
  ledcWrite(ledChannel2, 0);
  ledcWrite(ledChannel3, 0);
  delay(1000);
  int TempV = 0;
  for(int i = 0; i <= 20; i++){
    TempV += analogRead(PhotoResPin);
    delay(5);
  }
  MinPhotoResValue = TempV/20;
  //Calculate maximum value of the photoresistor with all LED on  
  ledcWrite(ledChannel1, 255);
  ledcWrite(ledChannel2, 255);
  ledcWrite(ledChannel3, 255);
  delay(500);
  TempV = 0;
  for(int i = 0; i <= 20; i++){
    TempV += analogRead(PhotoResPin);
    delay(5);
  }
  MaxPhotoResValue = TempV/20;
  
  ledcWrite(ledChannel1, 0);
  ledcWrite(ledChannel2, 0);
  ledcWrite(ledChannel3, 0);
  
  Serial.print("MaxPhotoResValue: ");
  Serial.println(MaxPhotoResValue);
  Serial.print("MinPhotoResValue: ");
  Serial.println(MinPhotoResValue);

 //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //Start dealing with storage files
  listAllFiles();   //show all files in flash memory

  if (LITTLEFS.remove("/dataDHT.csv")){
  Serial.println("old dataDHT.csv file deleted");
  }
  
  File datafile = LITTLEFS.open("/dataDHT.csv", FILE_APPEND);
  datafile.println("AAPL_time,AAPL_Temp,AAPL_Hum");
  datafile.close();
  readFile("/dataDHT.csv");
}

//Stuff to do in Core 0, that which is normally not used
void Task1code( void * pvParameters ){
  Serial.print("Task1 doing PI control running on core ");
  Serial.println(xPortGetCoreID());
  PhotoResValue = analogRead(PhotoResPin);
  Serial.println(PhotoResValue);
  
  for(;;){
    currentMillis = millis();
    
//    Serial.println(PhotoResValue);
    // ------------------------------------------
    // ------------ PI Control ------------------
    if (currentMillis - PrevControlMillis > ControlMillisInterval){
      PhotoResValue = analogRead(PhotoResPin);
      if (Control_Active =="on"){
//        Serial.print("Control is Active, photoresistance reading is: ");
//        Serial.println(PhotoResValue);
        error = Control_SetPoint - PhotoResValue;
        //Control_Ti should be in seconds, hence the 1000
        Interror = Interror + Control_Kp/Control_Ti*error*ControlMillisInterval/1000;
        float P = Control_Kp*error;
        float I = Interror;
        int opTemp = 0 + P + I;
//        Serial.print("opTemp is: ");
//        Serial.println(opTemp);
        //Implement anti-reset windup
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
//        Serial.print("LEDValue is: ");
//        Serial.println(LEDValue);
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
        
        
      }
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
//      Serial.print("PI should do something");
      PrevControlMillis = currentMillis;
    }
    // ------------------------------------------------------------
    // ------------ DHT measurements & recording  -----------------
    if (currentMillis - PrevDHTMillis > DHTMillisInterval) {
        getDHTdata();
//        readFile("/dataDHT.csv");
        PrevDHTMillis = currentMillis;
    }
    
    //Artificicial delay in the control loop - to simulate doing something serious
    delay(50);
    } 
}

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


void loop() {
// In the loop using Core 1, only take care of the webserver
  server.handleClient(); delay(1);
}
