HX711
=====

A pure-AVR library to interface the Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC) for Weight Scales.

Datasheet for the chip is here :
http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf


NOTE : this code is a pure-AVR C rewrite of the following code :
https://github.com/bogde/HX711

Even this file was originally part of the former repository/project.
NOTE : as written, the code is designed to manage ONE (and only one) instance of HX711

# What I did
1. switch to plain C functions instead of C++ class. No more ctor / begin method : replaced by

2. Remove any reference to arduino API (mainly calls to `digitalRead()` / `digitalWtrite()`)

3. provide some pin configuration facility (not very neat, but working) using C defines.


# TODO
1. Change the pin configuration facility, requesting the user to provide PORT as a letter, and PIN as a number

2. Provide a plain old makefile instead of using generic 'Arduino-Makefile'

3. Implement / enable managing multiple instances of HX711


NOTE : following lines are adapted from original project

# Advantages of this library
Although other libraries exist, I needed a slightly different approach, so here's how my library is different than others:

1. It provides a `HX711_tare()` function, which "resets" the scale to 0. Many other implementations calculate the tare weight when the ADC is initialized only. I needed a way to be able to set the tare weight at any time. Use case: place an empty container on the scale, call `HX711_tare()` to reset the readings to 0, fill the container and get the weight of the content.

2. It provides a `HX711_power_down()` function, to put the ADC into a low power mode. According to the datasheet, "When PD_SCK pin changes from low to high and stays at high for longer than 60μs, HX711 enters power down mode". Use case: battery powered scales. Accordingly, there is a `HX711_power_up()` function to get the chip out of the low power mode.

3. It has a `HX711_set_gain(uint8_t gain)` function that allows you to set the gain factor and select the channel. According to the datasheet, "Channel A can be programmed with a gain of 128 or 64, corresponding to a full-scale differential input voltage of ±20mV or ±40mV respectively, when a 5V supply is connected to AVDD analog power supply pin. Channel B has a fixed gain of 32.". The same function is used to select the channel A or channel B, by passing 128 or 64 for channel A, or 32 for channel B as the parameter. The default value is 128, which means "channel A with a gain factor of 128", so one can simply call `HX711_set_gain()`. Also, the function is called from the init function.

4. The constructor has an extra parameter "gain" that allows you to set the gain factor and channel. The constructor calls the "set_gain" function mentioned above.

5. The "get_value" and "get_units" functions can receive an extra parameter "times", and they will return the average of multiple readings instead of a single reading.

# How to Calibrate Your Scale

1. Call `HX711_set_scale(1.0)`.
2. Call `HX711_tare(10)` (or any other value, but 10 should be OK).
3. Place a known weight on the scale and `HX711_call get_mean_units(10)`.
4. Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale.
5. Adjust the parameter in step 4 until you get an accurate reading.

# How to use
See the example in examples/HX711_avrc/main.c
NB : it uses [Arduino-Makefile](https://github.com/sudar/Arduino-Makefile) to build, but should be easy to adapt
