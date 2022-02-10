"STA_6mob_5O5I_2Ports_v1.12" Arduino Readme file

This stems from "STA_6mob_6Outputs_5Inputs_2Ports_v1.11"
This interworks with "Control_1Panel.v2.b" android app.

In this version:

1) The button D10 (or Tx) is not the output modified by input pin D8 anymore, so D10 is not controlled anymore by the mobile app.
In simple terms, D10 is out.
D10 will now be an O/P red LED to be ON when the WiFi is off, thus it can't connect to the WiFi.
So:
D2 I/P controls D5 O/P
D7 I/P controls D6 O/P
D8 I/P controls D9 O/P
(done...)

2) As a way of action for D10, in "loop" I will check whether wifi is still connected and accordingly D10 will blink. (done...)
