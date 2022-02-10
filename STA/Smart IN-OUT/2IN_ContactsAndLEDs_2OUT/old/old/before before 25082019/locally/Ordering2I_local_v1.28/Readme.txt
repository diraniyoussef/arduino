Ordering2I_local_v1.28  - Arduino

--This version stems from "Ordering2I_mainlyRemote_v1.27"
--This arduino folder interworks with "Obeying2O_mainlyLocal_v1.27" arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.6"
--This interworks with Design Spark : B1_v1.8 in folder Ordering_2In. It also interworks with AutoReflected_2In\B1_v1.5
--This interworks with Android Out3P_Ref1P-2In.v3.4
--This panel is more compatible with informing contacts not buttons (in effect of mobile app connectivity and so on). In order to make it otherwise you have to make some changes.

THIS IS A WORKING VERSION

***Motivation behind this version
-- Going local. This is a local informing panel. In InformLocal.h class, the IPs of the obeying panels this panel connects to must be known to this panel.
The user will send the IP(s) of the informing panel(s) to this panel. 
To simplify things, I decided to do that in the AP_mode environment. The user will send 2 messages when this panel is in AP mode. 
Only when 2 messages are received, one for the network configurations of this same panel, and one for the IPs of the obeying panels, only then AP mode is exited and this panel goes back to normal operation.
Perhaps a more decent way is to let each obeying panel reply back with its static IP to a broadcast message initiated by the ordering panel. The broadcast message is an UDP message. This will imply a modification in the code of the obeying panel as well.


***Details
-- I made a separate class named "ObeyingIPs" in Setup.h. This class will analyze the incoming message (which will be something like "obey1192.168.1.15\" or like "obey2192.168.1.15\192.168.1.108\"), if valid (I test the validity to a large extent although this should be made in the mobile app), then we are still awaiting a network config message from the user (which will be gotten in the next loop) [Please note that this control to get the 2 messages is made in the "checkIncomingMessageAndReturnToNormalOperation()" method of the "AP_Op" class], if the latter message is also valid, then the "ObeyingIPs" class writes the info to the EEPROM and we go back to normal operation.
In the setup() method of InformLocal class, we read from EEPROM the IPs and proceed. If we were not able to read correctly, then we simply toggle notifier pin.
--As noted the "ObeyingIPs" class will analyze the incoming message (which will be something like "obey1192.168.1.15\"), the good news is that it also knows how to analyze a message that declares 2, 3 or more (up to 6, in coordination with the EEPROM) obeying IPs (thus in case this panel informs more than 1 obeying panel).  This means scalability without the need to upload a modified version of the arduino code.

***Requirement
--

***Fixes
--
