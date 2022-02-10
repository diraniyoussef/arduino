Arduino "AP_3mob_6Outputs_5Inputs_v1.0" Readme file:

Usually, this is in coherence with the Android code of folder Control6O5PanelInput.

Concerning the orderin which the code analyzes each incoming message:
We know that the android code in Control6O5PanelInput doesn't destroy sockets, instead it tests if the socket 
is closed or not. This means that one socket might involve many messages. So having say 2 mobiles connected,
we don't know when any of them has sent a message or not, so a fair algorithm like round robin among the clients
is not bad. Remember that each loop in the Arduino code handles only 1 message.

The following is WRONG:
In addition since we are ok with one socket in Control6O5PanelInput, if e.g. serverClients[0] was the socket made by the 
mob1, then serverClients[1] MUST be referring to another mobile because each serverClients[i] is a socket so all 
the messages of a particular mobile are gotten through its dedicated serverClients[i].
This is the COREECTION:
The same mob1 can occuppy 2 serverClients if I was fast enough, and this is obvious because although there is only one
listening server (on one port) it is server.available() that assigns a new socket and this can be called to be assigned
to any serverClients[i].
