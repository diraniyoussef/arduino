"STA_6mob_5O5I_2Ports_v1.13" Arduino Readme file

This stems from "STA_6mob_5O5I_2Ports_v1.12"
This interworks with "Control_1Panel.v2.b" android app (or even "Control_1Panel.v3.b" I believe).

In this version:

1) A connected client is either accepted or refused in order to allow only particular clients (apps) to control the NodeMCU. This is actually done for a server which normally accepts many clients of which are to be refused. It is for directioning purpose. It is not really needed here but it's clean. - done

2) In a loop all connected clients are served! (Non-accepted clients are disconnected). Thus neglecting the idea to serve just one client per loop. - done

3) Clean the code. TO BE IMPLEMENTED
	a) making an array "server" instead of server1 and server2 - done
	b) in MessageAndOperation I made this method getLastCharOccurence which is the correct method. - done
	c) making some objects on the heap in order to delete them manually, avoiding them on the stack. The reason is that I am assuming that when I do ESP.restart() what is on the stack is not freed. Not sure really but I'll do it. - done
	d) cleaning the code. - done
	e) Should I save some info on the eeprom for the first time? In order to avoid some abnormal on or off? Done.
	f) protect the buffer analysis section from a random message sent that may block the NodeMCU... It seems this is already made in the analyze() method - done
