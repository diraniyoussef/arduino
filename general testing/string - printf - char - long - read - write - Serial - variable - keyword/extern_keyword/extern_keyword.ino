//#include "cppfile.cpp" //doesn't work because int* i is also defined here.

int* i;
extern int* int1;
//extern class1* class12; ////here it doesn't work since it hasn't gotten the chance to know it.

void setup() {
  // put your setup code here, to run once:
i = new int();

}

void loop() {
  // put your main code here, to run repeatedly:

}
