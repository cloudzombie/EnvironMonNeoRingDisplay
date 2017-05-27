// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

#include "application.h"

//SerialDebugOutput debugOutput;          // default is 9600 and log everything

#define TIMEOUT_PERIOD 30000  // 30 seconds without a message

STARTUP( combobulator());

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define PIXEL_PIN D6
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

bool gotHumidity = false; // a global semaphore to set true when you get a new temperature value from master Particle.subscribe() function...

//static false = chase(strip.Color(255, 0, 0)); // Red

int gotMessage;

double hum = 0.0;

struct setLeds;

struct LedLevels{
  double hum;
  int stripPercentage;
  byte red;
  byte green;
  byte blue;
};

LedLevels ledLevels[]{
 { 55.00, 100, 255,   0, 0}, // c to f 90.5, RGB: RED
 { 52.00,  90, 255, 128, 0}, // c to f 82.0, RGB: ORANGE
 { 50.00,  80, 204, 255, 0}, // c to f 80.0, RGB: YELLOW
 { 45.00,  70, 0, 255, 0}, // c to F 70.012, RGB: GREEN
 { 40.00,  60, 0, 255, 50}, // c to F 68.00, RGB: LIGHT GREEN
 { 35.00,  50, 0, 0, 255}, // c to F 60.008, RGB: BLUE
 { 30.00,  40, 255, 0, 255}, // c to F 55.004, RGB: CYAN
 { 25.00,  30, 255,  0, 127}, // c to F 32.486, RGB: MAGENTA
 {-99.0 ,  20, 255, 255, 255}  // CAPTURE ALL THE REST RGB: WHITE
};

void WiFiSetup()
{
  //WiFi.setCredentials(SSID, PASSWORD);    
  WiFi.setCredentials("Fisher", "5034895673");
  WiFi.connect();
}

void combobulator(){
    
    
    Particle.variable("HumidB", &hum, DOUBLE);
    Particle.subscribe("HumidB", myHandler, MY_DEVICES);
}


void myHandler(const char *event, const char *data)
{
    hum = atof(data);  // convert the incoming string literal to float and put the value in the global variable
    gotMessage = true; // a global semaphore to set true when you get a new temperature value from master Particle.subscribe() function
    Particle.publish("GotHB", String(hum));

}




void setup()
{
  WiFiSetup();  
  combobulator(); // initialize STARTUP function handler
  strip.begin();
  strip.setBrightness(25); // set brightness limit (value between 0-255)
}


void setLeds(double hum)
{
  for(int i = 0; i < sizeof(ledLevels)/sizeof(ledLevels[0]); i++)
  {
    if(hum > ledLevels[i].hum)
    {
      for (int j = 0; j < PIXEL_COUNT; j++)
      {
        if(j < (int(ledLevels[i].stripPercentage  / 100.0 * PIXEL_COUNT)))
        {
          strip.setPixelColor(j, ledLevels[i].red, ledLevels[i].green, ledLevels[i].blue);
        }
        else
        {
          strip.setPixelColor(j, 0, 0, 0);
        }
      }
      strip.show();
      return;
    }
  }
}


void loop(){
    
  static int messageTimer = 0;
  if(gotMessage);
  {
    setLeds(hum);
    gotMessage = false;
    messageTimer = millis();  // each time the message is received, reset the timeout clock to millis()
  }
  if(millis() - messageTimer > TIMEOUT_PERIOD)  // check to see if your timeout has expired
  {
    chase(strip.Color(255, 0, 0)); // Red
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+6; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-6, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}


uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

