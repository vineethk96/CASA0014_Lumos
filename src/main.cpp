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
// #include "LuminaMsg.hpp"
#include "EncoderKnob.hpp"
#include "MagTrio.hpp"

// Pins
#define ENCODER_A 7
#define ENCODER_B 6

#define TEST_NO_WIFI // For at home testing with no MQTT

/* TODO: REMOVE GLOBALS */

#ifndef TEST_NO_WIFI
// Global Variables
const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
const char *mqtt_username = SECRET_MQTTUSER;
const char *mqtt_password = SECRET_MQTTPASS;
const char *mqtt_server = "mqtt.cetools.org";
const int port = 1884;
wl_status_t status = WL_IDLE_STATUS; // the Wifi radio's status

// Global Objects
WiFiServer server(80);
WiFiClient wifiClient;
#endif

char *topic;

#ifndef TEST_NO_WIFI
WiFiClient mkrClient;
PubSubClient client(mkrClient);
#endif

// LuminaMsg mqtt_msg(topic_base);
EncoderKnob ledDial(ENCODER_A, ENCODER_B, 0, Wire);
MagTrio mags(Wire);

// Function Declarations
void startWifi(void);
void reconnectMQTT(void);
void sendMQTT(char *topic, char *msg);
void lightNode(uint8_t nodeNum, uint8_t red, uint8_t green, uint8_t blue, uint8_t white);

void setup()
{
  // HARDWARE SETUP
  // Begin the Serial Comms for Debugging
  Serial.begin(9600);
  // Wait for Serial to boot up...
  while (!Serial)
    ;

  // Begin LED Dial Unit
  if (!ledDial.begin())
  {
    Serial.println("Led Dial Failed to Initialize.");
    while (1)
      ;
  }

  mags.begin();

#ifndef TEST_NO_WIFI
  // MQTT SETUP
  WiFi.setHostname("Lumina_Vineeth");
  startWifi();
  client.setServer(mqtt_server, port);
  Serial.println("setup complete");
#endif
}

void loop()
{

  static unsigned long timestamp = millis();

#ifndef TEST_NO_WIFI
  // we need to make sure the arduino is still connected to the MQTT broker
  // otherwise we will not receive any messages
  if (!client.connected())
  {
    Serial.println("Attempting to reconnect to MQTT...");
    reconnectMQTT();
  }

  // we also need to make sure we are connected to the wifi
  // otherwise it will be impossible to connect to MQTT!
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Attempting to reconnect to WiFi");
    startWifi();
  }
#endif

  // Update the Magnetometer readings
  mags.update();

  // Update at the fastest speed possible
  ledDial.update();

  /**
   * Actions go here!
   *
   */

  // Every 500 ms, send a new message
  if (millis() - timestamp > 500)
  {
    lightNode(ledDial.getReading(), mags.getRed(), mags.getGreen(), mags.getBlue(), 100);
    timestamp = millis(); // Update Timestamp
  }

#ifndef TEST_NO_WIFI
  // check for messages from the broker and ensuring that any outgoing messages are sent.
  client.loop();
#endif
}

void startWifi()
{
#ifndef TEST_NO_WIFI
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

#endif
}

void reconnectMQTT(void)
{
#ifndef TEST_NO_WIFI
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
#endif
}

void sendMQTT(char *topic, char *msg)
{

  Serial.println("SENDING MSG...");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(msg);

#ifndef TEST_NO_WIFI
  if (client.publish(topic, msg))
  {
    Serial.println("Message published");
  }
  else
  {
    Serial.println("Failed to publish message");
  }
#endif
}

void lightNode(uint8_t nodeNum, uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
  // Setup the Topic
  char mqttTopic[35];
  sprintf(mqttTopic, "student/CASA0014/light/%d/pixel/", nodeNum);

  // Send messages to each pixel
  for (uint8_t i = 0; i < 12; i++)
  {
    char mqttMsg[60];
    sprintf(mqttMsg, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}", i, red, green, blue, white);
    sendMQTT(mqttTopic, mqttMsg);
  }
}
