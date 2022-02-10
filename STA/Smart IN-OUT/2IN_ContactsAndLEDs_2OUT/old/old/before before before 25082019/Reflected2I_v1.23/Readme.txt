Reflected2I_v1.23

*This version stems from the series of "Reflected2I_v1.21" in the Backup file.
*This arduino folder interworks with "Reflect2O_v1.23" arduino folder.
*Mobile apps do not control this panel but may ask to get a report. The mobile app version is "Out3P_Ref1P-2In.v3.1"
*This is associated with DesignSpark file "Reflected_2In".

 - Internet is tested (STABLE VERSION FOR INTERNET) and response time is good. I may have noticed once that the read chars were more than the usual, like maybe 64 or so. Anyway, I may have to enhance analyze() so that we can reaad more than just one message per buffer.
 - Extra panels is not yet tested.

***Enhancement:
--onData event is now able to be triggered one after another without overriding the registered incoming message even if it happened before analyzing the messages. 
--Our panel is not sending anything to the intermediate when it switches socket (like every minutes) and this is bad since it will not register a new mod that way. So a send Hi intermediate is necessary.



***To Be FIXED later:
--There is still a little trick in the rush in case of intermediate relay. Assume we are informing like 15 remote panels through the intermediate server. We will receive many ack messages from the intermediate server (and most probably they will be concatenated). Since we only process one buffer at a time, then only one message will be processed. So for this message we will not receive an ack, but we may still receive an intelligible report from every remote panel (if we were lucky enoough and the reports weren't concatenated).
SO FIX THE ANALYSIS SO WE CAN ANALYZE MULTIPLE CONCATENATED MESSAGES IN ONE BUFFER.
AND THIS IS NOT ONLY USEFUL FOR ACK, ACTUALLY IT'S FOR EVERYTHING.



***Features:
As for the case that for a stupid reason, we lost connection to the intermediate server and we still think we're connected (don't know really if this can happen but why not.), then the old mechanism is fine since after about two minutes we will handle it. So nothing additional is made for this case.




Note: I don't think I have deleted the code relative to OUT pins except the array maybe.


***Future Perspective

Unfortunately, life isn't so mutually exclusive. A reformation of the 4 files must be done to meet the following targets:
--Unnecessary new connections: 
Case 1: Assume that a client entity has already connected to this NodeMCU through the LocalServer.h file, in this case, we don't have to make a new async client connection to send a report on an IN pin change. --THIS WILL NEVER BE IMPLEMENTED.
Case 2: Assume that we made a connection to a local entity to inform it (we always do connect, all the time, even if no IN pin has changed). Then the local entity (like being a mobile e.g.) wanted to request a report from our panel, in this case, the local entity can use the pre-made connection instead of contacting the local server our panel has set and is listening to. --This can later be implemented.
Note that this is NOT A PROBLEM for remote entities since all remote entities use the same remote connection. 
Ok. Still, is this really a problem for local entities? Is this really needed? I think "no, not really needed". Will I implement it? Not case 1, but case 2 yes. Because our panel is forced once being powered on to connect to the other entity. So if the other entity wanted to get a report from our panel it will first check its wifi clients in its local server file, if it has found our panel then it sends its report through that connection. If it hasn't found it (which is very unlikely), then it initiates a connection in its local async client file through which it sends its request to our local sever. Again, even in case 2, it's not needed. --This is TO BE IMPLEMENTED IN THE OTHER ENTITY and even here TO BE AMENDED to meet the implementation in the local entity as follows: 
InformLocal must check within its own set of received messages for incoming messages from local_server_mob_i_Id_buff. If so, and if the message was sent as a request for a report e.g., then InformLocal must make an instance of LocalMessageOp and will only use its processMessage() method. Some amendments will be necessary to LocalMessageOp .


***The motivation and philosophy behind this is: 
We have 2 latching inputs that can be considered as "electricity mains" and "generator mains". This panel will transfer its IN pins statuses to 1 or more other panels, whether local or remote.
Note: Although we are transferring the status of some pins, but in this panel's code we are assuring a way of COMMUNICATION BETWEEN PANELS!

--On change state of any of these 2 inputs, the NodeMCU will have to send (as a client panel) to a particular and well known server panel (later it may send this to a particular well known mobile) an outgoing request reflecting the change of the state of the input. -----DONE

--This panel will also reply to a report request (:R?) coming from a mobile with the state of the 2 input pins, as usual. -----DONE

--This panel will have an LED that will be continuously ON when disconnected to wifi. This notification LED will blink if not "strictly" connected to all the server panels. By "strictly" we mean that it must, in the normal case, receive an ack from the panel itself. 
BTW, is it really reliable now in terms of constant connection and readiness? Not sure since IDK if the connection will be lost if e.g. the other entity was turned off e.g., for local entities.
And situation is worse in case of remote entities since the remote entity may not be connected to intermediate server for some reason like being turned off or having no internet connection. 
YES though, it is reliable whenever it sends a report and expects a reply back.--DONE.
This mechanism has no effect on the usual processing concerned with being a server that gets requests. 
We can make it reliable by sending a periodic report, but what should be the period? 5 seconds, 30 seconds, 1 minute, ...???

