/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
 outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.
 
 Setup your scale and start the sketch WITHOUT a weight on the scale
 Once readings are displayed place the weight on the scale
 Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
 Use this calibration_factor on the example sketch
 
 This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
 calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).
 
 Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
 and the direction the sensors deflect from zero state

 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND
 
 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
 
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
 
*/

#include "HX711.h"
#include "avr_uart.h"
#include <util/delay.h>

float calibration_factor = 12800;

void init(void) {
    avr_uart_init();
    stdout = &avr_uart_output;
    stdin  = &avr_uart_input_echo;
    HX711_init(128);
}

int main(void) {
    init();

    printf("HX711 calibration sketch\n");
    printf("Remove all weight from scale\n");
    printf("After readings begin, place known weight on scale\n");
    printf("Press + or a to increase calibration factor\n");
    printf("Press - or z to decrease calibration factor\n");

    HX711_set_scale(1.0);
    HX711_tare(10);	//Reset the scale to 0

    long zero_factor = HX711_read_average(10); //Get a baseline reading
    printf("Zero factor: %ld", zero_factor); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.

    while(1) {
        HX711_set_scale(calibration_factor); //Adjust to this calibration factor

        printf("Reading: %.2f Kg calibration_factor: %f\n", HX711_get_mean_units(1), calibration_factor);

        if(avr_uart_data_available())
        {
            char temp = avr_uart_getchar(stdin);
            if(temp == '+' || temp == 'a')
                calibration_factor += 10;
            else if(temp == '-' || temp == 'z')
                calibration_factor -= 10;
        }

        _delay_ms(500);
    }
}
