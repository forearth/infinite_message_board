#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// neopixel 라이브러리 불러오기 및 설정
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 4
#define NUMPIXELS 26
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// MAX7219 설정
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CS_PIN 15

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
AsyncWebServer server(80);

// AP 접속을 위한 id/pwd 설정
const char *ssid = "wifi name";
const char *password = "wifi_password";

const char *input_parameter1 = "input_string";

// 메시지 전역변수로 설정
char m[100] = "connect...";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Title</title>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center; padding:10px;}
    h2 {font-size: 3.0rem; color: #FF0000;}
    div {text-align: center;}
  </style>
  </head><body>
<div>
insert base64 image
<div>
 <form action="/get">
    <input type="text" name="input_string" placeholder="Enter your message in English." style="height:50px; width:90%"><br/><br/>
    <input type="submit" value="Change Message" style="height:50px; width:90%">
  </form><br>

</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void colorWipe(uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {                                // For each pixel in strip...
    strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
    strip.show();                  //  Update strip to match
    delay(wait);                   //  Pause for a moment
  }
}

void theaterChase(uint32_t color, int wait)
{
  for (int a = 0; a < 10; a++)
  { // Repeat 10 times...
    for (int b = 0; b < 3; b++)
    {                //  'b' counts from 0 to 2...
      strip.clear(); //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void rainbow(int wait)
{
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
  {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChaseRainbow(int wait)
{
  int firstPixelHue = 0;
  for (int a = 0; a < 30; a++)
  {
    for (int b = 0; b < 3; b++)
    {
      strip.clear();
      for (int c = b; c < strip.numPixels(); c += 3)
      {
        int hue = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color);
      }
      strip.show();
      delay(wait);
      firstPixelHue += 65536 / 90;
    }
  }
}

void setup()
{

  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  Display.begin();
  Display.setIntensity(10);
  Display.displayClear();
  Display.displayScroll(m, PA_RIGHT, PA_SCROLL_LEFT, 150);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String input_message;
    String input_parameter;
    rainbow(10);

    if (request->hasParam(input_parameter1)) {
      input_message = request->getParam(input_parameter1)->value();
      input_parameter = input_parameter1;
      int m_length = input_message.length() + 1;

      input_message.toCharArray(m, 100);

      Serial.println(m_length);
      Display.displayClear();
      Display.displayScroll(m, PA_RIGHT, PA_SCROLL_LEFT, 150);
    } else {
      input_message = "No message sent";
      input_parameter = "none";
    }
    Serial.println(input_message);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + input_parameter + ") with value: " + input_message + "<br><a href=\"/\">Return to Home Page</a>"); });
  server.onNotFound(notFound);
  server.begin();

  // neopixel 초기화
  // 극장간판에 달린 전구가 빛나는것과 유사한 효과
  // theaterChase(스트립 색상, 딜레이 시간)
  strip.begin();
  theaterChase(strip.Color(127, 127, 127), 100); // White, half brightness
  theaterChase(strip.Color(127, 0, 0), 100);     // Red, half brightness
  theaterChase(strip.Color(0, 0, 127), 100);     // Blue, half brightness

  // 전체 스트립에 색을 흐르는 무지개빛처럼 돌아가며 출력
  rainbow(5);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
}

void loop()
{

  if (Display.displayAnimate())
  {
    Display.displayReset();
  }
}
