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

#include <ArduinoJson.h>

#define PIXEL_RING_SIZE 12
#define NODE_AMOUNT 52

typedef enum
{
    SELECT_PIXEL,
    SELECT_NODE,
    SET_BRIGHTNESS
} subTopic_t;

typedef enum
{
    CLEAR_RING,
    ALL_RANDOM,
    ONE_RANDOM,
    PULSE_WHITE
} method_t;

class LuminaMsg
{
private:
    char *topicBase;
    uint8_t currNode;
    JsonDocument pixelBase_msg;
    JsonDocument pixelRing_msg;
    JsonDocument brightness_msg;
    JsonDocument methods_msg;
    uint8_t lastChangeBitmap = 0;

public:
    LuminaMsg(char *topicBase);

    void changePixel(uint8_t node, uint8_t pixel, uint8_t R, uint8_t G, uint8_t B, uint8_t W);

    void changeNode(uint8_t node, uint8_t R, uint8_t G, uint8_t B, uint8_t W);

    void changeBrightness(uint8_t node, uint8_t brightness);

    void changeMethod(uint8_t node, method_t method);

    char *getTopic(void);

    char *getJSON(void);
};