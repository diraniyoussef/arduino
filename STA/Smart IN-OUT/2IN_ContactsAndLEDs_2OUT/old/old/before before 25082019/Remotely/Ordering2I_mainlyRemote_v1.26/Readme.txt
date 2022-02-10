Ordering2I_mainlyRemote_v1.26  - Arduino

--This version stems from "Ordering2I_mainlyRemote_v1.25"
--This arduino folder interworks with "Obeying2O_remote_v1.26" arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.6"
--This interworks with Design Spark : B1_v1.8 in folder Ordering_2In. It also interworks with AutoReflected_2In\B1_v1.5

THIS IS A WORKING VERSION

***Motivation behind this version
The user can set/change, according to will, the network configuarations of the panel

***Features
--The only progress over the previous version is the addition of the capability to assign the SSID and password to the module. The SSID and password are given from the mobile app. They will be written on the EEPROM.
The procedure is as follows:
-The module when connected to electricity checks the correct place in EEPROM e.g. at memory location 201 or 101 (whatever) whether it has an assigned SSID or not. (a null char is enough for that)
	- If no SSID is assigned, then the notification LED turns ON forever. And the module does not restart. The module in this case must be in the Access Point AP mode. And opening a special server which will listen to any incoming socket connection (from the mobile app, of course).
		-The user standing next to the module will notice the continuous light of the notification LED, and so will open the mobile app to the corresponding panel. The mobile app will send the new SSID and password, then an acknowledge will be sent back from the module to the user (if everything is in the correct format) - NOT REALLY, I OMITTED THIS ACK FOR NOW, and data will be saved in the dedicated place on the EEPROM 
-What will be saved in the mobile app?
Just the module's static IP for now.
-AFTER ASSIGNING THE STATIC IP (in case it was assigned), HOW WOULD THE OTHER MOBILES KNOW IT? The users of the remaining mobiles will have to type in the static IP.
-A special button linked to a dedicated IN pin of the panel is pressed by the user for 5 seconds (sometimes for 15 seconds - probably in case the panel is continuously restarting when there are saved configuration on the EEPROM but not coherent with the router's). Then the module forgets the recorded SSID. (Any opened sockets must be closed beforehand.)



***Requirement
--No need for anything. In case the memory in 101 was not '\n' (e.g. - it is the 'trailor' in general, and it's not '\n') then the code flow will lead to the AP mode.


***Fixes
--I have removed this line WIFI::serverSetup(); from the main sketch file since it's only related to LocalServer.h file. So I removed the  object declaration from ConnectionSetup.h onto LocalServer class. Same for the serverSetup() method.

