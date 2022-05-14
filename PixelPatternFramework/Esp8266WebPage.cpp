/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * ESP8266 Web Page Provider                                       *
 *                                                                 *
 * by Ross Butler   Oct. 2016                                      *
 *                                                                 *
 *******************************************************************/

#ifdef ESP8266

#include <Arduino.h>
#include "Esp8266WebPage.h"
#include <ESP8266WiFi.h>
#include "ExternalControlSelector.h"
#include "PatternSequence.h"

using namespace pixelPattern;


Esp8266WebPage::Esp8266WebPage()
{
  wiFiServer = new WiFiServer(80);
}


Esp8266WebPage::~Esp8266WebPage()
{
  delete wiFiServer;
}


void Esp8266WebPage::addPatternSequence(PatternSequence* patternSequence)
{
    patternSequences.emplace_back(patternSequence);
}


void Esp8266WebPage::addPreset(const Preset& preset)
{
    presets.emplace_back(preset);
}


void Esp8266WebPage::addPreset(const String& name, const std::vector<unsigned int>& patternNums)
{
    Preset preset({name, patternNums});
    presets.emplace_back(preset);
}


void Esp8266WebPage::addPreset(const std::vector<Preset>& newPresets)
{
    presets.insert(presets.end(), newPresets.begin(), newPresets.end());
}


void Esp8266WebPage::clearPresets()
{
    presets.clear();
}


bool Esp8266WebPage::doWiFi()
{
  bool patternSelectionChanged = false;

  checkWiFiStaConnected();

  // Check if a client has connected
  WiFiClient client = wiFiServer->available();
  if (client) {
    patternSelectionChanged = handleClientRequest(client);
  }

  return patternSelectionChanged;
}


bool Esp8266WebPage::handleClientRequest(WiFiClient& client)
{
  bool patternSelectionChanged = false;

  String req = client.readStringUntil('\r');
  Serial.println(req);

  client.flush();
  // TODO:  need to do this second flush?  client.flush();

  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";

  s += "<h1>" + titleText + "</h1>";

  s += "<h3>";
  if (enableStationMode) {
    s += staSsid + ": " + staIpAddress + " --- ";
  }
  if (enableSoftAp) {
    s += apSsid + ": " + apIpAddress;
  }
  s += "</h3>";

  s += statusText;

  int8_t selectedPresetIdx = -1;
  bool autoMode = false;

  if (req.indexOf("GET") != -1) {
    String val;
    val = getRequestValue(req, "preset");
    if (val.length() != 0) {
      selectedPresetIdx = val.toInt();
    }
    for (unsigned int sectionIdx = 0; sectionIdx < patternSequences.size(); ++sectionIdx) {
      String sectionLabel = String("section") + String(sectionIdx);
      val = getRequestValue(req, sectionLabel);
      if (val.length() != 0) {
        Serial.print("Request:  sectionIdx=");
        Serial.print(sectionIdx);
        Serial.print(" val=");
        Serial.println(val);
        patternSequences[sectionIdx]->patternSelector->setPatternNum(val.toInt());
        patternSelectionChanged = true;
      }
    }
    val = getRequestValue(req, "auto");
    if (val.length() != 0) {
      autoMode = true;
    }
  }

  for (uint8_t presetIdx = 0; presetIdx < presets.size(); ++presetIdx) {
    String buttonBackgroundColor = presetIdx == selectedPresetIdx ? "background-color:blue;" : "";
    s += "<p> <a href=\"?preset=" + String(presetIdx);
    for (unsigned int sectionIdx = 0;
         sectionIdx < patternSequences.size() && sectionIdx < presets[presetIdx].patternNums.size();
         ++sectionIdx)
    {
      String sectionLabel = String("section") + String(sectionIdx);
      s += String("&") + sectionLabel + "=" + String(presets[presetIdx].patternNums[sectionIdx]);
    }
    s += "\"><button style=\"" + buttonBackgroundColor + "width:600px;height:60px;font: bold 36px Arial"
      + "\">" + presets[presetIdx].name + "</button></a>";
  }

  client.print(s);
  delay(1);
  s = "";

  s += "<table style=\"width:100%\">";

  s += "<tr>";
  for (unsigned int sectionIdx = 0; sectionIdx < patternSequences.size(); ++sectionIdx) {
    String sectionName = String("Section ") + String(sectionIdx);
    s += String("<td><h1>") + sectionName + String("</h1></td>");
  }
  s += "</tr>";

  // TODO:  fix problem of empty names creating gaps when there are multiple sections
  int patternIdx = 0;
  bool reachedEndOfAllPatterns = false;
  while (!reachedEndOfAllPatterns) {
    reachedEndOfAllPatterns = true;
    s += "<tr>";
    for (unsigned int sectionIdx = 0; sectionIdx < patternSequences.size(); ++sectionIdx) {
      s += "<td>";
      if (patternIdx < patternSequences[sectionIdx]->numPatterns) {
        reachedEndOfAllPatterns = false;
        String patternName;
        patternSequences[sectionIdx]->readPatternDefinitionFromFlash(patternIdx, nullptr, nullptr, nullptr, &patternName);
        if (0 != patternName.length()) {
          String sectionLabel = String("section") + String(sectionIdx);
          String buttonBackgroundColor =
              patternIdx == patternSequences[sectionIdx]->patternSelector->getPatternNum() ? "background-color:green;" : "";
          s += "<p> <a href=\"?" + sectionLabel + "=" + String(patternIdx)
            + "\"><button style=\"" + buttonBackgroundColor + "width:400px;height:60px;font: 28px Arial"
            + "\">" + patternName + "</button></a>";
        }
      }
      s += "</td>";
    }
    s += "</tr>";

    client.print(s);
    delay(1);
    s = "";
    
    ++patternIdx;
  }
  
  s += "</table>";

  client.print(s);
  delay(1);
  s = "";

  s += String("<br><br><p> <a href=\"?auto=1");
  for (unsigned int sectionIdx = 0; sectionIdx < patternSequences.size(); ++sectionIdx) {
    String sectionLabel = String("section") + String(sectionIdx);
    s += String("&") + sectionLabel + String("=255");
  }
  s += "\"><button style=\"background-color:yellow;width:300px;height:60px;font: bold 36px Arial\">Automatic Mode</button></a>";

  s += String("<br><br><p> <a href=\"?doNothing") 
    + "\"><button style=\"width:300px;height:60px;font: bold 36px Arial\">Refresh</button></a>";

  s += "</html>\n";

  client.print(s);
  delay(1);

  Serial.println("Client disonnected");
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed

  return patternSelectionChanged;
}


