# Custom Wireless Pixel Display
<img src="https://github.com/codycoogan/PixelsDisplay/blob/main/hello_world.gif?raw=true" width="200">

Code to drive a custom 10x20 LED board which allows for http requests to set scrolling text, letter and background colors, brightness and scroll speed adjustments. Powered with an ESP8266 and WS2811 lights.

Companion controller iOS app: https://github.com/codycoogan/LightMatrixController

# Tutorial
1. Position the WS2811 LEDs in a 10x20 matrix, I used pegboard. 

2. Give the lights power, I use a 4.5V (3 AA batteries) battery pack 

3. Wire the ESP8266 with WiFi computer to the lights. I give it power from another battery pack.

4. Flash the code onto the board via the Arduino IDE.

5. Flash the iOS controller app onto your device [found here: https://github.com/codycoogan/LightMatrixController] 

6. Turn on the board and the lights, and connect your device to the Access Point network that you setup in the board code. Now you can send HTTP requests to the display to control it. You can do this easily via the app interface or send the requests how you like.  
