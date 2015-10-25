#include <cstdlib>
#include <cstdio>
#include <cstring>


class test{
	public:
int a;
int b;
test(int a, int b)
{
	this->a = a;
	this->b = b;
}
	
~test()
{
};
	
test &operator = (test t) 
{
	this->a = t.a;
	this->b = t.b;
	return *this;
};
	
	
};

typedef struct record RECORD;
struct record{
	
	int size;
	test *t;
};


int main(int argc, char* argv[])
{
	
	RECORD src;
	RECORD dst;
	char *recup;
	recup = new char[34];
	char *testChar = new char[50];
	strcpy(testChar,"salut les copains");
	printf("%s\n",testChar);
	testChar += 20;
	memcpy(testChar,testChar,5*sizeof(char));
	printf("%s\n",testChar);
	testChar -= 18;
	printf("%s\n",testChar);

	
	src.size = 10;
	src.t = new test(1,2);

	memcpy(recup,&src,sizeof(record));
	memcpy(&dst,recup,sizeof(record));

	printf("size = %d, a = %d, b = %d\n",dst.size,dst.t->a,dst.t->b);
	//printf("sizeRecord = %d\n ",sizeof(record));
	//printf("sizeTest = %d\n ",sizeof(test)+sizeof(int));
	
	
	test *tsrc;
	test *tdst;
	tsrc = new test(1,2);
	tdst = tsrc;
	printf("a = %d, b = %d\n",tdst->a, tdst->b);
	


	exit(0);
}
