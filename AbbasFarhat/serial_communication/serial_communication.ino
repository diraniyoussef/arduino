void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("I'm in loop\n");
  delay(5000);
  char c  = Serial.read();
  if (c == 't') {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    if(c == 'f') {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}
