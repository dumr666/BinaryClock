/*
   Binary clock
   
   Shows hours and minutes. Adjustable by three buttons, layout of 
   leds is in schematics. It usees TinyRTC module but can be modified
   to run on millis.

   Author: Denis Leskovar
   Email: dumr666<at>gmail.com
   Date: 28.1.2018

   Released under MIT Licence.

   Please don't just copy paste the code for your school projects, do your
   research, it pays off in the long run. If you need any help understanding
   I am glad to help.
*/

// Includes
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Time.h>
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif


// Defines
// LED Defines
#define LEDM20 6
#define LEDM21 7
#define LEDM22 8
#define LEDM23 9
#define LEDM10 17
#define LEDM11 16
#define LEDM12 15
#define LEDH20 14
#define LEDH21 13
#define LEDH22 2
#define LEDH23 3
#define LEDH10 4
#define LEDH11 5
// Pushbutton Defines
#define T1 12 
#define T2 11 
#define T3 10 


//Variables
//Pushbuttons
int stateT1 = 0;  //Variable for reading button state
int stateT2 = 0;  //Variable for reading button state
int stateT3 = 0;  //Variable for reading button state

int stateT1old = 0;  //Variable for reading button state
int stateT2old = 0;  //Variable for reading button state
int stateT3old = 0;  //Variable for reading button state
int Tstates[] = {stateT1, stateT2, stateT3};
//LED
int LEDMinutes[] = {LEDM12, LEDM11, LEDM10, LEDM23, LEDM22, LEDM21, LEDM20};
int LEDHours[] = {LEDH11, LEDH10, LEDH23, LEDH22, LEDH21, LEDH20};

//RTC Module
RTC_DS1307 rtc;
DateTime curr;
tmElements_t tm;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int adjustPlace = 0;
int adjustYear = 0;
int adjustMonth = 0;
int adjustDay = 0;
int adjustHour = 0;
int adjustMinutes = 0;
int adjustSeconds = 0;
int adjustActive = 0;
int adjustArray[] = {adjustYear, adjustMonth, adjustDay, adjustHour, adjustMinutes, adjustSeconds};
bool LEDadjustHours = false;
bool LEDadjustMinutes = false;
bool LEDOff = false;

//LED, hour, minutes variables
int currHour = 0;
int currHourL = 0;
int currMinutes = 0;
int currMinutesL = 0;

//Millis and multitasking
long millisNew = 0;   //Current count of millis
// Millis  Task 1
long millisOld1 = 0;   //Previous Millis value
long millisInterval1 = 1000;//First number for interval
// Millis  Task 2
long millisOld2 = 0;   //Previous Millis value
long millisInterval2 = 50;//First number for interval
// Millis  Task 3
long millisOld3 = 0;   //Previous Millis value
long millisInterval3 = 500;//First number for interval
// Millis  Task 4
long millisOld4 = 0;   //Previous Millis value
long millisInterval4 = 100;//First number for interval




