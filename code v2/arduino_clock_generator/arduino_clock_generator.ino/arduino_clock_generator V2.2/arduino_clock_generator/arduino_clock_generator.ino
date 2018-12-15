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
#include <EEPROM.h>
SevSeg sevseg;
Rotary encoder = Rotary(14, 2); // select pins that is the encoder connected to (the order doen`t matter)

int output = 16;  //clock output (you can also connect an LED with a 1k resistor to signalize the bpm)
int reverseDelay; //this is the variable that controls the delay between output being high and low
int bpm = 1;  //
int bpmEncoderRotation = 0;
int startingValue = 100;
int outputState = LOW;
int menuVar1 = 0;
int menuVar2 = 0;
int menuVar3 = 0;
int menuVar4 = 0;
int menuEncoderVar1 = 0;
int menuEncoderVar2 = 0;
int menuEncoderVar3 = 0;
int menuEncoderVar4 = 0;
int menuEncoderVar5 = 0;
int menuEncoderVar6 = 0;
int menuEncoderVar7 = 0;
int menuEncoderVar8 = 0;
int decimalPoint = 0;
int displayBpmMode = 0;
int normalClick = 0;
int bpmMultiply = 1;
int bpmDivide = 1;
int swing = 50;
int swingVar = 0;
OneButton coderButton(17, true);
unsigned long previousMillis0 = 0;

unsigned long previousMillis1 = 0;

void bpmTick() { //this is a copy of blink without delay arduino example - only the delay is a variable instead of a constant value
  unsigned long currentMillis = millis();
  long interval0;
  long interval1;


  if (swingVar == 0) interval0 = reverseDelay * swing / 100;
  if (swingVar == 1) interval0 = reverseDelay * (100 - swing) / 100;
  if (currentMillis - previousMillis1 >= interval0) {
    previousMillis1 = currentMillis;
    if (outputState == LOW) {
      outputState = 1;
    } else {
      outputState = 0;
      if (swingVar == 0 ) swingVar = 1;
      else swingVar = 0;
    }
    Serial.print(outputState);
    Serial.print(",");
    Serial.println(swingVar);
    digitalWrite(output, outputState);
  }
  //digitalWrite(output, outputState);
  /*if (swingVar == 1) {
    if (currentMillis - previousMillis0 >= interval1) {
      previousMillis0 = currentMillis;
    //Serial.println("5");
      if (outputState == LOW) {
        outputState = 1;
        //Serial.println("6");
      } else {
        outputState = 0;
        //Serial.println("7");
        swingVar = 0;
      }
    Serial.println(outputState);
     // digitalWrite(output, outputState);
    }
    }*/
  //digitalWrite(output, outputState);
}

void countChange() { //this is an interrupt part that reacts to the encoder immediately but it is just a copy of void loop without serial and tick part
  /* bpm = bpmEncoderRotation / 1 + 100;
    sevseg.setNumber(bpm);
    sevseg.refreshDisplay();
    reverseDelay = 60000 / bpm;*/
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
  if (menuVar2 == 0) {
    menuVar2 = 1;
    return;
  }
  if (menuVar2 == 2) {
    menuVar2 = 0;
    return;
  }
  if (menuVar2 >= 4) {
    menuVar2 = 2;
    return;
  }
}

void setup() {

  byte numDigits = 4; //number of digits, if you want to add more digits than change this number and add the pin that you connected the digit pin to the digitPins below
  byte digitPins[] = {15, 3, 4, 5}; //the pins that ar the digit pins connected to, left digit is on the left and than it goes on and on for the rest of the leds
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};  //these are the pins where are the segments connected to, it goes in alphabetical order from left to right
  sevseg.setBrightness(0); //higher brightnes means more flicker of the screen, I don`t know why
  //6 is pin A; 7 is pin B and so on, the dot is the last pin - 13

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);  //if you have a display with common cathode than change the COMMON_ANODE to COMMON_CATHODE
  attachInterrupt (2, countChange, CHANGE); //this is the interrupt for the encoder that activate the countChange void when you move the encoder
  coderButton.attachClick(singleClick);
  coderButton.attachLongPressStop(longClick);
  Serial.begin(2000000);
  bpmEncoderRotation = startingValue;

  swing = EEPROM.read(0);
  bpmMultiply = EEPROM.read(1);
  bpmDivide = EEPROM.read(2);
  displayBpmMode = EEPROM.read(3);

  pinMode(output, OUTPUT);
}

