Reflector2O_v1.23 - Arduino

*This version stems from the series of "Reflector2O_v1.22" found in the Backup file.
*This arduino folder interworks with "Reflected2I_v1.23" arduino folder.
*Mobile apps do not control this panel but won't ask to get a report since it doesn't have non-reflecting output pins.
*This is associated with DesignSpark file "" especially board "".

- Internet is tested (STABLE VERSION FOR INTERNET)

--onData event is now able to be triggered one after another without overriding the registered incoming message even if it happened before analyzing the messages. 


***Features
--Is sending hi to intermediate is enough?
Well let's discuss. I was thinking that no, and that when we directly connect I won't be sending sendHiIntermediate, instead I will be continuously (based on a counter) sending a report request to the reflected. And I thiought that because, imagine the 2 panels (reflected and reflector) were ON and connected, then the reflector went off. The reflected panel has no way to know that. TRUE BUT REMEMBER THAT THE REFLECTOR REMEMBERS ITS STATE when it is on. In addition the REFLECTED IS ALWAYS CARFUL TO BE IN COHERENCE WITH THE REFLECTOR OR ELSE IT WILL KEEP SENDING REPORTS. So imagine that they were coherent then electricity went down for the reflector, then back on, then all is fine. Try to imagine any other scenario, and it will be just fine.
So the 
ANSWER 
IS 
YES. SENDING HI IS ENOUGH.



***The motivation and philosophy behind this is: 
--reflect the status of 2 IN pins from another panel. This relfects only 1 other panel, no more. It's possible though to reflect more than 1 panel but I didn't implement it here. I did implement though both LocalServer and Remote classes and I can choose whichever I want from either.





***Details
--In another (reflected, informing) panel, we have 2 latching inputs that can be considered as "electricity mains" and "generator mains". That panel will send to our reflector panel reports about its IN pins and this panel will map them to our 2 reflector output pins.--------

--Talking about the reflecting part, not any additional non-reflecting output pins (which do not exist for this panel), there is no need to reply to request, since the requests coming from a mobile will go to the reflected panel. So this panel won't send a report to anyone. It will reply back with the same report it has gotten though when it receives a message from a panel.-----

--This panel will have an LED that will be continuously ON when disconnected to wifi. This notification LED will blink if not connected to all the reflected panels, local and remote (it is loose for the remote since we only check the connection to the intermediate server). 
If the connection was lost for any silly reason, will this panel know about it? Not sure! It's best effort after all. 
So in each loop we have to check for connectivity.--------

--Once this panel powers on, nothing has to be done but to listen to ports and to incoming messages, local and remote.




***FIXES
--A cleaner code in LocalServer.h and Remote.h. And some methods are written once now in Remote.h and used in LocalServer.h which is cleaner.
--I remove async_client member from RemoteServerMessageOp in Remote.h
--



















