Arduino "AP_3mob_6Outputs_5Inputs_v1.z" Readme file:

Usually, this is in coherence with the Android code of folder Control6O5PanelInput.

Version 1.1 is enhanced over the previous version by the order in which it analyzes the received messages.
We know that each loop iteration handles only one message.
And we know that each new socket has a message with it.
It handles them from the older to the most recent.			(NOT YET IMPLEMENTED!)