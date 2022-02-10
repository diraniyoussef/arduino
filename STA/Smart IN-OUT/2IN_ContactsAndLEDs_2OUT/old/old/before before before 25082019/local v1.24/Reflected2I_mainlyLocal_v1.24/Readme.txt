Reflected2I_mainlyLocal_v1.24

*This version stems from the series of "Reflected2I_v1.23" in the Backup file.
*This arduino folder interworks with "Reflector2O_local_v1.24" arduino folder.
*This interworks with eclipse intermediate code "ControlLinker_v0.5"
*Mobile apps do not control this panel but may ask to get a report. The mobile app version is "Out3P_Ref1P-2In.v3"
*This is associated with DesignSpark file "Reflected_2In".

Note: Perhaps there are some old important notes in previous Readme files that are worth of being checked.

***Feature
--It will inform locally a certain local panel, and it will receive report requests from locally connected mobile apps. What is new is that a remote mobile app will be able to send report requests too. This is necessary for the application of knowing the electricity status.
We do care about making static IPs.
In order to do that I made some minor changes in the main sketch file and in connectionSetup() in ConnectionSetup.h file. I also made some changes in Remote.h in order to avoid the inform remote related stuff. I made //To be commented next to those parts.