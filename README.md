# BinaryClock
Binary clock made with arduino pro mini, can be adapted to more powerfull versions. It uses TinyRTC to remember current date and automaticly swaps through summer and winter times. When code is compiled and uploaded to arduino, it sets current time and date to when sketch was compiled. It is convinient, atleast for me.

Layout of the leds can be seen in schematics. Outputs are for now simple led driven through arduino output pins with 330Ohm resistors. You can use different LED's and more complex power supply system for them, but core electronics are the same.

Clock is adjustable with three buttons. T1 changes place to adjust, T2 adds one to hours or minutes, T3 substract one from hours or minutes. LED blinks with 1Hz when current unit is adjusted.

