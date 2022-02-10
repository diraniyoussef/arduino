void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

}

void loop() {
  int i;
  static int j;
  i = i + 1;
  j++;
  Serial.printf("value of i: %d\n", i);
  Serial.printf("value of j: %d\n", j);
  delay(3000);
  // put your main code here, to run repeatedly:

}
