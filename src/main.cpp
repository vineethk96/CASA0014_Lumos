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
#include "string.h"

// My Lib
#include "./arduino_secrets.h"

/* TODO: REMOVE GLOBALS */

// Global Objects
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Global Variables
const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
const char *broker = "mqtt.cetools.org";
const int port = 1884;
const char *topic = "student/CASA0014/light/";

void setup()
{
  // Begin the Serial Comms for Debugging
  Serial.begin(9600);
  // Wait for Serial to boot up...
  while (!Serial)
    ;

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
}