--As soon as this panel powers on it must inform other panels (local and remote) and it keeps trying to connect to other entities which have not yet been connected or connected but for some reason didn't receive a replied report from them. -----DONE

--Hardwarewise this panel will have 2 LEDs associated directly to the 2 IN pins (no need for OUT pins). -----CONSIDER IT DONE

--Once this NodeMCU is turned on, it tries to establish connection with all the ones it needs to inform an IN pin change. --DONE
And in case a connect attempt has failed, then it keeps trying to connect (--DONE. Note: now it's 20 seconds for both local and remote entities but this can be changed).

--In case the entity didn't reply back (mainly in case of remote entity --DONE. You may think it's a little silly to wait for a replied report from local entities, and it may be true, nevertheless it's a lot better to implement there too. --DONE.) and has not yet been connected (in case of local entity. With remote entity, the retrial to connect to intermediate server is already implemented. Anyway for local and remote it's --DONE), this NodeMCU keeps trying to do it again, say each 5 seconds. 

--Then I will implement the functionality of getting reply back from other entities, and based on it our panel will blink the LED accordingly. If didn't receive ack1 within a certain period of time (5 seconds) from any informed entity then blink! Also, if not connected to intermediate server or to any local entity then blink!

--The other local entity is not forced to initiate a connection to our panel when it's powered on.








***FIXES
1) Since when restarting we make pins 15 - 0 - 2 (D8, D3, D4 resp.) Low, high, high resp. So making D8 the connectFailureNotifierPin is a good idea! That is to not to make it an IN pin at all, since IN pins, when user intervenes, are pulled to 3.3V. 
2) I separated the method "preConnectivity()" in Intermediate.h for better organization. 
3) I created this new variable last_connected_index in Intermediate.h 
4) I got rid of is_PCF_connected and I retained one simple test of PCF wiring connectivity in loop() based on which the program flows correctly or not. There should be a blinking for that...
5) The whole process in loop to handle the NodeMCU as a server able to receive incoming connections and requests is now moved to a special file called LocalServer.h
6) The variables in Intermediate.h which were outside of the main class are now inside due to a fix in each on-event function; [this] issue.
7) Making a lot of arrays as const in the sketch file and make the necessary modifications accordingly.
8) In the local server message handling, thus in the usual flow of (previously) messageAndOperation, we must have     the_request.pin = '\0';      right in the beginning of analyze() because we're not deleting the messageAndOperation object in the (previously) flow of Loop. But its old place may have been fine though.
9) Blinking of ConnectFailureNotifierPin must be fixed. This must be controlled from one place, maybe the loop. ------TO BE MADE IN FUTURE VERSIONS
10) Pins D3 and D4 are Normally HIGH,  so if the user is pressing any of them (thus directly grounding them) and if it happens that panel wants to reset then it will stuck! This is why I changed the in pins to D5 and D6.




***Code structure:
InformLocal is like an API that uses class InformEntity in Remote.h file. So the operations are actually in InformEntity.
class LocalServer in file LocalServer.h is totally independent from the panels we need to inform or receive a replied report from. It's like the old days.
class InformRemoteAPI in file Remote.h is an API that uses class InformEntity in Remote.h file. 
Class Remote implements the instance of InformRemoteAPI. Class Remote also runs as a remote server that accepts requests from remote clients and reply back with a report. As you can see, class Remote handles the informing operation and the replying as a server operation, and that is because all this is made through the intermediate server. But please note that each functionality of the 2 has its own set of entities to accept to interact with.
Since the informing is done by this panel being a client, so I decided to use AsyncClient (to avoid timeout waiting, which stalls the panel in normal flow case) for both local and remote informing. For local informing we have as many asynchro clients as we have entities to inform, while in remote case we still have the same 1 asynchro client. So InformLocal uses AsynchroClient (which has 2 AsyncClient's since we use the 2 connections mechanism) class, and to connect to the intermediate server, Remote class uses AsynchroClient as well. (BTW the connection to the AsynchroClient is handled in each loop through preProcess() and postProcess() methods)


***Details
1) Got rid of OUT pins array. The sendReport whether to mobile or other panels will be about the IN pins only. **DONE
(If some time in the future you wanted to make some OUT pins on the same NodeMCU then you have to add them again and make necessary modifications.)
2) I commented everything about PCF but kept it in code. **DONE
3) Clearly there will be no updatePinAndEEPROM and there is no checkManualUpdate **DONE
4) It's wrong to remember the state of the IN pins in the EEPROM. **DONE
5) In each loop I check the state of the IN pins and compare them to their last state which is remembered. **DONE
6) The sent report out of this panel is still like O3TO4F even though it is about IN pins. This is ok since I will do a sort of mapping in the other panel between the read pins and its own pins there.
7) Since I chose the NodeMCU's mechanism to be an async client (whether for Internet connection (that is to inform remote server panels or to act as a remote server for remote entities) or for to contact local server panels) so I made a special file for that (which is basically Intermediate.h previously).
*SO BASICALLY THIS NODEMCU HAS 4 ROLES TO PLAY:
1- a local server to receive report requests (no pin-change-state requests since this panel is only for pins).
2- a client to inform remote servers through the intermediate server.
3- a server to reply back to remote clients through the same intermediate server connections.
4- a client to inform local servers through the same connections to the intermediate server.
8) 







