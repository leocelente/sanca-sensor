# SancaSensor
Publish BMP280 pressure & temperature reading to a Google spreadsheet. 


## How to use this
1. Add your `wifi_credentials.h` to the sensor folder
2. Follow the steps on [this tutorial](https://developers.google.com/sheets/api/quickstart/nodejs) to create a Google Cloud project and OAuth client.
3. Rename the downloaded json to `client_secret.json`
4. Create the spreadsheet and add its id to line 71 on `server.js:L71`
5. To run the `server.js` script, I recommend using docker
   1. `docker build .`
   2. `docker run -p 9090:9090 <hash>`
6. Change the server IP address at line 72 in `sensor.ino:L72` to the computer running the `server.js` program
7. Flash the Arduino sketch to an ESP8266 board with an BMP280 connected via I2C (@0x76)
8. By default the ESP8266 will sleep for 30min between measurements, you can change that at line 17 of the `sensor.ino:L17`

