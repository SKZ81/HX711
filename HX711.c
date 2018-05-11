#include <HX711.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

// static uint8_t PD_SCK;    // Power Down and Serial Clock Input Pin
// static uint8_t DOUT;// Serial Data Output Pin
static uint8_t GAIN;// amplification factor
static long OFFSET = 0;    // used for tare weight
static float SCALE = 1;    // used to return weight in grams, kg, ounces, whatever

#define clock_high() (PORT_CLK |=  (1 << PIN_CLK))
#define clock_low()  (PORT_CLK &= ~(1 << PIN_CLK))
#define get_DOUT()   ((READ_PORT_DOUT & (1 << PIN_DOUT)) >> PIN_DOUT)

void HX711_init(uint8_t gain) {
//     PD_SCK = pd_sck;
//     DOUT = dout;

//    pinMode(PD_SCK, OUTPUT);
    DDR_CLK |= (1 << PIN_CLK);
//    pinMode(DOUT, INPUT);
    DDR_DOUT &= ~(1 << PIN_DOUT);
    HX711_set_gain(gain);
}

uint8_t HX711_is_ready() {
    //return digitalRead(DOUT) == LOW;
    return (get_DOUT() == 0);
}

void HX711_set_gain(uint8_t gain) {
    switch (gain) {
        case 128:        // channel A, gain factor 128
            GAIN = 1;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 3;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 2;
            break;
    }

    //digitalWrite(PD_SCK, LOW);
    clock_low();
    HX711_read();
}



long HX711_read() {
    // wait for the chip to become ready
    while (!HX711_is_ready()) {
        _NOP();
    }

    unsigned long value = 0;
    uint8_t data[3] = { 0 };
    uint8_t filler = 0x00;

    // pulse the clock pin 24 times to read the data
    for(int8_t n = 2; n>=0; n--) {
        for(int8_t i=7; i>=0; i--) {
            clock_high();
            _delay_us(1); // let some time to hx711 to update output value
            data[n] |= get_DOUT() << i;
            clock_low();
        }
    }
    // set the channel and the gain factor for the next reading using the clock pin
    for (unsigned int i = 0; i < GAIN; i++) {
        clock_high();
        _delay_us(1); // let some time to hx711 to understand the command
        clock_low();
    }

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if (data[2] & 0x80) {
        filler = 0xFF;
    } else {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = ( (unsigned long)(filler) << 24
            | (unsigned long)(data[2]) << 16
            | (unsigned long)(data[1]) << 8
            | (unsigned long)(data[0]) );

    return (long)(value);
}

long HX711_read_average(uint8_t times) {
    long sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += HX711_read();
//         yield();
    }
    return sum / times;
}

double HX711_get_value() {
    return HX711_read_average(1) - OFFSET;
}

double HX711_get_mean_value(uint8_t times) {
    return HX711_read_average(times) - OFFSET;
}

float HX711_get_units() {
    return HX711_get_value() / SCALE;
}

float HX711_get_mean_units(uint8_t times) {
    return HX711_get_mean_value(times) / SCALE;
}

void HX711_tare(uint8_t times) {
    double sum = HX711_read_average(times);
    HX711_set_offset(sum);
}

void HX711_set_scale(float scale) {
    SCALE = scale;
}

float HX711_get_scale() {
    return SCALE;
}

void HX711_set_offset(long offset) {
    OFFSET = offset;
}

long HX711_get_offset() {
    return OFFSET;
}

void HX711_power_down() {
    clock_low();
    clock_high();
}

void HX711_power_up() {
    clock_low();
}
