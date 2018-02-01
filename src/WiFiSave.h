

#include <ESP8266WiFi.h>

class WiFiSave {

  private:
     // The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
    // so the RTC data structure should be padded to a 4-byte multiple.
    struct {
      uint32_t crc32;   // 4 bytes
      IPAddress staticIP;  // 1 byte,   5 in total
      IPAddress gatewayIP; // 6 bytes, 11 in total
      IPAddress subnetMask;  // 1 byte,  12 in total
      IPAddress dnsIP0;
      IPAddress dnsIP1;
    } rtcData;

      bool rtcValid = false;

      const char *WLAN_SSID;
      const char *WLAN_PASSWD;

 public:
    WiFiSave(char *WLAN_SSID_IN, char *WLAN_PASSWD_IN);
    void writeRTC();
    boolean readRTC();
    void beginConnect();
    uint32_t calculateCRC32( const uint8_t *data, size_t length ) ;

};
