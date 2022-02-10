Reflected2I_mainlyRemote_v1.24

*This version stems from the series of "Reflected2I_v1.23" in the Backup file.
*This arduino folder interworks with "Reflector2O_remote_v1.24" arduino folder.
*This interworks with eclipse intermediate code "ControlLinker_v0.5"
*Mobile apps do not control this panel but may ask to get a report. The mobile app version is "Out3P_Ref1P-2In.v3"
*This is associated with DesignSpark file "Reflected_2In".

Note: Perhaps there are some old important notes in previous Readme files that are worth of being checked.

***Feature
--It will inform through internet a certain remote panel, and it will receive report requests from remotely connected mobile apps. It will also be able to receive report requests too from local mobile apps (which is necessary for the application in terms of monitoring).
We don't care about static IPs.
In order to do that I made some minor changes in the main sketch file and in connectionSetup() in ConnectionSetup.h file.