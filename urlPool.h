#ifndef URLPOOL_H_INCLUDED
#define URLPOOL_H_INCLUDED



#include<map>
using namespace std;
int curPageCount=0;

map<int,int> urlMap;

int hashCode(char* s){

    int hash=0;
    while(*s){
        hash=31*hash+(int)*s++;
    }
    return hash;
}
  
int getNum(char* url){
	map<int,int>::iterator it;
	it=urlMap.find(hashCode(url));
    if(it!=urlMap.end()){
    	return it->second;
   }
    return -1;

}

int addnewPage(char *url){
	urlMap[hashCode(url)]=curPageCount++;
	return curPageCount-1;
}

#endif // URLPOOL_H_INCLUDED








