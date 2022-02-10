STA_6mob_12O4I_Internet_v1.21

This version stems from "STA_6mob_5O5I_Internet_v1.20" and from "STA_6mob_12O4I_2Ports_v1.13".
NodeMCU is still controled by Control_3P.v3 android studio.
This is associated with DesignSpark file "Check board B3 of the C:\Users\Public\Documents\DesignSpark PCB 8.1\Examples\SixteenO" especially board B3.

**The motivation behind this is to fix the problem of sudden restart of PCF followed by an all pins HIGH state.

Experimental finding:
**Has the mentioned problem of sudden restart been solved? probably yes.
One time the NodeMCU restarted for unknown reason except heavy load. Is it a problem in buffering? Like being overwhelmed by sent messages? Maybe.

1) All pins of the PCF8574A are made outputs.
2) PCF pins now are normally made low! So the LED's out PCF pin is made on if the PCF pin was set high as well. This will ease the hardware.
3)
