The code makes the ESP host a wifi server, AP (Access Point) style allowing the user to connect to it using their device and access a web page with information send from the ESP. right now its a just a print statement for "alive" and a counter for how many times it sends it.  

Note for an Arduino, IO framework, digital pins are defined in GPIO which dont always equal the D pins on the dev board. on this dev board. GPIO pin 4 corresponds to D3 on the board. 

Right now there are three files in the codebase, one for wifi server, one for reed switch and one main file which controls both. 

The code makes the ESP host a server with the URL: http://192.168.4.1 which can be accessed after connecting to the SSID: HiveNode-ESP32 and password: hivepassword.

the web page then displays the time spent in the current state (1). once the magnet is brought close to the sensor, the web page turns red, the state changes to 0 and the clock is paused. once the magnet is removed, the state goes back to 1 and resets the clock. 

The ESP is recieving data from the REED switch via D3, GPIO 4, with power 3.3 v

Decide between STA style and AP style Wifi server

NORMAL MODE:
  Deep Sleep
  RTC Wake
  Read Sensors
  Transmit Data
  Sleep

IF CONFIG BUTTON / REED SWITCH:
  Wake
  Enable AP Mode
  Start Web Server
  Wait for Client
  Timeout
  Sleep
