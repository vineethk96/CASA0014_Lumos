/**
 * @file main.cpp
 * @author Vineeth Kirandumkara (vineeth.kirandumkara.24@ucl.ac.uk)
 * @brief Lumos is a project to communicate with a wall of LED Nodes within the CE Lab.
 * @version 0.1
 * @date 2024-10-29
 *
 * @copyright Copyright (c) 2024
 *
 */

// Third Party Lib
#include <Arduino.h>
#include <WiFiNINA.h>
#include <MqttClient.h>
#include <utility/wifi_drv.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include "string.h"

// My Lib
#include "./arduino_secrets.h"

// Pins
#define NEOPIXEL_RING 0
#define ENCODER_A 4
#define ENCODER_B 5
#define OLED_SDA 11
#define OLED_SCL 12

// Default Values
#define PIXEL_RING 12
#define PIXEL_STRIP 8

#define OLED_WIDTH 128 // OLED display width in pixels
#define OLED_HEIGHT 64 // OLED display height in pixels
#define OLED_RESET -1

#define OLED_I2C_ADDR 0x3C

/* TODO: REMOVE GLOBALS */

// Global Objects
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

Adafruit_NeoPixel led_ring(PIXEL_STRIP, NEOPIXEL_RING);

Encoder encoder(ENCODER_A, ENCODER_B);

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// Global Variables
const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
const char *broker = "mqtt.cetools.org";
const int port = 1884;
const char *topic = "student/CASA0014/light/";

// Function Declarations

void setup()
{
  // Begin the Serial Comms for Debugging
  Serial.begin(9600);
  // Wait for Serial to boot up...
  while (!Serial)
    ;

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Begin the NeoPixel Comms
  led_ring.begin();
  led_ring.clear();
  led_ring.show();

  // Clear the buffer
  display.clearDisplay();

  /**
   * MQTT Connection Code
   */
  // // Begin the Wifi Comms for the MQTT Service
  // Serial.print("Connecting to SSID: ");
  // Serial.println(ssid);
  // while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  // {
  //   Serial.print(".");
  // }
  // Serial.println("Successfully Connected to WiFi!");

  // // Begin the MQTT Connection
  // Serial.print("Connecting to MQTT Broker: ");
  // Serial.println(broker);
  // if (!mqttClient.connect(broker, port))
  // {
  //   Serial.print("MQTT connection failed. Error Code: ");
  //   Serial.println(mqttClient.connectError());
  //   while (1)
  //     ; // Block Loop
  // }

  // Serial.println("Connected to MQTT Broker");
}

void loop()
{

  // TEST
  static int oldVal = 0;

  uint8_t encoderVal = (encoder.read() / 4);
  Serial.print("encoderVal: ");
  Serial.println(encoderVal);

  if (oldVal != encoderVal)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor((OLED_WIDTH / 2), (OLED_HEIGHT / 2));
    display.print(encoderVal);
    // display.drawChar((OLED_WIDTH / 2), (OLED_HEIGHT / 2), (unsigned char)encoderVal, SSD1306_WHITE, SSD1306_BLACK, 2);
    display.display();
    if (encoderVal <= 8)
    {
      {
        if (oldVal < encoderVal)
        {
          led_ring.setPixelColor((encoderVal - 1), 255, 0, 0);
        }
        else if (oldVal > encoderVal)
        {
          led_ring.setPixelColor((encoderVal), 0, 0, 0);
        }
        oldVal = encoderVal;
        led_ring.show();
      }
    }
  }

  /////////////////////////////////

  // for (uint8_t i = 0; i < PIXEL_STRIP; i++)
  // {
  //   led_ring.setPixelColor(i, 255, 0, 0, 10);
  //   led_ring.show();
  //   Serial.print(i);
  //   Serial.println(" led turns on.");
  //   delay(100);
  // }

  // for (uint8_t i = 0; i < PIXEL_STRIP; i++)
  // {
  //   led_ring.setPixelColor(i, 0, 0, 0, 255);
  //   led_ring.show();
  //   Serial.print(i);
  //   Serial.println(" led turns off.");
  //   delay(100);
  // }
}