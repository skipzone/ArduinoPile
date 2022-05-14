/*******************************************************************
 *                                                                 *
 * Addressable LED Pixel Pattern Framework                         *
 *                                                                 *
 * ESP8266 Web Page Provider                                       *
 *                                                                 *
 * by Ross Butler   Oct. 2016                                      *
 *                                                                 *
 *******************************************************************/

#pragma once

#ifdef ESP8266

#include <Arduino.h>
#include <vector>


class IPAddress;
class WiFiServer;
class WiFiClient;


namespace pixelPattern {

class PatternSequence;


class Esp8266WebPage {

public:

    struct Preset {
        String name;
        std::vector<unsigned int> patternNums;
    };

    Esp8266WebPage();

    virtual ~Esp8266WebPage();

    Esp8266WebPage(const Esp8266WebPage&) = delete;
    Esp8266WebPage& operator =(const Esp8266WebPage&) = delete;

    void addPatternSequence(PatternSequence* patternSequence);
    void addPreset(const Preset& preset);
    void addPreset(const String& name, const std::vector<unsigned int>& patternNums);
    void addPreset(const std::vector<Preset>& newPresets);
    void clearPresets();

    bool doWiFi();

    void init();

    String ipAddressToString(const IPAddress& ipAddr);
//    IPAddress stringToIpAddress(const String& s);

    void setAp(const String& ssid, const String& password);
    void setHostname(const String& name) { hostname = name; }
    void setSta(const String& ssid, const String& passkey);
    void setStaticIpAddress(const String& ip,
                            const String& gateway,
                            const String& subnet,
                            const String& dns1 = "0.0.0.0",
                            const String& dns2 = "0.0.0.0");
    void setStatusText(const String& text) { statusText = text; }
    void setTitleText(const String& text) { titleText = text; }

protected:

private:

    void checkWiFiStaConnected();
    String getRequestValue(const String& req, const String& key);
    bool handleClientRequest(WiFiClient& client);

    String apIpAddress;
    String apPassword;
    String apSsid;
    String dns1Address;
    String dns2Address;
    bool enableSoftAp;
    bool enableStationMode;
    String gatewayAddress;
    String hostname;
    String staticIpAddress;
    std::vector<PatternSequence*> patternSequences;
    std::vector<Preset> presets;
    String staIpAddress;
    String staSsid;
    String staPasskey;
    String statusText;
    String titleText;
    WiFiServer* wiFiServer;
    String subnetMask;
    bool usingStaticIpAddress;
};

}

#endif  // #ifdef ESP8266

