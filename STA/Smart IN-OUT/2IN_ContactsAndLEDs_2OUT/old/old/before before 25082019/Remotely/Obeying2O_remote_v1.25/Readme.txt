Obeying2O_remote_v1.25 - Arduino

--This version stems from the series of "Reflector2O_remote_v1.24" found in the Backup file.
--This arduino folder interworks with "Ordering2I_mainlyRemote_v1.25" arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.5"
--Mobile apps do not control (for this version) this panel but are able to ask to get a report since what's really interesting is the state of this panel's relays (not the ordering panel).
--This is associated with DesignSpark file "Obeying_B1_v1.5.pcb" in folder "Reflector_2Relays".


***Features
--It reflects a remote ordering (informer) panel through internet and will change the state of the 2 relays within according to the state of the (latching) buttons of the ordering panel. DONE
--It will receive report requests from remote and local mobile apps. DONE
--We will have a memory for the state of the relays and their buttons, so whenever it turns on it remembers the last state. DONE
--

***Procedural Notes
--We still care for static IPs for local mobiles in this panel's WIFI network.
--

***Fixes
--When sending reports and hi to intermediate we used to duplicate the "\", so I made it to happen only once.











