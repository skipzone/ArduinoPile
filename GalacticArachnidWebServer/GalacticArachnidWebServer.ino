#include <ESP8266WiFi.h>
#include <Wire.h>

const uint8_t i2cAddress = 42;

const String apPassword = "gertiespider";
const String apSsid = "GalacticArachnid";
const String staSsid = "";    // set to empty string to disable station mode
const String staPasskey = "";
//const String staSsid = "trees";
//const String staPasskey = "2083433061";
//const String staSsid = "XanaduShop";
//const String staPasskey = "xanadushop";

const int LED_PIN = 5;      // Thing's onboard, blue LED
const int ANALOG_PIN = A0;  // the only analog pin on the Thing
const int EXTERNAL_CONTROL_TX_EN_PIN = 4;
const int SDA_PIN = 14;
const int SCL_PIN = 2;

const float vBatFullScale = (1023.0 * 12.42) / 690.0;   // input to voltage divider when A0 is 1023


struct Preset {
  String name;
  int8_t legs;
  int8_t body;
  int8_t eyes;
};

//                      name       legs body eyes
Preset presets[] = { 
  {"Power Up"            ,    1,    1,    1},
  {"Lasers"              ,    2,    2,    2},
  {"Rainbow"             ,    3,    3,    3},
  {"Soul Sucker"         ,    4,    4,    4},
  {"Random Inv. Pong"    ,    5,    5,    5},
  {"Bloodstream"         ,    6,    6,    6},
  {"Zip and Hold"        ,    7,    7,    7},
  {"MultiWave 1"         ,    8,    8,    8},
  {"Split Rotation"      ,    9,    9,    9},
  {"MultiWave 2"         ,   10,   10,   10},
  {"All Off"              ,   0,    0,    0}
};

String legPatterns[] = {"Off", "Fill", "Lasers", "Rainbow", "Soul Sucker", "RandInvPong", "Bloodstream", "Zip and Hold", "Multiwave 1", "SplitRot", "Multiwave 2"};

String bodyPatterns[] = {"Off", "MultiWave 1", "Lasers", "Rainbow", "MWOrange", "Rainbow 90s", "Bloodstream", "Rainbow 9s", "MultiWave 1", "MultiWave 1", "FstSolidRnbw", "Solid Greens"};

String eyePatterns[] = {"Off", "Aqua", "Fast Spin", "Slow Pong", "Medium Spin", "Slow Spin", "Red", "Slow Spin", "Medium Spin", "Fast Pong", "Slow Spin"};

const uint8_t numPresets = sizeof(presets) / sizeof(Preset);
const uint8_t numLegPatterns = sizeof(legPatterns) / sizeof(String);
const uint8_t numBodyPatterns = sizeof(bodyPatterns) / sizeof(String);
const uint8_t numEyePatterns = sizeof(eyePatterns) / sizeof(String);

uint8_t selectedLegPattern = 255;
uint8_t selectedBodyPattern = 255;
uint8_t selectedEyePattern = 255;

bool sendSelections = false;
bool sendLegSelection = false;
bool sendBodySelection = false;
bool sendEyeSelection = false;

WiFiServer server(80);

String apIpAddress;
String staIpAddress;


void checkWiFiStaConnected()
{
  if (staSsid != "") {
    if (staIpAddress == "" && WiFi.status() == WL_CONNECTED)
    {
      ipAddressToString(WiFi.localIP(), staIpAddress);
      Serial.println("WiFi connected");  
      Serial.println("IP address: ");
      Serial.println(staIpAddress);
    }
    else if (staIpAddress != "" && WiFi.status() != WL_CONNECTED)
    {
      staIpAddress = "";
      Serial.println("WiFi disconnected");
    }
  }
}


