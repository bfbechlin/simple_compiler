#include <stdio.h>

int i = 0;
int f(int a, int b, int c, int d){
	return a+b;
}
void main(){
	f(1, 2, 3, 4);
	i = f(i, 2, 3, 4);
}
