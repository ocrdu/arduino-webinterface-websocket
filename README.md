## An example implementation of a web interface for an Arduino Nano 33 IoT using a websocket

This web interface communicates with the Arduino using a websocket on port 8080.

The Arduino serves as both a web server and a websocket server.

### Libraries used:

- https://github.com/ocrdu/WiFiNINA for WiFi connection and servers (**Don't** use https://github.com/arduino-libraries/WiFiNINA 1.4.0 for this sketch)
- https://github.com/ocrdu/NINA-Websocket as the websocket library (Minor change from https://github.com/morrissinger/ESP8266-Websocket);
- https://github.com/ocrdu/Arduino_LSM6DS3_T as the LSM6DS3 (IMU) library (Adds code for LSM6DS3 internal temperature sensor to https://github.com/arduino-libraries/Arduino_LSM6DS3);
- https://github.com/adamvr/arduino-base64 for base64 decoding;
- https://github.com/arduino-libraries/RTCZero as the Real Time Clock library.

### Notes:

There is no documentation yet, but the least you need to do to get it to work is this:

- Edit the wifi_secrets.h to use your own WiFi SSID and password.

This example will show the interface in your web browser when you point it at the Arduino's IP address  http://x.x.x.99/ , where x.x.x are the first three bytes of the IP addresses of your network. If x.x.x.99 is already taken, you need to change the code.

Almost forgot: if you want to see something happen, put a LED on pin 12 😎.

If you feel like donating for this, you can do so here: http://ocrdu.nl/donations .