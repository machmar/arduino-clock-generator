/* Code by Marek Mach. If you have any problems with the code or you have ideas for improoving the code, just DM me on twitter @marekmach24 .
 * Sorry for my bad English.
 * This code generates a clock signal that is 0-5V square.
 * The number that you see on the display are BPM and the serial connection is displaying the delay between ticks.
 * If you don`t know how to connect the display, I`d suggest you take look at the sevseg github (https://github.com/DeanIsMe/SevSeg).
 * This code was tested and built on arduino pro mini so it should also work on arduino nano.
 * I am using herere the analog pins as digital pins so to clarify it here is a list on what analog pins corespond to digital pins:A0 = 14
 *                                                                                                                                 A1 = 15
 *                                                                                                                                 A2 = 16
 *                                                                                                                                 A3 = 17
 *                                                                                                                                 A4 = 18
 *                                                                                                                                 A5 = 19
 *                                                                                                                                 A6 = 20
 *                                                                                                                                 A7 = 21
 * The encoder, display and output connections are described in the code.
 * My Github : https://github.com/machmar 
 * Marek Mach 2018 Licensed under GNU GPLv3.
 */

#include <Encoder.h>  //include encoder library
#include <SevSeg.h> //include sevn segment library

SevSeg sevseg;
Encoder encoder(14, 2); // select pins that is the encoder connected to (the order doen`t matter)

int output = 16;  //clock output (you can also connect an LED with a 1k resistor to signalize the bpm)
int reverseDelay; //
int encoderData = 100;
int outputState = LOW;

unsigned long previousMillis = 0;

void setup() {

  byte numDigits = 4; //number of digits, if you want to add more digits than change this number and add the pin that you connected the digit pin to the digitPins below
  byte digitPins[] = {15, 3, 4, 5}; //the pins that ar the digit pins connected to, left digit is on the left and than it goes on and on for the rest of the leds
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};  //these are the pins where are the segments connected to, it goes in alphabetical order from left to right
                                                      //6 is pin A-7 is pin B and so on, the dot is the last pin - 13

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);  //if you have a display with common cathode than change the COMMON_ANODE to COMMON_CATHODE
  sevseg.setBrightness(10); //higher brightnes means more flicker of the screen, I don`t know why
  attachInterrupt (2, countChange, CHANGE); //this is the interrupt for the encoder that activate thecountChange void when you move the encoder

  Serial.begin(9600); //starts on serial connection

  pinMode(output, OUTPUT);
}

long positionLeft  = -999; //I don`t know what is this but it was in the example sketch.

void loop() {
  encoderData = encoder.read() / 4 + 100; //this converts the raw encoder data to usable data, the /4 is there because when there was nothing my encoder 
                                          //was adding four numbers per step instead of one
                                          //the +100 sets the bpm that the arduino starts with - change it to make arduino start with different bpm
  
  sevseg.setNumber(encoderData); //this sets what is the display supposed to show
  sevseg.refreshDisplay();  // this refreshes the display

  //Serial.println(reverseDelay); //this is to monitor the bpm to millis conversion, if you comment it back in than the display will flicker

  reverseDelay = 60000 / encoderData; //this converts the numbers on the displys into millisecconds like the numbers are bpm - http://guitargearfinder.com/guides/convert-ms-milliseconds-bpm-beats-per-minute-vice-versa/
  tick(); // this is just that I don`t have to make void loop visualy longer but it is the same as writing the tick void in void loop
}

void tick() { //this is a copy of blink without delay arduino example - only the delay is a variable instead of a constant value
  long interval = reverseDelay / 4 ;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

        if (outputState == LOW) {
      outputState = HIGH;
    } else {
      outputState = LOW;
    }

    digitalWrite(output, outputState);
  }
}
void countChange() { //this is an interrupt part that reacts to the encoder immediately but it is just a copy of void loop without serial and tick part
  encoderData = encoder.read() / 4 + 100;
  sevseg.setNumber(encoderData);
  sevseg.refreshDisplay();
  reverseDelay = 60000 / encoderData;
}

