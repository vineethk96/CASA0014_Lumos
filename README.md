# CASA0014_Lumos

<p align="center">
    <img src="/docs/lightWall.jpg" alt="drawing" width="500"/>
</p>

### Description

Lumos is an User Interface that allows individuals to control individual lights on the Light Wall.

The Light Wall works by using an MQTT Server that listens to incoming messages to the Topic `mqtt.cetools.org/student/CASA0014/light/`
These messages can identify which LED Node to select by adding the corresponding number to the topic, as well as exactly which pixel to illuminate and it's color within the message body as so:

```
{
"pixelid": ledNode,
"R": red_byte,
"G": green_byte,
"B": blue_byte,
"W": white_byte
}
```

To interact with the Light Wall, Lumos uses a collection of 3 TLV493D Magnetometers to read the distance a magnet is from them to create a light value between 0 and 255. Each of the 3 magnetometers are designated Red, Green, and Blue, and the magnet itself is held within the wand.
Users can also select a specific LED Node to light up using the dial to the left of the ring.

## Features

- LED Node Selection via Dial
- Color Changeability via Ring and Wand

## Contents

- Hardware Used
  - Areas for Improvement
- Software Overview
  - Installation and Running
  - Architecture
  - Algorithms
  - Areas for Improvement
- Inspiration and Helpful Links

## Hardware

- Arduino MKR WIFI 1010 (Microcontroller)
- TLV493D (Magnetometer)
- TCA9548A (I2C Multiplexer)
- PEC11 (Rotary Encoder)
- SSD1306 (OLED Display)

### Areas for Improvement

The major area for improvement is with the TLV493D magnetometers. They are not strong enough to detect a proper magnetic disturbance outside of 3-4cm away.

## Software

### Installation and Running

This project was developed using the PlatformIO extension on Visual Studio Code. To start with users must install both items, which can be found at the below links:

- [Visual Studio Code](https://code.visualstudio.com/download)
- [PlatformIO IDE](https://platformio.org/platformio-ide)

Additionally, the following libraries must be installed:

```
	arduino-libraries/WiFiNINA@^1.8.14
	adafruit/Adafruit NeoPixel@^1.12.3
	adafruit/Adafruit GFX Library@^1.11.11
	adafruit/Adafruit SSD1306@^2.5.13
	adafruit/Adafruit BusIO@^1.16.2
	paulstoffregen/Encoder@^1.4.4
	bblanchon/ArduinoJson@^7.2.0
	knolleary/PubSubClient@^2.8
	infineon/TLV493D-Magnetic-Sensor@^1.0.4
	wifwaf/TCA9548A@^1.1.3
```

Once the above applications, extensions, and libraries have been installed, the application can be run by clicking the Arrow button at the bottom of the screen, and opening the Serial Monitor with the Plug.

#### Quick PlatformIO Info:

##### Install Libraries

Once PlatformIO is installed, an alien head should pop up on the left toolbar
![PlatformIO_Logo](/docs/PlatformIO_Logo.png)

Click the head, and on the left bar there will be a `Quick Access` table, and under `PIO Home` there will be a `Libraries` entry.
From here, you can install all the libraries.

##### Build and Running Applications

At the bottom toolbar you will see a couple new items.

- The Checkmark will build your code.
- The Arrow will upload your code to the microcontroller
- The Plug will open the Serial Terminal

### Architecture

#### Hardware

![Basic Circuit](/docs/arch_diagram.drawio.png)

#### Software

At a high-level everything is broken down by it's physical functionality. The project itself is comprised of 2 parts: the dial, and the ring. As such, the `EncoderKnob` library contains everything to communicate with the components of the ring, those being the PEC11 Rotary Encoder, and the SSD1306 OLED Display. Equally the ring is simply the readings from the 3 magnetometers, and the mapping.

### Algorithms

Two simple algorithms were build for this application. One for rotating between I2C Channels on the Mux, and the other for cleaning up the incoming readings from the magnetometers to get a more meaningful and accurate representation of the data.

#### I2C Switching

The TCA9548A multiplexer works by allowing the user access to 1 of it's 8 possible I2C channels at a single time. Therefore, when interfacing with multiple components with the same I2C Address, I designed a simple, but effective method to switch between channels, while also saving memory. This works by making a single generic magnetometer object, and using a enumerated value to iterate between the channels.

#### Input Data Cleanup

As a result of using this magnetometer for the project, the recorded strength of the magnet significantly dropped off when about 5 to 6 cm away. This paired with the speed and switching of device readings, led to very inconsistent readings which didn't make a lot of sense to what the users would percieve. To solve this, I applied a rolling average algorithm, along with a different mapping to adjust the values. The current issue with this, is since the values are so low at around 5 to 6 cm away, the color saturation really just transitions from on to off, without much of a gradient betwen them. This can be cleaned up with a magnetometer with a larger field of reading, or even more powerful magnets.

### Areas for Improvement

The primary area would be to clean up the input mappings, also selecting magnetometers with an adjustable I2C address would remove the need for the mux, and simplify the code, while providing faster readings.
Also, there was an attempt at creating a library to quickly create and send the MQTT messages, however after testing, using the MQTT message to change the whole light at once added too much extra complexity for an inconsistent output.

## Inspiration and Helpful Links

- https://www.instructables.com/OLED-I2C-DISPLAY-WITH-ARDUINO-Tutorial/
- https://www.instructables.com/Arduino-and-the-SSD1306-OLED-I2C-128x64-Display/
- https://github.com/scottbez1/smartknob/tree/master/cad
- https://www.instructables.com/Hackable-Rotary-Control-With-Built-in-Display-a-Te/
- https://www.thingiverse.com/thing:2641409
