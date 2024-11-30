/**
 * @file MagTrio.hpp
 * @author Vineeth Kirandumkara
 * @brief A library to encapsulate the actions of the trio of magnetometers and their I2C Mux
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Wire.h>
#include <TCA9548A.h>
#include <Tlv493d.h>

class MagTrio
{
private:
    enum
    {
        MAG_R,
        MAG_G,
        MAG_B
    } mag_t;

    TCA9548A i2cMux; // I2C Mux Object
    Tlv493d mag;     // Magnetometer Object (Single object can be used across multiple mags)

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

public:
    MagTrio(TwoWire i2cBus = Wire);

    void begin(void);

    void update(void);

    uint8_t getRed(void);

    uint8_t getGreen(void);

    uint8_t getBlue(void);
};