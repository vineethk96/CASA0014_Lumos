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
#include "LuminaMsg.hpp"

// Pins
#define NEOPIXEL_RING 0
#define ENCODER_A 4
#define ENCODER_B 5
#define I2C_SDA 11
#define I2C_SCL 12

// Default Values
#define PIXEL_RING 12
#define PIXEL_STRIP 8

#define OLED_WIDTH 128 // OLED display width in pixels
#define OLED_HEIGHT 64 // OLED display height in pixels
#define OLED_RESET -1

#define OLED_I2C_ADDR 0x3C
// Create I2C Addrs for Magnetometers

/* TODO: REMOVE GLOBALS */

// Global Variables
const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
const char *mqtt_username = SECRET_MQTTUSER;
const char *mqtt_password = SECRET_MQTTPASS;
char broker[] = "mqtt.cetools.org";
char topic_base[] = "student/CASA0014/light/";
const int port = 1884;
wl_status_t status = WL_IDLE_STATUS; // the Wifi radio's status

// Global Objects
WiFiServer server(80);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

Adafruit_NeoPixel led_ring(PIXEL_STRIP, NEOPIXEL_RING);

Encoder encoder(ENCODER_A, ENCODER_B);

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

LuminaMsg mqtt_msg(topic_base);

// Function Declarations
void startWiFi(void);

void connectMQTT(void);

void setup()
{
  /**
   * HARDWARE SETUP
   */
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
   * MQTT SETUP
   */
  // Begin the Wifi Comms for the MQTT Service
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);
  WiFi.setHostname("Lumina_Vineeth");
  mqttClient.setUsernamePassword(mqtt_username, mqtt_password);
}

void loop()
{

  // Start the WiFi Connection
  if (!wifiClient.connected())
  {
    startWiFi();
  }

  // Connect the MQTT Client
  if (!mqttClient.connected())
  {
    connectMQTT();
  }

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

    long red = random(0, 255);
    long green = random(0, 255);
    long blue = random(0, 255);

    mqtt_msg.changeNode(20, red, green, blue, 0);
    char *topic = mqtt_msg.getTopic();
    char *json = mqtt_msg.getJSON();

    Serial.print("topic: ");
    Serial.println(topic);
    Serial.print("json: ");
    Serial.println(json);

    // mqttClient.beginMessage(topic);
    // mqttClient.print(json);
    // mqttClient.endMessage();

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

void startWiFi(void)
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }
  const char *fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // Function for connecting to a WiFi network
  // is looking for UCL_IoT and a back up network (usually a home one!)
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    // loop through all the networks and if you find UCL_IoT or the backup - ssid1
    // then connect to wifi
    Serial.print("Trying to connect to: ");
    Serial.println(ssid);
    for (int i = 0; i < n; ++i)
    {
      String availablessid = WiFi.SSID(i);
      // Primary network
      if (availablessid.equals(ssid))
      {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, pass);
        while (WiFi.status() != WL_CONNECTED)
        {
          delay(600);
          Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.println("Connected to " + String(ssid));
          break; // Exit the loop if connected
        }
        else
        {
          Serial.println("Failed to connect to " + String(ssid));
        }
      }
      else
      {
        Serial.print(availablessid);
        Serial.println(" - this network is not in my list");
      }
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT(void)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    startWiFi();
  }
  else
  {
    Serial.println(WiFi.localIP());
  }

  // Loop until we're reconnected
  while (!mqttClient.connected())
  { // while not (!) connected....
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (mqttClient.connect(broker, port))
    {
      Serial.println("connected");
      // ... and subscribe to messages on broker
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.connectError());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
