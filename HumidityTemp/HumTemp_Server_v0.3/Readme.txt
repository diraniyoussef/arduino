HumTemp_Server_v0.3  - Arduino

--This version stems from "HumTemp_Loc_v0.2"
--This will interwork with eclipse intermediate code ""
--This interworks with Android App ""

THIS IS A WORKING VERSION

***Details
-- This has the functionality to switch either to Internet commmunication or to local commmunication, by an order initiated by the mobile app.
--One inconvenience is that the Setup.h has been modified to support user assigning the panel as an Internet server, a local server, or both, and at the same time this version of Setup.h does not support Obeying IPs. This means some work to do when wanting to support obeying IPs.
--Note that this panel does not inform any other panel. Nor local, nor remote. 
--In terms of implementation, I was able to distinguish remote server (thus Internet server) functionality from the inform remote functionality using a boolean variable set in the main sketch file.
--Later, it may be advisable to even make the inform functionality set by the mobile app configuration. But I don't want to live with myself too much, so this is left as per need.

***Problems
-- 

***Requirement
--

***Fixes
--
