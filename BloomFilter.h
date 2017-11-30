#ifndef BLOOMFILTER_H_INCLUDED
#define BLOOMFILTER_H_INCLUDED

#define mBloomFilter 9999991   //位向量大小
#define innerPageBloomFilter 997 //
#define kBloomFilter 8    //hash函数个数
/*
注意写一个素数
*/

unsigned int RSHash(char*);
unsigned int JSHash(char*);
unsigned int PJWHash(char*);
unsigned int ELFHash(char*);
unsigned int BKDRHash(char*);
unsigned int SDBMHash(char*);
unsigned int DJBHash(char*);
unsigned int APHash(char*);

//hash函数指针数组
unsigned int (*hash[])(char* str)={RSHash,JSHash,PJWHash,ELFHash,BKDRHash,SDBMHash,DJBHash,APHash};
unsigned char bitArray[mBloomFilter/8+1];   //位向量
unsigned char innerPagebitArray[innerPageBloomFilter/8+1];
unsigned char mask[8];  //mask code

void initBoolmFilter(){
    memset(bitArray,0,mBloomFilter/8+1);
    int i=0;
    for(;i<8;i++){
        mask[i]=1<<(8-i-1);
    }
}
// n=hash[i](url) 表示的是第n位bit应该置为1。

void dokhash(char *url){
   int i=0;
   int bitIndex;
   int NoByte,innderByte;
   for(;i<kBloomFilter;i++){
       bitIndex=hash[i](url)%mBloomFilter;
       NoByte=bitIndex/8;
       innderByte=bitIndex%8;
       bitArray[NoByte]=bitArray[NoByte] | mask[innderByte];
   }
}
int isInpool(char *url){ 
   int i=0;
   int bitIndex;
   int NoByte,innderByte;
   for(;i<kBloomFilter;i++){

        bitIndex=hash[i](url)%mBloomFilter;
        NoByte=bitIndex/8;
        innderByte=bitIndex%8;
        if(!(bitArray[NoByte] & mask[innderByte])){
            dokhash(url);
            return 0;
        }
   }
   return 1;
}
void dokhashInnerPage(char* url){
    int i=0;
    int bitIndex;
    int NoByte,innderByte;
    for(;i<kBloomFilter;i++){
        bitIndex=hash[i](url)%innerPageBloomFilter;
        NoByte=bitIndex/8;
        innderByte=bitIndex%8;
        innerPagebitArray[NoByte]=innerPagebitArray[NoByte] | mask[innderByte];
    }
}
int isInpoolInnerPage(char* url){
    int i=0;
    int bitIndex;
    int NoByte,innderByte;
    for(;i<kBloomFilter;i++){
        bitIndex=hash[i](url)%innerPageBloomFilter;
        NoByte=bitIndex/8;
        innderByte=bitIndex%8;
        if(!(innerPagebitArray[NoByte] & mask[innderByte])){
            dokhashInnerPage(url);
            return 0;
        }
    }
    return 1;
}


// RS Hash Function
unsigned int RSHash(char* str){

    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    while(*str){
        hash = hash * a + (*str++);
        a *= b;
    }
    return (hash & 0x7FFFFFFF);
}

// JS Hash Function
unsigned int JSHash(char* str){
      
    unsigned int hash = 1315423911;
    while(*str)
        hash ^= ((hash << 5) + (*str++) + (hash >> 2));
    return (hash & 0x7FFFFFFF);
}

// P. J. Weinberger Hash Function
unsigned int PJWHash(char* str){
      
    unsigned int BitsInUnignedInt = (unsigned int )( sizeof (unsigned int)*8);
    unsigned int ThreeQuarters = (unsigned int )((BitsInUnignedInt*3 ) / 4);
    unsigned int OneEighth = (unsigned int )(BitsInUnignedInt / 8);
    unsigned int HighBits = (unsigned int )( 0xFFFFFFFF ) << (BitsInUnignedInt - OneEighth);
    unsigned int hash = 0;
    unsigned int test = 0;
    while(*str){
        hash = (hash << OneEighth) + (*str++);
        if((test = hash & HighBits) != 0)
            hash = ((hash ^ (test >> ThreeQuarters)) & ( ~ HighBits));
    }
    return (hash & 0x7FFFFFFF);
}

// ELF Hash Function
unsigned int ELFHash(char* str){
     
    unsigned int hash = 0;
    unsigned int x = 0;
    while(*str){
        hash = (hash << 4 ) + (*str ++ );
        if((x = hash & 0xF0000000L ) != 0 ){
            hash ^= (x >> 24 );
            hash &= ~ x;
        }
    }
    return (hash & 0x7FFFFFFF);
}

// BKDR Hash Function
unsigned int BKDRHash(char* str){
       
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while(*str)
        hash = hash*seed + (*str ++ );
    return (hash & 0x7FFFFFFF);
}

// SDBM Hash Function
unsigned int SDBMHash(char* str){
   
    unsigned int hash = 0;
    while(*str)
        hash = (*str ++ ) + (hash << 6 ) + (hash << 16 ) - hash;
    return (hash & 0x7FFFFFFF);
}

// DJB Hash Function
unsigned int DJBHash(char* str){
    unsigned int hash = 5381;
    while(*str)
        hash += (hash << 5 ) + (*str ++ );
    return (hash & 0x7FFFFFFF);
}

// AP Hash Function
unsigned int APHash(char* str){
    unsigned int hash = 0;
    int i;
    for(i = 0 ;*str; i ++ ){
        if((i & 1 ) == 0 )
            hash ^= ((hash << 7 ) ^ (*str ++ ) ^ (hash >> 3 ));
        else
            hash ^= ( ~ ((hash << 11 ) ^ (*str ++ ) ^ (hash >> 5 )));
    }
    return (hash & 0x7FFFFFFF);
}


#endif // BLOOMFILTER_H_INCLUDED
