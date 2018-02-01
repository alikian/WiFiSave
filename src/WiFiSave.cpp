#include "WiFiSave.h"



WiFiSave::WiFiSave(char *WLAN_SSID_IN, char *WLAN_PASSWD_IN){
   this->WLAN_SSID=WLAN_SSID_IN;
   this->WLAN_PASSWD=WLAN_PASSWD_IN;
}

boolean WiFiSave::readRTC(){
   // Try to read WiFi settings from RTC memory
   rtcValid = false;
   if( ESP.rtcUserMemoryRead( 0, (uint32_t*)&rtcData, sizeof( rtcData ) ) ) {
      // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
      uint32_t crc = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
      if( crc == rtcData.crc32 ) {
         rtcValid = true;
      }
   }
   Serial.print("rtcValid: ");
   Serial.println(rtcValid?"Valid":"Invalid");

   return rtcValid;
}

uint32_t WiFiSave::calculateCRC32( const uint8_t *data, size_t length ) {
   uint32_t crc = 0xffffffff;
   while( length-- ) {
      uint8_t c = *data++;
      for( uint32_t i = 0x80; i > 0; i >>= 1 ) {
         bool bit = crc & 0x80000000;
         if( c & i ) {
            bit = !bit;
         }

         crc <<= 1;
         if( bit ) {
            crc ^= 0x04c11db7;
         }
      }
   }

   return crc;
}

void WiFiSave::beginConnect(){

   readRTC();

   if( rtcValid ) {
      Serial.println("Connecting using staticIP");
      WiFi.begin(WLAN_SSID, WLAN_PASSWD);
      WiFi.config(rtcData.staticIP, rtcData.gatewayIP, rtcData.subnetMask,rtcData.dnsIP0,rtcData.dnsIP1);
   }
   else {
      // The RTC data was not valid, so make a regular connection
      Serial.println("Connecting using dynamic ip");
      WiFi.begin(WLAN_SSID, WLAN_PASSWD);
   }
   WiFi.setAutoConnect(false);

   unsigned long start = millis();
   if(  WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.print("Connected Succesfully in: ");
      Serial.println(millis()-start);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Gateway IP: ");
      Serial.println(WiFi.gatewayIP());
      Serial.print("Subnet: ");
      Serial.println(WiFi.subnetMask());
      Serial.print("DNS1: ");
      Serial.println(WiFi.dnsIP(0));
      Serial.print("DNS2: ");
      Serial.println(WiFi.dnsIP(1));

      if(! rtcValid ) {
         writeRTC();
      }
   }else{
      Serial.println("Failed to Connect");

   }

}

void WiFiSave::writeRTC(){
   // Write current connection info back to RTC
   Serial.println("wrting ip IPAddress");
   rtcData.staticIP = WiFi.localIP();
   rtcData.gatewayIP = WiFi.gatewayIP();
   rtcData.subnetMask = WiFi.subnetMask();
   rtcData.dnsIP0 = WiFi.dnsIP(0);
   rtcData.dnsIP1 = WiFi.dnsIP(1);
   rtcData.crc32 = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
   ESP.rtcUserMemoryWrite( 0, (uint32_t*)&rtcData, sizeof( rtcData ) );
}
