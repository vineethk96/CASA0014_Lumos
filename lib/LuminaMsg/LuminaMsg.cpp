/**
 * @file LuminaMsg.cpp
 * @author Vineeth Kirandumkara
 * @brief Library Definitions for the Lumina Messages
 * @version 0.1
 * @date 2024-11-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "LuminaMsg.hpp"
#include <algorithm>

#define allLED "allLEDs"
#define pixelID "pixelid"
#define method "method"
#define red "R"
#define green "G"
#define blue "B"
#define white "W"
#define brightness "brightness"
#define clear "clear"
#define oneRandom "onerandom"
#define allRandom "allrandom"
#define pulseWhite "pulsewhite"

LuminaMsg::LuminaMsg(char *topicBase) : topicRoot(topicBase)
{
}

void LuminaMsg::setPixelColor(uint8_t pixel, uint8_t R, uint8_t G, uint8_t B, uint8_t W)
{
    // Set the key value pairs for a single pixel
    this->pixelMsg[pixelID].set(pixel);
    this->pixelMsg[red].set(R);
    this->pixelMsg[green].set(G);
    this->pixelMsg[blue].set(B);
    this->pixelMsg[white].set(W);

    this->lastSetMsg = pixel_m;

    // TODO: Add check for json doc overflow
}

void LuminaMsg::setNodeColor(uint8_t R, uint8_t G, uint8_t B, uint8_t W)
{
    // Clear the previous array
    this->nodeMsg.to<JsonArray>();

    // Iterate through the ring, and set the individual pixels
    for (uint8_t i = 0; i < PIXEL_RING_SIZE; i++)
    {
        this->setPixelColor(i, R, G, B, W);
        this->nodeMsg[allLED].add(this->pixelMsg); // Add the pixelMsg json to the array
    }

    this->lastSetMsg = node_m;
}

void LuminaMsg::setBrightness(uint8_t luminosity)
{
    this->brightnessMsg[brightness].set(luminosity);

    this->lastSetMsg = brightness_m;
}

void LuminaMsg::setMethod(uint8_t node, method_t cmd)
{
    switch (cmd)
    {
    case CLEAR_RING:
        this->methodsMsg[method].set(clear);
        break;
    case ALL_RANDOM:
        this->methodsMsg[method].set(allRandom);
        break;
    case ONE_RANDOM:
        this->methodsMsg[method].set(oneRandom);
        break;
    case PULSE_WHITE:
        this->methodsMsg[method].set(pulseWhite);
        break;
    }

    this->lastSetMsg = method_m;
}

void LuminaMsg::sendNode(uint8_t node, char *topic, char *json)
{
    topic = this->topicRoot;
    char *nodeNum;
    itoa(node, nodeNum, 10); // Double check this to see if nodeNum has a newline char
    strcat(topic, nodeNum);

    switch (this->lastSetMsg)
    {
    case pixel_m:
        strcat(topic, "/pixel/");
        serializeJson(this->pixelMsg, json, PIXELMSG_SIZE);
        break;
    case node_m:
        strcat(topic, "/all/");
        serializeJson(this->nodeMsg, json, NODEMSG_SIZE);
        break;
    case brightness_m:
        strcat(topic, "/brightness/");
        serializeJson(this->brightnessMsg, json, BRIGHTNESSMSG_SIZE);
        break;
    case method_m:
        strcat(topic, "/all/");
        serializeJson(this->methodsMsg, json, METHODSMSG_SIZE);
        break;
    }
}
