/* Code by Marek Mach. If you have any problems with the code or you have ideas for improoving the code, just DM me on twitter @marekmach24 .
   Sorry for my bad English.
   This code generates a clock signal that is 0-5V square.
   The number that you see on the display are BPM and the serial connection is displaying the delay between ticks.
   If you don`t know how to connect the display, I`d suggest you take look at the sevseg github (https://github.com/DeanIsMe/SevSeg).
   This code was tested and built on arduino pro mini so it should also work on arduino nano.
   I am using herere the analog pins as digital pins so to clarify it here is a list on what analog pins corespond to digital pins:A0 = 14
                                                                                                                                   A1 = 15
                                                                                                                                   A2 = 16
                                                                                                                                   A3 = 17
                                                                                                                                   A4 = 18
                                                                                                                                   A5 = 19
                                                                                                                                   A6 = doesn`t
                                                                                                                                   A7 = doesn`t
   The encoder, display and output connections are described in the code.
   My Github : https://github.com/machmar
   Marek Mach 2018 Licensed under GNU GPLv3.
*/

#include <Rotary.h>
#include <SevSeg.h>
#include <OneButton.h>
SevSeg sevseg;
Rotary encoder = Rotary(14, 2); // select pins that is the encoder connected to (the order doen`t matter)

int output = 16;  //clock output (you can also connect an LED with a 1k resistor to signalize the bpm)
int reverseDelay; //this is the variable that controls the delay between output being high and low
int bpm = 1;  //
int bpmEncoderRotation = 0;
int outputState = LOW;
int menuVar1 = 0;
int menuVar2 = 0;
int menuVar3 = 0;
int menuVar4 = 0;
int menuEncoderVar1 = 0;
int menuEncoderVar2 = 0;
int decimalPoint = 0;
int normalClick = 0;
OneButton coderButton(17, true);
unsigned long previousMillis2 = 0;

unsigned long previousMillis = 0;

void bpmTick() { //this is a copy of blink without delay arduino example - only the delay is a variable instead of a constant value
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
  bpm = bpmEncoderRotation / 1 + 100;
  sevseg.setNumber(bpm);
  sevseg.refreshDisplay();
  reverseDelay = 60000 / bpm;
}

void singleClick() {
  if (menuVar2 == 0) menuVar1++;
  if (menuVar2 >= 1) {
    menuVar3++;
    if (normalClick == 0) {
      normalClick = 1;
      return;
    }
    if (normalClick == 1) {
      normalClick = 0;
      return;
    }
  }
}

void longClick() {
  menuVar2++;
}

void setup() {

  byte numDigits = 4; //number of digits, if you want to add more digits than change this number and add the pin that you connected the digit pin to the digitPins below
  byte digitPins[] = {15, 3, 4, 5}; //the pins that ar the digit pins connected to, left digit is on the left and than it goes on and on for the rest of the leds
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};  //these are the pins where are the segments connected to, it goes in alphabetical order from left to right
  sevseg.setBrightness(0); //higher brightnes means more flicker of the screen, I don`t know why
  //6 is pin A; 7 is pin B and so on, the dot is the last pin - 13

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);  //if you have a display with common cathode than change the COMMON_ANODE to COMMON_CATHODE
  attachInterrupt (2, countChange, CHANGE); //this is the interrupt for the encoder that activate the countChange void when you move the encoder
  //Serial.begin(9600); //starts serial connection

  coderButton.attachClick(singleClick);
  coderButton.attachLongPressStop(longClick);

  pinMode(output, OUTPUT);
}

