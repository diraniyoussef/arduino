void setup() {
	Serial.begin(115200);	
	pinMode(0, INPUT_PULLUP);
}

void loop() {
	if(digitalRead(0)) {
		Serial.println("flash button not pressed");
	} else {
		Serial.println("flash button pressed");
	}
	delay(3000);
}
