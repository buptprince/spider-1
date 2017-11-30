
#pragma once

#include<iostream>
#include<vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <queue>

#include "BloomFilter.h"
#include "urlPool.h"
#include "cqueue.h"
#include "write2file.h"


#define bool int
#define TRUE 1
#define FALSE 0

using namespace std;

//服务器的地址  ip:port
// char *host;

//url等待队列
cqueue* waitUrlPool=initQueue();

int  Validation(char*);
char* makehost(char*,char*);
char* makeRequest(char*);
void htmlParse(char*);
char* AnalyseURL(char*,const char*);
void write2File(char*);

//数据校验，1，判断状态码是否为200  2，取得Content-Length
//返回值Content-Length，如果状态码不是200，返回0
int  Validation(char* head){
    char judgement[4]={'2','0','0','\0'};  
    if(strncmp(judgement,head+9,3)!=0){ 
        return -1;
    }
   int Number=0;         
   char *p=head;
        
    while(!((*(p+1))=='\r' && (*(p+2))=='\n'&& (*(p+3))=='\r' && (*(p+4))=='\n')){          
        if(!(*(p+1)=='\r' && *(p+2)=='\n'))
        {      
            Number++;
            p++; 
        } 
       else{
        p=p+2;  
      }
    }

	//得到数据的总长度
	char   ContentLength_str[8];
	char*  ContentLengthPos =strstr(head,"Content-Length:");
	if(ContentLengthPos==NULL){
		return -1;
	}
	sscanf(ContentLengthPos+15,"%[^\n]", ContentLength_str);
	int ContentLength;
	sscanf(ContentLength_str,"%d",&ContentLength);
	//总长度
	int Total; 
	Total=Number+ContentLength;  
	return Total;
}

//字符串拼接ip:port
char* makehost(char* ip,char * port){

    char *host = (char *) malloc(strlen(ip) + strlen(port) + 2);   
    if (host == NULL) exit (1);  
    char *hosthead = host; //把首地址存下来  
    while (*ip != '\0') {  
        *host++ = *ip++;  
    }  
    *host++=':';
    while ((*host++ = *port++) != '\0');
    return hosthead;
}

char host[]="127.0.0.1:8080";

char* makeRequest(char*  path){
      
	  char *request=(char*)malloc(8000);
      //这里构造数据包没有数据体，一定要注意在最后加一个空行作为数据头的结尾
 	  sprintf(request, "GET %s HTTP/1.1\r\n"
            "Host:news.sohu.com\r\n"
            "Connection:Closed\r\n\r\n"
            ,path+13);
 	  return request;
}


char* makeRequest_alive(char*  path){
      
	  char *request=(char*)malloc(8000);

	  // printf("%s\n",path+12);
      //这里构造数据包没有数据体，一定要注意在最后加一个空行作为数据头的结尾
 	  sprintf(request, "GET %s HTTP/1.1\r\n"
            "Host:news.sohu.com\r\n"
            "Connection:keep-alive\r\n\r\n"
            ,path+13);

 	  // free(path);
 	  return request;
}



