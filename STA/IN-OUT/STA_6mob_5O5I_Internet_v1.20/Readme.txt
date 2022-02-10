"STA_6mob_5O5I_Internet_v1.20" Arduino Readme file

This stems from "STA_6mob_5O5I-PCF-disabled_2Ports_v1.14" found in folder "Zaher - STA_6mob_5O5I_2Ports_v1.12 - Working"

This interworks with "Control_3P.v3" android app, panel 2 specifically.

In this version:

1) Going EXTERNAL.
* In addition to NodeMCU being a server (we have 2 servers, each on a port), it will be a client that will connect to an external computer server. I will follow the 2 clients mechanism (from the same mobile) that will interchange all the time each 2 minutes, i.e. creating a new socket then killing the old one 4 seconds later. This has the benefit to properly receive any message through the old socket during the time of new socket setup.
	* Once a new socket is created the NodeMCU will send an ack to the intermediate server to inform it about itself (owner and index of panel). For technical reasons the NodeMCU might send a report instead of an ack.
* CANCELLED - The NodeMCU listens each say 1.5 seconds; the NodeMCU must receive a polling packet from the computer server. (Note: if the user downloads 1 KB each 1.5 second then it's 1.7 GB per month!! -not efficient.) - later - I will need to check the data consumption, and expect the average daily consumption, through Wireshark e.g. - By the way, I didn't do that even without the external capability (I mean between the NodeMCU and the mobile alone)!
	* If no packet was received, the NodeMCU will directly create a new socket.
* After a failed attempt to create a new socket while the other port makes no connection as well (another approach is to wait 7 seconds after receiving no response from server but I dropped this heart beat mechanism) then D10 pin will be set to blink and the NodeMCU will keep trying to connect to server periodically (it can be 5 or 7 seconds or even more). I chose to let loosing WiFi connection more prioritized than loosing connection to computer server, thus turning on instead of just blinking.
* In each loop the NodeMCU checks if it has received a message from the intermediate server and processes it if the message was valid.

* Note the the variable current_port in IntermediateServer in file ConnnectionSetup.h is like active_client_index in android mob code.
* 

2) make all the messages format owner:sender:receiver:message_content

3) the messages sent to the intermediate server are ended by the 'z' instead of the '\0' char because the server there couldn't deal with the '\0'. Also necessary modifications were made in the intermediate server mod section.

4) Notifier pin is fine and working...

5) pin 10 as input and pin 9 its corresponding output pin is also tested and working.

6) too much locks to protect in order not to call createSocket1() twice ( same for createSocket2() ). Not tested but should work and the locks won't do any good, it is for protection nevertheless.