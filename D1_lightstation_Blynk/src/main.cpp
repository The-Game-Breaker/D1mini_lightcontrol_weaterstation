#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

#include <SimpleTimer.h>
#include <BH1750.h>
#include "FastLED.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
BH1750 lightMeter;
BlynkTimer timer;
BlynkTimer timer2;

#define FASTLED_ESP8266_RAW_PIN_ORDER

#define NUM_LEDS1 300
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
CRGB leds1[NUM_LEDS1];
char auth[] = "DnseZDCWadka4ELtcW71syltsF0MkTg_";
char ssid[] = "Laureyn";
char pass[] = "Breedsmoelkikker138";
char host[] = "192.168.50.191";
#define PIN1 D3
int data = 255;
int r, g, b;

int lighttreshold;
int allowlokaloverwrite;
int rainbowmode;
//oled
#define OLED_RESET 0 // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

Adafruit_BME280 bme; // I2C


void static1(int r, int g, int b, int brightness)
{
  FastLED.setBrightness(brightness);
  for (int i = 0; i < NUM_LEDS1; i++)
  {
    leds1[i] = CRGB(r, g, b);
  }
  FastLED.show();
}
void TimeOperation()
{
float Temp = bme.readTemperature();
float hpa = bme.readPressure() / 100.0F;
float Humid = bme.readHumidity();
  uint16_t lux = lightMeter.readLightLevel();
  Blynk.virtualWrite(V5, lux);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print("lx= ");
  display.println(lux);
  display.print("trg= ");
  display.println(lighttreshold);

  display.print(Temp);
  display.println(" C");
  Blynk.virtualWrite(V14, Temp);

  display.print(hpa);
  display.println("hPa");
  Blynk.virtualWrite(V15, hpa);

  display.print(Humid);
  display.println(" %");
  Blynk.virtualWrite(V16, Humid);

  display.display();

  if (lux <= lighttreshold && allowlokaloverwrite == 1)
  {
    int intTemp = Temp+30;
    int inthpa = hpa/10-30;
    int inthumid = Humid+30;
    r = intTemp;
    g = inthpa;
    b = inthumid;
    static1(r, g, b, 255);
  }
  if (lux >= lighttreshold && allowlokaloverwrite == 1)
  {
    static1(0, 0, 0, 255);
  }
  if (rainbowmode == 1 && allowlokaloverwrite == 0)
  {
    uint8_t thisSpeed = 10;
    uint8_t deltaHue = 10;
    uint8_t thisHue = beat8(thisSpeed, 255);
    fill_rainbow(leds1, NUM_LEDS1, thisHue, deltaHue);
    FastLED.show();
  }
}

BLYNK_WRITE(V3)
{
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();

  static1(r, g, b, data);
}
BLYNK_WRITE(V2)
{
  data = param.asInt();
  static1(r, g, b, data);
}
BLYNK_WRITE(V11)
{
  lighttreshold = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}
BLYNK_READ(V12)
{
  Blynk.virtualWrite(V12, millis() / 1000);
}
BLYNK_WRITE(V12)
{
  allowlokaloverwrite = param.asInt();
}
BLYNK_READ(V13)
{
  Blynk.virtualWrite(V13, millis() / 1000);
}
BLYNK_WRITE(V13)
{
  rainbowmode = param.asInt();
}
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 50, 191), 8080);
  FastLED.addLeds<LED_TYPE, PIN1, COLOR_ORDER>(leds1, NUM_LEDS1).setCorrection(TypicalLEDStrip);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  timer.setInterval(100L, TimeOperation);
  lightMeter.begin();
  bme.begin(0x76);
}
void loop()
{
  Blynk.run();
  timer.run();
}