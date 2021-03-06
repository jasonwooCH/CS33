#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <stdbool.h>

#define S       16     //  (16 cache sets)
#define E       4      //  (4-way associative cache)
#define B       32     //  (32 elements in each block)
#define T       7      //  (7 tag bits)
#define M       65536  //  (65536 location memory)
#define READ    1
#define WRITE   0

int s ;
int b ;
int m ;

int rhits = 0 ; int rmiss = 0 ; int whits = 0 ; int wmiss = 0 ; int dwrit = 0 ;

struct cache_t
{
    char dirty ;
    char valid ;
    int  tag   ;
    int 	last  ;
    int  *block ;
} cache[S][E] ;

int *memory ;

int callno = 0 ;

int x  =     0 ;
int y  = 16384 ;
int z  = 32768 ;
int ni =    20 ;
int nj =    20 ;

void stats( char *t )
{
    int i,j,k,A ;
    
    for( i=0; i<S; i++ )
        for( j=0; j<E; j++ )
		{
            if( cache[i][j].valid & cache[i][j].dirty )
			{
                A = cache[i][j].tag*exp2(m-T)+i*B ;
                for( k=0; k<B; k++ )
                    memory[A+k] = cache[i][j].block[k] ;
                dwrit = dwrit+1 ;
			}
            cache[i][j].valid = 0 ;
            cache[i][j].dirty = 0 ;
		}
	
    printf( "%8s y=%5d ni=%5d nj=%5d rh=%5d rm=%5d wh=%5d wm= %5d dw=%5d\n", t, y,ni,nj,
           rhits, rmiss, whits, wmiss, dwrit ) ;
    rhits = 0 ; rmiss = 0 ;
    whits = 0 ; wmiss = 0 ;
    dwrit = 0 ;
    
}

void initcache()
{
    //
    // put code to calculate s, b, m here
    //
    s = log(S) / log(2);
    b = log(B) / log(2);
    m = log(M) / log(2);
    
    printf( "S= %d E= %d B= %d T= %d M= %d s= %d b= %d m= %d\n", S,E,B,T,M,s,b,m ) ;
    
    //
    // put code to initialize cache and memory here
    //
    // cache init
    int p;
    for (p = 0; p < S; p++)
    {
        int q;
        int tagNum = 0;
        for (q = 0; q < E; q++)
        {
            cache[p][q].dirty = 0;
            cache[p][q].valid = 0;
            cache[p][q].block = (int*)malloc(B);
            cache[p][q].last = 0;
            cache[p][q].tag = tagNum;
            tagNum++;
        }
    }
    
    //memory init
    memory = (int*) malloc (M);
}

void readwritecache( int readwrite, int a, int *value, int *hitmiss, int voice )
{
    //
    // readwrite = READ (1) for read, WRITE (0) for write
    // a = binary memory address ( 0 <= a < 65535 ) to read or write
    // *value is memory value returned when readwrite = READ
    //           memory value to store when readwrite = WRITE
    // hitmiss is set to 0 for a miss, 1 for a hit, in either read or write case
    // voice is a debugging switch
    //
    //
    //   compute si, ta, and bo from the address a
    int si, ta, bo, li ;
    //	si = stack, index
    //	ta = tag
    //	bo = block offset
    //      increment callno
    //
    
    int num = a;
    ta = num / pow (2, s + b);
    num = a % (int)pow(2, s + b);
    
    si = num / pow(2, b);
    num %= (int)pow(2, b);
    
    bo = num;
    
    callno = callno + 1;
    
    if( voice )
        printf( "%5d rw= %d a= %5d bo= %3d si= %3d ta= %3d", callno,readwrite,a,bo,si,ta ) ;
    //
    //   check each line of the set:
    //	if( cache[si][line#].valid && cache[si][line#].tag = ta )
    //	to find a hit
    //
    //   if no hit, choose a line (not valid or LRU )
    //
    //   if chosen line dirty, copy to memory
    //
    
    int p;

    int invalid = 0;
    int LRU = 1;
    int hasInvalid = 0;
    int lruLine = 0;
    *hitmiss = 0;
    
    for (p = 0; p < E; p++)
    {
        // if hit, leave loop
        // otherwise, keep track of invalid lines and LRU for future use
        if (cache[si][p].valid && cache[si][p].tag == ta)
        {
            *hitmiss = 1;
            li = p;
            break;
        }
        else if (!cache[si][p].valid)
        {
            hasInvalid = 1;
            invalid = p;
        }
        
        if (cache[si][p].last >= LRU)
        {
            LRU = cache[si][p].last;
            lruLine = p;
        }
    }
    if (!(*hitmiss))
    {
        // if miss, check if there's invalid line
        // otherwise, use LRU
        if (hasInvalid)
            li = invalid;
        else
            li = lruLine;
    }
    // check cache at chosen line -> if dirty copy to memory
    if (cache[si][li].dirty & cache[si][li].valid)
    {
        int addr = cache[si][li].tag * exp2(m-T)+si*B;
        int k;
        for( k=0; k<B; k++ )
        {
            memory[addr+k] = cache[si][li].block[k];
        }
        dwrit = dwrit + 1;
        cache[si][li].dirty = 0;
    }
    
    if( voice )
        printf( "li= %d", li ) ;
    //
    //    copy from memory to chosen line
    //
    //    if write, copy value into line, set dirty
    //
    //    else copy value from line, not dirty
    //
    //    set last for line
    //
    for (p = 0; p < S; p++)
    {
        int q;
        for (q = 0; q < E; q++)
            cache[p][q].last++;
    }
    
    if (readwrite == READ)
    {
        if (*hitmiss)
        {
            rhits = rhits + 1;
            *value = cache[si][li].block[bo];
            cache[si][li].last = 0;
        }
        else
        {
            rmiss = rmiss + 1;
            int fetch = a - (a % 32);
            int k;
            for (k = 0; k < B; k++)
                cache[si][li].block[k] = memory[fetch+k];
            cache[si][li].tag = ta;
            cache[si][li].valid = 1;
            cache[si][li].last = 0;
            *value = cache[si][li].block[bo];
        }
    }
    else
    {
        if (*hitmiss)
            whits = whits + 1;
        else
        {
            wmiss = wmiss + 1;
            cache[si][li].tag = ta;
        }
        cache[si][li].block[bo] = *value;
        cache[si][li].dirty = 1;
        cache[si][li].valid = 1;
        cache[si][li].last  = 0;
    }

    if( voice )
        printf( " %d %d %d\n", *value, cache[si][li].valid, cache[si][li].dirty ) ;
}

