/**
 * @file MagTrio.cpp
 * @author Vineeth Kirandumkara
 * @brief Library Definitions for the MagTrio
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "MagTrio.hpp"

#define RED_MIN 0.1
#define RED_MAX 60
#define GREEN_MIN 0.1
#define GREEN_MAX 60
#define BLUE_MIN 0.1
#define BLUE_MAX 60

#define MIN_SAT 0
#define MAX_SAT 255

MagTrio::MagTrio(TwoWire i2cBus)
{
}

void MagTrio::begin(void)
{
    // Initialize the Mux
    i2cMux.begin(Wire);
    i2cMux.closeAll();

    // Iterate and Initialize all Magnetometers
    for (uint8_t i = MAG_R; i < MAG_B; i++)
    {
        i2cMux.openChannel(i);
        mag.begin();
        i2cMux.closeChannel(i);
    }
}

void MagTrio::update(void)
{
    static uint8_t currMag = MAG_R;
    float reading = 0;

    i2cMux.openChannel(currMag);
    mag.updateData();
    delay(100);
    reading = mag.getAmount();

    switch (currMag)
    {
    case MAG_R:
        red = map(reading, RED_MIN, RED_MAX, MIN_SAT, MAX_SAT);
        break;
    case MAG_G:
        green = map(reading, GREEN_MIN, GREEN_MAX, MIN_SAT, MAX_SAT);
        break;
    case MAG_B:
        blue = map(reading, BLUE_MIN, BLUE_MAX, MIN_SAT, MAX_SAT);
        break;
    }

    delay(500);
    i2cMux.closeChannel(currMag);

    currMag = (currMag < MAG_B) ? (currMag + 1) : MAG_R;
}

uint8_t MagTrio::getRed(void)
{
    return red;
}

uint8_t MagTrio::getGreen(void)
{
    return green;
}

uint8_t MagTrio::getBlue(void)
{
    return blue;
}
