#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
	/**
	char fileName[20];
	strcpy(fileName, "hello");
	char newFileName[20];
	//on récupère le nom filename
	strcpy(newFileName, fileName);
	
	//on concatène avec le numéro d'index
	sprintf(newFileName,"%s.%d",newFileName,32);
	printf("%s\n",newFileName);
	**/

char *tmp;
char *page = new char[100];
char *pageApres = new char[100];
tmp = page;
int i;
for(i = 0; i<100/4; i++)
{
	
page = tmp + i*sizeof(int);
memcpy(page, &i, sizeof(int));
}

page = tmp+3*sizeof(int);
memcpy(pageApres, page,4*sizeof(int));
pageApres += 3*sizeof(int);
pageApres -= sizeof(int);
int res;
memcpy(&res, pageApres, sizeof(int));
printf("indice = %d\n",res);


}
