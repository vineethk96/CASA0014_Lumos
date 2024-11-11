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

#define PIXELID "pixelid"
#define RED "R"
#define BLUE "B"
#define GREEN "G"
#define WHITE "W"
#define ALLLED "allLEDs"
#define METHOD "method"
#define BRIGHTNESS "brightness"
#define CLEAR "clear"
#define ALLRANDOM "allrandom"
#define ONERANDOM "onerandom"
#define PULSEWHITE "pulsewhite"
#define PIXEL_ENDPOINT "/pixel/"
#define NODE_ENDPOINT "/all/"
#define BRIGHTNESS_ENDPOINT "/brightness/"

#define CHANGE_PIXEL 0
#define CHANGE_NODE 1
#define CHANGE_BRIGHTNESS 2
#define CHANGE_METHOD 3

LuminaMsg::LuminaMsg(char *topic)
{
    topicBase = topic;
    Serial.print("topic in Lumina class: ");
    Serial.println(topicBase);
}

void LuminaMsg::changePixel(uint8_t node, uint8_t pixel, uint8_t R, uint8_t G, uint8_t B, uint8_t W)
{
    if (pixel > PIXEL_RING_SIZE)
    {
        return;
    }

    currNode = node;

    pixelBase_msg[PIXELID] = pixel;
    pixelBase_msg[RED] = R;
    pixelBase_msg[GREEN] = G;
    pixelBase_msg[BLUE] = B;
    pixelBase_msg[WHITE] = W;

    // setting CHANGE_PIXEL bit using bitmask
    lastChangeBitmap = 0;
    lastChangeBitmap = lastChangeBitmap | 1 << CHANGE_PIXEL;
}

void LuminaMsg::changeNode(uint8_t node, uint8_t R, uint8_t G, uint8_t B, uint8_t W)
{
    for (uint8_t i = 0; i < PIXEL_RING_SIZE; i++)
    {
        // Add the Pixels to the Pixel Ring Array
        changePixel(node, i, R, G, B, W);
        pixelRing_msg[ALLLED].add(pixelBase_msg);
    }

    // setting CHANGE_NODE bit using bitmask
    lastChangeBitmap = 0;
    lastChangeBitmap = lastChangeBitmap | 1 << CHANGE_NODE;
}

void LuminaMsg::changeBrightness(uint8_t node, uint8_t brightness)
{
    currNode = node;
    brightness_msg[BRIGHTNESS] = brightness;

    // setting CHANGE_BRIGHTNESS bit using bitmask
    lastChangeBitmap = 0;
    lastChangeBitmap = lastChangeBitmap | 1 << CHANGE_BRIGHTNESS;
}

void LuminaMsg::changeMethod(uint8_t node, method_t method)
{
    currNode = node;
    switch (method)
    {
    case CLEAR_RING:
        methods_msg[METHOD] = CLEAR;
        break;
    case ALL_RANDOM:
        methods_msg[METHOD] = ALLRANDOM;
        break;
    case ONE_RANDOM:
        methods_msg[METHOD] = ONERANDOM;
        break;
    case PULSE_WHITE:
        methods_msg[METHOD] = PULSEWHITE;
        break;
    }

    // setting CHANGE_METHOD bit using bitmask
    lastChangeBitmap = 0;
    lastChangeBitmap = lastChangeBitmap | 1 << CHANGE_METHOD;
}

char *LuminaMsg::getTopic(void)
{
    char *tempTopic = topicBase + (char)currNode;

    // Check what was the last command sent
    if (lastChangeBitmap & (1 << CHANGE_PIXEL))
    {
        strcat(tempTopic, PIXEL_ENDPOINT);
    }
    else if ((lastChangeBitmap & (1 << CHANGE_NODE)) || (lastChangeBitmap & (1 << CHANGE_METHOD)))
    {
        strcat(tempTopic, NODE_ENDPOINT);
    }
    else if (lastChangeBitmap & (1 << CHANGE_BRIGHTNESS))
    {
        strcat(tempTopic, BRIGHTNESS_ENDPOINT);
    }

    Serial.print("topic in Get Topic: ");
    Serial.println(tempTopic);

    return tempTopic;
}

char *LuminaMsg::getJSON(void)
{
    char tempDoc[100];
    // Check what was the last command sent
    if (lastChangeBitmap & (1 << CHANGE_PIXEL))
    {
        serializeJson(pixelBase_msg, tempDoc);
    }
    else if (lastChangeBitmap & (1 << CHANGE_NODE))
    {
        serializeJson(pixelRing_msg, tempDoc);
    }
    else if (lastChangeBitmap & (1 << CHANGE_BRIGHTNESS))
    {
        serializeJson(brightness_msg, tempDoc);
    }
    else if (lastChangeBitmap & (1 << CHANGE_METHOD))
    {
        serializeJson(methods_msg, tempDoc);
    }

    return tempDoc;
}
