//This sketch seems successful. Pin 14 makes an interrupt when it gets grounded. It fires a function "callPolice".

//const byte outPin =  4;
const byte inPin = 14;
volatile unsigned long last_button_time = 0; 
unsigned long button_time = 0; 
unsigned long disabled_interrupt_time_interval = 1 * 60 * 1000; //1 minutes. A typical minimal value is probably 250 or 1000, but a 1 minutes value is ok for the police application                                                                
unsigned long long_int_max = 4294967295;
volatile boolean not_first_time = 0;

void setup() {
  Serial.printf("Youssef: Entering setup!\n");
  Serial.begin(74880);
  Serial.setDebugOutput(true); //this is dedicated to GPIO2 naturally  
  

  //Serial.printf("State of pin %d is %d\n", inPin, digitalRead(inPin));
  //Asssign an interrupt to pin 14.
  pinMode(inPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(inPin), callPolice, FALLING);
  sei(); //enable interrupts
  //Serial.printf("After being an interrupt, the state of pin %d is %d\n", inPin, digitalRead(inPin));
  ///pinMode(outPin, OUTPUT);

  //Go Internet...
  
}


void callPolice() {
  button_time = millis();
  Serial.printf("Interrupt entered, time in ms is: %u\n", button_time);
  //The following if statement is just to prevent reentering the interrupt functionality in the transient case of voltage change, also not to call the police within a short amount of time.
  if (button_time - last_button_time > disabled_interrupt_time_interval || (button_time < last_button_time && (button_time + long_int_max - last_button_time > disabled_interrupt_time_interval)) || not_first_time == 0)//after about 50 days, millis() will overflow
  {
    Serial.printf("Help! Help! I'm being robbed!, time in ms is: %u\n", millis());
    last_button_time = button_time;
  }
  not_first_time = 1;
  
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("Youssef: Entering loop again!, time in ms is: %u\n", millis());
  delay(20000);
}



