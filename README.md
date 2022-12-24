# 4runner-Seat-Heat

This repo tracks the code for the 4Runner Seat Heat project (to allow generic heated seats to be used with the OEM switches in a 4th gen 4Runner and selected FJ Cruisers).

For the PCB works see [PCB Repo](https://github.com/alorman/4runner-Seat-Heat-PCB)

This requires PCB version Rev03 or higher. Rev02 requires a dead bug wire to fix the 12V analog input.

## Hardware
This code compiles in the Arduino IDE for a Teensy 3.2 or LC.

## Notes
By calling `GlobalPWMFreq` we can set the PWM rate (stock is about 20khz) so that our MOSFETs can fully latch. If they don't their Rdson is too high and they catch fire. 
