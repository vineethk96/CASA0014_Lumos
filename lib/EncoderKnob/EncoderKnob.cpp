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

EncoderKnob::EncoderKnob(uint8_t encoder_pinA, uint8_t encoder_pinB, uint8_t encoder_pinC, TwoWire *twi)
{

    // Update the pointers with the constructed objects
    encoder = new Encoder(encoder_pinA, encoder_pinB);
    display = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

    // TODO: Add Functionality for encoder pin C to allow for clicking
}

bool EncoderKnob::begin(uint8_t display_i2c_addr = DEFAULT_OLED_I2C_ADDRESS)
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!this->display->begin(SSD1306_SWITCHCAPVCC, display_i2c_addr))
    {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    // Clear the buffer
    this->display->clearDisplay();
    return true;
}

uint32_t EncoderKnob::getReading(void)
{
    return this->encoder->read() / 4;
}

void EncoderKnob::update(void)
{
    uint32_t currReading = this->getReading();

    if (prevReading != currReading)
    {
        this->display->clearDisplay();
        this->display->setTextSize(2);
        this->display->setTextColor(SSD1306_WHITE);
        this->display->setCursor((OLED_WIDTH / 2), (OLED_HEIGHT / 2));
        this->display->println("LED Node: ");
        this->display->print(currReading);
        this->display->display();
    }
}
