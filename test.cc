#include <stdlib.h>
#include <stdio.h>
#include <cstring>

typedef struct record RECORD;
struct record{
	
	int size;
};


int main(int argc, char* argv[])
{
	/**
RECORD rec;
rec.size = 16;
char* retour = new char[1024];
int i;

char *PData = new char[rec.size];

PData = retour;
memcpy(PData,&"hello",rec.size);
PData = retour+rec.size;
memcpy(PData,&"salut",rec.size);
**/
//printf("%s\n",retour+rec.size);

int *tab;
tab = new int[8];
int i;
for(i = 0;i<8;i++)
{
tab[i] = 0;
	
}

for(i = 0;i<8;i++)
{
tab[i] = tab[i]|1;
	
}

for(i = 0;i<8;i++)
{
tab[i] = tab[i]&0;
	
}

for(i = 0;i<8;i++)
{
printf("%d, \n",tab[i]);
	
}


	exit(0);
}
