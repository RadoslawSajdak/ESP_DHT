#include "DHT.h"
#include <WiFi.h>
#include "passwords.h"

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const char *SSID = "T-mobile_5G_tests";
const char *WiFiPassword = WIFI_PASSWORD;

WiFiServer Server(1313);


int resetCounter = 0;

void wifiConnect(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
      resetCounter ++;
      if(resetCounter > 5)
      {
        ESP.restart();
      }
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  wifiConnect();
  Server.begin();
  
}


float h,t;
void loop() {
  if(WiFi.isConnected())
  {
    WiFiClient RemoteClient = Server.available();
    if (RemoteClient)
    {
      Serial.println("I got client!");
      uint8_t ReceiveBuffer[30];
      while (RemoteClient.connected())
      { 
        int received = RemoteClient.read(ReceiveBuffer, sizeof(ReceiveBuffer));
        if(received > 0){
            h = dht.readHumidity();
            t = dht.readTemperature();
            
            String toSend = "Temp: " + String(t, 2) + " Hum: " + String(h, 2);
            RemoteClient.write(toSend.c_str(), toSend.length());
            
        }
      }
    }
  }else{
    WiFi.reconnect();
  }
  sleep(1);
}