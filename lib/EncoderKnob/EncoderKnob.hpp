/**
 * @file EncoderKnob.hpp
 * @author Vineeth Kirandumkara
 * @brief A Library for encapsulating the Rotary Encoder Knob
 * @version 0.1
 * @date 2024-11-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include <Wire.h>

#define OLED_WIDTH 128 // OLED display width in pixels
#define OLED_HEIGHT 64 // OLED display height in pixels
#define OLED_RESET -1

#define DEFAULT_OLED_I2C_ADDRESS 0x3C

class EncoderKnob
{
private:
    Encoder *encoder;          // Pointer to the Encoder Object
    Adafruit_SSD1306 *display; // Pointer to Screen Object
    uint32_t prevReading = 0;  // Tracks the encoder readings to display on screen

public:
    /**
     * @brief Construct a new Encoder Knob object
     *
     * @param encoder_pinA (uint8_t): Pin for pin A on the PEC11 Encoder
     * @param encoder_pinB (uint8_t): Pin for pin B on the PEC11 Encoder
     * @param encoder_pinC (uint8_t): Pin for pin C on the PEC11 Encoder
     * @param twi (TwoWire*): I2C Comms object for the display
     */
    EncoderKnob(uint8_t encoder_pinA, uint8_t encoder_pinB, uint8_t encoder_pinC, TwoWire *twi);

    /**
     * @brief Completes the initialization tasks for the display
     *
     * @param display_i2c_addr (uint8_t): I2C Address for the Display, defaults to 0x3C
     * @return true: Upon successful initialization of the display
     * @return false: Upon failure to initialize
     */
    bool begin(uint8_t display_i2c_addr);

    /**
     * @brief Get the current reading from the encoder
     *
     * @return uint32_t: The most recent encoder value
     */
    uint32_t getReading(void);

    /**
     * @brief Updates the Screen with the most recent encoder value
     *
     */
    void update(void);
};