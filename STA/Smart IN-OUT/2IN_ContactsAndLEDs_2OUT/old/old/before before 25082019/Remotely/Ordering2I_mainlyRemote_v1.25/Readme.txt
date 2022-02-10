Ordering2I_mainlyRemote_v1.25  - Arduino

--This version stems from "Reflected2I_mainlyRemote_v1.24"
--This arduino folder interworks with "Obeying2O_remote_v1.25" arduino folder.
--This interworks with eclipse intermediate code "???ControlLinker_v0.5"
--Mobile apps do not control this panel but may ask to get a report not from the ordering (informing - reflected) panel, instead it's from the receiving (reflector) panel. The mobile app version is "??Out3P_Ref1P-2In.v3"
--This is associated with DesignSpark file "B1_v1.7.pcb" found in folder "Ordering_2In".


***Features
--It will inform through internet a certain remote panel. Of course it will inform only 1 panel no more, to know one reason why look at the next comment. DONE
--I will be using latching self-locking buttons (connected to pins D5 and D6) (instead of pulse buttons because the decision is final and it is determined right from this panel) and a LED that corresponds to the switched button (This is why we order only 1 panel not 2 e.g. since in case of 2 panels we would need 2 LEDs for each button, thus one LED for per panel). This LED (2 LEDs connected to pins D3 and D4) will reflect the status of the relay contact in the "obeying" (reflector) panel through received reports. DONE
--No received report requests from mobile apps by this ordering (or name it reflected or informing) panel. DONE
--Discussion : "Notification LED is not needed anymore. I will be using the state LEDs to blink in case of a connection failure. This is still a good user experience since he will still know the last state in case of a connection failure, and this is through checking his act! Of course, if the buttons were pulse buttons then a separate notification LED would be of need." NOOOO I WILL BE USING A SEPARATE DEDICATED NOTIFICATION LED BECAUSE although that user knows the last state of the buttons but any other late user would still need to know them, and if they were blinking he wouldn't be able. DONE
--Looking at the previous comment, I need to make a memory for the state buttons. DONE
--Please note that for the purpose of easing the design of the PCB, I read the LOW state of the IN pin as an ON, and I read the HIGH state of the IN pin as an off. This is why I made the method "getInPinStateAsConsidered" as one place for this consideration. This affects the report I'm sending to the obeying panel (the IN pins status) and the report I'm receiving from the obeying panel (the IN pins status).

***Fixes
--The reports this panel sends to the intermediate server end with "\", it was duplicated in the sendHi report, so I fixed it.


