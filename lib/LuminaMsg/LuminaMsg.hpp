/**
 * @file LuminaMsg.hpp
 * @author Vineeth Kirandumkara
 * @brief A Library meant to help construct the JSON messages for the Lumina Ring
 * @version 0.1
 * @date 2024-11-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <utility/wifi_drv.h>

#define PIXEL_RING_SIZE 12
#define NODE_AMOUNT 52
#define PIXELMSG_SIZE 60
#define NODEMSG_SIZE 740
#define BRIGHTNESSMSG_SIZE 20
#define METHODSMSG_SIZE 30

enum method_t
{
    CLEAR_RING,
    ALL_RANDOM,
    ONE_RANDOM,
    PULSE_WHITE
};

enum msg_t
{
    pixel_m,
    node_m,
    brightness_m,
    method_m
};

class LuminaMsg
{
private:
    char *topicRoot;
    uint8_t currNode;
    JsonDocument pixelMsg;
    JsonDocument nodeMsg;
    JsonDocument brightnessMsg;
    JsonDocument methodsMsg;
    uint8_t lastChangeBitmap = 0;
    msg_t lastSetMsg;

public:
    /**
     * @brief Construct a new Lumina Msg object
     *
     * @param topicBase (char*): Base topic string
     */
    LuminaMsg(char *topicBase);

    /**
     * @brief Set the Pixel Color
     *
     * @param pixel (uint8_t): Select an individual pixel on a Node.
     * @param R (uint8_t): 0-255 value for Red
     * @param G (uint8_t): 0-255 value for Green
     * @param B (uint8_t): 0-255 value for Blue
     * @param W (uint8_t): 0-255 value for White
     */
    void setPixelColor(uint8_t pixel, uint8_t R, uint8_t G, uint8_t B, uint8_t W);

    /**
     * @brief Set the Color for a full Node
     *
     * @param R (uint8_t): 0-255 value for Red
     * @param G (uint8_t): 0-255 value for Green
     * @param B (uint8_t): 0-255 value for Blue
     * @param W (uint8_t): 0-255 value for White
     */
    void setNodeColor(uint8_t R, uint8_t G, uint8_t B, uint8_t W);

    /**
     * @brief Set the Brightness
     *
     * @param luminosity (uint8_t): The brightness level for the selected node
     */
    void setBrightness(uint8_t luminosity);

    /**
     * @brief Set one of the preset methods
     *
     * @param node (uint8_t): Specific Node the method should be sent to
     * @param cmd (method_t): The specified method
     */
    void setMethod(uint8_t node, method_t cmd);

    /**
     * @brief Send the premade message to MQTT
     *
     * @param node (uint8_t): Specific Node the method should be sent to
     * @param topic (char*): A pointer to the full topic string
     * @param json (char*): A pointer to the full json
     */
    void sendNode(uint8_t node, char *topic, char *json);
};