A few problems in this sub-version:
1) When there is a sudden change in 2 pins, and one after another with a short while, two messages would be sent like this e.g.: Youssef_70853721:4:5:O3TO4F and Youssef_70853721:4:5:O3TO4T. 
And when they are read by the panel 5, they come totally concatenated like this: Youssef_70853721:4:5:O3TO4FYoussef_70853721:4:5:O3TO4T, and only the O3TO4F is considered and an ack1 is sent back. The result is incoherence between panels 4 and 5.
Solution: is to let panel 5 reply back with a report instead of ack1. This solution works fine as long as the reflector panel analyzes only 1 incoming message at a time. - DONE

2) For some reason, in the message received from 5 (which is the report), the readCharNumber is extra increased by 1.

3) Sometimes a couple of messages which are not sent at their time are being received by our NodeMCU directly as a flow one after another. But that's not a problem.

4) If panel 5 connects then disconnects, our panel 4 won't know that it has disconnected. But the good thing is that the code handles this. Please do know that even with this fixed, the need for replied report mentality is still valid.

5) sometimes it took about 1 minute or more to connect to panel 5. But it eventually worked. To be checked. One time, after I played around with counter_to_force_close_socket, it didn't connect at all. 
Another trial showed the following: always the same weird case of direct disconnection then sudddenly it connected but couldn't get a reply from the panel (the reason could had been that messages were stuck somehow in the buffer because it happened once that I received a flow of 3 messages from the server one after another, after many informing attempt that didn't had a reply), then later it disconnected thanks to suspecting the disconnectivity and continued with the same weird case, then it connected and worked fine. In this case min_threshold_to_force_close_socket didn't interfer.
	Can our panel connect to any other panel in this case?
	Does restarting the panels solve it? Or does restarting the other panel (5) fix it? Actually, restarting both worked. (BUT THIS CAN'T BE A GOOD SOLUTION) Restarting each one on its own didn't work.
	Could stopping the server in panel 5 help? Then beginning it again? implemented in reflector... what about in reflected?? 
	Flushing the messages can help? Don't know how to do it. And Flush has another meaning. Will it whelp in the other meaning? guess not. 
	**Maybe it's really useful to flush any client before you stop it.... We want to clean the buffers...
	I commented the stop() in onError method.
	Must understand the mechanism...
	Testing a quick min_threshold_to_force_close_socket ......... I'm not sure if this causes a problem...
	
Again What are the signs of the problem? 
1- The most obvious is a direct disconnection after connection. This appears in both panels AFAIK. This is implemented in Reflector but not tested yet.
2- Another sign is a series of unsuccessful attempts... This appears in the reflected panel. Does it appear in the reflector? 
	Proposed solution for the reflector: After 2 minutes and a little, if no connection is received from our expected active informer on the other port, or if there was no connection at all for say 20 seconds, then restart the server. NO. I DIDN'T FOLLOW THAT. I PREFER TO LET THE REFLECTED FIX ITSELF unless refreshing the listener in the refelctor helps.
	Perhaps THE BEST SOLUTION  is to have the server panel listen to MANY ports, at maximum two ports at a time and let it stop listening to the other ones. And let our informer panel create a connection with all ports, send to port it was able to connect to! For each and every message, and then close the connection! Simply.... I don't need a continuous connection because I'm not sending large files. How can this solve the problem? The server is able to switch ports and not go back to them right away, this way it can close the other ports and not listen to them for some time. E.g. a port might get 2 minutes of rest and listen for just 25 seconds. This solution relieves all ports so all ports will be fine assuming that the problem was the usage of the same ports. I don't know if listening to about 20 ports is fine. Why not. THE PROBLEM IS WHEN WE WORK WITH THE INTERNET, then the reflector must always be connected second by second.
EXPERIMENTAL FINDING ABOUT THE PROBLEM: In out informer panel, I tried stopping then waiting a little time just before directly connecting, and it didn't work!





