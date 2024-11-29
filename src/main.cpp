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
#include <PubSubClient.h>
#include <utility/wifi_drv.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "string.h"

// My Lib
#include "./arduino_secrets.h"
#include "LuminaMsg.hpp"
#include "EncoderKnob.hpp"

// Pins
#define ENCODER_A 4
#define ENCODER_B 5

#define OLED_I2C_ADDR 0x3C
#define MAGMUX_I2C_ADDR 0x70

/* TODO: REMOVE GLOBALS */

// Global Variables
const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
const char *mqtt_username = SECRET_MQTTUSER;
const char *mqtt_password = SECRET_MQTTPASS;
const char *mqtt_server = "mqtt.cetools.org";
const int port = 1884;
wl_status_t status = WL_IDLE_STATUS; // the Wifi radio's status
char topic_base[] = "student/CASA0014/light/";

// Global Objects
WiFiServer server(80);
WiFiClient wifiClient;

WiFiClient mkrClient;
PubSubClient client(mkrClient);

LuminaMsg mqtt_msg(topic_base);
EncoderKnob ledDial(ENCODER_A, ENCODER_B, 0, &Wire);

// Function Declarations
void startWifi(void);
void reconnectMQTT(void);

void setup()
{
  // HARDWARE SETUP
  // Begin the Serial Comms for Debugging
  Serial.begin(9600);
  // Wait for Serial to boot up...
  while (!Serial)
    ;

  // Begin LED Dial Unit
  if (!ledDial.begin(OLED_I2C_ADDR))
  {
    Serial.println("Led Dial Failed to Initialize.");
    while (1)
      ;
  }

  // MQTT SETUP
  WiFi.setHostname("Lumina_Vineeth");
  startWifi();
  client.setServer(mqtt_server, port);
  Serial.println("setup complete");
}

void loop()
{
  // we need to make sure the arduino is still connected to the MQTT broker
  // otherwise we will not receive any messages
  if (!client.connected())
  {
    reconnectMQTT();
  }

  // we also need to make sure we are connected to the wifi
  // otherwise it will be impossible to connect to MQTT!
  if (WiFi.status() != WL_CONNECTED)
  {
    startWifi();
  }

  char *newTopic;
  char *json;
  mqtt_msg.setNodeColor(255, 0, 0, 100);
  mqtt_msg.sendNode(20, newTopic, json);
  Serial.print("Topic: ");
  Serial.println(newTopic);
  Serial.print("JSON msg: ");
  Serial.println(json);
  delay(100);

  /**
   * Actions go here!
   *
   */
  // Update at the fastest speed possible
  ledDial.update();

  // check for messages from the broker and ensuring that any outgoing messages are sent.
  client.loop();
}

void startWifi()
{

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }
  String fv = WiFi.firmwareVersion();
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

void reconnectMQTT(void)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    startWifi();
  }
  else
  {
    // Serial.println(WiFi.localIP());
  }

  // Loop until we're reconnected
  while (!client.connected())
  { // while not (!) connected....
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("connected");
      // ... and subscribe to messages on broker
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}