void loop() {
  coderButton.tick();
  unsigned char result = encoder.process();
  bpm = bpmEncoderRotation / 1 + 100; //this converts the raw encoder data to usable data, the /2 is there because when there was nothing my encoder
  //was adding four numbers per step instead of one
  //the +100 sets the bpm that the arduino starts with - change it to make arduino start with different bpm

  if (menuVar2 == 0) {
    sevseg.setNumber(bpm, decimalPoint); //this sets what is the display supposed to show
    sevseg.refreshDisplay();  // this refreshes the display
  }

  coderButton.tick();

  //Serial.print(normalClick);
  //Serial.print(",");
  //Serial.println(menuVar2);//this is to monitor the bpm to millis conversion, if you comment it back in than the display will flicker

  reverseDelay = 60000 / bpm; //this converts the numbers on the displys into millisecconds like the numbers are bpm - http://guitargearfinder.com/guides/convert-ms-milliseconds-bpm-beats-per-minute-vice-versa/
  bpmTick(); // this is just that I don`t have to make void loop visualy longer but it is the same as writing the tick void in void loop

  if (menuVar2 == 0) {
    if (menuVar1 == 0) {
      decimalPoint = 0;
      if (result == DIR_CW) {
        bpmEncoderRotation = bpmEncoderRotation + 1;
      } else if (result == DIR_CCW) {
        bpmEncoderRotation = bpmEncoderRotation - 1;
      }
    }

    if (menuVar1 == 1) {
      decimalPoint = 1;
      if (result == DIR_CW) {
        bpmEncoderRotation  = bpmEncoderRotation + 10;
      } else if (result == DIR_CCW) {
        bpmEncoderRotation  = bpmEncoderRotation - 10;
      }
    }

    if (menuVar1 == 2) {
      decimalPoint = 2;
      if (result == DIR_CW) {
        bpmEncoderRotation  = bpmEncoderRotation + 100;
      } else if (result == DIR_CCW) {
        bpmEncoderRotation  = bpmEncoderRotation - 100;
      }
    }

    if (menuVar1 == 3) {
      decimalPoint = 3;
      if (result == DIR_CW) {
        bpmEncoderRotation  = bpmEncoderRotation + 1000;
      } else if (result == DIR_CCW) {
        bpmEncoderRotation  = bpmEncoderRotation - 1000;
      }
    }
    if (menuVar1 == 4) {
      decimalPoint = 2;
      if (result == DIR_CW) {
        bpmEncoderRotation = bpmEncoderRotation + 100;
      } else if (result == DIR_CCW) {
        bpmEncoderRotation = bpmEncoderRotation - 100;
      }
    }
    if (menuVar1 == 5) {
      decimalPoint = 1;
      if (result == DIR_CW) {
        bpmEncoderRotation  = bpmEncoderRotation + 10;
      } else if (result == DIR_CCW) {
        bpmEncoderRotation  = bpmEncoderRotation - 10;
      }
    }
    if (menuVar1 == 6) menuVar1 = 0;
  }

  if (menuVar2 == 1) {
    normalClick = 0;
    menuEncoderVar1 = 0;
    menuVar2 = 2;
  }

  if (menuVar2 == 2) {
    if (result == DIR_CW) {
      menuEncoderVar1 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar1 --;
    }
    if (menuEncoderVar1 == -1) {
      sevseg.setChars("RST");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation=0;
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar1 == 0) {
      sevseg.setChars("men1");
      sevseg.refreshDisplay();
    }
    if (menuEncoderVar1 == 1) {
      sevseg.setChars("men2");
      sevseg.refreshDisplay();
    }
    if (menuEncoderVar1 == 2) {
      sevseg.setChars("men3");
      sevseg.refreshDisplay();
    }
    if (menuEncoderVar1 == 3) {
      sevseg.setChars("SET");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 3;
      }
    }
    if (menuEncoderVar1 == 4) {
      sevseg.setChars("CANC");
      sevseg.refreshDisplay();
      if (normalClick == 1) menuVar2 = 0;
      normalClick = 0;
    }

    if (menuEncoderVar1 == 5) menuEncoderVar1 = 4;
    if (menuEncoderVar1 == -2) menuEncoderVar1 = -1;
  }
  if (menuVar2 == 3) {
    menuEncoderVar2 = 0;
    normalClick = 0;
    menuVar2 = 4;
  }

  if (menuVar2 == 4) {
    if (result == DIR_CW) {
      menuEncoderVar2 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar2 --;
    }
    if (menuEncoderVar2 == 0) {
      sevseg.setNumber(100);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpm = 123;
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 1) {
      sevseg.setNumber(140);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 40;
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 2) {
      sevseg.setNumber(220);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 120;
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 3) {
      sevseg.setNumber(480);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 380;
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 4) {
      sevseg.setNumber(320);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 220;
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 5) menuEncoderVar2 = 0;
  }
  if (menuVar2 == 10) menuVar2 = 0;
}




