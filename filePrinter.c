#include<stdio.h>
#include<stdlib.h>


int main()
{
	FILE* pf = fopen("Fifa08","w");

	for(int i=0; i< 100000; i++)
	{
		fprintf(pf,"%d", i /1000);
	
	
	}

	fclose(pf);



}
