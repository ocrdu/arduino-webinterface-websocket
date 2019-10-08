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
