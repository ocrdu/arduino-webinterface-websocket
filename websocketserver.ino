#include <WiFiNINA.h>
#include <WebSocketServer.h>
#include <Arduino_LSM6DS3.h>
#include <Base64.h>
#include <RTCZero.h>
#include "wifi_secrets.h"

//#define DEBUG 1

#ifdef DEBUG
  #define Sprint(a) (Serial.print(a))
  #define Sprintln(a) (Serial.println(a))
#else
  #define Sprint(a)
  #define Sprintln(a)
#endif

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
const int IPLastByte = 99;
const int webPort = 80;
const int socketPort = 8080;
WiFiServer webServer(webPort);
WiFiServer socketServer(socketPort);
WebSocketServer webSocketServer;
WiFiClient socketClient;
RTCZero rtc;
const int ledPin = 12;
boolean on = false;
float volume = 0;
int cowbell = 1000;
float zdata[] = {1,1,1,1,1};
int zdatasize = sizeof(zdata) / sizeof(zdata[0]);
float tdata[] = {20,20,20,20,20,20,20,20,20,20};
int tdatasize = sizeof(tdata) / sizeof(tdata[0]);
int ledState = 0;
unsigned long epoch;
unsigned long previousCowbellMillis = 0;
unsigned long previousGravityMillis = 0;
unsigned long previousTemperatureMillis = 0;
unsigned long previousClockMillis = 0;
unsigned long previousAdjustMillis = 0;
char interface_gz_base64[] = "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";

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  rtc.begin();
  IMU.begin();
  WiFiConnect();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    Sprintln("\n--Lost WiFi connection");
    WiFi.end();
    WiFiConnect();
  }

  WiFiClient webClient = webServer.available();
  if (webClient.connected()) {
    Sprint("\n--New client: "); Sprint(webClient.remoteIP()); Sprint(":"); Sprintln(webClient.remotePort());
    String header = "";
    while (webClient.available()) {
      char c = webClient.read();
      if (c != '\r') {
        header += c;
      }
      if (header.substring(0,1) != "G" && header.substring(0,2) != "GE" && header.substring(0,3) != "GET") {
        Sprintln("--Wrong method in header");
        webClient.println("HTTP/1.1 405 Method Not Allowed\nAllow: GET\nContent-Type: text/plain; charset=utf-8\n\n405 Method not allowed; GET only\n");
        webClient.stop();
        Sprintln("--Client disconnected");
      }
      if (header.length() > 512) {
        Sprintln("--Header too long");
        webClient.println("HTTP/1.1 431 Request Header Fields Too Large\nContent-Type: text/plain; charset=utf-8\n\n431 Request header fields too large\n");
        webClient.stop();
        Sprintln("--Client disconnected");
      }
      if (header.substring(header.length() - 2) == "\n\n") {
        Sprint(header.substring(0, header.length() - 1));
        if (header.indexOf("GET / HTTP") > -1) {
          sendBase64Page(interface_gz_base64, webClient, 1024);
          Sprintln("--Interface webpage sent");
        } else if (header.indexOf("GET /author HTTP") > -1) {
          webClient.println("HTTP/1.1 200 OK\nContent-Type: text/plain; charset=utf-8\n\nOscar den Uijl, oscar@den-uijl.nl\n");
          Sprintln("--Email address sent");
        } else if (header.indexOf("GET /coffee HTTP") > -1) {
          webClient.println("HTTP/1.1 418 I'm a teapot\nContent-Type: text/plain; charset=utf-8\n\n418 I'm a teapot\n");
          Sprintln("--Coffee request denied");
        } else {
          webClient.println("HTTP/1.1 404 Not Found\nContent-Type: text/plain; charset=utf-8\n\n404 Not Found\n");
          Sprintln("--Page not found");
        }
        webClient.stop();
        Sprintln("--Client disconnected");
      }
    }
  }

  if (!socketClient.connected()) {
    socketClient = socketServer.available();
    if (socketClient.connected() && webSocketServer.handshake(socketClient)) {
      Sprint("\n--Websocket connected to: "); Sprint(socketClient.remoteIP()); Sprint(":"); Sprintln(socketClient.remotePort());
      if (on) {
        webSocketServer.sendData("sw:true");
      } else {
        webSocketServer.sendData("sw:false");
      }
      webSocketServer.sendData("volume:" + (String)round((volume*100)/255));
      webSocketServer.sendData("cowbell:" + (String)((1000-cowbell)/10));
      Sprintln("\n--Settings sent");
    } else {
      Sprintln("\n--Couldn't connect websocket");
      socketClient.stop();
      delay(100);
    }
  }

  if (socketClient.connected()) {
    String data = webSocketServer.getData();
    if (data.length() > 0) {
      String name = data.substring(0, data.indexOf(":"));
      String value = data.substring(data.indexOf(":") + 1);
      boolean goodSettings = true;
      if (name == "switch") {
        on = (value == "true");
      } else if (name == "volume") {
        volume = (value.toFloat()*255)/100;
        if (cowbell == 1000) {
          analogWrite(ledPin, (int)round(volume));
        }
      } else if (name == "cowbell") {
        cowbell = 1000 - (value.toInt()*10);
      } else {
        goodSettings = false;
        webSocketServer.sendData("message:Bad data; ignored");
      }
      if (goodSettings) {
        webSocketServer.sendData("message:" + name + " set to " + value);
      }
    }
  }

  unsigned long currentCowbellMillis = millis();
  if ((currentCowbellMillis - previousCowbellMillis) > cowbell) {
    previousCowbellMillis = currentCowbellMillis;
    if (on) {
      if (cowbell == 1000) {
        analogWrite(ledPin, (int) round(volume));
      } else {
        if (ledState == 0) {
          ledState = 1;
          analogWrite(ledPin, (int) round(volume));
        } else {
          ledState = 0;
          analogWrite(ledPin, 0);
        }
      }
    } else {
      analogWrite(ledPin, 0);
    }
  }

  float t, tsum = 0;
  if (IMU.temperatureAvailable()) {
    IMU.readTemperature(t);
    for (int i = 0; i < tdatasize - 1; i++) {
      tdata[i] = tdata[i+1];
    }
    tdata[tdatasize-1] = t;
    for (int i = 0; i < tdatasize; i++) {
      tsum += tdata[i];
    }
    if (socketClient.connected()) {
      unsigned long currentTemperatureMillis = millis();
      if ((currentTemperatureMillis - previousTemperatureMillis) > 5000) {
        previousTemperatureMillis = currentTemperatureMillis;
        webSocketServer.sendData("temperature:" + String(tsum/tdatasize, 3));
      }
    }
  }
  
  float x, y, z, zsum = 0;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    if (x > 0.2 || x < -0.2 || y > 0.2 || y < -0.2 || z > 1.2 || z < 0.8) {
      analogWrite(ledPin, 0);
      Sprintln("\n--TILT!!");
      if (on) {
        on = false;
        if (socketClient.connected()) {
          webSocketServer.sendData("sw:false");
        }
      }
    }
    for (int i = 0; i < zdatasize - 1; i++) {
      zdata[i] = zdata[i+1];
    }
    zdata[zdatasize-1] = z;
    for (int i = 0; i < zdatasize; i++) {
      zsum += zdata[i];
    }
    if (socketClient.connected()) {
      unsigned long currentGravityMillis = millis();
      if ((currentGravityMillis - previousGravityMillis) > 2000) {
        previousGravityMillis = currentGravityMillis;
        webSocketServer.sendData("gravity:" + String(zsum/zdatasize, 3));
      }
    }
  }

  unsigned long currentClockMillis = millis();
  if (socketClient.connected()) {
    if ((currentClockMillis - previousClockMillis) > 1000) {
      previousClockMillis = currentClockMillis;
      webSocketServer.sendData("time:" + String(rtc.getEpoch()));
    }
  }

  unsigned long currentAdjustMillis = millis();
  if ((currentAdjustMillis - previousAdjustMillis) > 300000) {
    previousAdjustMillis = currentAdjustMillis;
    epoch = WiFi.getTime();
    if (epoch > 0) {
      rtc.setEpoch(epoch);
      Sprintln("\n--RTC adjusted");
    };
  }

}