void Esp8266WebPage::init()
{

Serial.print("staticIpAddress=");
Serial.println(staticIpAddress);
Serial.print("gatewayAddress=");
Serial.println(gatewayAddress);
Serial.print("subnetMask=");
Serial.println(subnetMask);
Serial.print("dns1Address=");
Serial.println(dns1Address);
Serial.print("dns2Address=");
Serial.println(dns2Address);

    usingStaticIpAddress = false;
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
    if (enableStationMode) {
        if (   ip.fromString(staticIpAddress)
            && gateway.fromString(gatewayAddress)
            && subnet.fromString(subnetMask)
            && dns1.fromString(dns1Address)
            && dns2.fromString(dns2Address))
        {
            usingStaticIpAddress = true;
            Serial.print("Using static IP address ");
            Serial.println(staticIpAddress);
        }
        else {
            Serial.println("Using DHCP");
        }
    }

    if (enableSoftAp && enableStationMode) {
        Serial.println("AP+station mode");
        WiFi.mode(WIFI_AP_STA);
        WiFi.disconnect();
        while (WiFi.isConnected()) {
            Serial.println("waiting for disconnect");
        }
        WiFi.hostname(hostname.c_str());
        WiFi.softAP(apSsid.c_str(), apPassword.c_str());
        WiFi.begin(staSsid.c_str(), staPasskey.c_str());
        if (usingStaticIpAddress) {
            bool result = WiFi.config(ip, gateway, subnet);   //, dns1, dns2);
            Serial.print("WiFi.config result = ");
            Serial.println(result);
        }
        WiFi.setAutoReconnect(true);
        //WiFi.reconnect();
        apIpAddress = ipAddressToString(WiFi.softAPIP());
        Serial.print("ssid=");
        Serial.print(apSsid);
        Serial.print("  pwd=");
        Serial.print(apPassword);
        Serial.print("  ap ip address: ");
        Serial.println(apIpAddress);
    }
    else if (enableSoftAp) {
        Serial.println("AP mode");
        WiFi.mode(WIFI_AP);
        WiFi.hostname(hostname.c_str());
        WiFi.softAP(apSsid.c_str(), apPassword.c_str());
        apIpAddress = ipAddressToString(WiFi.softAPIP());
        Serial.print("ssid=");
        Serial.print(apSsid);
        Serial.print("  pwd=");
        Serial.print(apPassword);
        Serial.print("  ap ip address: ");
        Serial.println(apIpAddress);
    }
    else if (enableStationMode) {
        Serial.println("station mode");
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        while (WiFi.isConnected()) {
            Serial.println("waiting for disconnect");
        }
        WiFi.hostname(hostname.c_str());
        WiFi.begin(staSsid.c_str(), staPasskey.c_str());
        if (usingStaticIpAddress) {
            bool result = WiFi.config(ip, gateway, subnet);   //, dns1, dns2);
            Serial.print("WiFi.config result = ");
            Serial.println(result);
        }
        WiFi.setAutoReconnect(true);
        //WiFi.reconnect();
    }

    wiFiServer->begin();
}


void Esp8266WebPage::setAp(const String& ssid, const String& password)
{
    apSsid = ssid;
    apPassword = password;
    enableSoftAp = ssid.length() > 0;
}


void Esp8266WebPage::setSta(const String& ssid, const String& passkey)
{
    staSsid = ssid;
    staPasskey = passkey;
    enableStationMode = staSsid.length() > 0;
}


void Esp8266WebPage::setStaticIpAddress(
    const String& ip,
    const String& gateway,
    const String& subnet,
    const String& dns1,
    const String& dns2)
{
    staticIpAddress = ip;
    gatewayAddress = gateway;
    subnetMask = subnet;
    dns1Address = dns1;
    dns2Address = dns2;
}


void Esp8266WebPage::checkWiFiStaConnected()
{
  if (enableStationMode) {
    if (staIpAddress == "" && WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi connected");
      staIpAddress = ipAddressToString(WiFi.localIP());
      Serial.print("IP address: ");
      Serial.println(staIpAddress);
    }
    else if (staIpAddress != "" && WiFi.status() != WL_CONNECTED)
    {
      staIpAddress = "";
      Serial.println("WiFi disconnected");
    }
  }
}


String Esp8266WebPage::ipAddressToString(const IPAddress& ipAddr)
{
    return String(ipAddr[0]) + "." + String(ipAddr[1]) + "." + String(ipAddr[2]) + "." + String(ipAddr[3]);
}


//IPAddress Esp8266WebPage::stringToIpAddress(const String& s)
//{
//    int a[4];
//    if (sscanf(s, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]) == 4) {
//        return IPAddress(a[0], a[1], a[2], a[3]);
//    }
//    return IPAddress(0, 0, 0, 0);
//}


String Esp8266WebPage::getRequestValue(const String& req, const String& key)
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


#endif  // #ifdef ESP8266

