// C program to illustrate 
// pipe system call in C 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>



void main(){
	int a=1;
	void xyz(int , int);
	xyz(++a,a++);
	xyz(a++,++a);
	printf("%d",a);
	}
   void xyz(int x, int y){
    printf("%d%d",x,y);
   }