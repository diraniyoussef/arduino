#include <ESP8266WiFi.h> //I use the delay() function, this is why I must include it here.
#include <Wire.h>
#include "ESP_PCF8574.h"

/**
 * Constructor for the class
 */
ESP_PCF8574::ESP_PCF8574() {
	m_address = 0;
  m_currentOutput = 255;
  data_read = false; //data_read should be used in coherence only with getBit method. 
                     // setBit method doesn't need data_read since it itself returns info about correctly setting the bit or not.
} // End of constructor

/**
 * Set the address and pins to be used for SDA and CLK in the Two Wire (I2C) protocol.
 */
void ESP_PCF8574::begin( uint8_t address ) {
  begin(address, SDA_PIN, SCL_PIN);
}
void ESP_PCF8574::begin( uint8_t address, uint8_t sda, uint8_t scl ) {
	m_address = address;
	Wire.begin(sda,scl); //doesn't return anything.
 /*
  Wire.pins(sda,scl);
  Wire.begin();
  */
} // End of setPins

/**
 * Set the bit (range 0-7) of the GPIOs to the supplied value.
 */
bool ESP_PCF8574::setBit(uint8_t bit, bool value) {
	// Check that the bit is in range.  Must be 0-7.
  if (bit < 0 || bit > 7) {
		return false;
	}
  bool success; 
	if (value == true) {
		success = setByte( m_currentOutput | (1 << bit) );
	} else {
		success = setByte( m_currentOutput & ~(1 << bit) );
	}
  return( success );
} // End of setBit

bool ESP_PCF8574::isDataRead () {
  return data_read;
}

/**
 * Get the value of the bit of the GPIO as input.
 */
bool ESP_PCF8574::getBit(uint8_t bit) {
	if (bit < 0 || bit > 7) {
		return 0;
	}
	uint8_t byteValue = getByte();
	return( byteValue & (1<<bit) ) != 0;
} // End of getBit

/**
 * Write the value of the byte as the output of the GPIOs.
 */
 
bool ESP_PCF8574::setByte(uint8_t value) { 
  // Guard that we have been initialized
  if (m_address == 0) {
    return false;
  }
  m_currentOutput = value;
  //*****************************************************************************************This block is to make sure the I2C device is connected. 
  //if Vcc wasn't connected it may not write correctly. And I cannot tell using this block.
  int number_of_bytes_to_read = 1;
  delay(25);
  Wire.requestFrom(m_address, number_of_bytes_to_read);
  delay(1);
  if( Wire.available() == number_of_bytes_to_read) {
    Wire.read();/*I don't care what's been read. But I think I must use it if there 
                 *was some kind of a buffer, so that I don't keep a waste info there.
                 */
  //*****************************************************************************************End of the block
    Wire.beginTransmission(m_address);
    Wire.write(m_currentOutput); //The return value of the "write()" method should normally be the number of written bytes.  
                                  //In reality, it always returns 1, even if PCF8574 is totally unconnected, so unfortunately I cannot rely on it.                                  
    Wire.endTransmission();
    return true;
  } else {
    Serial.println("Couldn't write to PCF8574. Is it connected?");
    return false;
  }
  
} // End of setByte


/**
 * Get the value of the byte of the GPIOs as input.
 */
uint8_t ESP_PCF8574::getByte() {
  int number_of_bytes_to_read = 1;
  delay(25);
  Wire.requestFrom( m_address , number_of_bytes_to_read );
  delay(1);
  if( Wire.available() == number_of_bytes_to_read ) { //that's a guarantee that GND, SDA and SCL are connected, not a guarantee that Vcc is connected.
    data_read = true;
    return Wire.read(); 
  } else {
    data_read = false;
    Serial.println("Data not available from PCF8574!");
    return 0;
  }
} // End of getByte



// End of file