//根据有限状态自动机来查找a标签中的href链接
//注意调用之前，需要把html拼接完成
void htmlParse(char *html,char* host){

	// printf("ParseURL %s\n",host);

	int currentPageNo=getNum(host);
	memset(innerPagebitArray,0,innerPageBloomFilter/8+1);

	writeUrl(host,currentPageNo);
	int toPageNo=0;
	char * urlString,*urlStringPointer;
	
	urlString=(char*)malloc(1000);
	int state=0,count=0;


	while(*html){
		
		switch(state){
			case 0: 
			    if(*html=='<'){
                     state=1;    
			    }
			    html++;
			    break;
			case 1:
			    if(!strncmp(html,"a ",2)){
			    	state=2;
			    	html+=2;
			    }
			    else if(*html==' '){
			    	html++;
			    }
			    else{
			    	state=0;
			    	html++;
			    }
			    break;
 			case 2:
			   if(!strncmp(html,"href",4)){
			   	   html+=4;
			   	   state=3;
			   	   break;
			   }
			   switch(*html){
			   		case ' ':break;
			   		case '>':state=0;break;
			   		default:state=6;break;
			   }
			   html++;
			   break;
			case 3:
			   switch(*html){
			   		case '=':state=4;break;
			   		case ' ':break;
			   		case '>':state=0;break;
			   		default:state=6;break;
			   }
			   html++;
			   break;
			case 4:
			   switch(*html){
			   		case '\"':state=5;break;
			   		case ' ':break;
			   		case '>':state=0;break;
			   		default:state=6;break;
			   }
			   html++;
			   break;
			case 5:
			    
			    urlStringPointer=urlString;
			    while(*html && *html!='\"'){
			    	*urlStringPointer++=*html++;
			    }

			    if(*html=='\"'){
			    	state=0;
			    	html++;
			    	*urlStringPointer='\0';

			    	char* res=AnalyseURL(urlString,host);
					
					
					//url valid
			    	if(res!=NULL){
		    		    while(res[strlen(res)-1]==' '||res[strlen(res)-1]=='\n'||res[strlen(res)-1]=='\r')
		    		    	res[strlen(res)-1]='\0';

		    		    if(!isInpool(res)){   
		    		    	push(waitUrlPool,res);
		                	toPageNo=addnewPage(res);
	    	            	urlString=(char*)malloc(1000);
	        	        }
	        	        else if(!isInpoolInnerPage(res)){
	            	    	toPageNo=getNum(res);
	            	    	if(toPageNo<0)
	            	    		break;
	            	    	//去掉指向自身的情形
	            	    	if(currentPageNo!=toPageNo)
		            	    	writePair(currentPageNo,toPageNo);
	        	        }
			 		}

			    }		    
			    break;
            case 6:
               switch(*html){
			   		case ' ':state=2;break;
			   		case '>':state=0;break;
			   		default:break;
			   }
			   html++;
			   break;			 
		}
	}
}


//递归的处理url
char* domoreAnalyse(char *rawUrl, char *host){

	if(*rawUrl=='.'){

		if(strlen(rawUrl)>3 && *(rawUrl+1)=='.' && *(rawUrl+2)=='/'){
		    char* thelastp=strrchr(host,'/');
		    if(thelastp==NULL)
		    	return NULL;
		   	*(thelastp)='\0';
		   	thelastp=strrchr(host,'/');
		    if(thelastp==NULL)
		    	return NULL;
		   	*(thelastp+1)='\0';
		   	return domoreAnalyse(rawUrl+3,host);
		}
		if(strlen(rawUrl)>2 && *(rawUrl+1)=='/'){
			return domoreAnalyse(rawUrl+1,host);
		}
		return NULL;
	}
	else if(*rawUrl=='/'){
		char* thelastp=strrchr(host,'/');
		if(thelastp==NULL)
			return NULL;
		*(thelastp+1)='\0';
		return domoreAnalyse(rawUrl+1,host);
	}
	else{
		char* thelastp=strrchr(host,'/');
		*(thelastp+1)='\0';

		char *ret=(char*)malloc(strlen(host)+strlen(rawUrl)+1);
		memcpy(ret,host,strlen(host));
		return strcat(ret,rawUrl);
	} 
}

//把url处理成可以直接访问的形式
char* AnalyseURL(char *rawUrl,const char *host){

	if(strlen(rawUrl)==0)
		return NULL;
	    
	char *ss=strstr(rawUrl,"news.sohu.com");
	if(ss&&strlen(ss)==13){
		*(ss+13)='/';
		*(ss+14)='\0';
	}
    if(ss!=NULL) 
     	return ss;

    
    if(*rawUrl=='.' || *rawUrl=='/'){
    	char* newhost=(char*)malloc(strlen(host));
	    memcpy(newhost,host,strlen(host));
	    char * z =domoreAnalyse(rawUrl,newhost);
	    free(newhost);
	    return z;
	}

	return NULL;
}
