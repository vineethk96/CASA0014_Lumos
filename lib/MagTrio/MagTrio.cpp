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

#define RED_MIN 0
#define RED_MAX 0.37
#define GREEN_MIN 0
#define GREEN_MAX 0.37
#define BLUE_MIN 0
#define BLUE_MAX 0.37

#define MIN_SAT 0
#define MAX_SAT 255

void MagTrio::updateArray(mag_t magSel, float reading)
{
    float *arr = nullptr;     // Pointer to simplify access to the array
    float *currSum = nullptr; // Pointer to the current sum value

    // Reduces repeated code by linking the appropriate mag using pointers
    switch (magSel)
    {
    case MAG_R:
        arr = &redArr[0];
        currSum = &red;
        break;
    case MAG_G:
        arr = &greenArr[0];
        currSum = &green;
        break;
    case MAG_B:
        arr = &blueArr[0];
        currSum = &blue;
        break;
    }

    // Check if this is the first run
    if (arr[newEntry] != 0)
    {
        // We need to remove the old value from the sum
        *currSum -= arr[newEntry];
    }

    // Add new reading to the total
    *currSum += reading;

    // Update the tracking array
    arr[newEntry] = reading;
}

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
    reading = abs(mag.getAmount());

    switch (currMag)
    {
    case MAG_R:
        // Serial.print("Raw: ");
        // Serial.println(reading);
        updateArray(MAG_R, reading);
        // Serial.print("Red Avg: ");
        // Serial.println(red / ROLLING_AVG_SIZE);
        break;
    case MAG_G:
        updateArray(MAG_G, reading);
        // Serial.print("Green Avg: ");
        // Serial.println(green / ROLLING_AVG_SIZE);
        break;
    case MAG_B:
        updateArray(MAG_B, reading);
        // Serial.print("Blue Avg: ");
        // Serial.println(blue / ROLLING_AVG_SIZE);
        newEntry = (newEntry < ROLLING_AVG_SIZE) ? newEntry + 1 : 0;
        break;
    }

    delay(500);
    i2cMux.closeChannel(currMag);

    currMag = (currMag < MAG_B) ? (currMag + 1) : MAG_R;
}

uint8_t MagTrio::getRed(void)
{
    float avg = red / ROLLING_AVG_SIZE;
    Serial.print("Red Avg: ");
    Serial.println(avg);
    return (avg > RED_MAX) ? MAX_SAT : map(avg, RED_MIN, RED_MAX, MIN_SAT, MAX_SAT);
}

uint8_t MagTrio::getGreen(void)
{
    float avg = green / ROLLING_AVG_SIZE;
    Serial.print("Green Avg: ");
    Serial.println(avg);
    return (avg > GREEN_MAX) ? MAX_SAT : map(avg, GREEN_MIN, GREEN_MAX, MIN_SAT, MAX_SAT);
}

uint8_t MagTrio::getBlue(void)
{
    float avg = blue / ROLLING_AVG_SIZE;
    Serial.print("Blue Avg: ");
    Serial.println(avg);
    return (avg > BLUE_MAX) ? MAX_SAT : map(avg, BLUE_MIN, BLUE_MAX, MIN_SAT, MAX_SAT);
}
