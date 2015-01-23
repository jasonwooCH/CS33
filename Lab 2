#include <stdio.h>
#include <stdlib.h>

char *min_ptr = NULL , *max_ptr ;
char *saved_stack = NULL;

void spray_paint( char *x, int sz, char tok, char *t )
{
    printf( "%p %3d %x %s\n", x,sz,(unsigned char) tok,t ) ;
    
    min_ptr = x;
    for (int i = 0; i < sz; i++)
        *(x + i) = tok;
    max_ptr = (x + sz - 1);
}

void dumper(unsigned char *x, int n)
{
    int round = (int) x % 16;
    if (round)
        x = x - round;
	while (n > 0)
	{
		printf("%p ", x);
		for (int j = 0; j <= 3; j++)
		{
			for (int i = 3; i >= 0; i--)
				printf("%02x", *(x + i));
			printf(" ");
			x += 4;
		}
		printf("\n");
		n--;
	}
}


void sub2()
{
    int i ;
    char* a ;
    char x[20] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xf1, "sub2.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xf2, "sub2.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xf3, "sub2.x" ) ;
    printf ( "Min= %p Max= %p\n", min_ptr, max_ptr ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    
    int size = (int) (max_ptr - min_ptr) + 1;
    saved_stack = (char*) malloc (size);
    for (int j = 0; j < size; j++)  // save the contents
    {
        *(saved_stack + j) = *(min_ptr + j);
    }
    
    spray_paint(min_ptr, size, '*', "destroy"); // spray_paint over them
    
    printf( " destroyed stack\n" ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    
    for (int j = 0; j < size; j++)
    {
        *(min_ptr + j) = *(saved_stack + j); // now restore them
    }
    
    printf( " restored stack\n" ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    
    free(saved_stack); // free the malloc'd memory block saved stack
}

void sub1()
{
    float i ;
    short a ;
    int x[20] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xe1, "sub1.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xe2, "sub1.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xe3, "sub1.x" ) ;
    sub2() ;
}

int main()
{
    /*
    //int g = 0x12345678;
    int y = 0x01234567;
    char *x = (char *) &y;
    dumper(x, 3);
     
    spray_paint(x, 4, 'K', "nah");
     
    dumper(x, 3);
     */
    
    struct mine {
        char* a ;
        char x[20] ;
        float z ;
    } i;
    
    union crazy {
        float a ;
        char b ;
        int s ;
    } a ;
    
    char x[50] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xd1, "main.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xd2, "main.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xd3, "main.x" ) ;
    
    sub1() ;
    
    return 0 ;

}
