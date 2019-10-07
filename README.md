## An example implementation of a web interface for an Arduino Nano 33 IoT using a websocket

This web interface communicates with the Arduino using a websocket on port 8080.

### Libraries used:

https://github.com/arduino-libraries/WiFiNINA for WiFi connection and servers;
https://github.com/ocrdu/NINA-Websocket as the websocket library;
https://github.com/ocrdu/Arduino_LSM6DS3_T as the LSM6DS3 (IMU) library;
https://github.com/adamvr/arduino-base64 for base64 decoding;
https://github.com/arduino-libraries/RTCZero as the RTC library.

### Notes:

There is no documentation yet, but the least you need to do to get it to work is this:

- Edit the wifi_secrets.h to use your own WiFi SSID and password.

This example will show the interface in your web browser when you point it at the Arduino's IP address  http://x.x.x.99/ , where x.x.x are the first three bytes of the IP addresses of your network. If x.x.x.99 is already taken, you need to change the code.

Almost forgot: if you want to see something happen, put a LED on pin 12 8-).

If you feel like donating for this, you can do so here: ocrdu.nl/donations.