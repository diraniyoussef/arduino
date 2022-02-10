"STA_6mob_6Outputs_5Inputs_2Ports_v1.11" Arduino Readme file

This stems from "AP_6mob_6Outputs_5Inputs_2Ports_v1.11"

In this version:

1) NodeMCU will be a station so I can connect a lot of mobs to it. I will check if the NODEMCU can handle e.g. 6 concurrent 
sockets or maybe 8...

2) I have changed the delay of each loop from 200 ms to 750 ms because I wanted the button press to be stable. It is hard to be effective with the 200 ms. 
Clearly maxLoopCounterToAssignANewClient had to change as well.