void ipAddressToString(IPAddress ipAddr, String& ipStr)
{
    char buf[28];
    sprintf(buf, "%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    ipStr = buf;
}


String getRequestValue(const String& req, const String& key)
{
  String val;

  int p = req.indexOf(key + "=");
  if (p != -1) {
    p += key.length() + 1;
    int p1 = req.indexOf("&", p);
    if (p1 == -1) {
      p1 = req.indexOf(" ", p);
    }
    if (p1 > p) {
      val = req.substring(p, p1);
    }
  }

  return val;
}


void handleClientRequest(WiFiClient& client)
{
  // Read the first line of the request
  String req = client.readStringUntil('\r');

  Serial.println(req);
  client.flush();

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  s += "<h1>Galactic Arachnid</h1>";
  s += "<h3>" + staSsid + ": " + staIpAddress + "  " + apSsid + ": " + apIpAddress + "</h3>";

  float a0 = analogRead(ANALOG_PIN);
  float vBat = vBatFullScale * a0 / 1024.0;
  s += "<h3>Battery: " + String(vBat, 1) + " V<h3>";

  // GET http://arduino/?cmd1=functionA&cmd2=functionB HTTP/1.1

  int8_t selectedPreset = -1;
  bool autoMode = false;

  if (req.indexOf("GET") != -1) {
    String val;
    val = getRequestValue(req, "preset");
    if (val.length() != 0) {
      selectedPreset = val.toInt();
    }
    val = getRequestValue(req, "legs");
    if (val.length() != 0) {
      selectedLegPattern = val.toInt();
      sendLegSelection = true;
    }
    val = getRequestValue(req, "body");
    if (val.length() != 0) {
      selectedBodyPattern = val.toInt();
      sendBodySelection = true;
    }
    val = getRequestValue(req, "eyes");
    if (val.length() != 0) {
      selectedEyePattern = val.toInt();
      sendEyeSelection = true;
    }
    val = getRequestValue(req, "auto");
    if (val.length() != 0) {
      autoMode = true;
    }
  }

  for (uint8_t i = 0; i < numPresets; ++i) {
    String buttonBackgroundColor = i == selectedPreset ? "background-color:blue;" : "";
    s += "<p> <a href=\"?preset=" + String(i)
      + "&legs=" + String(presets[i].legs) + 
      + "&body=" + String(presets[i].body) + 
      + "&eyes=" + String(presets[i].eyes) + 
      + "\"><button style=\"" + buttonBackgroundColor + "width:600px;height:60px;font: bold 36px Arial"
      + "\">" + presets[i].name + "</button></a>";
  }

  client.print(s);
  delay(1);
  s = "";

  s += "<table style=\"width:100%\">";
  s += "<tr><td><h1>Legs</h1></td><td><h1>Body</h1></td><td><h1>Eyes</h1></td></tr>";
  bool buttonAdded = true;
  for (uint8_t i = 0; buttonAdded && i < 128; ++i) {
    buttonAdded = false;
    s += "<tr>";

    s += "<td>";
    if (i < numLegPatterns) {
      buttonAdded = true;
      String buttonBackgroundColor = i == selectedLegPattern ? "background-color:green;" : "";
      s += "<p> <a href=\"?legs=" + String(i)
        + "\"><button style=\"" + buttonBackgroundColor + "width:300px;height:60px;font: bold 36px Arial"
        + "\">" + legPatterns[i] + "</button></a>";
    }
    s += "</td>";
    
    s += "<td>";
    if (i < numBodyPatterns) {
      buttonAdded = true;
      String buttonBackgroundColor = i == selectedBodyPattern ? "background-color:green;" : "";
      s += "<p> <a href=\"?body=" + String(i)
        + "\"><button style=\"" + buttonBackgroundColor + "width:300px;height:60px;font: bold 36px Arial"
        + "\">" + bodyPatterns[i] + "</button></a>";
    }
    s += "</td>";
  
    s += "<td>";
    if (i < numEyePatterns) {
      buttonAdded = true;
      String buttonBackgroundColor = i == selectedEyePattern ? "background-color:green;" : "";
      s += "<p> <a href=\"?eyes=" + String(i)
        + "\"><button style=\"" + buttonBackgroundColor + "width:300px;height:60px;font: bold 36px Arial"
        + "\">" + eyePatterns[i] + "</button></a>";
    }
    s += "</td>";

    s += "</tr>";

    client.print(s);
    delay(1);
    s = "";
  }
  s += "</table>";

  client.print(s);
  delay(1);
  s = "";

  s += String("<br><br><p> <a href=\"?auto=1&legs=255&body=255&eyes=255") 
    + "\"><button style=\"background-color:yellow;width:300px;height:60px;font: bold 36px Arial\">Automatic Mode</button></a>";

  s += String("<br><br><p> <a href=\"?doNothing") 
    + "\"><button style=\"width:300px;height:60px;font: bold 36px Arial\">Refresh</button></a>";

  s += "</html>\n";

  client.print(s);
  delay(1);

  Serial.println("Client disonnected");
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed

  sendSelections = true;
}


void sendSelectionsViaI2C()
{
  Wire.beginTransmission(42); // transmit to device #42
  Wire.write(sendLegSelection ? selectedLegPattern : 254);
  Wire.write(sendBodySelection ? selectedBodyPattern : 254);
  Wire.write(sendEyeSelection ? selectedEyePattern : 254);
  Wire.endTransmission();    // stop transmitting

  sendLegSelection = false;
  sendBodySelection = false;
  sendEyeSelection = false;
}


//void handleI2CRequestEvent()
//{
//  Serial.println("got I2C request);
//  Wire.write(selectedLegPattern);
//  Wire.write(selectedBodyPattern);
//  Wire.write(selectedEyePattern);
//}


void initHardware()
{
  Serial.begin(115200);
  pinMode(EXTERNAL_CONTROL_TX_EN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.println("hardware init done");
  // Don't need to set ANALOG_PIN as input because that's all it can be.
}


void setupI2C()
{
//  Wire.begin(i2cAddress);
//  Wire.onRequest(handleI2CRequestEvent);
  Wire.begin(SDA_PIN, SCL_PIN);
}


void setupWiFi()
{
  if (staSsid != "") {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apSsid.c_str(), apPassword.c_str());
    WiFi.begin(staSsid.c_str(), staPasskey.c_str());
    ipAddressToString(WiFi.softAPIP(), apIpAddress);
  }
  else
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid.c_str(), apPassword.c_str());
    ipAddressToString(WiFi.softAPIP(), apIpAddress);
  }
}


void setup() 
{
  initHardware();
  setupI2C();
  setupWiFi();
  server.begin();
}


void loop() 
{
  checkWiFiStaConnected();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    handleClientRequest(client);
  }

  digitalWrite(LED_PIN, selectedLegPattern == 1 ? 0 : 1);

  if (sendSelections && digitalRead(EXTERNAL_CONTROL_TX_EN_PIN)) {
    sendSelections = false;
    sendSelectionsViaI2C();
  }

}


