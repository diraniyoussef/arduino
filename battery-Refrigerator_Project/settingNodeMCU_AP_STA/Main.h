#include "Setup.h"

/*The following few variables should'd been private inside the class*/

/*
const byte Start_AP_Index_In_EEPROM = 101;
const byte Max_AP_Buffer_Size = 250;

const int max_EEPROM = Start_AP_Index_In_EEPROM + Max_AP_Buffer_Size;
const char trailor = 127; // should'd been public inside the class.

void resetEEPROM()
{
	// This will close all opened sockets because all operations
	Serial.println("Resetting the EEPROM");
	EEPROM.write(Start_AP_Index_In_EEPROM, trailor);
	EEPROM.commit();
}
*/