void locationexample()
{
    int i,j,k,hm, r;
    
    for( y=16374; y< 16395; y=y+1 )
    {
        for( i=0; i<nj;i++ )
        {
            readwritecache( READ,   x+i, &r, &hm, 1 ) ;
            readwritecache( WRITE,  y+i, &r, &hm, 1 ) ;
        }
        stats( "loc copy" ) ;
    }
    
    
    //
    // code for row wise transponse
    //
    for (i = 0; i < ni; i++) {
        for (j = 0; j < nj; j++) {
            readwritecache(READ, y + i*nj + j , &k, &hm, 1);
            readwritecache(WRITE, x + j*ni + i, &k, &hm, 1);
        }
    }
    stats( "loc rows" ) ;
    
    //
    // code for col wise transponse
    //
    for (j = 0; j < ni; j++) {
        for (i = 0; i < nj; i++) {
            readwritecache(READ, y + i*nj + j , &k, &hm, 1);
            readwritecache(WRITE, x + j*ni + i, &k, &hm, 1);
        }
    }
    stats( "loc cols" ) ;
    
}

void wsexample()
{
    int i,j,ii,jj,hm,r ;
    
    y = 20000 ;
    /*
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for row wise transpose
        //
        for (i = 0; i < ni; i++) {
            for (j = 0; j < nj; j++) {
                readwritecache(READ, y + i*nj + j , &r, &hm, 1);
                readwritecache(WRITE, x + j*ni + i, &r, &hm, 1);
            }
        }
        stats( "ws rows" ) ;
    }
    */
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for row wise transpose with 8x blocking
        //
        for (ii = 0;ii < ni; ii = ii + 8) {
            for (jj = 0; jj < nj; jj = jj + 8) {
                for (i = ii; i < ii + 8; i++) {
                    for (j = jj; j < jj + 8; j++) {
                        readwritecache(READ, y + i*nj + j , &r, &hm, 1);
                        readwritecache(WRITE, x + j*ni + i, &r, &hm, 1);
                    }
                }
            }
        }
        stats( "wsbrows" ) ;
    }
    /*
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for col wise transpose
        //
        for (j = 0; j < ni; j++) {
            for (i = 0; i < nj; i++) {
                readwritecache(READ, y + i*nj + j , &r, &hm, 1);
                readwritecache(WRITE, x + j*ni + i, &r, &hm, 1);
            }
        }
        stats( "ws cols" ) ;
    }
    */
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for col wise transpose with 8x blocking
        //
        for (jj = 0;jj < ni; jj = jj + 8) {
            for (ii = 0; ii < nj; ii = ii + 8) {
                for (j = jj; j < jj + 8; j++) {
                    for (i = ii; i < ii + 8; i++) {
                        readwritecache(READ, y + i*nj + j , &r, &hm, 1);
                        readwritecache(WRITE, x + j*ni + i, &r, &hm, 1);
                    }
                }
            }
        }
        stats( "wsbcols" ) ;
    }
}

int main()
{
    initcache() ;
    int l;
    for (l = 0; l < M; l++) {
        memory[l] = l;
    }
    //locationexample();
    wsexample();

    return 0 ;
}
