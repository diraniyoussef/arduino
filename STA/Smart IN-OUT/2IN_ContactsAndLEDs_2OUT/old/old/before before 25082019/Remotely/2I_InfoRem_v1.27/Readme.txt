2I_InfoRem_v1.27  - Arduino

--This version stems from "Ordering2I_mainlyRemote_v1.26"
--This arduino folder interworks with "Obeying2O_remote_v1.27" arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.6"
--This interworks with Design Spark : B1_v1.8 in folder Ordering_2In. It also interworks with AutoReflected_2In\B1_v1.5

THIS IS A WORKING VERSION

***Motivation behind this version
-- User will have control over whether to let this panel connects to its peer panel locally or over the internet.
( Clearly, a panel with no mobile app won't have this functionality. Otherwise, I will need to dedicate a pin for that.)

***Details
-- In EEPROM, there will be a dedicated bit for this panel to connect to its peer.
-- User will send a specific message for this purpose, and the panel will interpret it, save the dedicated EEPROM bit, and control whether to run local_server.serverSetup();	or	  remote.setup();

***Requirement
--

***Fixes
--
