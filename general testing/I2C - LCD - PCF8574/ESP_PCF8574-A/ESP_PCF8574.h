#ifndef ESP_PCF8574_h
#define ESP_PCF8574_h

#define SDA_PIN D2
#define SCL_PIN D1
/**
 * Class definition for the PC8574 device.
 */
class ESP_PCF8574 {
public:
	/**
	 * Constructor for the class
	 */
	ESP_PCF8574();

	/**
	 * Set the address and pins to be used for SDA and CLK in the Two Wire (I2C) protocol.
	 */
	void begin(uint8_t address, uint8_t sda, uint8_t clk);

	/**
	 * Set the bit (range 0-7) of the GPIOs to the supplied value.
   * The return value is an indication whether the bit has been really written or not.
	 */
	bool setBit(uint8_t bit, bool value);

  /*
   * get the value of data_read. It needs to be checked BEFORE accounting on the value of getBit.
   */
  bool isDataRead();
  
	/**
	 * Write the value of the byte as the output of the GPIOs.
	 */
	bool getBit(uint8_t bit);

private:
	int m_address;
	uint8_t m_currentOutput;
  bool data_read;
  bool setByte(uint8_t value);
  uint8_t getByte();

};
#endif
// Nothing after here
