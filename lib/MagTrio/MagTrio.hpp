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

#define ROLLING_AVG_SIZE 5

class MagTrio
{
private:
    typedef enum magType
    {
        MAG_R,
        MAG_G,
        MAG_B
    } mag_t; // Enum adds clarity to which mag is being read

    TCA9548A i2cMux; // I2C Mux Object
    Tlv493d mag;     // Magnetometer Object (Single object can be used across multiple mags)

    // Tracks the net sum
    float red = 0;
    float green = 0;
    float blue = 0;

    // Rolling Avg Arrays
    float redArr[ROLLING_AVG_SIZE] = {0};
    float greenArr[ROLLING_AVG_SIZE] = {0};
    float blueArr[ROLLING_AVG_SIZE] = {0};

    // Tracks the location in the array for the new value.
    uint8_t newEntry = 0;

    void updateArray(mag_t magSel, float reading);

public:
    MagTrio(TwoWire i2cBus = Wire);

    void begin(void);

    void update(void);

    uint8_t getRed(void);

    uint8_t getGreen(void);

    uint8_t getBlue(void);
};