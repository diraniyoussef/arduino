"AP_6mob_6Outputs_5Inputs_2Ports_v1.11" Arduino Readme file

This stems from "AP_3mob_6Outputs_5Inputs_2Ports_v1.11"

1) The goal is to check how many clients can it hold, so I'll begin with debugging... I noticed that the server can handle 4
concurrent sockets but the wifi AP couldn't handle them all. That's why I'm going to make the NodeMCU a station connected
to Electrotel_Dirani network.