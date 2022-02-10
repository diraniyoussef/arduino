extern int* i;

int* int1 = new int();


//class1* class12 = new class1(); //here it doesn't work since it hasn't gotten the chance to know it.

class class1{
  public: 
  class1(){}
  
};

class1* class12 = new class1();