void loop() {
  coderButton.tick();
  unsigned char result = encoder.process();
  bpm = bpmEncoderRotation * bpmMultiply / bpmDivide; //this converts the raw encoder data to usable data, the /2 is there because when there was nothing my encoder
  //was adding four numbers per step instead of one
  //the +100 sets the bpm that the arduino starts with - change it to make arduino start with different bpm
  if (menuVar2 == 0) {
    if (displayBpmMode == 0) {
      sevseg.setNumber(bpm, decimalPoint); //this sets what is the display supposed to show
      sevseg.refreshDisplay();  // this refreshes the display
    }
    if (displayBpmMode == 1) {
      sevseg.setNumber(bpm / bpmMultiply * bpmDivide, decimalPoint); //this sets what is the display supposed to show
      sevseg.refreshDisplay();  // this refreshes the display
    }
  }
  coderButton.tick();

  reverseDelay = 60000 / bpm; //this converts the numbers on the displys into millisecconds like the numbers are bpm - http://guitargearfinder.com/guides/convert-ms-milliseconds-bpm-beats-per-minute-vice-versa/
  bpmTick(); // this is just that I don`t have to make void loop visualy longer but it is the same as writing the tick void in void loop

  if (menuVar2 == 0) { //--------------------BPM-MENU---------------
    if (displayBpmMode == 1) {
      if (menuVar1 == 0) {
        decimalPoint = 0;
        if (result == DIR_CW) {
          bpmEncoderRotation = bpmEncoderRotation + 1;//* bpmMultiply;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation = bpmEncoderRotation - 1;//* bpmMultiply;
        }
      }

      if (menuVar1 == 1) {
        decimalPoint = 1;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 10;// * bpmMultiply / bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 10;// * bpmMultiply / bpmDivide;
        }
      }

      if (menuVar1 == 2) {
        decimalPoint = 2;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 100;// * bpmMultiply / bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 100;// * bpmMultiply / bpmDivide;
        }
      }

      if (menuVar1 == 3) {
        decimalPoint = 3;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 1000;// * bpmMultiply / bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 1000;// * bpmMultiply / bpmDivide;
        }
      }
      if (menuVar1 == 4) {
        decimalPoint = 2;
        if (result == DIR_CW) {
          bpmEncoderRotation = bpmEncoderRotation + 100;// * bpmMultiply / bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation = bpmEncoderRotation - 100;// * bpmMultiply / bpmDivide;
        }
      }
      if (menuVar1 == 5) {
        decimalPoint = 1;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 10;// * bpmMultiply / bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 10;// * bpmMultiply / bpmDivide;
        }
      }
      if (menuVar1 == 6) menuVar1 = 0;
    }
    if (displayBpmMode == 0) {
      if (menuVar1 == 0) {
        decimalPoint = 0;
        if (result == DIR_CW) {
          bpmEncoderRotation = bpmEncoderRotation + 1 * bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation = bpmEncoderRotation - 1 * bpmDivide;
        }
      }

      if (menuVar1 == 1) {
        decimalPoint = 1;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 10 / bpmMultiply * bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 10 / bpmMultiply * bpmDivide;
        }
      }

      if (menuVar1 == 2) {
        decimalPoint = 2;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 100 / bpmMultiply * bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 100 / bpmMultiply * bpmDivide;
        }
      }

      if (menuVar1 == 3) {
        decimalPoint = 3;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 1000 / bpmMultiply * bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 1000 / bpmMultiply * bpmDivide;
        }
      }
      if (menuVar1 == 4) {
        decimalPoint = 2;
        if (result == DIR_CW) {
          bpmEncoderRotation = bpmEncoderRotation + 100 / bpmMultiply * bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation = bpmEncoderRotation - 100 / bpmMultiply * bpmDivide;
        }
      }
      if (menuVar1 == 5) {
        decimalPoint = 1;
        if (result == DIR_CW) {
          bpmEncoderRotation  = bpmEncoderRotation + 10 / bpmMultiply * bpmDivide;
        } else if (result == DIR_CCW) {
          bpmEncoderRotation  = bpmEncoderRotation - 10 / bpmMultiply * bpmDivide;
        }
      }
      if (menuVar1 == 6) menuVar1 = 0;
    }
  }
  if (menuVar2 == 1) {
    normalClick = 0;
    menuEncoderVar1 = 0;
    menuVar2 = 2;
    //Serial.println("3");
  }

  if (menuVar2 == 2) { //--------------------MAIN-MENU--------------
    if (result == DIR_CW) {
      menuEncoderVar1 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar1 --;
    }
    if (menuEncoderVar1 == -1) {
      sevseg.setChars("rst");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 5;
      }
    }
    if (menuEncoderVar1 == 0) {
      sevseg.setChars("DISP");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 7;
      }
    }
    if (menuEncoderVar1 == 1) {
      sevseg.setChars("ULT");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 9;
      }
    }
    if (menuEncoderVar1 == 2) {
      sevseg.setChars("DIV");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 11;
      }
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
      sevseg.setChars("SVIN");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 13;
      }
    }
    if (menuEncoderVar1 == 5) {
      sevseg.setChars("SAVE");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 15;
      }
    }
    if (menuEncoderVar1 == 6) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 0;
        normalClick = 0;
      }
    }

    if (menuEncoderVar1 == 7) menuEncoderVar1 = 6;
    if (menuEncoderVar1 == -2) menuEncoderVar1 = -1;
  }

  if (menuVar2 == 3) {
    menuEncoderVar2 = 0;
    normalClick = 0;
    menuVar2 = 4;
  }

  if (menuVar2 == 4) { //---------------------------SET-SUBMENU--------------------
    if (result == DIR_CW) {
      menuEncoderVar2 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar2 --;
    }
    if (menuEncoderVar2 == 0) {
      sevseg.setNumber(100);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 100;
        menuVar2 = 1;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 1) {
      sevseg.setNumber(140);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 140;
        menuVar2 = 1;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 2) {
      sevseg.setNumber(220);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 220;
        menuVar2 = 1;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 3) {
      sevseg.setNumber(480);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 480;
        menuVar2 = 1;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 4) {
      sevseg.setNumber(320);
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = 320;
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 5) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 0;
        normalClick = 0;
      }
    }
    if (menuEncoderVar2 == 6) menuEncoderVar2 = 5;
    if (menuEncoderVar2 == -1) menuEncoderVar2 = 0;
  }

  if (menuVar2 == 5) {
    normalClick = 0;
    menuEncoderVar3 = 0;
    menuVar2 = 6;
  }

  if (menuVar2 == 6) {    //-------------------------------RST-SUBMENU----------------------
    if (result == DIR_CW) {
      menuEncoderVar3 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar3 --;
    }
    if (menuEncoderVar3 == 0) {
      sevseg.setChars("CANC");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar3 == 1) {
      sevseg.setChars("OK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        bpmEncoderRotation = startingValue;
        menuVar2 = 0;
        normalClick = 0;
        menuVar1 = 0;
        menuVar2 = 0;
        menuVar3 = 0;
        menuVar4 = 0;
        menuEncoderVar1 = 0;
        menuEncoderVar2 = 0;
        menuEncoderVar3 = 0;
        menuEncoderVar4 = 0;
        menuEncoderVar5 = 0;
        menuEncoderVar6 = 0;
        decimalPoint = 0;
        displayBpmMode = 0;
        normalClick = 0;
        bpmMultiply = 1;
        bpmDivide = 1;
        swing = 50;
      }
    }
    if (menuEncoderVar3 == 2) menuEncoderVar3 = 1;
    if (menuEncoderVar3 == -1) menuEncoderVar3 = 0;
  }

  if (menuVar2 == 7) {
    normalClick = 0;
    menuEncoderVar4 = 0;
    menuVar2 = 8;
  }

  if (menuVar2 == 8) {   //--------------------------DISP-SUBMENU-----------------------
    if (result == DIR_CW) {
      menuEncoderVar4 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar4 --;
    }
    if (menuEncoderVar4 == 0) {
      if (displayBpmMode == 1) {
        sevseg.setChars("REAL");
        sevseg.refreshDisplay();
      }
      if (displayBpmMode == 0) {
        sevseg.setChars("-REA");
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        normalClick = 0;
        displayBpmMode = 0;
      }
    }
    if (menuEncoderVar4 == 1) {
      if (displayBpmMode == 0) {
        sevseg.setChars("SET");
        sevseg.refreshDisplay();
      }
      if (displayBpmMode == 1) {
        sevseg.setChars("-SET");
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        normalClick = 0;
        displayBpmMode = 1;
      }
    }
    if (menuEncoderVar4 == 2) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 1;
        normalClick = 0;
      }
    }
    if (menuEncoderVar4 == 3) menuEncoderVar4 = 2;
    if (menuEncoderVar4 == -1) menuEncoderVar4 = 0;
  }

  if (menuVar2 == 9) {
    normalClick = 0;
    menuEncoderVar5 = 0;
    menuVar2 = 10;
  }

  if (menuVar2 == 10) {    //-------------------------------MULT-SUBMENU----------------------
    if (result == DIR_CW) {
      menuEncoderVar5 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar5 --;
    }
    if (menuEncoderVar5 == -1) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 2;
      }
    }
    if (menuEncoderVar5 == 0) {
      if (bpmMultiply == 1) {
        sevseg.setNumber(-1);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(1);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 1;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 1) {
      if (bpmMultiply == 2) {
        sevseg.setNumber(-2);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(2);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 2;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 2) {
      if (bpmMultiply == 3) {
        sevseg.setNumber(-3);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(3);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 3;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 3) {
      if (bpmMultiply == 4) {
        sevseg.setNumber(-4);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(4);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 4;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 4) {
      if (bpmMultiply == 5) {
        sevseg.setNumber(-5);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(5);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 5;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 5) {
      if (bpmMultiply == 6) {
        sevseg.setNumber(-6);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(6);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 6;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 6) {
      if (bpmMultiply == 7) {
        sevseg.setNumber(-7);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(7);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 7;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 7) {
      if (bpmMultiply == 8) {
        sevseg.setNumber(-8);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(8);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 8;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 8) {
      if (bpmMultiply == 9) {
        sevseg.setNumber(-9);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(9);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 9;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 9) {
      if (bpmMultiply == 10) {
        sevseg.setNumber(-10);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(10);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 10;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 10) {
      if (bpmMultiply == 11) {
        sevseg.setNumber(-11);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(11);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 11;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 11) {
      if (bpmMultiply == 12) {
        sevseg.setNumber(-12);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(12);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 12;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 12) {
      if (bpmMultiply == 13) {
        sevseg.setNumber(-13);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(13);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 13;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 13) {
      if (bpmMultiply == 14) {
        sevseg.setNumber(-14);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(14);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 14;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 14) {
      if (bpmMultiply == 15) {
        sevseg.setNumber(-15);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(15);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 15;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 15) {
      if (bpmMultiply == 16) {
        sevseg.setNumber(-16);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(16);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmMultiply = 16;
        normalClick = 0;
        bpmDivide = 1;
      }
    }
    if (menuEncoderVar5 == 16) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        normalClick = 0;
        menuVar2 = 2;
      }
    }
    if (menuEncoderVar5 == 17) menuEncoderVar5 = 16;
    if (menuEncoderVar5 == -2) menuEncoderVar5 = -1;
  }
  if (menuVar2 == 11) {
    normalClick = 0;
    menuEncoderVar6 = 0;
    menuVar2 = 12;
  }

  if (menuVar2 == 12) {    //-------------------------------DIV-SUBMENU----------------------
    if (result == DIR_CW) {
      menuEncoderVar6 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar6 --;
    }
    if (menuEncoderVar6 == -1) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar6 == 0) {
      if (bpmDivide == 1) {
        sevseg.setNumber(-1);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(1);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 1;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 1) {
      if (bpmDivide == 2) {
        sevseg.setNumber(-2);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(2);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 2;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 2) {
      if (bpmDivide == 3) {
        sevseg.setNumber(-3);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(3);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 3;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 3) {
      if (bpmDivide == 4) {
        sevseg.setNumber(-4);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(4);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 4;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 4) {
      if (bpmDivide == 5) {
        sevseg.setNumber(-5);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(5);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 5;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 5) {
      if (bpmDivide == 6) {
        sevseg.setNumber(-6);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(6);
        sevseg.refreshDisplay();
      }
      if (normalClick == 7) {
        bpmDivide = 6;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 6) {
      if (bpmDivide == 7) {
        sevseg.setNumber(-7);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(7);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 7;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 7) {
      if (bpmDivide == 8) {
        sevseg.setNumber(-8);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(8);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 8;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 8) {
      if (bpmDivide == 9) {
        sevseg.setNumber(-9);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(9);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 9;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 9) {
      if (bpmDivide == 10) {
        sevseg.setNumber(-10);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(10);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 10;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 10) {
      if (bpmDivide == 11) {
        sevseg.setNumber(-11);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(11);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 11;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 11) {
      if (bpmDivide == 12) {
        sevseg.setNumber(-12);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(12);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 12;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 12) {
      if (bpmDivide == 13) {
        sevseg.setNumber(-13);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(13);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 13;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 13) {
      if (bpmDivide == 14) {
        sevseg.setNumber(-14);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(14);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 14;
        normalClick = 0;
      }
    }
    if (menuEncoderVar6 == 14) {
      if (bpmDivide == 15) {
        sevseg.setNumber(-15);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(15);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide = 15;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 15) {
      if (bpmDivide = 1) {
        sevseg.setNumber(-16);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(16);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        bpmDivide == 16;
        normalClick = 0;
        bpmMultiply = 1;
      }
    }
    if (menuEncoderVar6 == 16) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar6 == 17) menuEncoderVar6 = 16;
    if (menuEncoderVar6 == -2) menuEncoderVar6 = -1;
  }

  if (menuVar2 == 13) {
    menuEncoderVar7 = 0;
    normalClick = 0;
    menuVar2 = 14;
  }

  if (menuVar2 == 14) {   //-----------------------------SWIN-SUBMENU-----------------------
    if (result == DIR_CW) {
      menuEncoderVar7 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar7 --;
    }
    if (menuEncoderVar7 == -1) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 0) {
      if (swing == 10) {
        sevseg.setNumber(-10);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(10);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 10;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 1) {
      if (swing == 20) {
        sevseg.setNumber(-20);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(20);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 20;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 2) {
      if (swing == 30) {
        sevseg.setNumber(-30);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(30);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 30;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 3) {
      if (swing == 40) {
        sevseg.setNumber(-40);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(40);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 40;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 4) {
      if (swing == 50) {
        sevseg.setNumber(-50);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(50);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 50;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 5) {
      if (swing == 60) {
        sevseg.setNumber(-60);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(60);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 60;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 6) {
      if (swing == 70) {
        sevseg.setNumber(-70);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(70);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 70;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 7) {
      if (swing == 80) {
        sevseg.setNumber(-80);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(80);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 80;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 8) {
      if (swing == 90) {
        sevseg.setNumber(-90);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(90);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 90;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 9) {
      if (swing == 100) {
        sevseg.setNumber(-100);
        sevseg.refreshDisplay();
      }
      else {
        sevseg.setNumber(100);
        sevseg.refreshDisplay();
      }
      if (normalClick == 1) {
        swing = 100;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 10) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar7 == 11) menuEncoderVar7 = 10;
    if (menuEncoderVar7 == -2) menuEncoderVar7 = -1;
  }
  if (menuVar2 == 100) menuVar2 = 0;

  if (menuVar2 == 15) {   //-----------------------------SAVE-SUBMENU-----------------------
    if (result == DIR_CW) {
      menuEncoderVar8 ++;
    } else if (result == DIR_CCW) {
      menuEncoderVar8 --;
    }
    if (menuEncoderVar8 == 0) {
      sevseg.setChars("BACK");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuVar2 = 2;
        normalClick = 0;
      }
    }
    if (menuEncoderVar8 == 1) {
      sevseg.setChars("SAVE");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuEncoderVar8 = 0;
        normalClick = 0;
        EEPROM.write(0, swing);
        EEPROM.write(1, bpmMultiply);
        EEPROM.write(2, bpmDivide);
        EEPROM.write(3, displayBpmMode);
      }
    }
    if (menuEncoderVar8 == 2) {
      sevseg.setChars("LOAD");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuEncoderVar8 = 0;
        normalClick = 0;
        swing = EEPROM.read(0);
        bpmMultiply = EEPROM.read(1);
        bpmDivide = EEPROM.read(2);
        displayBpmMode = EEPROM.read(3);
      }
    }
    if (menuEncoderVar8 == 3) {
      sevseg.setChars("CLR");
      sevseg.refreshDisplay();
      if (normalClick == 1) {
        menuEncoderVar8 = 0;
        normalClick = 0;
        EEPROM.write(0, 50);
        EEPROM.write(1, 1);
        EEPROM.write(2, 1);
        EEPROM.write(3, 0);
      }
    }
    if (menuEncoderVar8 == 4) menuEncoderVar8 = 3;
    if (menuEncoderVar8 == -1) menuEncoderVar8 = 0;
  }
}