void WiFiConnect() {
  WiFi.setHostname("nano-webserver");
  while (WiFi.status() != WL_CONNECTED) {
    Sprintln("Connecting to " + (String)ssid + " ...");
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  IPAddress IP = WiFi.localIP();
  IP[3] = IPLastByte;
  WiFi.config(IP, WiFi.gatewayIP(), WiFi.gatewayIP(), WiFi.subnetMask());
  Sprintln("Connected to " + (String)ssid);
  int tries = 5;
  do {
    Sprintln("Getting NTP time ...");
    epoch = WiFi.getTime();
    tries--;
    delay(500);
  } while (epoch == 0 && tries > 0);
  if (tries == 0) {
    Sprintln("Failed to get NTP time");
  } else {
    rtc.setEpoch(epoch);
    Sprintln("RTC set to NTP time");
  }
  webServer.begin();
  socketServer.begin();
  #ifdef DEBUG
  printWifiStatus();
  #endif
  WiFi.lowPowerMode();
  digitalWrite(LED_BUILTIN, HIGH);
}

void sendBase64Page(char base64Page[], WiFiClient client, int packetSize) {
  const int base64Page_length = strlen(base64Page);
  const int page_length = base64_dec_len(base64Page, base64Page_length);
  char page[page_length];
  int done = 0;
  base64_decode(page, base64Page, base64Page_length);
  client.println("HTTP/1.1 200 OK\nContent-Type: text/html");
  if (page[0] == 0x1f && page[1] == 0x8b) {
    client.println("Content-Encoding: gzip");
  }
  client.println("");
  while (page_length > done) {
    if (page_length - done < packetSize) {
      packetSize = page_length - done;
    }
    client.write(page + done, packetSize * sizeof(char));
    done = done + packetSize;
  }
}

#ifdef DEBUG
void printWifiStatus() {
  Sprint("SSID: "); Sprintln(WiFi.SSID());
  Sprint("Signal strength (RSSI): "); Sprint(WiFi.RSSI()); Sprintln(" dBm");
  Sprint("IP address: "); Sprintln(WiFi.localIP());
  Sprint("Gateway: "); Sprintln(WiFi.gatewayIP());
  Sprint("Netmask: "); Sprintln(WiFi.subnetMask());
  Sprint("Websocket is at http://"); Sprint(WiFi.localIP()); Sprintln(":" + (String)socketPort + "/");
}
#endif
