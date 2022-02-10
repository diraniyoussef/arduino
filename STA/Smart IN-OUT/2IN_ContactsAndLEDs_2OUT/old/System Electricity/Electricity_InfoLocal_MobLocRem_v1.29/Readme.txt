Electricity_InfoLocal_MobLocRem_v1.29  - Arduino

--This version stems from "Electricity_Ordering2I_local_v1.28"
--This arduino folder interworks with "Electricity_ObeyLocal_v1.28" arduino folder.
--This interworks with eclipse intermediate code "ControlLinker_v0.8"
--This interworks with Design Spark : ( > B1_v1.8) in folder Ordering_2In. It also interworks with AutoReflected_2In\( > B1_v1.5)
--This interworks with Android Out3P_Ref1P-2In.v3.41

THIS IS A WORKING VERSION

***Motivation behind this version
-- Adding RST functionality.
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