//------ Setup Function --------------------------//
void setup() {
  // Init
  // LED init
  LEDInit();
  //Serial Init
  Serial.begin(57600);

  //RTC Init
  if (! rtc.begin()) {
    //Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  //following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

//-------- Loop Function --------------------------//
void loop() {
  millisTasks();
}


//------- Other Functions ------------------------//
int pushButtonRead() {
  stateT1old = stateT1;
  stateT2old = stateT2;
  stateT3old = stateT3;
  stateT1 = digitalRead(T1);
  stateT2 = digitalRead(T2);
  stateT3 = digitalRead(T3);

}

// MIllis Tasks -------------------------------------------------------
int millisTasks() {
  millisNew = millis();

  // Task 1 -------------------------------
  if (millisNew - millisOld1 > millisInterval1) {
    millisOld1 = millisNew;
    // Tasks for millis 1
    digitalClockDisplay();
  }

  // Task 2 --------------------------------
  if (millisNew - millisOld2 > millisInterval2) {
    millisOld2 = millisNew;

    //Tasks for millis 2
    pushButtonRead();   // Read pushbutton states
    
    // Check for adjust
    if ((stateT1old == LOW) && (stateT1 == HIGH)) {
      if(adjustPlace == 2) {
        adjustPlace = 0;
      } else {
        adjustPlace++;
      }
      if (adjustPlace == 0) {
        LEDadjustHours = false;
        LEDadjustMinutes = false;
      }
      if(adjustPlace == 1) {
        LEDadjustHours = true;
        LEDadjustMinutes = false;
      }      
      // Adjust is made on minutes
      if(adjustPlace == 2) {
        LEDadjustHours = false;
        LEDadjustMinutes = true;
      }      
    }
    //If second button is pressed
    if ((stateT2old == LOW) && (stateT2 == HIGH)) {        // nc

      curr = rtc.now();
      if (adjustPlace == 1 ) {
        if (adjustHour == 23) {
          adjustHour = 0;
        }
        else {
          adjustHour++;
        }
        rtc.adjust(DateTime(curr.year(), curr.month(), curr.day(), adjustHour, curr.minute(), 0));
      }

      if (adjustPlace == 2) {
        adjustMinutes = curr.minute();
        if (adjustMinutes == 59) {
          adjustMinutes = 0;
        }
        else {
          adjustMinutes++;
        }
        rtc.adjust(DateTime(curr.year(), curr.month(), curr.day(), curr.hour(), adjustMinutes, 0));
      }

    }
    // Check if T3 was pressed
    if ((stateT3old == LOW) && (stateT3 == HIGH)) {        // add an hour!
      curr = rtc.now();
      if (adjustPlace == 1 ) {
        adjustHour = curr.hour();
        if (adjustHour == 0) {
          adjustHour = 23;
        }
        else {
          adjustHour--;
        }
        rtc.adjust(DateTime(curr.year(), curr.month(), curr.day(), adjustHour, curr.minute(), 0));
      }

      if (adjustPlace == 2) {
        adjustMinutes = curr.minute();
        if (adjustMinutes == 0) {
          adjustMinutes = 59;
        }
        else {
          adjustMinutes--;
        }
        rtc.adjust(DateTime(curr.year(), curr.month(), curr.day(), curr.hour(), adjustMinutes, 0));

      }
    }
    
  }
  // Task 3 -------------------------------
  if (millisNew - millisOld3 > millisInterval3) {
    millisOld3 = millisNew;
    
    // Task operations
    LEDOff = !LEDOff; // blinking the led if adjusted
  }
  // Task 4 -----------------------------
  if (millisNew - millisOld4 > millisInterval4) {
    millisOld4 = millisNew;
    // Tasks operations
    LEDClkShow();
  }  
}

void digitalClockDisplay() {
  Serial.print(curr.year(), DEC);
  Serial.print('/');
  Serial.print(curr.month(), DEC);
  Serial.print('/');
  Serial.print(curr.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[curr.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(curr.hour(), DEC);
  Serial.print(':');
  Serial.print(curr.minute(), DEC);
  Serial.print(':');
  Serial.print(curr.second(), DEC);
  Serial.println();
  return 0;

}

void LEDInit() {
  //  Serial.println("LED INIT" );
  pinMode(LEDM20, OUTPUT);
  pinMode(LEDM21, OUTPUT);
  pinMode(LEDM22, OUTPUT);
  pinMode(LEDM23, OUTPUT);
  pinMode(LEDM10, OUTPUT);
  pinMode(LEDM11, OUTPUT);
  pinMode(LEDM12, OUTPUT);
  pinMode(LEDH20, OUTPUT);
  pinMode(LEDH21, OUTPUT);
  pinMode(LEDH22, OUTPUT);
  pinMode(LEDH23, OUTPUT);
  pinMode(LEDH10, OUTPUT);
  pinMode(LEDH11, OUTPUT);

  // Also pushbutton init
  // Push buttons
  pinMode(T1, INPUT);
  pinMode(T2, INPUT);
  pinMode(T3, INPUT);
  
  int i = 0;
  int LEDdelay =  100;
  int MinutesIndex = 6;
  int HoursIndex = 5;

  // Check if all leds are okay
  // Hours array
  for (i = 0; i <= HoursIndex; i++)
  {
    digitalWrite(LEDHours[i], HIGH);
    delay(LEDdelay);
  }
  //Minutes array
  for (i = 0; i <= MinutesIndex; i++)
  {
    digitalWrite(LEDMinutes[i], HIGH);
    delay(LEDdelay);
  }

  for (i = 0; i <= HoursIndex; i++)
  {
    digitalWrite(LEDHours[i], LOW);
    delay(LEDdelay);
  }
  //

  for (i = 0; i <= MinutesIndex; i++)
  {
    digitalWrite(LEDMinutes[i], LOW);
    delay(LEDdelay);
  }
  Serial.println("LED Init Finished");
}

void LEDReset() {
  // Reset all leds, before write new value
  digitalWrite(LEDM20, LOW);
  digitalWrite(LEDM21, LOW);
  digitalWrite(LEDM22, LOW);
  digitalWrite(LEDM23, LOW);
  digitalWrite(LEDM10, LOW);
  digitalWrite(LEDM11, LOW);
  digitalWrite(LEDM12, LOW);

  digitalWrite(LEDH20, LOW);
  digitalWrite(LEDH21, LOW);
  digitalWrite(LEDH22, LOW);
  digitalWrite(LEDH23, LOW);
  digitalWrite(LEDH10, LOW);
  digitalWrite(LEDH11, LOW);
}

void LEDClkShow() {

  curr = rtc.now();
  currHour = curr.hour();
  currHourL = currHour % 10;
  currMinutes = curr.minute();
  currMinutesL = currMinutes % 10;
  LEDReset();
  //SET LEDS
  // Code is quite self explanatory
  //minutes units
  if (LEDadjustMinutes == false || (LEDadjustMinutes == true && LEDOff == false)) {
    if (currMinutesL == 1 || currMinutesL == 3 || currMinutesL == 5 || currMinutesL == 7 || currMinutesL == 9) {
      digitalWrite(LEDM20, HIGH);
    } else {
      digitalWrite(LEDM20, LOW);
    }
    if (currMinutesL == 2 || currMinutesL == 3 || currMinutesL == 6 || currMinutesL == 7) {
      digitalWrite(LEDM21, HIGH);
    } else {
      digitalWrite(LEDM21, LOW);
    }
    if (currMinutesL == 4 || currMinutesL == 5 || currMinutesL == 6 || currMinutesL == 7) {
      digitalWrite(LEDM22, HIGH);
    } else {
      digitalWrite(LEDM22, LOW);
    }
    if (currMinutesL == 8 || currMinutesL == 9) {
      digitalWrite(LEDM23, HIGH);
    } else {
      digitalWrite(LEDM23, LOW);
    }

    //minutes
    if ((currMinutes >= 10 && currMinutes < 20) || (currMinutes >= 30 && currMinutes < 40) || (currMinutes >= 50 && currMinutes < 60))  {
      digitalWrite(LEDM10, HIGH);
    } else {
      digitalWrite(LEDM10, LOW);
    }
    if (currMinutes >= 20 && currMinutes < 40)  {
      digitalWrite(LEDM11, HIGH);
    } else {
      digitalWrite(LEDM11, LOW);
    }
    if (currMinutes >= 40 && currMinutes < 60) {
      digitalWrite(LEDM12, HIGH);
    } else {
      digitalWrite(LEDM12, LOW);
    }
  }
  //hour units
  if (LEDadjustHours == false || (LEDadjustHours == true && LEDOff == true)) {
    if (currHourL == 1 || currHourL == 3 || currHourL == 5 || currHourL == 7 || currHourL == 9) {
      digitalWrite(LEDH20, HIGH);
    } else {
      digitalWrite(LEDH20, LOW);
    }
    if (currHourL == 2 || currHourL == 3 || currHourL == 6 || currHourL == 7) {
      digitalWrite(LEDH21, HIGH);
    } else {
      digitalWrite(LEDH21, LOW);
    }
    if (currHourL == 4 || currHourL == 5 || currHourL == 6 || currHourL == 7) {
      digitalWrite(LEDH22, HIGH);
    } else {
      digitalWrite(LEDH22, LOW);
    }
    if (currHourL == 8 || currHourL == 9 ) {
      digitalWrite(LEDH23, HIGH);
    } else {
      digitalWrite(LEDH23, LOW);
    }

    //hour
    if (currHour >= 10 && currHour < 20)  {
      digitalWrite(LEDH10, HIGH);
    } else {
      digitalWrite(LEDH10, LOW);
    }
    if (currHour >= 20 && currHour < 24)  {
      digitalWrite(LEDH11, HIGH);
    } else {
      digitalWrite(11, LOW);
    }
  }
}

