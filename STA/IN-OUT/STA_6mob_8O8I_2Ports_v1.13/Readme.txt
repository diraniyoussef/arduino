"STA_6mob_8O8I_2Ports_v1.13" Arduino Readme file

This stems from "STA_6mob_5O5I_2Ports_v1.13"
This interworks with "Control_3P.v1" android app - I used the first panel. 

In this version:

1) Using PCF8574. I made 4 input pins of the PCF8574A which are 0, 1, 2, and 3. I made 4 outputs which corrspond to the input pins; they are 4, 5, 6, and 7.
In this version, the output pins are linked to input pins of the same pcf and not from another pcf or from Node MCU's input pins.

There's a little change in MessageAndOperation class, especially in analyze() method, updatePinAndEEPROM() method, sendReport() method, others maybe. 

PCF8574A has a weird behavior; sometimes for no reason or maybe because of a mechanical agitation of a certain wire like SCL, all output pins suddenly are set to HIGH, so the LED's begin to be off (as well as the relays), and this can be fixed by setting any output pin to any value, and I do that at the end of the loop.

If an output pin were to be toggled based on an input pin, in order to do the toggling, I read the state of the output pin from the EEPROM and not from the output pin directly because of the weird behavior of the PCF8574A that is mentioned previously.

I didn't use the Interrupt pin since I read the input pins periodically.

Programmatically, I made a class in Generic.h header specific to PCF. More than one PCF can be connected to the Node MCU and each can be have a dedicated instance of that class. (NOT TESTED ON MORE THAN 1 PCF)

I made use of getBit and setBit to know if the PCF8574A is connected or not, so if it was not connected I will not save anything to the EEPROM and will not send a report to the mobile.

What is written on the EEPROM or exchanged between the mobile and the module as states of pins e.g. pin 'c' is 'T' or 'F', this letter 'T' or 'F' contradicts with the state of the pin i.e. the argument given to setBit.
