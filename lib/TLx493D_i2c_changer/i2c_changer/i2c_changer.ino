/**
 * @file changeI2C_TLx493D.cpp
 * @author Vineeth Kirandumkara
 * @brief Quick .ino file that is used to change the i2c address
 * @version 0.1
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Tlv493d.h>

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Tlv493dMagnetic3DSensor.begin(Wire, TLV493D_ADDRESS3, true);
}

void loop() {
  Tlv493dMagnetic3DSensor.updateData();
  delay(100);

  Serial.print("X = ");
  Serial.print(Tlv493dMagnetic3DSensor.getX());
  Serial.print(" mT; Y = ");
  Serial.print(Tlv493dMagnetic3DSensor.getY());
  Serial.print(" mT; Z = ");
  Serial.print(Tlv493dMagnetic3DSensor.getZ());
  Serial.println(" mT");
   
  delay(500);
}
