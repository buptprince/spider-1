#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WRITE2FILE_H
#define WRITE2FILE_H

/*
url.txt：一行一个url及其编号，空行，后面是url链接关系，一行两个编号表示链接关系
*/

//current url num
int urlNoCount=0;

FILE* urlfp;
FILE* pairfp=fopen("tmp","w+");


void writeUrl(char *data,int No){
  fprintf(urlfp,"%s %d\n",data,No);
}
void writePair(int from,int to){
  fprintf(pairfp,"\n%d %d",from,to);
}


void mergefile(){
   char line[100];
   fseek(pairfp,0,0);

   while(1){ 
		fgets(line,100,pairfp);
		fprintf(urlfp,"%s",line);

		if(feof(pairfp))
			break;
	}
	fprintf(urlfp,"\n");
	fclose(urlfp);
	fclose(pairfp);
	remove("tmp");
}

#endif
