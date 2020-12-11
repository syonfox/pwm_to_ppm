//#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
//#include <ArduinoOTA.h>
//#include <WiFiUdp.h>

//#include <WebSocketsServer.h>
//#include <DNSServer.h>
//#include <Hash.h>
//#include "MSP.h"

/* Set these to your desired credentials. */

#define CPU_MHZ 80
#define CHANNEL_NUMBER 8  //set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1100  //set the default servo value
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 0  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 5 //set PPM signal output pin on the arduino
//#define DEBUGPIN 4



#define PWM_NUMBER 5 // the number of pwm input chanals

//working  14, 12, 13, 4
//pin name d5, d6, d7, d2
//gpio 2 = d4 ##README## this pin must be disconected during bootup

const unsigned int PWM_PINS[] = {14, 12, 13, 4 ,2};

volatile unsigned long next;
volatile unsigned int ppm_running = 1;

int ppm[CHANNEL_NUMBER];

//ppm
volatile int pwm_value = 0;
volatile int prev_time = 0;
//uint8_t latest_interrupted_pin;
volatile int pwm_time[PWM_NUMBER];//array for tracking the timeings of each pwm pin
volatile int pwm[PWM_NUMBER]; // an array for storing the current pwm values
volatile int current_pwm_pin = 0;


ICACHE_RAM_ATTR void risingPWM() {
//    pwm_time[current_pwm_pin] = micros();
    prev_time = micros();
//    Serial.print("PIN");
//    Serial.print(current_pwm_pin);
//    Serial.print(": Rise");
//    Serial.print("\n");
    attachInterrupt(PWM_PINS[current_pwm_pin], fallingPWM, FALLING);
}

ICACHE_RAM_ATTR void fallingPWM() {

    ppm[current_pwm_pin] = micros() - prev_time;
//    Serial.print("PIN");
//    Serial.print(current_pwm_pin);
//    Serial.print(": Fall");
//    Serial.print("\n");

//    ppm[current_pwm_pin] = micros() - pwm_time[current_pwm_pin];
    Serial.print("PIN");
    Serial.print(current_pwm_pin);
    Serial.print(":");
    Serial.print(ppm[current_pwm_pin]);
    Serial.print(",");

    if (current_pwm_pin == PWM_NUMBER - 1) {
        Serial.print("\n");
    }
    detachInterrupt(PWM_PINS[current_pwm_pin]);
    current_pwm_pin = (current_pwm_pin + 1) % PWM_NUMBER;

    attachInterrupt(PWM_PINS[current_pwm_pin], risingPWM, RISING);
}


void inline ppmISR(void) {
    static boolean state = true;

    if (state) {  //start pulse
        digitalWrite(sigPin, onState);
        next = next + (PULSE_LENGTH * CPU_MHZ);
        state = false;
//        alivecount++;
    } else {  //end pulse and calculate when to start the next pulse
        static byte cur_chan_numb;
        static unsigned int calc_rest;

        digitalWrite(sigPin, !onState);
        state = true;

        if (cur_chan_numb >= CHANNEL_NUMBER) {
            cur_chan_numb = 0;
            calc_rest = calc_rest + PULSE_LENGTH;//
            next = next + ((FRAME_LENGTH - calc_rest) * CPU_MHZ);
            calc_rest = 0;
//            digitalWrite(DEBUGPIN, !digitalRead(DEBUGPIN));
        } else {
            next = next + ((ppm[cur_chan_numb] - PULSE_LENGTH) * CPU_MHZ);
            calc_rest = calc_rest + ppm[cur_chan_numb];
            cur_chan_numb++;
        }
    }
    timer0_write(next);
}


void setup() {
    pinMode(sigPin, OUTPUT);
    digitalWrite(sigPin, !onState); //set the PPM signal pin to the default state (off)

    //Setup each pwm input pin
    for (int i = 0; i < PWM_NUMBER; i++) {
        pinMode(PWM_PINS[i], INPUT);
    }

    noInterrupts();

    //initiate
    attachInterrupt(digitalPinToInterrupt(PWM_PINS[0]), risingPWM, FALLING);

    timer0_isr_init();
    timer0_attachInterrupt(ppmISR);
    next = ESP.getCycleCount() + 1000;
    timer0_write(next);
    for (int i = 0; i < CHANNEL_NUMBER; i++) {
        ppm[i] = CHANNEL_DEFAULT_VALUE;
    }
    interrupts();
    Serial.begin(115200);
//    msp.begin(Serial);
}

unsigned long time_now = 0;

void loop() {
//
//    int time = millis();
//    ppm[0] = pwm[0];
////    ppm[0] = 1000 + (time % 10000) / 10;
//    ppm[1] = 1000 + ((time + 500) % 10000) / 10;
//
////
////    for (int i = 0; i < numInputs; i++) {
////        pinMode(inputPins[i], INPUT);
////    }
//
//
//
//    for (int i = 2; i < CHANNEL_NUMBER; i++) {
//
//        ppm[i] = 1500;
////        }
//    }


//    yield();
}
