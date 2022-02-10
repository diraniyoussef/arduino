2I_InfoRem_v1.30  - Arduino

--This version stems from "2I_InfoRem_v1.29"
--This arduino folder interworks with "Obeying2O_remote_v1.28" or even some previous versions arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.6"
--This interworks with Design Spark : It also interworks with AutoReflected_2In\B1_v2.2

THIS IS A WORKING VERSION

***Motivation behind this version
-- Presenting the D7 pin as an OUT pin and responsible for resetting the NodeMCU. Mainly, the only place, for now, to use it is at DNS failure. (I have disabled this functionality for now, since I may be uploading the sketch to a PCB design that does not conform with the D7 pin handling)
-- Please note that some amendments (not yet tested but I believe no problem will happen) in the informing mechanism to prevent a weird error I once noticed.
These include : NodeMCU::isInOutIncoherent() in NodeMCU class
and in Remote.h in class InformEntity, the following :
informAllIfInPinChanged()
sendReportAndSetCounter( int i )
checkIncomingMessageAndFixCounter( char* message_buff, int message_length )


***Details
-- 

***Requirement
--

***Fixes
--
