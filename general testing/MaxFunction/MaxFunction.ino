const int i3 = 1;
const int i4 = 2;
//char b[ max(i3, i4)]; //This can't work

void setup() {
  // put your setup code here, to run once:
  const int i1 = 1;
  const int i2 = 2;
  char a[ max(i1, i2)]; //This works because it is inside a method.
}

void loop() {
  // put your main code here, to run repeatedly:

}
