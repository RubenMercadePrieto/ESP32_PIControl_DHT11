
//-------------- HTTP Handlers ------------------------
// what the Webserver should do when recieving HTTP requests 
void handleRoot()
{
 server.send(200, "text/html", webpageCode);
}
void handleDHTPage()
{
 server.send(200, "text/html", webpageCodeDHT);
}

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

void handleFormulari(){
  Serial.println("Data recieved!");
  String Form1 = server.arg("myRange1");
  String Form2 = server.arg("myRange2");
  String Form3 = server.arg("myRange3");
  LedRed = server.arg("LedRED");
  LedGreen = server.arg("LedGREEN");
  LedBlue = server.arg("LedBLUE");
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
  //change to float numbers
  Control_SetPoint = Form1.toFloat();
  Control_Kp = Form2.toFloat();
  Control_Ti = Form3.toFloat();

  Serial.println("Real varaibles");
  Serial.println(Control_SetPoint);
  Serial.println(Control_Kp);
  Serial.println(Control_Ti);
  Serial.print("Control Active : ");
  Serial.println(Control_Active);
  Serial.print("LED Red : ");
  Serial.println(LedRed);
  Serial.print("LED Green : ");
  Serial.println(LedGreen);
  Serial.print("LED Blue : ");
  Serial.println(LedBlue);
}


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
void handledataDHTSize(){
  XML="<?xml version='1.0'?>";
  XML+="<inputs>";
  XML+="<SizeDataDHT>" + String(SizeDataDHT) + "</SizeDataDHT>";
  XML+="<SizeFlashFree>" + String(SizeFlashFree) + "</SizeFlashFree>";
  XML+="</inputs>";
  server.send(200,"text/XML",XML);
}
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


