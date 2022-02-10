"STA_6mob_5O5I_2Ports_v1.13_discontinued" Arduino Readme file

This stems from "STA_6mob_5O5I_2Ports_v1.12"

In this version:

1) A fix: inside "loop", the NodeMCU will listen to clients in order. (I have CANCELLED the idea ALTHOUGH I HAVE SET IT UP, in a loop I can listen to 6 clients or even 12 and nothing will happen! What really matters is that a connected client is either accepted or refused in order to allow only particular clients (apps) to control the NodeMCU, which I will implement in "STA_6mob_5O5I_2Ports_v1.13".)

2) Other fixes like :
	a) making an array "server" instead of server1 and server2
	b) in MessageAndOperation I made this method getLastCharOccurence which is the correct method.

