2O_ObeyLoc_v1.31 - Arduino

--This version stems from "2O_ObeyLoc_v1.30".
--This arduino folder interworks with "Ordering2I_local_v1.29" (or even some previous versions) arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.6"
--Mobile apps do not control (for this version) this panel but are able to ask to get a report since what's really interesting is the state of this panel's relays (not the ordering panel).
--This is associated with DesignSpark file "Obeying_B1_v1.9.pcb" (and not any previous version) in folder "Reflector_2Relays".

***Motivation of this version 
--We're using transistors in this hardware, so setting the out pins to be coherent with their state, e.g. say D5 needs to be ON (LED and relay) then we set it to HIGH, and vice versa. Same for D6.

***Features
--almost no change at all. BTW, the LEDs work in parallel with pins so asConsidered is amended accordingly.

***Procedural Notes
--

***Fixes
--
--










