#include <NeoPixelBus.h>
#include <NeoPixelBrightnessBus.h>
#include <ESP8266WebServer.h>
#include "matrixletters.h"

#define colorSaturation 128

const uint8_t PixelCount = 200;
const uint8_t CharacterNumberLimit = 255; 

static const int END_MARKER = 255;
static const int NUM_ROWS = 10;
unsigned int* letterIndices;
int UPDATES_PER_SECOND = 6;

char** rowChars;
int numChars = 0;
int rowPos = -1;
RgbColor letterColor;
RgbColor backgroundColor;

// Access Point Settings
ESP8266WebServer server(80);
const char ssid[] = "";
const char password[] = "";

//NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount);
NeoPixelBrightnessBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount);

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial attach

  // malloc rowChars
  rowChars = (char**) malloc(NUM_ROWS * (sizeof(char*)));
  for (int i = 0; i < NUM_ROWS; i++)
    rowChars[i] = (char*) malloc((CharacterNumberLimit + 1) * 5 * sizeof(char));

  letterIndices = (unsigned int*) malloc((CharacterNumberLimit + 2) * sizeof(unsigned int));  // add one for end mark and space
  letterIndices[0] = END_MARKER;

  // init colors
  letterColor = RgbColor(255, 0, 0);
  backgroundColor = RgbColor(0, 0, 0);

  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();

  setupWifi();

  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();

  strip.SetBrightness(100);
  Serial.println();
  Serial.println("Running...");
}

void setupWifi() {
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println("AP Up");
  Serial.print("Device IP: ");
  Serial.println(IP);
  server.on("/data", HTTP_GET, handleNewDisplay);
  server.on("/speed", HTTP_GET, handleChangeSpeed);
  server.on("/color", HTTP_GET, handleChangeLetterColor);
  server.on("/reset", HTTP_GET, handleReset);
  server.on("/backgroundColor", HTTP_GET, handleChangeBackgroundColor);
  server.on("/brightness", HTTP_GET, handleChangeBrightness); \
  server.begin();
}

void loop() {
  server.handleClient();

  scrollLetters();
  strip.Show();
  delay(1000 / UPDATES_PER_SECOND);
}

void scrollLetters() {
  if (rowPos < 0)
    return;
  char posChar;
  int colorIdx;
  char* rowString;

  for (int i = 0; i < NUM_ROWS; i++) {
    rowString = *(rowChars + i);
    for (int o = 0; o < 20; o++) {
      posChar = rowString[(o + rowPos) % (numChars * 5)]; // first row diff

      colorIdx = (i % 2 == 0) ? (i * 20) + o : ((i + 1) * 20) - o - 1;

      if (posChar == '1')
        strip.SetPixelColor(colorIdx, letterColor);
      else
        strip.SetPixelColor(colorIdx, backgroundColor);
    }
  }
  delay(1);
  rowPos++;
  if (rowPos == 2147483647) rowPos - 0; // if we reach max int
}

void getLedPositions() {
  // for each row
  for (int i = 0; i < NUM_ROWS; i++) {
    char rowString[5 * numChars + 1]; // each character gets (4 lights + space) + terminator in a row
    for (int l = 0; l < CharacterNumberLimit; l++) {
      // check if we reached the end of the letter indicies
      if (letterIndices[l] == END_MARKER) {
        rowString[5 * numChars] = '\0';
        break;
      }

      for (int r = 0; r < 4; r++)
        rowString[(l * 5) + r] = (i == 0 || i == 9) ? '0' : letters[letterIndices[l]][i - 1][r];
      rowString[(l * 5) + 4] = '0';
    }
    strncpy(*(rowChars + i), rowString, 5 * numChars + 1);
  }
  rowPos = 0;
}

void handleNewDisplay() {
  server.send(200, "text/html", "Data received");
  Serial.println("New Phrase Request");

  if (server.hasArg("word")) { // this is the variable sent from the client
    String recievedWord = server.arg("word");
    Serial.println(recievedWord);
    if (recievedWord.length()+1 >= CharacterNumberLimit) {
      Serial.println("too many characters");
      return;
    }
    for (int i = 0; i < recievedWord.length(); i++) {
      letterIndices[i] = letterMap[recievedWord.charAt(i)];
    }
    // end with a space
    letterIndices[recievedWord.length()] = letterMap['&'];
    letterIndices[recievedWord.length() + 1] = END_MARKER;
    numChars = recievedWord.length() + 1;
  }
  getLedPositions();
}

void handleChangeSpeed() {
  server.send(200, "text/html", "Data received");
  Serial.println("New Speed Request");

  if (server.hasArg("speed")) {
    int updateSpeed = server.arg("speed").toInt();
    if (updateSpeed == 0) return;
    UPDATES_PER_SECOND = updateSpeed;
  }
}

void handleChangeLetterColor() {
  server.send(200, "text/html", "Data received");
  Serial.println("letter color Request");

  if (server.hasArg("red") && server.hasArg("green") && server.hasArg("blue")) {
    float red = server.arg("red").toFloat() * 255;
    float green = server.arg("green").toFloat() * 255;
    float blue = server.arg("blue").toFloat() * 255 ;
    letterColor = RgbColor(red, green, blue);
  }
}

void handleChangeBackgroundColor() {
  server.send(200, "text/html", "Data received");
  Serial.println("Background Color Request");

  if (server.hasArg("red") && server.hasArg("green") && server.hasArg("blue")) {
    float red = server.arg("red").toFloat() * 255;
    float green = server.arg("green").toFloat() * 255;
    float blue = server.arg("blue").toFloat() * 255 ;
    backgroundColor = RgbColor(red, green, blue);
  }
}

void handleReset() {
  server.send(200, "text/html", "Data received");
  Serial.println("Reset Request");

  letterIndices[0] = END_MARKER;
  getLedPositions();

  // init colors
  letterColor = RgbColor(255, 0, 0);
  backgroundColor = RgbColor(0, 0, 0);

  setupWifi();

  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();
}

void handleChangeBrightness() {
  server.send(200, "text/html", "Data received");
  Serial.println("Brightness Request");

  if (server.hasArg("brightness")) {
    int updateBrightness = server.arg("brightness").toInt();
    if (updateBrightness == 0) return;
    strip.SetBrightness(updateBrightness);
  }
}
