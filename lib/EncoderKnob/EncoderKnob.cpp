/**
 * @file EncoderKnob.cpp
 * @author Vineeth Kirandumkara
 * @brief Library Definitions for the Encoder Knob
 * @version 0.1
 * @date 2024-11-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "EncoderKnob.hpp"

#define TOTAL_NODES 52
#define ENCODER_DIVIDER 4
#define NEGATIVE_READING 255

EncoderKnob::EncoderKnob(uint8_t encoder_pinA, uint8_t encoder_pinB, uint8_t encoder_pinC, TwoWire twi)
{

    // Update the pointers with the constructed objects
    encoder = new Encoder(encoder_pinA, encoder_pinB);
    display = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

    // TODO: Add Functionality for encoder pin C to allow for clicking
}

bool EncoderKnob::begin(void)
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!this->display->begin(SSD1306_SWITCHCAPVCC, DEFAULT_OLED_I2C_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    // Clear the buffer
    this->display->clearDisplay();
    this->display->setRotation(2);
    return true;
}

uint32_t EncoderKnob::getReading(void)
{
    return this->encoder->read() / ENCODER_DIVIDER;
}

void EncoderKnob::update(void)
{
    uint32_t currReading = this->getReading();

    if (currReading > TOTAL_NODES)
    {
        currReading = 0;
    }

    if (prevReading != currReading)
    {
        this->display->clearDisplay();
        this->display->setTextSize(2);
        this->display->setTextColor(SSD1306_WHITE);
        this->display->setCursor((10), (0));
        this->display->println("LED Node: ");
        this->display->setCursor((OLED_WIDTH / 2), (OLED_HEIGHT / 2));
        this->display->print(currReading);
        this->display->display();
    }
}
