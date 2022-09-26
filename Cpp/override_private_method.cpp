//https://youtu.be/H2zeBl6h_Vc

#include <stdio.h>

class Base {
  private:
	void private1()
	{
		printf("private1 base\n");
	}
	virtual void private2()
	{
		printf("private2 base\n");
	}
	void private3()
	{
		printf("private3 base calling public3\n");
		public3();
	}

  public:
	void public1() 
	{
		printf("public1 base\n");
		private1();
	}
	void public2()
	{
		printf("public2 base\n");
		private2();
	}
	virtual void public3() 
	{
		printf("public3 base\n");		
	}
	void public4() 
	{
		printf("public4 base calling private3 which in its turn calls public3\n");		
		private3();
	}


};

class Derived: public Base {
  private:
	void private1()
	{
		printf("private1 derived\n");
	}
	void private2() override
	{
		printf("private2 derived\n");
	}
	void public3() override
	{
		printf("public3 derived\n");
	}
	/*
  public:
	void public2() 
	{
		printf("public derived");
		//private1();
	}
	*/
};

int main()
{
	
	Derived* d = new Derived();
	/*
	//The answer is yes, you can override a private method and well use it.
	d->public1();
	printf("\n");
	d->public2();
	printf("\n");
	*/
	
	d->public4(); //Here I override a public method called by a private one. This is a complicated thing not needed. Directly overriding private methods is enough.
	
	
}